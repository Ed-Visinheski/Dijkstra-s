#include "map.h"
#include "route.h"
#include "node.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <ctime>
#include <memory>


Map::Map(int num) {
    srand(static_cast<unsigned int>(time(0)));
    num_nodes = num;
    genNodes3D();
}

Map::~Map() {
    nodeList.clear();

}

void Map::genNodes3D() {
    nodeList.clear();
    for (int i = 0; i < num_nodes; i++) {
        float xpos = rand() % 200;
        float ypos = rand() % 200;
        float zpos = rand() % 200;
        std::string name = "Node " + std::to_string(i);
        nodeList.push_back(new Node(xpos, ypos,zpos, name));
    }
    genNeighbours();
}

bool Map::isNodeInNeighbourList(Node* node, const std::vector<Node*>& vertexList) {
    return std::find(vertexList.begin(), vertexList.end(), node) != vertexList.end();
}

void Map::genNeighbours() {
    for (auto& x : nodeList) {
        int numOfVertex = rand() % 3 + 1;
        std::vector<Node*> nodeNeighbourList = x->getNodeVertex();

        for (int i = 0; i < numOfVertex && nodeNeighbourList.size() < 4; i++) {
            int randNode = rand() % nodeList.size();
            while (nodeList.at(randNode) == x || isNodeInNeighbourList(nodeList.at(randNode),
                                                                       (nodeNeighbourList))) {
                randNode = rand() % nodeList.size();
            }
            x->addVertex(nodeList.at(randNode));
            nodeList.at(randNode)->addVertex(x);
        }
    }
}

const std::array<Node*, 2>  Map::genSelection() {
    int nodeA = rand() % nodeList.size();
    int nodeB = rand() % nodeList.size();
    while (nodeA == nodeB) { nodeB = rand() % nodeList.size(); }

    return std::array<Node*, 2>{nodeList.at(nodeA), nodeList.at(nodeB)};
}

void Map::viewNodesPos3D() {
    for (const auto& n : nodeList) {
        std::cout << "=====================" << '\n';
        std::cout << n->getNodeName() << '\n';
        std::cout << "X: " << n->get3DPosition()[0] << '\n';
        std::cout << "Y: " << n->get3DPosition()[1] << '\n';
        std::cout << "Z: " << n->get3DPosition()[2] << '\n';
    }
}

void Map::viewLinks() {
    for (const auto& n : nodeList) {
        std::cout << "=====================" << '\n';
        std::cout << n->getNodeName() << '\n';
        const std::vector<Node*>& nodeVertex = n->getNodeVertex();
        for (const auto& x : nodeVertex) {
            std::cout << x->getNodeName() << '\n';
        }
    }
}

std::vector<Node*>& Map::getNodeList(){
    for(auto& x : nodeList){
        std::cout<<x->getNodeName()<< ":" << &x<<std::endl;
    }
    return nodeList;
}

void Map::updateNumNodes(int num) {
    num_nodes = num;
}