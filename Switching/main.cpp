#include "main.h"

char* edgeTypeString[3] = { "CB", "D", "C" };

inline ostream& operator<<(ostream& os, const Disconnector& cb)
{
	os << "D" << cb.num;
	return os;
}
inline ostream& operator<<(ostream& os, const DisOpen& disOpen)
{
	os << "Open " << *disOpen.d;
	return os;
}
inline ostream& operator<<(ostream& os, const DisClose& disClose)
{
	os << "DisClose " << *disClose.d;
	return os;
}

inline ostream& operator<<(ostream& os, const CircuitBreaker& cb)
{
	os << "CB" << cb.num;
	return os;
}
inline ostream& operator<<(ostream& os, const Open& open)
{
	os << "Open " << *open.cb;
	return os;
}
inline ostream& operator<<(ostream& os, const Close& close)
{
	os << "Close " << *close.cb;
	return os;
}
inline ostream& operator<<(ostream& os, const Circuit& cir){
	os << "Cir";
	return os;
}

inline ostream& operator<<(ostream& os, const Edge& edge) {
	os << edgeTypeString[edge.type()] << edge.getNum();
	return os;
}

inline ostream& operator<<(ostream& os, const DeltaU& deltaU) {
	if (deltaU.newU == -1) {
		os << " Open";
	}
	else if (deltaU.newU == 1) {
		os << " Close";
	}
	else {
		os << " ERROR";
	}
	return os;
}

inline ostream& operator<<(ostream& os, const BusData& bd) {

	string name;
	if (bd.type == 1) {
		name = "PQ";
	}
	else if (bd.type == 2) {
		name = "PV";
	}
	else {
		name = "Sk";
	}

	os << "[" << bd.node << "] " << name;
	return os;
}

//inline ostream& operator<<(ostream& os, const Contribution& con) {
//	os << con.reason << ": " << con.amount << "\n";
//	return os;
//}

inline ostream& operator<<(ostream& os, const ObjectiveResult& or) {
	for (size_t i = 0; i < or.contributions.size(); i++)
	{
		os << or.contributions[i];
	}
	return os;
}

//inline ostream& operator<<(ostream& os, const OperatingSequence& ops) {
//	for(int i=0; i < ops.sequence.size(); i++)
//	{
//		os << "\nOperating Seqeunce\n";
//		os << "\nSteady State Objective:\n";
//		os << ops.steadyStateObjective[i];
//
//		os << ops.sequence[i]->print();
//		os << "\nTransient Objective:\n";
//		os << ops.transientObjective[i];
//	}
//
//	os << "Steady State Objective:\n";
//	os << ops.steadyStateObjective[ops.sequence.size()];
//
//	return os;
//}

inline ostream& operator<<(ostream& os, const PowerGrid& grid)
{

	os << "\033[1;36m" << "< ==== Power Grid ==== >\n\nCircuit Breakers\n";
	for (int i = 0; i < grid.cbs.size(); i++)
	{
		const CircuitBreaker* cb = &(grid.cbs[i]);
		os << *cb << "[" << cb->fNode << ", " << cb->tNode << "]: " << (grid.u[i] == OPEN ? "Open" : "Closed") << "\n";
	}

	os << "\nDisconnectors\n";
	for (int i = 0; i < grid.ds.size(); i++)
	{
		const Disconnector* d = &(grid.ds[i]);
		os << *d << "[" << d->fNode << ", " << d->tNode << "]: " << (grid.u[grid.cbs.size() -1 + i] == OPEN ? "Open" : "Closed") << "\n";
	}

	os << "\nCicuits\n";
	for (int i = 0; i < grid.cs.size(); i++)
	{
		Circuit* cir = (Circuit*)&(grid.cs[i]);
		os << *cir << "[" << cir->fbus << ", " << cir->tbus << "]: " << cir->admittance.realVal() << " + " << cir->admittance.imagVal() << "j\n";
	}

	os << "\nBusData\n";
	for (int i = 0; i < grid.bds.size(); i++)
	{
		BusData* bd = (BusData*)&(grid.bds[i]);
		os << *bd << ": " << bd->pq.realVal() << " + " << bd->pq.imagVal() << "j\n";
	}

	os << "\nNodesToEdges\n";
	for (int i = 0; i < grid.bds.size(); i++)
	{
		auto edges = (grid.nodesToEdges).at(i);

		os << "Node " << i << ": ";
		for (int j = 0; j < edges.size(); j++) {
			os << "" << edgeTypeString[edges[j]->type()] << edges[j]->getNum() << "[" << edges[j]->getFNode() << ", " << edges[j]->getTNode() << "]";

			if (j < edges.size() - 1)
			{
				os << ", ";
			}
		}
		os << "\n";
	}

	os << "\033[0m\n";
	return os;
}


int run1() {
	cout << std::fixed << std::setprecision(2);
	cout << "About to start reading model..." << endl;

	//Read grid model 
	PowerGrid* grid = PowerGrid::buildGridFromFiles();
	grid->init();
	cout << "Finished reading model:\n";
	cout << *(grid);

	grid->addTargetU(0, 0);
	grid->addTargetU(2, -1);
	grid->addTargetU(3, -1);

	auto node = aStar(grid);
	int depth = node->depth;

	for (int i = 0; i <= node->depth; i++) {
		node->getNodeAtDepth(i)->print();
	}

	cout << endl;
	return 0;
}

int runLorentz() {
	rungeKutta4Lorentz();
	return 1;
}

int main() {
	return runLorentz();
}