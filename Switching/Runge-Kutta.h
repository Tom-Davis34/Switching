#pragma once

#include "Grid.h"
#include "common.h"
#include "SparseMatrix.h"

#define RG 0.001
#define LG 0.01
#define CG 0.0001

#define CL 0.001
#define WRITE_OUTPUT true
#define PI 3.14159265359

float rho = 28;
float sigma = 10;
float beta = 8 / 3;
//x1` = sigma(x2 - x1)
//x2` = x1(rho - x3) - x2
//x3` = x1*x2 - beta*x3
vector<float> f(vector<float> x) {
	return vector<float> {
		sigma*(x[1] - x[0]),
			x[0] * (rho - x[2]) - x[1],
			x[0]* x[1] - beta * x[2]
	};
}

vector<float> rungeKuttaStep(vector<float> x, float dt) {
	auto f1 = f(x);
	auto f2 = f(f1 * (dt / 2) + x);
	auto f3 = f(f2 * (dt / 2) + x);
	auto f4 = f(f3 * dt + x);
	
	return x + (dt / 6) * (f1 + 2 * f2 + 2 * f3 + f4);
}

vector<float> buildX() {
	return  vector<float> {
		-8,
		8,
		27
	};
}

void rungeKutta4Lorentz() {
	float startTime = 0;
	float endTime = 16;
	float dt = 0.01;
	int numIter = (endTime - startTime) / dt;

	ofstream myfile;
	myfile.open("runge-kutta4.csv");

	auto x = buildX();

	for (size_t i = 0; i < numIter; i++)
	{
		myfile << x[0] << ", " << x[1] << ", " << x[2] << "\n";
		x = rungeKuttaStep(x, dt);
	}

	myfile.close();
}

vector<float> createZeroVector(int size) {
	vector<float> vec = vector<float>(size);

	for (int i = 0;i < size; i++) {
		vec[i] = 0;
	}

	return vec;
}

SparseMatrixReal buildA(PowerGrid* grid, int cbId, vector<PowerFlowNode> startingVoltages, vector<int> oldNodeToSuperNode,
	vector<int> newNodeToSuperNode) {
	grid->createSubGraph();

	int voltageNum = grid->superNodeToNode.size();
	int currentFromGndNum = voltageNum - 1;
	int cirCurrentNum = grid->cs.size();
	int totalRows = voltageNum + currentFromGndNum + cirCurrentNum;
	int genNum = grid->getGenNum();

	int indexOfCurrentFromGnd = voltageNum ;
	int indexOfCirCurrentNum = voltageNum + currentFromGndNum;

	vector<float> capToGnd = vector<float>(totalRows);
	vector<bool> containsEdge = vector<bool>(totalRows);

	//totalRows x totalRows
	SparseMatrixRealBuilder smbA = SparseMatrixRealBuilder(totalRows);

	for (size_t i = 0; i < voltageNum; i++)
	{
		vector<BusData> gens = grid->getGens(i);

		if (gens.size() == 0){
			continue;
		}

		float Lg = LG / gens.size();
		float Rg = RG / gens.size();
		float Cg = gens.size() * CG;

		int indexOfI = i + indexOfCurrentFromGnd - 1;
		int indexOfV = i;


		smbA.plus(indexOfI, indexOfI, -Rg / Lg);
		smbA.plus(indexOfI, indexOfV, -1 / Lg);
		//smbA.plus(indexOfV, indexOfV, -1 / (Lg * Cg));
		capToGnd[indexOfV] += Cg;

		smbA.set(indexOfV, indexOfI, 1);
	}


	for (size_t i = 0; i < voltageNum; i++)
	{
		cmplx totaladmittance = grid->getTotalAdmittance(i, startingVoltages, oldNodeToSuperNode);

		if (isZero(totaladmittance)) {
			continue;
		}

		cmplx totalInductance = cmplx(1.0) / totaladmittance;

		float Ll = abs(totalInductance.imag());
		float Rl = abs(totalInductance.real());

		int indexOfI = i + indexOfCurrentFromGnd - 1;
		int indexOfV = i;

		smbA.plus(indexOfI, indexOfI, -Rl / Ll);
		//smbA.plus(indexOfI, indexOfI, -Rl / Ll);
		smbA.plus(indexOfI, indexOfV, -1 / Ll);

		smbA.set(indexOfV, indexOfI, 1);
	}


	for (int k = 0; k < grid->cs.size(); k++) {
		Circuit cir = grid->cs[k];
		int i = newNodeToSuperNode[cir.fbus];
		int j = newNodeToSuperNode[cir.tbus];
		float chrg = cir.lineCharge;
		cmplx inductance = cmplx(1)/cir.admittance;
			
		float r = inductance.real();
		float ind = inductance.imag();

		int indexOfI = k + indexOfCirCurrentNum;

		//i dot
		smbA.plus(indexOfI, indexOfI, -r/ind);
		smbA.plus(indexOfI, i, -1/ind);
		smbA.plus(indexOfI, j, 1/ind);

		////cap
		//smbA.set(i, k, );
		//smbA.set(j, k, 0.5f * chrg);
	}

	return smbA.build();
}

