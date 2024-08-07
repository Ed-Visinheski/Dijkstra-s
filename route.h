#include "node.h"
#include "map.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <set>
#include "nodeWindow.h"
#include <stack>

#ifndef Route_H
#define Route_H

class Route {
public:
    Route();

    ~Route();

    std::vector<std::pair<Node*,std::vector<Node*>>>& getVisited();


    std::vector<Node*> routeCalc(Node* startNode, Node* endNode);

private:

    double getDistance3D(const Node& nodeA, const Node& nodeB) const;

    std::vector<std::pair<Node* , std::vector<Node*>>> visitedList;

    std::vector<Node*> route;
};


#endif