#pragma once

#include<string>



class Control {
public:
	virtual bool isValid() = 0;
	virtual void execute() = 0;
	virtual void reverse() = 0;
	virtual string print() = 0;
};