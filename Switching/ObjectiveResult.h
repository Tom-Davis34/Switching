#pragma once

#include "common.h"
#include <string>
#include <vector>



enum ConType {
	V, Steps, 
};

class Contribution {
public:
	string reason;
	float amount;
	
	Contribution(string reason, float amount) {
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