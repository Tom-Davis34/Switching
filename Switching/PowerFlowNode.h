#pragma once

#include "common.h"

class PowerFlowNode {
public:
	int node;
	complex pq;
	bool isPV;
	bool isLoad;
	real systemVoltage;
	complex unitVoltage;
};
