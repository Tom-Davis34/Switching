#pragma once

#include "Grid.h"
#include "common.h"
#include "SparseMatrix.h"


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

SparseMatrixReal buildMatrix(PowerGrid* grid, int cbId, vector<PowerFlowNode> startingVoltages) {
	grid->createSubGraph();

	int voltageNum = grid->superNodeToNode.size();
	int currentFromGndNum = voltageNum - 1;
	int cirCurrentNum = grid->cs.size();
	int totalRows = voltageNum + currentFromGndNum + cirCurrentNum;
	int genNum = grid->getGenNum();

	//totalRows x totalRows
	SparseMatrixRealBuilder smbA = SparseMatrixRealBuilder(totalRows);
	//totalRows x totalRows
	SparseMatrixRealBuilder smbB = SparseMatrixRealBuilder(totalRows);
	vector<float> gens(totalRows)





	return smbA.build();

}

//vector<float> buildX(PowerGrid* grid, int cbId) {
//
//}

void rungeKutta4(PowerGrid* grid, int cbId, vector<PowerFlowNode> startingVoltages) {
	SparseMatrixReal matrix = buildMatrix(grid, cbId, startingVoltages);


}


