#include "route.h"
#include "node.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct NodeCost {
    Node* node;
    double cost;

    bool operator>(const NodeCost& other) const {
        return cost > other.cost;
    }
};

Route::Route() = default;

Route::~Route() {
    route.clear();
}

//Determines shortest path using Dijkstra's algorithm
std::vector<Node*> Route::routeCalc(Node* startNode, Node* endNode) {
    route.clear();
    visitedList.clear();

    std::priority_queue<NodeCost, std::vector<NodeCost>, std::greater<>> priorityQueue;
    std::unordered_map<Node*, double> distances;
    std::unordered_map<Node*, Node*> previousNodes;
    std::unordered_set<Node*> visited;

    distances[startNode] = 0.0;
    priorityQueue.push({startNode, 0.0});

    while (!priorityQueue.empty()) {
        Node* currentNode = priorityQueue.top().node;
        double currentCost = priorityQueue.top().cost;
        priorityQueue.pop();

        if (!visited.insert(currentNode).second) {
            continue;
        }
        if (currentNode == endNode) {
            break;
        }
        const auto& nodes = currentNode->getNodeVertex();

        //Retains insertion order
        auto it = std::find_if(visitedList.begin(), visitedList.end(),
                               [currentNode](const std::pair<Node*, std::vector<Node*>>& entry) {
                                   return entry.first == currentNode;
                               });

        if (it == visitedList.end()) {
            visitedList.push_back({currentNode, {}});
            it = std::prev(visitedList.end());
        }

        for (Node* nextNode : nodes) {
            if (visited.find(nextNode) != visited.end()) {
                continue;
            }

            double newCost = currentCost + getDistance3D(*currentNode, *nextNode);

            if (distances.find(nextNode) == distances.end() || newCost < distances[nextNode]) {
                distances[nextNode] = newCost;
                previousNodes[nextNode] = currentNode;
                priorityQueue.push({nextNode, newCost});
            }

            if (std::find(it->second.begin(), it->second.end(), nextNode) == it->second.end()) {
                it->second.push_back(nextNode);
            }
        }
    }

    if (previousNodes.find(endNode) == previousNodes.end()) {
        return {};
    }
    for (Node* step = endNode; step != nullptr; step = previousNodes[step]) {
        route.push_back(step);
    }
    std::reverse(route.begin(), route.end());
    return route;
}


std::vector<std::pair<Node*, std::vector<Node*>>>& Route::getVisited() {
    return visitedList;
}

// Calculates the 3D distance between 2 nodes
double Route::getDistance3D(const Node& nodeA, const Node& nodeB) const {
    auto posA = nodeA.get3DPosition();
    auto posB = nodeB.get3DPosition();
    return std::sqrt(std::pow((posB[0] - posA[0]), 2) +
                     std::pow((posB[1] - posA[1]), 2) +
                     std::pow((posB[2] - posA[2]), 2));
}
