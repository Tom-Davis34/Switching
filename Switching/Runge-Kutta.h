#pragma once

#include "Grid.h"
#include "common.h"
#include "SparseMatrix.h"

//Z_base = 10000
#define RG 1
#define RGC 100
#define LG 0.00525
#define CG 0.000525

#define SWR 0.001

#define CL 0.0000000001
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

SparseMatrixRealBuilder builderA(PowerGrid* grid, int cbId, vector<PowerFlowNode> startingVoltages, vector<int> oldNodeToSuperNode,
	vector<int> newNodeToSuperNode, vector<float> capToGnd ) {

	int voltageNum = grid->superNodeToNode.size() - 1;
	int currentFromGndNum = voltageNum*2;
	int cirCurrentNum = grid->cs.size();
	int totalRows = voltageNum + currentFromGndNum + cirCurrentNum;
	int genNum = grid->getGenNum();

	int indexOfGenCurrent = voltageNum;
	int indexOfLoadCurrent = voltageNum*2;
	int indexOfCirCurrentNum = voltageNum*3;

	vector<bool> containsEdge = vector<bool>(totalRows);

	//totalRows x totalRows
	SparseMatrixRealBuilder smbA = SparseMatrixRealBuilder(totalRows);

	for (size_t i = 0; i < voltageNum; i++)
	{
		vector<BusData> gens = grid->getGens(i + 1);

		if (gens.size() == 0){
			continue;
		}

		float Lg = LG / gens.size();
		float Rg = RG / gens.size();
		float Rcg = RG / gens.size();
		float Cg = gens.size() * CG;

		int indexOfI = i + indexOfGenCurrent;
		int indexOfV = i;

		smbA.plus(indexOfI, indexOfI, -1 / Lg);
		smbA.plus(indexOfI, indexOfV, -Rg / Lg);

		smbA.plus(indexOfV, indexOfV, -1.0 / (float)RGC);

		capToGnd[indexOfV] += Cg;

		smbA.set(indexOfV, indexOfI, 1);
	}

	for (size_t i = 0; i < voltageNum; i++)
	{
		cmplx totaladmittance = grid->getTotalAdmittance(i + 1, startingVoltages, oldNodeToSuperNode);

		if (isZero(totaladmittance)) {
			continue;
		}

		cmplx totalInductance = cmplx(1.0) / totaladmittance;

		float Ll = abs(totalInductance.imag());
		float Rl = abs(totalInductance.real());

		int indexOfI = i + indexOfLoadCurrent;
		int indexOfV = i;

		smbA.plus(indexOfI, indexOfI, -Rl / Ll);
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
		capToGnd[i] += chrg / 2;
		capToGnd[j] += chrg / 2;
	}

	for (size_t i = 0; i < voltageNum; i++) {
		for (size_t j = 0; j < totalRows; j++) {
			smbA.divide(i, j, capToGnd[i]);
		}
	}

	return smbA;
}

SparseMatrixRealBuilder addATilder(SparseMatrixRealBuilder smbA, PowerGrid* grid, int node1, int node2, vector<float> capToGnd, vector<int> newNodeToSuperNode) {
	
	
	int voltageNum = grid->superNodeToNode.size() - 1;
	int currentFromGndNum = voltageNum * 2;
	int cirCurrentNum = grid->cs.size();
	int totalRows = voltageNum + currentFromGndNum + cirCurrentNum;
	int genNum = grid->getGenNum();

	int indexOfGenCurrent = voltageNum;
	int indexOfLoadCurrent = voltageNum * 2;
	int indexOfCirCurrentNum = voltageNum * 3;


	int i = newNodeToSuperNode[node1] - 1;
	int j = newNodeToSuperNode[node2] - 1;

	int indexOfIi = i + indexOfGenCurrent;
	int indexOfVi = i;
	int indexOfIj = j + indexOfGenCurrent;
	int indexOfVj = j;

	smbA.plus(indexOfVi, indexOfVi, -1 / SWR / capToGnd[indexOfVi]);
	smbA.plus(indexOfVi, indexOfVj,  1 / SWR / capToGnd[indexOfVi]);
	smbA.plus(indexOfVj, indexOfVj, -1 / SWR / capToGnd[indexOfVj]);
	smbA.plus(indexOfVj, indexOfVi,  1 / SWR / capToGnd[indexOfVj]);


	return smbA;
}

SparseMatrixReal buildB(PowerGrid* grid, int cbId, vector<PowerFlowNode> startingVoltages, vector<int> oldNodeToSuperNode,
	vector<int> newNodeToSuperNode) {
	grid->createSubGraph();

	int voltageNum = grid->superNodeToNode.size() - 1;
	int currentFromGndNum = voltageNum * 2;
	int cirCurrentNum = grid->cs.size();
	int totalRows = voltageNum + currentFromGndNum + cirCurrentNum;
	int genNum = grid->getGenNum();

	int indexOfGenCurrent = voltageNum;
	int indexOfLoadCurrent = voltageNum * 2;
	int indexOfCirCurrentNum = voltageNum * 3;

	//totalRows x totalRows
	SparseMatrixRealBuilder smbB = SparseMatrixRealBuilder(totalRows);

	for (size_t i = voltageNum; i < voltageNum*2; i++)
	{
		vector<BusData> gens = grid->getGens(i - voltageNum + 1);

		if (gens.size() == 0) {
			continue;
		}

		float Lg = LG / gens.size();

		int indexOfI = i;

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
	float dt = 0.00001;
	float t = 0;
	float endTime = 0.2;

	grid->createSubGraph();

	int voltageNum = grid->superNodeToNode.size() - 1;
	int currentFromGndNum = voltageNum * 2;
	int cirCurrentNum = grid->cs.size();
	int totalRows = voltageNum + currentFromGndNum + cirCurrentNum;
	vector<float> capToGnd = vector<float>(totalRows);

	SparseMatrixRealBuilder bA = builderA(grid, cbId, startingVoltages, oldNodeToSuperNode, newNodeToSuperNode, capToGnd);
	SparseMatrixReal A1 = bA.build();
	addATilder(bA, grid, grid->cbs[cbId].getFNode(), grid->cbs[cbId].getTNode(), capToGnd, newNodeToSuperNode);
	SparseMatrixReal A2 = bA.build();
	SparseMatrixReal B = buildB(grid, cbId, startingVoltages, oldNodeToSuperNode, newNodeToSuperNode);



	cout << "A1\n";
	A1.logSquareFull();
	cout << "A2\n";
	A2.logSquareFull();
	cout << "B\n";
	B.logSquareFull();

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

		//cout << t << ", " << sinf((2 * PI) * t * 50) << "   [" << vec[0] <<", " << vec[1] << ", " << vec[2] << "]" << "\n";

		int i = 0;
		while (i < currentFromGndNum + voltageNum) {
			sint[i] = sinf((2 * PI)*t*50);
			i++;
		}

		if (t <0) {
			vec = step(A1, B, vec, sint, dt);
		}
		else {
			vec = step(A2, B, vec, sint, dt);
		}

	
		t += dt;
	}
}


