cmake_minimum_required(VERSION 3.16)
project(tic-tac-toe LANGUAGES CXX)

set(ENGINE_BUILD_TESTS ON CACHE BOOL "" FORCE)
add_subdirectory(external/engine)
enable_testing()

engine_add_game(tic-tac-toe
    src/main.cpp
    src/game.cpp
    src/match_controller.cpp
    src/board.cpp
    src/bot_ai.cpp
    src/menu_view_model.cpp
    src/play_view_model.cpp
    src/play_presenter.cpp)

add_executable(tic_tac_toe_tests
    tests/board_test.cpp
    tests/bot_ai_test.cpp
    tests/match_controller_test.cpp
    src/board.cpp
    src/bot_ai.cpp
    src/match_controller.cpp)
target_include_directories(tic_tac_toe_tests PRIVATE include)
target_link_libraries(tic_tac_toe_tests PRIVATE GTest::gtest_main)
gtest_discover_tests(tic_tac_toe_tests DISCOVERY_MODE PRE_TEST)
