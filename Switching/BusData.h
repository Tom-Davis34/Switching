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

	complex load;
	complex gen;
	complex pq;

	bool isPV;
	int type;

	real voltage;
	real lineCharge;
	real cost;

	__host__ __device__ static __inline__ BusData makeBusData(complex load, complex gen, complex genMin, complex genMax, real voltage, real cost, int type, int node) {
		BusData res = BusData();

		res.isPV = type == 2;

		res.type = type;
		res.node = node;

		res.load = complex(load)/voltage;
		res.gen = complex(gen)/voltage;
		res.voltage = real(voltage);
		res.cost = real(cost);

		res.pq = res.gen - res.load;

		return res;
	}

	friend std::ostream& operator<<(std::ostream& os, const BusData& cb);
};
