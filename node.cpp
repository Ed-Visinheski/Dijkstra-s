#include <vector>
#include <string>
#include <array>
#include "node.h"

Node::Node(float xpos, float ypos,float zpos, const std::string& name)
        : pos3D{xpos, ypos, zpos}, nodeName(name) {}

std::string Node::getNodeName() const {
    return nodeName;
}

std::vector<Node*> Node::getNodeVertex() const {
    return nodeVertex;
}

void Node::addVertex(Node* n) {
    nodeVertex.push_back(n);
}

std::array<float, 3> Node::get3DPosition() const {
    return pos3D;
}
