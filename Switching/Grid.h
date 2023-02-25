#pragma once
#include "CircuitBreaker.h"
#include "Circuit.h"
#include "Disconnector.h"
#include "Edge.h"
#include "FileReader.h"
#include "SparseMatrix.h"
#include "SparseMatrixBuilder.h"
#include "PowerFlowNode.h"
#include "PowerFlowRunner.h"
#include "ObjectiveResult.h"

#include "DeltaU.h"
#include <vector>
#include <map>
#include <stack>
#include <sstream>
#include <unordered_set>
#include <functional>

class PowerGrid {
public:
	vector<CircuitBreaker> cbs;
	vector<Disconnector> ds;
	vector<Circuit> cs;
	vector<BusData> bds;

	vector<int> u;
	vector<int> targetU;

	int numNodes;

	map<int, vector<Edge*>> nodesToEdges;
	map<int, vector<int>> nodesToEdgeNum;

	vector<Edge*> edges;

	vector<int> nodeToSuperNode;
	vector<vector<int>> superNodeToNode;

	vector<cmplx> superVoltage;
	vector<cmplx> voltage;

	vector<int>* connectivity;

	PowerGrid(vector<CircuitBreaker> cbs,
		vector<Disconnector> ds,
		vector<Circuit> cs,
		vector<BusData> bds) {
		this->cbs = cbs;
		this->ds = ds;
		this->cs = cs;
		this->bds = bds;

		this->numNodes = bds.size();
		this->voltage = vector<cmplx>(bds.size());

		connectivity = new vector<int>();
	}

	void init() {
		initEdges();
		initNodesToEdges();
	}

	void addTargetU(DeltaU du) {
		targetU[du.index] = du.newU;
	}

	void applyDeltaU(DeltaU deltaU) {
		if (deltaU.index >= 0) {
			u[deltaU.index] = deltaU.newU;
		}
	}

	void resetTargetU() {
		for (int i = 0; i < cbs.size(); i++) {
			targetU[i] = cbs[i].isOpen ? OPEN : CLOSED;
		}

		for (int i = cbs.size(); i < cbs.size() + ds.size(); i++) {
			targetU[i] = ds[i].isOpen ? OPEN : CLOSED;
		}
	}

	void resetU() {
		for (int i = 0; i < cbs.size(); i++) {
			u[i] = cbs[i].isOpen ? OPEN : CLOSED;
		}

		for (int i = 0; i < ds.size(); i++) {
			u[cbs.size() + i] = ds[i].isOpen ? OPEN : CLOSED;
		}
	}

	float h(float scale) {
		int diff = 0;

		for (int i = 0; i < u.size(); i++) {
			diff += u[i] * targetU[i];
		}

		return diff*scale;
	}

	void initEdges() {
		for (int i = 0; i < cbs.size(); i++) {
				edges.push_back(&(cbs[i]));
				u.push_back(cbs[i].isOpen?OPEN:CLOSED);
				targetU.push_back(1);
		}

		for (int i = 0; i < ds.size(); i++) {
				edges.push_back(&(ds[i]));
				u.push_back(ds[i].isOpen ? OPEN : CLOSED);
				targetU.push_back(1);
		}

		for (int i = 0; i < cs.size(); i++) {
				edges.push_back(&(cs[i]));
		}
	}

	vector<Edge*> getEdgesSlow(int node) {
		vector<Edge*> retVal = vector<Edge*>();

		for (int i = 0; i < edges.size(); i++) {
			if (edges[i]->getFNode() == node || edges[i]->getTNode() == node) {
				retVal.push_back(edges[i]);
			}
		}

		return retVal;
	}

	void initNodesToEdges() {
		for (int i = 0; i < bds.size(); i++) {
			nodesToEdges[i] = getEdgesSlow(i);
		}
	}

	int disconnectorEgdeInt(int disNum) {
		return cbs.size() + disNum;
	}

