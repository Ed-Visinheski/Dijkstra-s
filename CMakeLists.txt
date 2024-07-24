cmake_minimum_required(VERSION 3.26)
project(djisktra)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)


set(SDL2_INCLUDE_DIR /opt/homebrew/Cellar/sdl2/2.30.5/include/SDL2)
set(SDL2_LIBRARY /opt/homebrew/Cellar/sdl2/2.30.5/lib/libSDL2.dylib)
set(TTF_INCLUDE_DIR /opt/homebrew/Cellar/sdl2_ttf/2.22.0/include/SDL2)
set(TTF_LIBRARY /opt/homebrew/Cellar/sdl2_ttf/2.22.0/lib/libSDL2_ttf.dylib)


add_executable(djisktra
        map.cpp
        node.h
        main.cpp
        node.cpp
        route.cpp
        nodeWindow.cpp
        nodeWindow.h)

include_directories(${SDL2_INCLUDE_DIR} ${TTF_INCLUDE_DIR})
target_link_libraries(djisktra ${SDL2_LIBRARY} ${TTF_LIBRARY})