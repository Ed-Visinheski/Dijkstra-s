#include "nodeWindow.h"
#include <iostream>
#include <algorithm>
#include <utility>
#include <functional>
#include <cmath>

// Static member initialization
size_t NodeWindow::currentNodeIndex = 0;
size_t NodeWindow::currentLinksIndex = 0;
size_t NodeWindow::currentIndex = 0;
size_t NodeWindow::innerIndex = 0;

NodeWindow::NodeWindow() :
        renderer(nullptr), screenWidth(1200), screenHeight(800),
        fov(300.0f), viewerDistance(10.0f),
        angleX(1.0f), angleY(1.0f), angleZ(10.0f) {}

void NodeWindow::setValues(SDL_Renderer* renderer, const std::vector<Node*>& nodeList, float angleX, float angleY, float angleZ) {
    this->renderer = renderer;
    this->nodeList = nodeList;
    this->screenWidth = screenWidth;
    this->screenHeight = screenHeight;
    this->fov = fov;
    this->viewerDistance = viewerDistance;
    this->angleX = angleX;
    this->angleY = angleY;
    this->angleZ = angleZ;
}

NodeWindow::~NodeWindow() {}

void NodeWindow::updateAngles(float x, float y, float z) {
    angleX = x;
    angleY = y;
    angleZ = z;
}

void NodeWindow::clearScreen() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

void NodeWindow::drawGraph(std::vector<Node*> route, std::vector<std::pair<Node*, std::vector<Node*>>>& visited, bool begin) {
    clearScreen();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);

    if (currentNodeIndex < nodeList.size()) {
        Node* currentNode = nodeList[currentNodeIndex];
        if (std::find(completedNodeList.begin(), completedNodeList.end(), currentNode) == completedNodeList.end()) {
            completedNodeList.push_back(currentNode);
        }
        currentNodeIndex++;
        SDL_Delay(60);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    drawNodes(route.front(), route.back());

    if (currentNodeIndex >= nodeList.size()) {
        if (currentLinksIndex < nodeList.size()) {
            Node* currentNode = nodeList[currentLinksIndex];
            if (std::find(completedLinksList.begin(), completedLinksList.end(), currentNode) ==
                completedLinksList.end()) {
                completedLinksList.push_back(currentNode);
            }
            currentLinksIndex++;
            SDL_Delay(60);
        }
        SDL_SetRenderDrawColor(renderer, 125, 125, 125, 175);
        drawStartLinks();
    }

    if (begin && currentLinksIndex >= nodeList.size()) {
        if (currentIndex < visited.size()) {
            auto& [currentNode, neighbors] = visited[currentIndex];
            if (innerIndex < neighbors.size()) {
                auto it = std::find_if(completedVisitedList.begin(), completedVisitedList.end(),
                                       [&](const std::pair<Node*, std::vector<Node*>>& entry) {
                                           return entry.first == currentNode;
                                       });

                if (it == completedVisitedList.end()) {
                    completedVisitedList.push_back({currentNode, {neighbors[innerIndex]}});
                } else {
                    it->second.push_back(neighbors[innerIndex]);
                }
                innerIndex++;
            } else {
                currentIndex++;
                innerIndex = 0;
            }
            SDL_Delay(120);
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        drawEdgesBetweenVisitedNodes(completedVisitedList);

        if (currentIndex >= visited.size()) {
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            drawEdgesBetweenNodes(route);
        }
    }
    SDL_RenderPresent(renderer);
}


void NodeWindow::drawStartLinks() {
    for (const auto& node : completedLinksList) {
        auto pos3D = node->get3DPosition();
        float x = pos3D[0];
        float y = pos3D[1];
        float z = pos3D[2];

        rotateX(y, z, angleX);
        rotateY(x, z, angleY);
        rotateZ(x, y, angleZ);

        auto projectedStart = project3DTo2D(x, y, z);

        for (const auto& neighbor : node->getNodeVertex()) {
            auto neighborPos3D = neighbor->get3DPosition();

            float nx = neighborPos3D[0];
            float ny = neighborPos3D[1];
            float nz = neighborPos3D[2];

            // Rotate neighbor position
            rotateX(ny, nz, angleX);
            rotateY(nx, nz, angleY);
            rotateZ(nx, ny, angleZ);

            auto projectedEnd = project3DTo2D(nx, ny, nz);

            SDL_RenderDrawLine(renderer, projectedStart.first, projectedStart.second, projectedEnd.first, projectedEnd.second);
        }
    }
}

// Draw edges between nodes for the provided path
void NodeWindow::drawEdgesBetweenNodes(const std::vector<Node*>& nodes) {
    for (auto it = nodes.begin(); it != std::prev(nodes.end()); it++) {
        auto currPos3D = (*it)->get3DPosition();
        float currx = currPos3D[0];
        float curry = currPos3D[1];
        float currz = currPos3D[2];

        rotateX(curry, currz, angleX);
        rotateY(currx, currz, angleY);
        rotateZ(currx, curry, angleZ);
        auto projectedStart = project3DTo2D(currx, curry, currz);

        auto nextPos3D = (*std::next(it, 1))->get3DPosition();
        float nextx = nextPos3D[0];
        float nexty = nextPos3D[1];
        float nextz = nextPos3D[2];
        rotateX(nexty, nextz, angleX);
        rotateY(nextx, nextz, angleY);
        rotateZ(nextx, nexty, angleZ);

        auto projectedEnd = project3DTo2D(nextx, nexty, nextz);
        SDL_RenderDrawLine(renderer, projectedStart.first, projectedStart.second, projectedEnd.first, projectedEnd.second);
    }
}

// Draw edges between visited nodes for visualization
void NodeWindow::drawEdgesBetweenVisitedNodes(const std::vector<std::pair<Node*, std::vector<Node*>>>& visited) {
    for (const auto& pair : visited) {
        auto pos3D = pair.first->get3DPosition();
        float x = pos3D[0];
        float y = pos3D[1];
        float z = pos3D[2];

        rotateX(y, z, angleX);
        rotateY(x, z, angleY);
        rotateZ(x, y, angleZ);

        auto projectedStart = project3DTo2D(x, y, z);

        for (const auto& nextNode : pair.second) {
            auto nextNodePos3D = nextNode->get3DPosition();

            float nx = nextNodePos3D[0];
            float ny = nextNodePos3D[1];
            float nz = nextNodePos3D[2];

            rotateX(ny, nz, angleX);
            rotateY(nx, nz, angleY);
            rotateZ(nx, ny, angleZ);

            auto projectedEnd = project3DTo2D(nx, ny, nz);

            SDL_RenderDrawLine(renderer, projectedStart.first, projectedStart.second, projectedEnd.first, projectedEnd.second);
        }
    }
}

// Draw the nodes in the graph
void NodeWindow::drawNodes(Node* start = nullptr, Node* end = nullptr) {
    for (const auto& node : completedNodeList) {
        if (node == start || node == end) {
            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 175);
        } else {
            SDL_SetRenderDrawColor(renderer, 175, 175, 175, 175);
        }
        auto nodePos3D = node->get3DPosition();
        float x = nodePos3D[0];
        float y = nodePos3D[1];
        float z = nodePos3D[2];

        rotateX(y, z, angleX);
        rotateY(x, z, angleY);
        rotateZ(x, y, angleZ);

        auto projected = project3DTo2D(x, y, z);

        int radius = 15;
        drawCircle(projected.first, projected.second, radius);
    }
}

