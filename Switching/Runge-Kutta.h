#pragma once

#include "Grid.h"
#include "common.h"
#include "SparseMatrix.h"


real ρ = 28;
real σ = 10;
real β = 8 / 3;
//x1` = σ(x2 - x1)
//x2` = x1(ρ - x3) - x2
//x3` = x1*x2 - β*x3
vector<real> f(vector<real> x) {
	return vector<real> {
		σ*(x[1] - x[0]),
			x[0] * (ρ - x[2]) - x[1],
			x[0]* x[1] - β * x[2]
	};
}

vector<real> rungeKuttaStep(vector<real> x, real dt) {
	auto f1 = f(x);
	auto f2 = f(f1 * (dt / 2) + x);
	auto f3 = f(f2 * (dt / 2) + x);
	auto f4 = f(f3 * dt + x);
	
	return x + (dt / 6) * (f1 + 2 * f2 + 2 * f3 + f4);
}

vector<real> buildX() {
	return  vector<real> {
		-8,
		8,
		27
	};
}

void rungeKutta4Lorentz() {
	real startTime = 0;
	real endTime = 16;
	real dt = 0.01;
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



SparseMatrixReal buildMatrix(PowerGrid* grid, int cbId, vector<PowerFlowNode> startingVoltages) {
	grid->createSubGraph();

	int voltageNum = grid->superNodeToNode.size();
	int currentFromGndNum = voltageNum - 1;
	int cirCurrentNum = grid->cs.size();
	int totalRows = voltageNum + currentFromGndNum + cirCurrentNum;
	int genNum = grid->getGenNum();

	//totalRows x totalRows
	SparseMatrixRealBuilder smbA = SparseMatrixRealBuilder(totalRows);
	//totalRows x genNum
	SparseMatrixRealBuilder smbB = SparseMatrixRealBuilder(totalRows);




}

vector<real> buildX(PowerGrid* grid, int cbId) {

}

void rungeKutta4(PowerGrid* grid, int cbId, vector<PowerFlowNode> startingVoltages) {
	SparseMatrixReal matrix = buildMatrix(grid, cbId, startingVoltages);


}


