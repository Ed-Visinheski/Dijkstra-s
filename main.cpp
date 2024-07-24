#include "map.h"
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include "nodeWindow.h"
#include "route.h"
#include <string>
#include <sstream>

SDL_Texture* renderText(SDL_Renderer* renderer, const std::string &message, TTF_Font* font, SDL_Color color) {
    SDL_Surface* surf = TTF_RenderText_Blended(font, message.c_str(), color);
    if (surf == nullptr) {
        std::cerr << "TTF_RenderText Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if (texture == nullptr) {
        std::cerr << "CreateTexture Error: " << SDL_GetError() << std::endl;
    }
    return texture;
}

int main() {
    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        return 1;
    }

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

    TTF_Font* font = TTF_OpenFont("/Users/EdVisinheski/Downloads/Orbitron modern/Orbitron-Regular.ttf", 24);
    if (font == nullptr) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    bool quit = false;
    bool moveAxis = false;
    bool begin = false;

    std::string inputText = "";
    bool inputMode = false;
    int nodeCount = 0;
    Map map = Map(5);
    Route route = Route();

    auto& nodeList = map.getNodeList();
    auto selection = map.genSelection();
    auto nodeRoutePath = route.routeCalc(selection[0], selection[1]);
    auto visitedList = route.getVisited();
    nodeWindow.setValues(renderer, nodeList, screenWidth, screenHeight, fov, viewerDistance, angleX, angleY, angleZ);

    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_i) {
                        inputMode = true;
                        inputText = "";
                    }

                    if (inputMode) {
                        if (event.key.keysym.sym == SDLK_RETURN) {
                            try {
                                nodeCount = std::stoi(inputText);
                                if (nodeCount >= 5 && nodeCount <= 25) {
                                    map.updateNumNodes(nodeCount);
                                    inputMode = false;
                                } else {
                                    std::cerr << "Invalid node count. Must be between 5 and 25." << std::endl;
                                }
                            } catch (std::invalid_argument& e) {
                                std::cerr << "Invalid input. Please enter a valid number." << std::endl;
                            }
                        } else if (event.key.keysym.sym == SDLK_BACKSPACE && !inputText.empty()) {
                            inputText.pop_back();
                        } else if ((event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9) || (event.key.keysym.sym >= SDLK_KP_0 && event.key.keysym.sym <= SDLK_KP_9)) {
                            inputText += static_cast<char>(event.key.keysym.sym);
                        }
                    } else {
                        switch(event.key.keysym.sym) {
                            case SDLK_SPACE:
                                moveAxis = !moveAxis;
                                break;
                            case SDLK_RIGHT:
                                begin = true;
                                break;
                            case SDLK_LEFT:
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
        if (inputMode) {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
            SDL_Color whiteColor = {255, 255, 255, 255};
            SDL_Texture* inputTexture = renderText(renderer, "Enter number of nodes: " + inputText, font, whiteColor);
            int iW, iH;
            SDL_QueryTexture(inputTexture, NULL, NULL, &iW, &iH);
            SDL_Rect inputRect = {50, 50, iW, iH};
            SDL_RenderCopy(renderer, inputTexture, NULL, &inputRect);
            SDL_DestroyTexture(inputTexture);
            SDL_RenderPresent(renderer);
        } else {
            nodeWindow.updateAngles(angleX, angleY, angleZ);
            nodeWindow.drawGraph(nodeRoutePath, visitedList, begin);
        }
        SDL_Delay(16);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
