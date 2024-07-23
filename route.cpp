#include "route.h"
#include "node.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>


struct NodeCost{
    Node* node;
    double cost ;

    bool operator>(const NodeCost& other) const{
        return cost > other.cost;
    }
};

Route::Route(){
}

Route::~Route() {
    std::cout << "Route destructor" << std::endl;
    route.clear();
}

std::vector<Node*> Route::routeCalc(Node* startNode, Node* endNode) {
    route.clear();
    visitedList.clear();
    std::priority_queue<NodeCost, std::vector<NodeCost>, std::greater<>> priorityQueue;
    std::unordered_map<Node*, double> distances;
    std::unordered_map<Node*, Node*> previousNodes;
    std::unordered_set<Node*> visited;

    distances[startNode] = 0;
    priorityQueue.push({startNode, 0});

    while (!priorityQueue.empty()) {
        Node* currentNode = priorityQueue.top().node;
        double currentCost = priorityQueue.top().cost;
        priorityQueue.pop();

        if (visited.find(currentNode) != visited.end()) {continue;}
        visited.insert(currentNode);
        if (currentNode == endNode) {break;}

        auto it = std::find_if(visitedList.begin(), visitedList.end(),
                               [currentNode](const std::pair<Node*, std::vector<Node*>>& entry) {
                                   return entry.first == currentNode;
                               });

        if (it == visitedList.end()) {
            visitedList.push_back({currentNode, {}});
            it = std::prev(visitedList.end());
        }

        for (Node* neighbour : currentNode->getNodeVertex()) {
            if (visited.find(neighbour) != visited.end()) {continue;}

            if (std::find(it->second.begin(), it->second.end(),
                          neighbour) == it->second.end()) {
                it->second.push_back(neighbour);
            }

            double newCost = currentCost + getDistance3D(*currentNode, *neighbour);
            if (distances.find(neighbour) == distances.end() || newCost < distances[neighbour]) {
                distances[neighbour] = newCost;
                previousNodes[neighbour] = currentNode;
                priorityQueue.push({neighbour, newCost});
            }
        }
    }

    if (previousNodes.find(endNode) == previousNodes.end()) {return {};}
    Node* step = endNode;
    while (step != nullptr) {
        route.push_back(step);
        if (step == startNode) {
            break;
        }
        auto it = previousNodes.find(step);
        if (it == previousNodes.end()) {
            std::cerr << "No predecessor found for " << step->getNodeName() << std::endl;
            break;
        }
        step = it->second;
    }

    std::reverse(route.begin(), route.end());
    std::cout<<"Route"<<std::endl;
    for(auto x : route){
        std::cout<< x->getNodeName()<<std::endl;
    }

    return route;
}

std::vector<Node*>& Route::getRoute() {
    return route;
}

std::vector<std::pair<Node*,std::vector<Node*>>>& Route::getVisited(){
    return visitedList;
}

void Route::viewRoute() {
    for (auto& x : route) {
        std::cout << x->getNodeName() << std::endl;
    }
}

double Route::getDistance3D(Node nodeA, Node nodeB){
    auto posA = nodeA.get3DPosition();
    auto posB = nodeB.get3DPosition();
    return (std::sqrt(std::pow((posB[0] - posA[0]), 2) + std::pow((posB[1] - posA[1]), 2)+ std::pow((posB[2] - posA[2]), 2)));
}