	bool isClosedSwitch(Edge* &edge) {
		if (edge->type() == CB) {
			CircuitBreaker* cb = (CircuitBreaker*)edge;
			return u[cb->getNum()] == CLOSED;
		}
		else if (edge->type() == Dis) {
			Disconnector* d = (Disconnector*)edge;
			return u[disconnectorEgdeInt(d->getNum())] == CLOSED;
		}
		else {
			return false;
		}
	}

	bool isOpenSwitch(Edge*& edge) {
		if (edge->type() == CB) {
			CircuitBreaker* cb = (CircuitBreaker*)edge;
			return u[cb->getNum()] == OPEN;
		}
		else if (edge->type() == Dis) {
			Disconnector* d = (Disconnector*)edge;
			return u[disconnectorEgdeInt(d->getNum())] == OPEN;
		}
		else {
			return false;
		}
	}

	bool isSwitch(Edge*& edge) {
		if (edge->type() == CB || edge->type() == Dis) {
			return true;
		} else {
			return false;
		}
	}

	bool connectedToTwoSwtiches(int node) {
		auto connectedEdges = nodesToEdges[node];
		if (connectedEdges.size() != 2) {
			return false;
		}

		return isSwitch(connectedEdges[0]) && isSwitch(connectedEdges[1]);
	}

	bool connectedToTwoEdges(int node) {
		auto connectedEdges = nodesToEdges[node];
		return connectedEdges.size() == 2;
	}

	bool noOp(int node) {
		return true;
	}

bool contains(unordered_set<int>& set, const int& node) {
	return !(set.find(node) == set.end());
}

vector<int> plagueAlgo(int const& startNode, unordered_set<int>& alreadyInfected) {
	if (contains(alreadyInfected, startNode)) {
		return vector<int>();
	}

	stack<int> stk = stack<int>();
	vector<int> infectedNodes = vector<int>();
	stk.push(startNode);

	int currentNode;
	while (!stk.empty()) {
		currentNode = stk.top();
		stk.pop();

		if (contains(alreadyInfected, currentNode)) {
			continue;
		}

		infectedNodes.push_back(currentNode);
		alreadyInfected.insert(currentNode);

		vector<Edge*> edgesForCurrentNode = nodesToEdges[currentNode];
		for (int i = 0; i < edgesForCurrentNode.size(); i++) {
			if (isClosedSwitch(edgesForCurrentNode[i])) {
				assert(
					edgesForCurrentNode[i]->getFNode() == currentNode ||
					edgesForCurrentNode[i]->getTNode() == currentNode
				);

				int otherNode;
				if (edgesForCurrentNode[i]->getFNode() == currentNode) {
					otherNode = edgesForCurrentNode[i]->getTNode();
				}
				else {
					otherNode = edgesForCurrentNode[i]->getFNode();
				}



				if (noOp(otherNode) && !contains(alreadyInfected, otherNode)) {
					//infect other node
					stk.push(otherNode);
				}
			}
		}
	}

	return infectedNodes;
}

bool loadOrGen(int node) {
	return !isZero(bds[node].gen + bds[node].load);
}

vector<int> deepClone(vector<int> vec) {
	vector<int> newVec = vector<int>();

	for (int i : vec) {
		newVec.push_back(i);
	}

	return newVec;
}

//return a list of targetU
vector<DeltaU> getOutage(int const& outageEdge) {
	int node1 = edges[outageEdge]->getFNode();
	int node2 = edges[outageEdge]->getTNode();

	if (loadOrGen(node1) || loadOrGen(node2)) {
		cout << "No outage for " << outageEdge;
		return vector<DeltaU>();
	}

	unordered_set<int> alreadyInfected = unordered_set<int>();

	stack<int> stk = stack<int>();
	stk.push(node1);
	stk.push(node2);

	int currentNode;
	while (!stk.empty()) {
		currentNode = stk.top();
		stk.pop();

		if (contains(alreadyInfected, currentNode)) {
			continue;
		}

		alreadyInfected.insert(currentNode);

		vector<Edge*> edgesForCurrentNode = nodesToEdges[currentNode];
		for (int i = 0; i < edgesForCurrentNode.size(); i++) {
			//if (isSwitch(edgesForCurrentNode[i])) {
			//	continue;
			//}

			int otherNode;
			if (edgesForCurrentNode[i]->getFNode() == currentNode) {
				otherNode = edgesForCurrentNode[i]->getTNode();
			}
			else {
				otherNode = edgesForCurrentNode[i]->getFNode();
			}

			if (!isZero(bds[otherNode].pq)) {
				cout << "No outage for " << outageEdge;
				return vector<DeltaU>();
			}

			if (!isSwitch(edgesForCurrentNode[i]) || connectedToTwoEdges(otherNode)) {
				stk.push(otherNode);
			}

		}
	}

	vector<DeltaU> targetU = vector<DeltaU>();
	for (int i = 0; i < cbs.size() + ds.size(); i++) {
		auto edge = edges[i];
		int numOfNodesInfectedForEdge = 0;
		if (contains(alreadyInfected, edge->getFNode())) {
			numOfNodesInfectedForEdge++;
		}
		if (contains(alreadyInfected, edge->getTNode())) {
			numOfNodesInfectedForEdge++;
		}

		if (numOfNodesInfectedForEdge == 2) {
			cout << *edge << " target U: " << 0 <<"\n";
			targetU.push_back(DeltaU(i, 0));
		}
		else if (numOfNodesInfectedForEdge == 1) {
			cout << *edge << " target U: OPEN" << "\n";
			targetU.push_back(DeltaU(i, OPEN));
		}
	}


	return targetU;
}

