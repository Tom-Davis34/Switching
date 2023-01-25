#pragma once
#define stringify( name ) #name

enum EdgeType {
	CB, Dis, Cir
};

class Edge {
public:
	virtual const int getNum() const = 0;
	virtual const EdgeType type() const = 0;
	virtual const int getFNode() const = 0;
	virtual const int getTNode() const = 0;

	friend ostream& operator<<(ostream& os, const Edge& edge);
};