SparseMatrixReal buildB(PowerGrid* grid, int cbId, vector<PowerFlowNode> startingVoltages, vector<int> oldNodeToSuperNode,
	vector<int> newNodeToSuperNode) {
	grid->createSubGraph();

	int voltageNum = grid->superNodeToNode.size();
	int currentFromGndNum = voltageNum - 1;
	int cirCurrentNum = grid->cs.size();
	int totalRows = voltageNum + currentFromGndNum + cirCurrentNum;
	int genNum = grid->getGenNum();

	int indexOfCurrentFromGnd = voltageNum;
	int indexOfCirCurrentNum = voltageNum + currentFromGndNum;

	//totalRows x totalRows
	SparseMatrixRealBuilder smbB = SparseMatrixRealBuilder(totalRows);

	for (size_t i = 1; i < voltageNum; i++)
	{
		vector<BusData> gens = grid->getGens(i);

		float Lg = LG / gens.size();
		float Rg = RG / gens.size();
		float Cg = gens.size() * CG;

		int indexOfI = i + indexOfCurrentFromGnd - 1;
		int indexOfV = i - 1;

		smbB.plus(indexOfI, indexOfI, 1 / Lg);
	}

	return smbB.build();
}

vector<float> mul(SparseMatrixReal A, SparseMatrixReal B, vector<float> vec, vector<float> sint) {
	vector<float> gentCur = B.mul(sint);
	 return A.mul(vec) + gentCur;
}

vector<float> step(SparseMatrixReal A, SparseMatrixReal B, vector<float> vec, vector<float> sint, float dt) {
	auto f1 = mul(A, B, vec, sint);
	auto f2 = mul(A, B, f1 * (dt / 2) + vec, sint);
	auto f3 = mul(A, B, f2 * (dt / 2) + vec, sint);
	auto f4 = mul(A, B, f3 * dt + vec, sint);

	return vec + (dt / 6) * (f1 + 2 * f2 + 2 * f3 + f4);
}


void writeVec(vector<float> vec, ofstream* myfile) {
	for (size_t i = 0; i < vec.size(); i++)
	{
		(*myfile) << vec[i] << ", ";
	}

	(*myfile) << "\n";

	return;
}

void rungeKutta4(PowerGrid* grid, int cbId, vector<PowerFlowNode> startingVoltages, vector<int> oldNodeToSuperNode, vector<int> newNodeToSuperNode) {
	float dt = 0.0001;
	float t = 0;
	float endTime = 5;

	int voltageNum = grid->superNodeToNode.size();
	int currentFromGndNum = voltageNum - 1;
	int cirCurrentNum = grid->cs.size();
	int totalRows = voltageNum + currentFromGndNum + cirCurrentNum;
	int genNum = grid->getGenNum();

	int indexOfCurrentFromGnd = voltageNum;
	int indexOfCirCurrentNum = voltageNum + currentFromGndNum;

	SparseMatrixReal A = buildA(grid, cbId, startingVoltages, oldNodeToSuperNode, newNodeToSuperNode);
	SparseMatrixReal B = buildB(grid, cbId, startingVoltages, oldNodeToSuperNode, newNodeToSuperNode);

	vector<float> vec = createZeroVector(totalRows);
	vector<float> sint = createZeroVector(totalRows);

	ofstream myfile;
	if (WRITE_OUTPUT) {
		myfile.open("transient.csv");
	}

	while (t < endTime) {
		if (WRITE_OUTPUT) {
			writeVec(vec, &myfile);
		}

		int i = 0;
		while (i < indexOfCirCurrentNum) {
			sint[i] = sinf((2 * PI)*t*50);
			i++;
		}

		//cout << sint[0] << "\n";

		vec = step(A, B, vec, sint, dt);
		t += dt;
	}
}


