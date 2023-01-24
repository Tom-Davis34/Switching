#pragma once

#include <string>
#include <iostream>
#include <vector>
#include "common.h"
#include "BusData.h"

using namespace std;

class FileBus {
public:
	int busId;
	int type;
	float  Pd;
	float Qd;
	float Gs;
	float	Bs;
	int area;
	float Vm;
	float Va;
	float baseKV;
	int	zone;
	float	Vmax;
	float	Vmin;


	static FileBus parseBus(vector<string> line) {
		auto bus = FileBus();
		bus.busId = stoi(line[0]);

		bus.type = stoi(line[1]);

		bus.Pd = stof(line[2]);
		bus.Qd = stof(line[3]);
		bus.Gs = stof(line[4]);
		bus.Bs = stof(line[5]);

		bus.baseKV = stof(line[9]);

		bus.Vmax = bus.baseKV * stof(line[11]);
		bus.Vmin = bus.baseKV * stof(line[12]);

		return bus;
	}

	static vector<FileBus> parseBusFile(vector<vector<string>> cells) {
		vector<FileBus> buses = vector<FileBus>();

		for (size_t i = 0; i < cells.size(); i++)
		{
			buses.push_back(parseBus(cells[i]));
		}

		return buses;
	}


};