// Project 3D coordinates to 2D screen coordinates
std::pair<int, int> NodeWindow::project3DTo2D(float x, float y, float z) const {
    float factor = fov / (viewerDistance + z);
    int x2D = static_cast<int>(x * factor + screenWidth / 2);
    int y2D = static_cast<int>(y * factor + screenHeight / 2);
    return {x2D, y2D};
}

// These 3 methods are used to transform the mouse position into
//rotation in their respective directions
void NodeWindow::rotateX(float& y, float& z, float angle) const {
    float rad = angle * M_PI / 180.0;
    float cosA = cos(rad);
    float sinA = sin(rad);
    float ynew = y * cosA - z * sinA;
    float znew = y * sinA + z * cosA;
    y = ynew;
    z = znew;
}

void NodeWindow::rotateY(float& x, float& z, float angle) const {
    float rad = angle * M_PI / 180.0;
    float cosA = cos(rad);
    float sinA = sin(rad);
    float xnew = x * cosA + z * sinA;
    float znew = -x * sinA + z * cosA;
    x = xnew;
    z = znew;
}

void NodeWindow::rotateZ(float& x, float& y, float angle) const {
    float rad = angle * M_PI / 180.0;
    float cosA = cos(rad);
    float sinA = sin(rad);
    float xnew = x * cosA - y * sinA;
    float ynew = x * sinA + y * cosA;
    x = xnew;
    y = ynew;
}

// Set a new viewer distance for perspective projection
void NodeWindow::setViewerDistance(float newViewerDistance) {
    viewerDistance = newViewerDistance;
}

float NodeWindow::getViewerDistance() {
    return viewerDistance;
}

// Draws a filled in circle
void NodeWindow::drawCircle(int centerX, int centerY, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // Horizontal offset
            int dy = radius - h; // Vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
            }
        }
    }
}

// Reset the window state to initial values
void NodeWindow::Reset() {
    clearScreen();
    viewerDistance = 10.0f;
    angleY = 1.0f;
    angleZ = 10.0f;
    angleX = 1.0f;
    currentNodeIndex = 0;
    currentLinksIndex = 0;
    currentIndex = 0;
    innerIndex = 0;
    renderer = nullptr;
    nodeList.clear();
    visitedLinks.clear();
    completedVisitedList.clear();
    completedNodeList.clear();
    completedLinksList.clear();
}