	bool nodeIsDead(int const& startNode) {
		if (!isZero(bds[startNode].pq)) {
			return false;
		}

		unordered_set<int> alreadyInfected = unordered_set<int>();

		stack<int> stk = stack<int>();
		stk.push(startNode);

		int currentNode;
		while (!stk.empty()) {
			currentNode = stk.top();
			stk.pop();

			if (contains(alreadyInfected, currentNode)) {
				continue;
			}

			alreadyInfected.insert(currentNode);

			vector<Edge*> edgesForCurrentNode = nodesToEdges[currentNode];
			for (int i = 0; i < edgesForCurrentNode.size(); i++) {
				if (isOpenSwitch(edgesForCurrentNode[i])) {
					continue;
				}

				int otherNode;
				if (edgesForCurrentNode[i]->getFNode() == currentNode) {
					otherNode = edgesForCurrentNode[i]->getTNode();
				} 
				else {
					otherNode = edgesForCurrentNode[i]->getFNode();
				}

				if (!isZero(bds[otherNode].pq)) {
					return false;
				}

				//infect other node
				stk.push(otherNode);
			}
		}

		return true;
	}

	void createSubGraph() {
		nodeToSuperNode = vector<int>();
		for(size_t i = 0; i < bds.size(); i++)
		{
			nodeToSuperNode.push_back(0);
		}
		superNodeToNode = vector<vector<int>>();
		unordered_set<int> mappedNodes = unordered_set<int>();

		for (int i = 0; i < bds.size(); i++) {
			vector<int> nodesWithSameVoltage = plagueAlgo(i, mappedNodes);

			if (nodesWithSameVoltage.size() == 0) {
				continue;
			}

			cout << "SuperNode " << superNodeToNode.size() << ": [";

			superNodeToNode.push_back(nodesWithSameVoltage);
			for (int j = 0; j < nodesWithSameVoltage.size(); j++) {
				nodeToSuperNode[nodesWithSameVoltage[j]] = superNodeToNode.size() - 1;
				cout << nodesWithSameVoltage[j];
				if (j < nodesWithSameVoltage.size() - 1) {
					cout << ", ";
				}
			}
			cout << "]\n";
		}
		cout << "\n";
	}

