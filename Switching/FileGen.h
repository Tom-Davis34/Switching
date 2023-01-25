#pragma once

#include <string>
#include <iostream>
#include <vector>
#include "common.h"
#include "BusData.h"



class FileGen {
public:
	int busId;

	cmplx power;
	cmplx minPower;
	cmplx maxPower;

	float cost;

	FileGen() {
		busId = 0;

		power = cmplx(0.0f, 0.0f);
		minPower = cmplx(0.0f, 0.0f);
		maxPower = cmplx(0.0f, 0.0f);

		cost = 0.0f;
	}

	static FileGen makeFileGen(int busId) {
		auto gen = FileGen();

		gen.busId = busId;

		return gen;
	}

	static FileGen parseGen(vector<string> lineGen) {
		auto gen = FileGen();
		gen.busId = stoi(lineGen[0]);

		gen.power = cmplx(stof(lineGen[1]), stof(lineGen[2]));
		gen.minPower = cmplx(stof(lineGen[9]), stof(lineGen[4]));
		gen.maxPower = cmplx(stof(lineGen[8]), stof(lineGen[3]));

		return gen;
	}

	static vector<FileGen> parseGenFiles(vector<vector<string>> genCells, int nodeCount) {
		vector<FileGen> gens = vector<FileGen>();

		int j = 0;
		for (size_t i = 0; i < nodeCount; i++)
		{
			if (j < genCells.size() && stoi(genCells[j][0]) == i + 1) {
				gens.push_back(parseGen(genCells[j]));
				j++;
			}
			else {
				gens.push_back(FileGen::makeFileGen(i + 1));
			}
		}

		return gens;
	}
};

