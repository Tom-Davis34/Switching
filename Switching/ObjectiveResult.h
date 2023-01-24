#pragma once

#include "common.h"
#include <string>
#include <vector>

using namespace std;

enum ConType {
	V, Steps, 
};

class Contribution {
public:
	string reason;
	real amount;
	
	Contribution(string reason, real amount) {
		this->reason = reason;
		this->amount = amount;
	}

	friend ostream& operator<<(ostream& os, const Contribution& con);
};

class ObjectiveResult {
public:
	vector<Contribution> contributions = vector<Contribution>();
	vector<Contribution> h = vector<Contribution>();
	
	void addContribution(Contribution contribution) {
		contributions.push_back(contribution);
	}

	friend ostream& operator<<(ostream& os, const ObjectiveResult& cb);
};