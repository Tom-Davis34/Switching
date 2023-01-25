#pragma once

#include "PowerFlowRunner.h"



cmplx updateVoltage(int tid, PowerFlowNode* powerFlowNode, SparseMatrixComplex admMat, cmplx* voltage, int slackBus, cmplx pq) {
	cmplx result = 0;
	cmplx diagonal = 0.0;
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

	if (!isZero(voltage[tid]) && !isZero(pq)) {
		result += (conj(pq) / voltage[tid]);
	}

	result = result / diagonal;

	if (powerFlowNode[tid].isPV && !isZero(result)) {
		result = result / abs(result);
	}

	return result;
}

cmplx updateReactivePower(int tid, PowerFlowNode* powerFlowNode, SparseMatrixComplex admMat, cmplx* voltage, int slackBus, cmplx pq) {
	cmplx result = 0;
	cmplx diagonal = 0.0;
	int eleIndex = admMat.row[tid];
	int lastEleIndex = admMat.row[tid + 1];

	if (powerFlowNode[tid].isPV) {
		cmplx adjustReactivePower = 0;

		while (eleIndex < lastEleIndex) {
			adjustReactivePower += (admMat.ele[eleIndex] * voltage[admMat.col[eleIndex]]);
			eleIndex++;
		}

		return cmplx(pq.real(), -(conj(adjustReactivePower) * voltage[tid]).imag());
	}
	else {
		return pq;
	}
}

void jacobiMethodCPU(PowerFlowNode* powerFlowNode, SparseMatrixComplex admMat, cmplx* voltage, int nodeNum, int slackBus) {

	voltage[slackBus] = cmplx(1, 0);

	//power injected/leaving a bus
	cmplx* pqs = new cmplx[nodeNum];

	for (int tid = 0; tid < nodeNum; tid++) {
		pqs[tid] = powerFlowNode[tid].pq;
	}
	float maxDiff = 1;
	float diff = 0;
	int iter = 0;
	while (maxDiff > 0.00001 && iter < 500000) {

		maxDiff = 0;
		for (int tid = 0; tid < nodeNum; tid++) {
			if (tid != slackBus) {
				//updates reactive power at each generator
				auto pq = pqs[tid];
				auto newPQ = updateReactivePower(tid, powerFlowNode, admMat, voltage, slackBus, pqs[tid]);
				auto delta = (pq - newPQ).imag();
				//pqs[tid] = newPQ;
				cmplx newVoltage = updateVoltage(tid, powerFlowNode, admMat, voltage, slackBus, pqs[tid]);

				diff = abs((newVoltage - voltage[tid]));
				if (diff > maxDiff) {
					maxDiff = diff;
				}
				voltage[tid] = newVoltage;
			}
		}
		iter++;
	}
}

void calculateAndPrintPower(SparseMatrixComplex admMat, PowerFlowNode* powerFlowNode, cmplx* voltage, size_t nodeNum) {

	fprintf(stderr, "\nBus Power: \n");
	for (size_t i = 0; i < nodeNum; i++)
	{
		int eleIndex = admMat.row[i];
		int lastEleIndex = admMat.row[i + 1];


		cmplx result;
		while (eleIndex < lastEleIndex) {

			int col = admMat.col[eleIndex];

			//fprintf(stderr, "Current [%d, %d]: %f + %fj pu\n", i, col, (admMat.ele[eleIndex] * voltage[col]).floatVal(), (admMat.ele[eleIndex] * voltage[col]).imagVal());

			result += (admMat.ele[eleIndex] * conj(voltage[admMat.col[eleIndex]])) * voltage[i];
			eleIndex++;
		}

		fprintf(stderr, "[%d] P + jQ: %f + %fj pu       P + jQ: %f + %fj MW       Bus Voltage = %f\n", i, result.real(), result.imag(), result.real() * powerFlowNode[i].systemVoltage, result.imag() * powerFlowNode[i].systemVoltage, powerFlowNode[i].systemVoltage);
	}

}

vector<cmplx> initVoltage(vector<PowerFlowNode> powerFlowNode) {
	vector<cmplx> h_voltage(powerFlowNode.size());

	for (size_t i = 0; i < powerFlowNode.size(); i++)
	{
		if (powerFlowNode[i].isPV) {
			h_voltage[i] = cmplx(1);
		}
		else {
			h_voltage[i] = cmplx(0);
		}

	}

	return h_voltage;
}

void powerFlowRunner(PowerFlowNode* h_powerFlowNode, SparseMatrixComplex h_admMat, cmplx* h_voltage, int nodeNum, int slackBus) {

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
		fprintf(stderr, "V[%d]: %f + %fj - |V| = %f\n", i, h_voltage[i].real(), h_voltage[i].imag(), abs(h_voltage[i]));
	}

	//calculateAndPrintPower(h_admMat, h_powerFlowNode, h_voltage, nodeNum);
}
