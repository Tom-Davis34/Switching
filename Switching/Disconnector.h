#pragma once

#include "common.h"
#include "Control.h"
#include "Edge.h"
#include <string>
#include <iostream>
#include <vector>


class Disconnector : public Edge {
public:
	int fNode;
	int tNode;
	bool isOpen;
	int num;

	Disconnector(int fNode, int tNode, bool isDisOpen, int num) {
		this->fNode = fNode;
		this->tNode = tNode;
		this->isOpen = isDisOpen;
		this->num = num;
	}

	Disconnector(vector<string> lineCb, int num) {
		fNode = stoi(lineCb[0]);
		tNode = stoi(lineCb[1]);
		isOpen = stoi(lineCb[2]) == 1;
		this->num = num;
	}

	const int getFNode() const {
		return fNode;
	}

	const int getTNode() const {
		return tNode;
	}

	const int getNum() const {
		return num;
	}

	const EdgeType type() const {
		return Dis;
	}

	static vector<Disconnector> parseDisconnectorFile(vector<vector<string>> cells) {
		vector<Disconnector> dis = vector<Disconnector>();

		for (size_t i = 0; i < cells.size(); i++)
		{
			dis.push_back(Disconnector(cells[i], i));
		}

		return dis;
	}

	friend ostream& operator<<(ostream& os, const Disconnector& cb);
};

class DisOpen : public Control {
public:
	Disconnector* d;

	bool isValid() {
		return !(d->isOpen);
	}

	void execute() {
		d->isOpen = true;
	}

	void reverse() {
		d->isOpen = false;
	}

	string print() {
		return "Open D" + d->num;
	}

	friend ostream& operator<<(ostream& os, const DisOpen& DisOpen);
};

class DisClose : public Control {
public:
	Disconnector* d;

	bool isValid() {
		return (d->isOpen);
	}

	void execute() {
		d->isOpen = false;
	}

	void reverse() {
		d->isOpen = true;
	}

	string print() {
		return "Close D" + d->num;
	}

	friend ostream& operator<<(ostream& os, const DisOpen& DisOpen);
};


