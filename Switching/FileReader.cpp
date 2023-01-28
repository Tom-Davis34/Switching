#include "FileReader.h"
#include <regex>

//string workingDir = "C:\\Users\\Tom\\Desktop\\Grids\\BRB\\";
string workingDir = "C:\\Users\\davist\\source\\repos\\Switching-master\\Switching-master\\Grids\\BRB";
//string workingDir = "/home/davist/CLionProjects/Switching/Grids/BRB/";

regex whiteSpace = regex("\\s+");

vector<vector<string>> readGridFile(string fileDir) {
	fstream myFile;

	myFile.open(fileDir, ios::in);
	string line;
	getline(myFile, line);
	std::cout << std::flush;
	cout << fileDir << std::endl;
	cout << line << std::endl;

	vector<vector<string>> cells = vector<vector<string>>();

	while (getline(myFile, line)) {
		cells.push_back(vector<string>());

		sregex_token_iterator iter = sregex_token_iterator(line.begin(), line.end(), whiteSpace, -1);
		sregex_token_iterator end;

		for (; iter != end; ++iter) {
			cells[cells.size() - 1].push_back(string(*iter));
		}
	}

	return cells;
}
vector<BusData> initialiseBusData(vector<FileGen> gen, vector<FileBus> buses) {
	assert(gen.size() == buses.size());

	vector<BusData> abomData = vector<BusData>();
	abomData.push_back(BusData::makeBusData(cmplx(0), cmplx(0), cmplx(0), cmplx(0), 1, 0, 1, 0));


	for (size_t i = 0; i < gen.size(); i++)
	{
		auto node = BusData::makeBusData(cmplx(buses[i].Pd, buses[i].Qd), gen[i].power, gen[i].minPower, gen[i].maxPower, buses[i].baseKV, gen[i].cost, buses[i].type, i + 1);

		abomData.push_back(node);
	}

	return abomData;
}

vector<CircuitBreaker> readCircuitBreakers() {
	vector<vector<string>> cells = readGridFile(workingDir + "CircuitBreaker.txt");

	return CircuitBreaker::parseCircuitBreakerFile(cells);
};

vector<Disconnector> readDisconnectors() {
	vector<vector<string>> cells = readGridFile(workingDir + "Disconnector.txt");

	return Disconnector::parseDisconnectorFile(cells);
};

vector<BusData> readBusData() {

	cout << "About to start Reading Buses..." << endl;

	vector<vector<string>> buses = readGridFile(workingDir + "Buses.txt");
	vector<vector<string>> gens = readGridFile(workingDir + "Gens.txt");

	vector<FileBus> parsedBuses = FileBus::parseBusFile(buses);
	vector<FileGen> parsedGens = FileGen::parseGenFiles(gens, buses.size());

	return initialiseBusData(parsedGens, parsedBuses);
};

vector<Circuit> readCircuits() {
	vector<vector<string>> cir = readGridFile(workingDir + "Circuits.txt");

	return Circuit::parseCir(cir);
};
