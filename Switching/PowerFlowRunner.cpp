#pragma once

#include "PowerFlowRunner.h"

using namespace std;

complex updateVoltage(int tid, PowerFlowNode* powerFlowNode, SparseMatrixComplex admMat, complex* voltage, int slackBus, complex pq) {
	complex result = 0;
	complex diagonal = 0.0;
	int eleIndex = admMat.row[tid];
	int lastEleIndex = admMat.row[tid + 1];

	eleIndex = admMat.row[tid];
	while (eleIndex < lastEleIndex) {
		if (admMat.col[eleIndex] == tid) {
			diagonal = admMat.ele[eleIndex];
		}
		else {
			result -= voltage[admMat.col[eleIndex]] * admMat.ele[eleIndex];
		}
		eleIndex++;
	}

	if (!voltage[tid].isZero() && !pq.isZero()) {
		result += (pq.conj() / voltage[tid]);
	}

	result = result / diagonal;

	if (powerFlowNode[tid].isPV && !result.isZero()) {
		result = result / result.abs();
	}

	return result;
}

complex updateReactivePower(int tid, PowerFlowNode* powerFlowNode, SparseMatrixComplex admMat, complex* voltage, int slackBus, complex pq) {
	complex result = 0;
	complex diagonal = 0.0;
	int eleIndex = admMat.row[tid];
	int lastEleIndex = admMat.row[tid + 1];

	if (powerFlowNode[tid].isPV) {
		complex adjustReactivePower = 0;

		while (eleIndex < lastEleIndex) {
			adjustReactivePower += (admMat.ele[eleIndex] * voltage[admMat.col[eleIndex]]);
			eleIndex++;
		}

		return Complex(pq.realVal(), -(adjustReactivePower.conj() * voltage[tid]).imagVal());
	}
	else {
		return pq;
	}
}

void jacobiMethodCPU(PowerFlowNode* powerFlowNode, SparseMatrixComplex admMat, complex* voltage, int nodeNum, int slackBus) {

	voltage[slackBus] = Complex(1, 0);

	//power injected/leaving a bus
	complex* pqs = new complex[nodeNum];

	for (int tid = 0; tid < nodeNum; tid++) {
		pqs[tid] = powerFlowNode[tid].pq;
	}
	real maxDiff = 1;
	real diff = 0;
	int iter = 0;
	while (maxDiff > 0.00001 && iter < 500000) {

		maxDiff = 0;
		for (int tid = 0; tid < nodeNum; tid++) {
			if (tid != slackBus) {
				//updates reactive power at each generator
				auto pq = pqs[tid];
				auto newPQ = updateReactivePower(tid, powerFlowNode, admMat, voltage, slackBus, pqs[tid]);
				auto delta = (pq - newPQ).imagVal();
				//pqs[tid] = newPQ;
				complex newVoltage = updateVoltage(tid, powerFlowNode, admMat, voltage, slackBus, pqs[tid]);

				diff = (newVoltage - voltage[tid]).abs();
				if (diff > maxDiff) {
					maxDiff = diff;
				}
				voltage[tid] = newVoltage;
			}
		}
		iter++;
	}
}

void calculateAndPrintPower(SparseMatrixComplex admMat, PowerFlowNode* powerFlowNode, complex* voltage, size_t nodeNum) {

	fprintf(stderr, "\nBus Power: \n");
	for (size_t i = 0; i < nodeNum; i++)
	{
		int eleIndex = admMat.row[i];
		int lastEleIndex = admMat.row[i + 1];


		complex result;
		while (eleIndex < lastEleIndex) {

			int col = admMat.col[eleIndex];

			//fprintf(stderr, "Current [%d, %d]: %f + %fj pu\n", i, col, (admMat.ele[eleIndex] * voltage[col]).realVal(), (admMat.ele[eleIndex] * voltage[col]).imagVal());

			result += (admMat.ele[eleIndex] * voltage[admMat.col[eleIndex]]).conj() * voltage[i];
			eleIndex++;
		}

		fprintf(stderr, "[%d] P + jQ: %f + %fj pu       P + jQ: %f + %fj MW       Bus Voltage = %f\n", i, result.realVal(), result.imagVal(), result.realVal() * powerFlowNode[i].systemVoltage, result.imagVal() * powerFlowNode[i].systemVoltage, powerFlowNode[i].systemVoltage);
	}

}

vector<complex> initVoltage(vector<PowerFlowNode> powerFlowNode) {
	vector<complex> h_voltage(powerFlowNode.size());

	for (size_t i = 0; i < powerFlowNode.size(); i++)
	{
		if (powerFlowNode[i].isPV) {
			h_voltage[i] = Complex(1);
		}
		else {
			h_voltage[i] = Complex(0);
		}

	}

	return h_voltage;
}

void powerFlowRunner(PowerFlowNode* h_powerFlowNode, SparseMatrixComplex h_admMat, complex* h_voltage, int nodeNum, int slackBus) {

	auto start = std::chrono::high_resolution_clock::now();
	//jacobiMethod<<<1, 30>>>(d_powerFlowNode, d_admMat, d_voltage, 0);
	jacobiMethodCPU(h_powerFlowNode, h_admMat, h_voltage, nodeNum, slackBus);

	//Wait for kernel to finish
	auto stop = std::chrono::high_resolution_clock::now();

	// Get duration. Substart timepoints to
	// get duration. To cast it to proper unit
	// use duration cast method
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

	cout << "\nTime taken by powerFlow function: "
		<< duration.count() << " microseconds" << "\n";
	//Copy volltage array from GPU to host
	//copyToHost(&h_voltage[0], d_voltage, h_powerFlowNode.size());
	for (int i = 0; i < nodeNum; i++) {
		h_powerFlowNode[i].unitVoltage = h_voltage[i];
		fprintf(stderr, "V[%d]: %f + %fj - |V| = %f\n", i, h_voltage[i].realVal(), h_voltage[i].imagVal(), h_voltage[i].abs());
	}

	//calculateAndPrintPower(h_admMat, h_powerFlowNode, h_voltage, nodeNum);
}
