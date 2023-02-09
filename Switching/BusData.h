#pragma once
#include "common.h"
#include <string>
#include <iostream>
#include <vector>

enum BusType {
	GND, PQ, PV, Sk 
};

class BusData {
public:
	int node;

	cmplx load;
	cmplx gen;
	cmplx pq;

	bool isPV;
	int type;

	float voltage;
	float lineCharge;
	float cost;

	static  BusData makeBusData(cmplx load, cmplx gen, cmplx genMin, cmplx genMax, float voltage, float cost, int type, int node) {
		BusData res = BusData();

		res.isPV = type == 2;

		res.type = type;
		res.node = node;

		res.load = load/voltage;
		res.gen = gen/voltage;
		res.voltage = float(voltage);
		res.cost = float(cost);

		res.pq = res.gen - res.load;

		return res;
	}

	friend std::ostream& operator<<(std::ostream& os, const BusData& cb);
};
