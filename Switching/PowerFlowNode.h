#pragma once

#include "common.h"

class PowerFlowNode {
public:
	int node;
	cmplx pq;
	bool isPV;
	bool isLoad;
	float systemVoltage;
	cmplx unitVoltage;
};
