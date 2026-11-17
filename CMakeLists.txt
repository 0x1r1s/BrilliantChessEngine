cmake_minimum_required(VERSION 3.20)

project(BrilliantChessEngine
    VERSION 0.1.0
    LANGUAGES CXX
)

include(FetchContent)

# chess-library owns the rules of chess: positions, legal moves, and FEN/PGN.
# The full commit hash keeps every developer on the same library version.
FetchContent_Declare(
    chess_library
    GIT_REPOSITORY https://github.com/Disservin/chess-library.git
    GIT_TAG 53e6a841dcda7059a2af363d85f785ef1817304a
)
FetchContent_MakeAvailable(chess_library)

add_library(bce-engine
    engine/src/evaluation/material.cpp
    engine/src/search/search.cpp
)

target_compile_features(bce-engine PUBLIC cxx_std_20)

target_include_directories(bce-engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/engine/include
        ${chess_library_SOURCE_DIR}/include
)

add_executable(bce-cli
    apps/cli/src/main.cpp
)

target_link_libraries(bce-cli PRIVATE bce-engine)

# Warnings catch common mistakes early (only for our own code).
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(bce-engine PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(bce-cli PRIVATE -Wall -Wextra -Wpedantic)
endif()