	const vector<PowerFlowNode> mergeBusData() {
		vector<PowerFlowNode> retVal = vector<PowerFlowNode>();

		for (size_t i = 0; i < superNodeToNode.size(); i++)
		{
			PowerFlowNode pfNode = PowerFlowNode();

			pfNode.node = i;
			pfNode.pq = cmplx(0);
			pfNode.isPV = false;
			pfNode.systemVoltage = 1;

			for (size_t j = 0; j < superNodeToNode[i].size(); j++)
			{
				pfNode.pq += bds[superNodeToNode[i][j]].pq;

				pfNode.systemVoltage = max(bds[superNodeToNode[i][j]].voltage, pfNode.systemVoltage);
				if (bds[superNodeToNode[i][j]].isPV) {
					pfNode.isPV = true;
				}
			}

			retVal.push_back(pfNode);
		}

		cout << "\nPowerFlowNodes\n";
		for (size_t i = 0; i < retVal.size(); i++)
		{
			cout << "[" << retVal[i].node << ", " << retVal[i].isPV << ", (" << retVal[i].pq.real() << ", " << retVal[i].pq.imag() << "), " << retVal[i].systemVoltage << "]\n";
		}
		cout << "\n";
		return retVal;
	}

	const vector<PowerFlowNode> runPowerFlow() {
		cout << "\033[1;33m";

		createSubGraph();

		vector<PowerFlowNode> pfNodes = mergeBusData();
		superVoltage = vector<cmplx>(pfNodes.size());

		SparseMatrixComplexBuilder smb = SparseMatrixComplexBuilder(superNodeToNode.size());
		smb.addCircuits(cs, nodeToSuperNode);

		SparseMatrixComplex sm = smb.build();

		int slackBus = 0;
		for (size_t i = 0; i < bds.size(); i++)
		{
			if (bds[i].type == 3) {
				slackBus = nodeToSuperNode[i];
			}
		}

		cout << "\nSlackBus is: " << slackBus << "\n";

		powerFlowRunner(&pfNodes[0], sm, &superVoltage[0], pfNodes.size(), slackBus);

		cout << "\nVoltages are:\n";
		for (size_t i = 0; i < voltage.size(); i++)
		{
			voltage[i] = superVoltage[nodeToSuperNode[i]];
			cout << "V [" << i << "]: " << voltage[i].real() << " + " << voltage[i].imag() << "j\n";
		}

		cout << "\033[0m\n";
		return pfNodes;
	}

	const int getGenNum() const {
		int genNum = 0;

		for (auto nodeList : superNodeToNode)
		{
			bool containsGen = false;

			for  (int node : nodeList)
			{
				if (!isZero(bds[node].gen)) {
					containsGen = true;
				}
			}

			genNum++;
		}

		return genNum;
	}

	static PowerGrid* buildGridFromFiles() {
		PowerGrid* grid = new PowerGrid(readCircuitBreakers(), readDisconnectors(), readCircuits(), readBusData());

		return grid;
	}

	const vector<BusData> getGens(int superNodeId) const {
		vector<BusData> gens = vector<BusData>();


		for (auto nodeList : superNodeToNode[superNodeId])
		{
			if (!isZero(bds[nodeList].gen)) {
				gens.push_back(bds[nodeList]);
			}
		}

		return gens;
	}

	const cmplx getTotalAdmittance(int superNodeId, vector<PowerFlowNode> startingVoltages, vector<int> nodeToSuperNode) const {
		cmplx admittance = 0;

		for (auto nodeId : superNodeToNode[superNodeId])
		{
			auto v = abs(startingVoltages[nodeToSuperNode[nodeId]].unitVoltage);
			if (v > 0 && bds[nodeId].type == 1) {
				admittance += bds[nodeId].load / abs(v * v);
			}
		}

		return admittance;
	}

	friend ostream& operator<<(ostream& os, const PowerGrid& grid);
};


