#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>
#include <memory>



class Node {
private:
    std::array<float, 3> pos3D;
    std::vector<Node *> nodeVertex;
    std::string nodeName;

public:

    Node(float xpos, float ypos, float zpos, const std::string &name);

    std::string getNodeName() const;

    std::vector<Node *> getNodeVertex() const;

    void addVertex(Node *n);

    std::pair<int, int> get2DPosition() const;

    std::array<float, 3> get3DPosition() const;

};
#endif