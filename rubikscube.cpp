#include <sys/mman.h>

#include <cassert>
#include <csignal>
#include <cstdio>
#include <ctime>
#include <vector>
#include <chrono>

// clang-format off
#include "defines.hpp"
#include "model.cpp"
#include "idastar.cpp"
#include "dfs.cpp"
// clang-format on


s32 main(void) {
    // used in search.cpp
    Init(goal);

    Cube cube;
    Init(cube);
    auto start = std::chrono::high_resolution_clock::now();
    auto nodes = Dfs(cube, 4);
    auto end = std::chrono::high_resolution_clock::now();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    printf("\n\n%ld us, nodes %lu \n", us, nodes);
    PrettyPrint(cube);
    return 0;
}
