#pragma once

class DeltaU {
public:
	int index;
	int newU;

	DeltaU() {
		index = -1;
		newU = 0;
	}

	DeltaU(int index, int newU) {
		this->index = index;
		this->newU = newU;
	}

	static bool isSame(int state1, int state2) {
		return state1 * state2;
	}

	static int not(int state) {
		return (int)(state * -1);
	}

	friend ostream& operator<<(ostream& os, const DeltaU& deltaU);
};
