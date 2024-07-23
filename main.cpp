#include "map.h"
#include <iostream>
#include <SDL.h>
#include "nodeWindow.h"
#include "route.h"
#include <chrono>

int main(int argc, char* argv[]) {
    NodeWindow nodeWindow;
    SDL_Event event;

    int screenWidth = 1200;
    int screenHeight = 800;
    float fov = 300.0f;
    float viewerDistance = 10.0f;

    float angleX = 1.0f;
    float angleY = 1.0f;
    float angleZ = 10.0f;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    SDL_Window* window = SDL_CreateWindow("Graph Visualization", 100, 100, screenWidth, screenHeight, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        SDL_DestroyWindow(window);
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    bool quit = false;
    bool moveAxis = false;
    bool begin = false;
    Map map = Map(1,20);
    Route route = Route();
    auto& nodeList = map.getNodeList();
    auto selection = map.genSelection();
    std::cout<<"starting Node: "<< selection[0]->getNodeName() << " EndNode "<< selection[1]->getNodeName()<<std::endl;
    std::cout<<"======="<<std::endl;
    auto nodeRoutePath = route.routeCalc(selection[0],selection[1]);
    auto visitedList = route.getVisited();
    nodeWindow.setValues(renderer,nodeList, screenWidth, screenHeight, fov, viewerDistance, angleX, angleY, angleZ);
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_SPACE:
                            moveAxis = !moveAxis;
                            break;
                        case SDLK_RIGHT:
                            begin = true;
                            break;
                        case SDLK_LEFT:
                            nodeWindow.clearScreen();
                            nodeWindow.Reset();
                            map.genNodes3D();
                            nodeList = map.getNodeList();
                            selection = map.genSelection();
                            nodeRoutePath = route.routeCalc(selection[0], selection[1]);
                            visitedList = route.getVisited();
                            nodeWindow.setValues(renderer, nodeList, screenWidth, screenHeight, fov, viewerDistance, angleX, angleY, angleZ);
                            begin = false;
                            break;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    if (moveAxis) {
                        int mouseX, mouseY;
                        SDL_GetMouseState(&mouseX, &mouseY);
                        angleX += event.motion.yrel * 0.2f;
                        angleY += event.motion.xrel * 0.2f;
                    }
                    break;
                case SDL_MOUSEWHEEL:
                    if (event.wheel.y > 0) {
                        angleZ += 10.0f;
                    } else if (event.wheel.y < 0) {
                        angleZ -= 10.0f;
                    }
                    break;
            }
        }
        nodeWindow.updateAngles(angleX, angleY, angleZ);
        nodeWindow.drawGraph(nodeRoutePath, visitedList, begin);
        SDL_Delay(16);
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
