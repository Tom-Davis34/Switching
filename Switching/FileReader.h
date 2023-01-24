#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include <cassert>
#include "FileBus.h"
#include "FileGen.h"
#include "Circuit.h"
#include "BusData.h"
#include "SparseMatrix.h"
#include "SparseMatrixBuilder.h"

vector<vector<string>> readGridFile(string fileDir);

vector<BusData> initialiseBusData(vector<FileGen> gen, vector<FileBus> buses);

vector<CircuitBreaker> readCircuitBreakers();

vector<Disconnector> readDisconnectors();

vector<BusData> readBusData();

vector<Circuit> readCircuits();
