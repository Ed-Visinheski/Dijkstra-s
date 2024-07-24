#ifndef NODEWINDOW_H
#define NODEWINDOW_H
#include <SDL.h>
#include <memory>
#include "node.h"
#include <vector>
#include <unordered_set>
#include "nodeWindow.h"
#include <iostream>
#include <algorithm>

class NodeWindow {
private:
    SDL_Renderer* renderer;
    std::vector<Node*> nodeList;
    int screenWidth, screenHeight;
    float fov, viewerDistance, angleX, angleY, angleZ;
    std::unordered_map<Node*, std::vector<Node*>> visitedLinks;
    std::pair<int, int> project3DTo2D(float x, float y, float z) const;
    std::vector<std::pair<Node*, std::vector<Node*>>> completedVisitedList;
    std::vector<Node*> completedNodeList;
    std::vector<Node*> completedLinksList;
    static size_t currentNodeIndex;
    static size_t currentLinksIndex;
    static size_t currentIndex;
    static size_t innerIndex;


    void rotateX(float& y, float& z, float angle) const;
    void rotateY(float& x, float& z, float angle) const;
    void rotateZ(float& x, float& y, float angle) const;
    void drawStartLinks();
    void drawEdgesBetweenNodes(const std::vector<Node*>& nodes);

public:
    NodeWindow();
    ~NodeWindow();
    void clearScreen();
    float getViewerDistance();
    void setViewerDistance(float newViewerDistance);
    void setValues(SDL_Renderer* renderer, const std::vector<Node*>& nodeList, float angleX, float angleY, float angleZ);
    void drawNodes(Node* start, Node* end);
    void drawGraph(std::vector<Node*> route, std::vector<std::pair<Node*,std::vector<Node*>>>& visited, bool begin);
    void updateAngles(float x, float y, float z);

    void drawEdgesBetweenVisitedNodes(const std::vector<std::pair<Node *, std::vector<Node *>>> &visited);

    void drawCircle(int centerX, int centerY, int radius);

    void Reset();
};

#endif
