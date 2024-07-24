#include "map.h"
#include "nodeWindow.h"
#include "route.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>

SDL_Texture* renderText(SDL_Renderer* renderer, const std::string &message, TTF_Font* font, SDL_Color color) {
    SDL_Surface* surf = TTF_RenderText_Blended(font, message.c_str(), color);
    if (!surf) {
        std::cerr << "TTF_RenderText Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_FreeSurface(surf);
    if (!texture) {
        std::cerr << "CreateTexture Error: " << SDL_GetError() << std::endl;
    }
    return texture;
}

bool initSDL(SDL_Window*& window, SDL_Renderer*& renderer, const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    return true;
}

void handleInput(SDL_Event& event, bool& quit, bool& mouseMovement, bool& begin, bool& inputMode, std::string& inputText, int& nodeCount, Map& map, NodeWindow& nodeWindow, Route& route, std::vector<Node*>& correctRoute,SDL_Renderer* renderer, float& angleX, float& angleY, float& angleZ) {
    switch (event.type) {
        case SDL_QUIT:
            quit = true;
            break;
        case SDL_KEYDOWN:
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
                } else if (event.key.keysym.sym >= SDLK_0 && event.key.keysym.sym <= SDLK_9)  {
                    inputText += static_cast<char>(event.key.keysym.sym);
                }
            } else {
                switch (event.key.keysym.sym) {
                    case SDLK_i:
                        inputMode = true;
                        inputText.clear();
                        break;
                    case SDLK_SPACE:
                        mouseMovement = !mouseMovement;
                        break;
                    case SDLK_DOWN:
                        nodeWindow.setViewerDistance(nodeWindow.getViewerDistance() + 2.5);
                        break;
                    case SDLK_UP:
                        nodeWindow.setViewerDistance(nodeWindow.getViewerDistance() - 2.5);
                        break;
                    case SDLK_RIGHT:
                        begin = true;
                        break;
                    case SDLK_LEFT:
                        nodeWindow.Reset();
                        map.genNodes3D();
                        auto x = map.genSelection();
                        nodeWindow.setValues(renderer, map.getNodeList(), angleX, angleY, angleZ);
                        correctRoute = route.routeCalc(x[0], x[1]);
                        begin = false;
                        break;
                }
            }
            break;

        case SDL_MOUSEMOTION:
            if (mouseMovement) {
                angleX += event.motion.yrel * 0.2f;
                angleY += event.motion.xrel * 0.2f;
            }
            break;
        case SDL_MOUSEWHEEL:
            angleZ += event.wheel.y > 0 ? 10.0f : -10.0f;
            break;
    }
}
void renderInputMode(SDL_Renderer* renderer, TTF_Font* font, const std::string& inputText) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_Color whiteColor = {255, 255, 255, 255};
    SDL_Texture* inputTexture = renderText(renderer, "Enter number of nodes: " + inputText, font, whiteColor);
    if (inputTexture) {
        int iW, iH;
        SDL_QueryTexture(inputTexture, nullptr, nullptr, &iW, &iH);
        SDL_Rect inputRect = {50, 50, iW, iH};
        SDL_RenderCopy(renderer, inputTexture, nullptr, &inputRect);
        SDL_DestroyTexture(inputTexture);
    }
    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    float angleX = 1.0f;
    float angleY = 1.0f;
    float angleZ = 10.0f;

    if (!initSDL(window, renderer, "Graph Visualization", 1200, 800)) {
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("/Users/EdVisinheski/Downloads/Orbitron modern/Orbitron-Regular.ttf", 24);
    if (!font) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    bool quit = false;
    bool moveMovement = false;
    bool begin = false;
    std::string inputText;
    bool inputMode = false;
    int nodeCount = 0;

    NodeWindow nodeWindow;
    Map map(5);
    Route route;
    auto selection = map.genSelection();
    auto correctRoute = route.routeCalc(selection[0], selection[1]);
    nodeWindow.setValues(renderer, map.getNodeList(), angleX, angleY, angleZ);
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            handleInput(event, quit, moveMovement, begin, inputMode,
                        inputText, nodeCount, map, nodeWindow, route,
                        correctRoute, renderer, angleX, angleY, angleZ);
        }
        if (inputMode) {
            renderInputMode(renderer, font, inputText);
        } else {
            nodeWindow.updateAngles(angleX, angleY, angleZ);
            nodeWindow.drawGraph(correctRoute, route.getVisited(), begin);
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
