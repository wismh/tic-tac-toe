cmake_minimum_required(VERSION 3.16)
project(tic-tac-toe LANGUAGES CXX)

add_subdirectory(external/engine)
engine_add_game(tic-tac-toe
    src/main.cpp
    src/game.cpp
    src/hud_view_model.cpp)
