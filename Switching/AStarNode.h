#pragma once

#include "common.h"
#include "Control.h"
#include "DeltaU.h"
#include "Grid.h"
#include "Runge-Kutta.h"
#include <unordered_map>

enum NodeState
{
	Init,
	SteadyStateCalculated,
	TransientCalculated,
	Finished
};

const char* edgeTypeToString[] = {
	stringify(CB),
	stringify(Dis),
	stringify(Cir)
};

string edgeToString(Edge* edge) {
	return edgeTypeToString[edge->type()] + to_string(edge->getNum());
}

class AStarNode {
public:
	NodeState state;
	PowerGrid* grid;
	const AStarNode* parent;
	DeltaU deltaU;
	vector<int> nodeToSuperNode;
	vector<PowerFlowNode> voltages;
	vector<Contribution> hObjective;
	vector<Contribution> otherObjective;
	vector<Contribution> steadyStateObjective;
	vector<Contribution> transientObjective;
	float sum;
	int depth;

	AStarNode() {

	}

	AStarNode(PowerGrid* grid, const AStarNode* parent, int index, int newU) {
		this->state = Init;
		this->grid = grid;
		this->parent = parent;
		this->depth = parent->depth + 1;
		this->deltaU = DeltaU(index, newU);
		this->hObjective = vector<Contribution>();
		this->steadyStateObjective = vector<Contribution>();
		this->transientObjective = vector<Contribution>();
		this->voltages = vector<PowerFlowNode>();
		this->nodeToSuperNode = vector<int>();

		setSum();
	}

	void calculateNextObjective() {
		switch (state)
		{
		case Init:
			cout << "Calculating Steady State...\n";
			applyNode();
			cout << "\033[1;36m";
			cout << (*grid) << "\n";
			cout << "\033[0m\n";
			voltages = grid->runPowerFlow();
			copyNodetoSuperNode();
			checkDisconnectors();
			checkSteadyState();
			state = SteadyStateCalculated;
			setSum();
			break;
		case SteadyStateCalculated:
			if (deltaU.index < grid->cbs.size() && deltaU.index >= 0) {
				rungeKutta4(grid, deltaU.index, voltages, nodeToSuperNode, parent->nodeToSuperNode);
			}
			state = TransientCalculated;
			setSum();
		case TransientCalculated:
			state = Finished;
		default:
			break;
		};
	}

	void copyNodetoSuperNode() {
		for(int i : grid->nodeToSuperNode) {
			nodeToSuperNode.push_back(i);
		}
	}

	vector<const AStarNode*> getOs() {
		vector<const AStarNode*> os = vector<const AStarNode*>(depth + 1);

		const AStarNode* current = this;
		while (current != nullptr) {
			os[current->depth] = current;
			current = current->parent;
		}

		return os;
	}

	void logOS() {
		vector<const AStarNode*> os = getOs();

		for (auto nodePtr : os) {
			nodePtr->printWithObjective();
		}

		cout << "\n";
	}

	void checkSteadyState() {
		for (int i = 0; i < voltages.size(); i++)
		{
			auto pfn = voltages[i];
			if (!isZero(pfn.pq) && isZero(pfn.unitVoltage)) {
				addSteadyStateObjective(Contribution("Zero voltage on node " + i, 10000));
			}
			else if (abs(pfn.unitVoltage) < 0.95) {
				addSteadyStateObjective(Contribution("Low voltage ", (0.95 - abs(pfn.unitVoltage)) * 10000));
			}
		}
	}

	void checkDisconnectors() {
		if (this->deltaU.index == -1) {
			return;
		}

		if (grid->edges[this->deltaU.index]->type() == Dis) {
			int fNode = grid->edges[this->deltaU.index]->getFNode();
			int tNode = grid->edges[this->deltaU.index]->getTNode();

			bool sameSuperNode = nodeToSuperNode[fNode] == nodeToSuperNode[tNode];

			bool tNodeDead = grid->nodeIsDead(tNode);
			bool fNodeDead = grid->nodeIsDead(fNode);

			if (sameSuperNode) {
				cout << "Same super node\n";
			}
			else if(tNodeDead || fNodeDead)
			{
				cout << "Dead node\n";
			}
			else {
				addOtherObjective(Contribution("Cant open disconnector", 10000));
			}
		}
	}

	void applyNode() {
		grid->resetU();

		vector<const AStarNode*> os = vector<const AStarNode*>(depth + 1);

		const AStarNode* current = this;
		while (current != nullptr) {
			os[current->depth] = current;
			current = current->parent;
		}
		
		for (auto node : os) {
			grid->applyDeltaU(node->deltaU);
		}
	}

	AStarNode* getChild(DeltaU du) {
		applyNode();

		AStarNode* retVal = new AStarNode(grid, this, du.index, du.newU);

		retVal->applyNode();
		retVal->setHammingDistance();

		return retVal;
	}

	AStarNode** getChildren() {
		applyNode();

		AStarNode** retVal = new AStarNode*[grid->u.size()];

		for (int i = 0; i < grid->u.size(); i++) {
			AStarNode* n = new AStarNode(grid, this, i, DeltaU::negate(grid->u[i]));
			retVal[i] = n;
		}

		for (int i = 0; i < grid->u.size(); i++) {
			retVal[i]->applyNode();
			retVal[i]->setHammingDistance();
		}

		return retVal;
	}

