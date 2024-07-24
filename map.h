#ifndef MAP_H
#define MAP_H

#include <vector>
#include <memory>
#include "node.h"
#include <algorithm>
#include <functional>

class Map {
private:

    int num_nodes;
    std::vector<Node*> nodeList;

public:
    Map(int num);
    ~Map();
    bool isNodeInNeighbourList(Node *node, const std::vector<Node *> &vertexList);
    const std::array<Node*, 2> genSelection();
    void genNeighbours();
    void viewLinks() const;
    std::vector<Node*>&getNodeList();
    void viewNodesPos3D();
    void genNodes3D();
    void updateNumNodes(int num);

};

#endif