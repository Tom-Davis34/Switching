#pragma once

#include "common.h"
#include "Control.h"
#include "Edge.h"
#include <iostream>
#include <string>
#include <vector>




class CircuitBreaker : public Edge {
public:
	int fNode;
	int tNode;
	bool isOpen;
	int num;

	CircuitBreaker(int fNode, int tNode, bool isOpen, int num) {
		this->fNode = fNode;
		this->tNode = tNode;
		this->isOpen = isOpen;
		this->num = num;
	}

	CircuitBreaker(){
		fNode = -1;
		tNode = -1;
		isOpen = false;
		num = -1;
	}

	CircuitBreaker(vector<string> lineCb, int num) {
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
		return CB;
	}
	
	static vector<CircuitBreaker> parseCircuitBreakerFile(vector<vector<string>> cells) {
		vector<CircuitBreaker> cirs = vector<CircuitBreaker>();

		for (size_t i = 0; i < cells.size(); i++)
		{
			cirs.push_back(CircuitBreaker(cells[i], i));
		}

		return cirs;
	}

	friend ostream& operator<<(ostream& os, const CircuitBreaker& cb);
};

class Open : public Control {
public:
	CircuitBreaker* cb;

	bool isValid() {
		return !(cb->isOpen);
	}

	void execute() {
		cb->isOpen = true;
	}

	void reverse() {
		cb->isOpen = false;
	}

	string print() {
		return "Open CB" + (cb->num);
	}

	friend ostream& operator<<(ostream& os, const Open& Open);
};

class Close : public Control {
public:
	CircuitBreaker* cb;

	Close(CircuitBreaker* cb) {
		this->cb = cb;
	}

	bool isValid() {
		return (cb->isOpen);
	}

	void execute() {
		cb->isOpen = false;
	}

	void reverse() {
		cb->isOpen = true;
	}

	string print() {
		return "Close CB" + (cb->num);
	}

	friend ostream& operator<<(ostream& os, const Open& Open);
};