	void setHammingDistance() {
		for (int i = 0; i < grid->u.size(); i++) {
			if (grid->targetU[i] != grid->u[i]) {
				addH(Contribution(
					edgeToString(grid->edges[i]),
					STEP_OBJECTIVE * abs(grid->targetU[i] * grid->u[i])
				));
			}
		}
	}

	void addH(Contribution newContribution) {
		hObjective.push_back(newContribution);
		sum += newContribution.amount;
	}

	void addOtherObjective(Contribution newContribution) {
		otherObjective.push_back(newContribution);
		sum += newContribution.amount;
	}

	void addSteadyStateObjective(Contribution newContribution) {
		steadyStateObjective.push_back(newContribution);
		sum += newContribution.amount;
	}

	void setTransientObjective(Contribution& newContribution) {
		transientObjective.push_back(newContribution);
		sum += newContribution.amount;
	}

	const AStarNode* getNodeAtDepth(int targetDepth) {
		const AStarNode* current = this;

		while (current->depth > targetDepth)
		{
			current = current->parent;
		}

		return current;
	}

	void printWithObjective() const {
		if (this->deltaU.index != -1) {
			cout << *(grid->edges[this->deltaU.index]) << this->deltaU;
		}
		else {
			cout << "START";
		}
		cout << "\n\tH: " << getHObjective();
		logContribution(hObjective);
		cout << "\n\tOther: " << ((state >= SteadyStateCalculated) ? std::to_string(getOtherObjective()) : "NA");
		logContribution(otherObjective);
		cout << "\n\tSteadyState: " << ((state >= SteadyStateCalculated) ? std::to_string(getSteadyStateObjective()) : "NA");
		logContribution(steadyStateObjective);
		if (this->deltaU.index != -1){
			cout << "\n\tTransient: " << ((state >= TransientCalculated) ? std::to_string(getTransientObjective()) : "NA");
			logContribution(transientObjective);
		}
		cout << "\n\tSum: " << sum << "\n";
	}

	void logContribution(vector<Contribution> cons) const {
		for (auto con : cons) {
			cout << "\n\t\t" << con.reason << ": " << con.amount;
		}
	}

	void print() const {
		if (this->deltaU.index == -1) {
			cout << "START\n";
		}
		else {
			cout << *(grid->edges[this->deltaU.index]) << this->deltaU << "\n";
		}
	}

	static AStarNode* createRoot(PowerGrid* grid) {
		auto root = new AStarNode();

		root->parent = nullptr;
		root->deltaU = DeltaU();
		root->hObjective = vector<Contribution>();
		root->otherObjective = vector<Contribution>();
		root->steadyStateObjective = vector<Contribution>();
		root->transientObjective = vector<Contribution>();
		root->sum = 0;
		root->depth = 0;
		root->grid = grid;
		root->state = Init;

		root->applyNode();
		root->setHammingDistance();

		return root;
	}

	bool operator > (const AStarNode& rhs) const {
		return (*this).sum < rhs.sum;
	}

	bool operator < (const AStarNode& rhs) const {
		return (*this).sum > rhs.sum;
	}

	float getHObjective() const {
		float sum = 0;

		for (auto con : hObjective) {
			sum += con.amount;
		}

		return sum;
	}

	float getOtherObjective() const {
		float sum = 0;

		for (auto con : otherObjective) {
			sum += con.amount;
		}

		return sum;
	}

	float getSteadyStateObjective() const {
		float sum = 0;

		for (auto con : steadyStateObjective) {
			sum += con.amount;
		}

		return sum;
	}

	float getTransientObjective() const {
		float sum = 0;

		for (auto con : transientObjective) {
			sum += con.amount;
		}

		return sum;
	}

	void setSum() {
		sum = STEP_OBJECTIVE * depth;
		sum += getHObjective();

		unordered_map<string, float> maxContri = unordered_map<string, float>();

		vector<const AStarNode*> os = getOs();

		for (auto ss : os) {
			for (auto oo : ss->otherObjective) {
				if (maxContri.find(oo.reason) != maxContri.end()) {
					float currentMax = maxContri.find(oo.reason)->second;
					maxContri[oo.reason] = max(oo.amount, currentMax);
				}
				else {
					maxContri[oo.reason] = oo.amount;
				}
			}
			for (auto oo : ss->steadyStateObjective) {
				if (maxContri.find(oo.reason) != maxContri.end()) {
					float currentMax = maxContri.find(oo.reason)->second;
					maxContri[oo.reason] = max(oo.amount, currentMax);
				}
				else {
					maxContri[oo.reason] = oo.amount;
				}
			}
			for (auto oo : ss->transientObjective) {
				if (maxContri.find(oo.reason) != maxContri.end()) {
					float currentMax = maxContri.find(oo.reason)->second;
					maxContri[oo.reason] = max(oo.amount, currentMax);
				}
				else {
					maxContri[oo.reason] = oo.amount;
				}
			}
		}
	}
};
