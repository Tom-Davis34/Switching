#pragma once

#include <string>
#include <iostream>
#include <vector>
#include "common.h"
#include "BusData.h"
#include "Edge.h"

using namespace std;

class Circuit : public Edge {
public:
	int fbus;
	int tbus;

	complex admittance;
	real lineCharge;

	real transformerRatio;

	int num;

	const int getFNode() const {
		return fbus;
	}

	const int getTNode() const {
		return tbus;
	}

	const int getNum() const {
		return num;
	}

	EdgeType const type() const {
		return Cir;
	}

	static Circuit parseCircuit(vector<string> line, int num) {
		auto circuit = Circuit();

		circuit.fbus = stoi(line[0]);
		circuit.tbus = stoi(line[1]);

		auto Z = Complex(stof(line[2]), stof(line[3]));
		circuit.admittance = Complex(1.0f) / Complex(stof(line[2]), stof(line[3]));
		circuit.lineCharge = stof(line[4]);

		circuit.transformerRatio = stof(line[8]);

		circuit.num = num;

		return circuit;
	}

	static vector<Circuit> parseCir(vector<vector<string>> cells) {
		vector<Circuit> cirs = vector<Circuit>();

		for (size_t i = 0; i < cells.size(); i++)
		{
			cirs.push_back(parseCircuit(cells[i], i));
		}

		return cirs;
	}

	friend ostream& operator<<(ostream& os, const Circuit& cb);
};