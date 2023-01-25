#include <iostream>
#include <string>
#include <vector>
#include "Disconnector.h"
#include "CircuitBreaker.h"
#include "Circuit.h"
#include "Grid.h"
#include "BusData.h" 

inline ostream& operator<<(ostream& os, const Disconnector& cb)
{
	os << "D" << cb.num;
	return os;
}
inline ostream& operator<<(ostream& os, const DisOpen& disOpen)
{
	os << "Open " << *disOpen.d;
	return os;
}
inline ostream& operator<<(ostream& os, const DisClose& disClose)
{
	os << "DisClose " << *disClose.d;
	return os;
}

inline ostream& operator<<(ostream& os, const CircuitBreaker& cb)
{
	os << "CB" << cb.num;
	return os;
}
inline ostream& operator<<(ostream& os, const Open& open)
{
	os << "Open " << *open.cb;
	return os;
}
inline ostream& operator<<(ostream& os, const Close& close)
{
	os << "Close " << *close.cb;
	return os;
}

inline ostream& operator<<(ostream& os, const PowerGrid& grid)
{
	os << "Circuit Breakers\n";
	for (int i = 0; i < grid.cbs.size(); i++)
	{
		const CircuitBreaker* cb = &(grid.cbs[i]);
		os << *(cb) << "[" << cb->fNode << ", " << cb->tNode << "]: " << (cb->isOpen?"Open":"Closed") << "\n";
	}

	//os << "Diconnectors\n";
	//for (&Disconnector d : grid.ds)
	//{
	//	os << &d << "[" << d.fNode << ", " << d.tNode << "]: " << d.isOpen ? "Open\n" : "Close\n";
	//}

	//os << "Cicuits\n";
	//for (Circuit c : grid.cs)
	//{
	//	os << *c << "[" << c.fbus << ", " << c.tbus << "]: " << c.acceptance << "\n";
	//}

	//os << "BusData\n";
	//int i = 0;
	//for (BusData bd : grid.bds)
	//{
	//	os << bd << "[" << i << "]: " << bd.pq;
	//	i++;
	//}

	//os << "NodesToEdges\n";
	//int i = 0;
	//for (BusData bd : grid.bds)
	//{
	//	os << bd << "[" << i << "]: " << bd.pq;
	//	i++;
	//}
	return os;
}
