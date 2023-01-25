#pragma once

#include "PowerFlowNode.h"
#include "common.h"
#include "SparseMatrix.h"
#include <iostream>
#include <vector>
#include <chrono>

void powerFlowRunner(PowerFlowNode* h_powerFlowNode, SparseMatrixComplex h_admMat, cmplx* h_voltage, int nodeNum, int slackBus);


