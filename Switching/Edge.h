#pragma once
#define stringify( name ) #name

enum EdgeType {
	CB, Dis, Cir
};

class Edge {
public:
	virtual const int const getNum() const = 0;
	virtual const EdgeType const type() const = 0;
	virtual const int const getFNode() const = 0;
	virtual const int const getTNode() const = 0;

	friend ostream& operator<<(ostream& os, const Edge& edge);
};
