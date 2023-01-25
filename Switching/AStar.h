#pragma once
#include <vector>
#include <ostream>
#include "Control.h"
#include "Grid.h"
#include "AStarNode.h"
#include <deque>
#include <iostream>
#include <list>
#include <unordered_map>
#include <queue>
#include <utility>



class CompareNode
{
public:
	bool operator() (AStarNode* n1, AStarNode* n2)
	{
		return n1->sum > n2->sum;
	}
};

AStarNode* aStar (PowerGrid* grid)
{
	int count = 0;
	int finishedNodeCount = 0;
	AStarNode* root = AStarNode::createRoot(grid);

	// Priority queue: discovered but unevaluated nodes starting from the start
	// The priority is the heuristic + cost thus prioritizing the most efficient
	// neighbour by placing it at the top of the queue
	std::priority_queue<AStarNode*, std::vector<AStarNode*>, CompareNode> frontier;

	frontier.emplace(root);

	AStarNode* current;
	// Continue the earch for optimal path as long as open nodes exist
	while (!frontier.empty()) {
		cout << "\033[1;32m ====== Iteration " + std::to_string(count) + " ====== \033[0m\n";
		current = frontier.top();
 		if (current->getHObjective() == 0 && current->state == Finished) {
			break; 
		}
		else {
			cout << "\033[1;34m";
			current->logOS();
			cout << "\033[0m\n";
		}

		frontier.pop();

		if (current->state == Finished) {
			finishedNodeCount++;
			cout << "Creating children...\n";
			auto children = current->getChildren();

			for (int i = 0; i < grid->u.size(); i++) {
				frontier.emplace(children[i]);
			}
		}
		else {
			current->calculateNextObjective();
			cout << "\033[1;34m";
			current->printWithObjective();
			cout << "\033[0m\n";
			frontier.emplace(current);
		}

		count++;
	}

	return current;
}

