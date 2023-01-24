#pragma once

#include <vector>
#include <ostream>
#include "Control.h";
#include "Grid.h";
#include <deque>
#include <iostream>
#include <list>
#include <unordered_map>
#include <queue>
#include <utility>

using namespace std;



//class OperatingSequence {
//public:
//	PowerGrid* grid;
//	vector<DeltaU> sequence = vector<DeltaU>();
//	vector<vector<Complex>> voltages = vector<vector<Complex>>();
//	vector<ObjectiveResult> steadyStateObjective = vector<ObjectiveResult>();
//	vector<ObjectiveResult> transientObjective = vector<ObjectiveResult>();
//
//	void addControl(DeltaU deltaU) {
//		sequence.push_back(deltaU);
//		voltages.push_back(vector<Complex>(grid->bds.size()));
//		steadyStateObjective.push_back(ObjectiveResult());
//		transientObjective.push_back(ObjectiveResult());
//	}
//
//	OperatingSequence(PowerGrid* grid) {
//		this->grid = grid;
//		runFirstSteadyState();
//	}
//
//	void runFirstSteadyState() {
//		grid->runPowerFlow();
//		steadyStateObjective.push_back(grid->steadyStateObjective());
//	}
//
//	void executeAndComputeObjective() {
//
//	}
//
//
//	void executeAndComputeObjective(int index) {
//		Control* con = sequence[index];
//		assert(con->isValid());
//		con->execute();
//
//		grid->runPowerFlow();
//
//		steadyStateObjective[index + 1] = grid->steadyStateObjective();
//		for (size_t i = 0; i < grid->numNodes; i++)
//		{
//			voltages[index][i] = grid->voltage[i];
//		}
//
//		transientObjective[index] = computeTransientObjective();
//	}
//
//	ObjectiveResult computeTransientObjective() {
//		return ObjectiveResult();
//	}
//
//	void apply() {
//		for each (Control* con in sequence)
//		{
//			assert(con->isValid());
//			con->execute();
//		}
//	}
//
//	void reverse(int index) {
//			sequence[index]->reverse();
//	}
//
//	void reverse() {
//		for (int i = sequence.size() - 1; i >=0; i--)
//		{
//			assert(sequence[i]->isValid());
//			sequence[i]->reverse();
//		}
//	}
//
//	void removeLast() {
//		sequence.pop_back();
//	}
//
//	friend ostream& operator<<(ostream& os, const OperatingSequence& con);
//};




