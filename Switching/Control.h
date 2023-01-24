#pragma once

#include<string>

using namespace std;

class Control {
public:
	virtual bool isValid() = 0;
	virtual void execute() = 0;
	virtual void reverse() = 0;
	virtual string print() = 0;
};