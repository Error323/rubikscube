#include <sys/mman.h>

#include <cassert>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <ctime>

// clang-format off
#include "utils.hpp"
#include "deque.cpp"
#include "model.cpp"
#include "dfs.cpp"
#include "bfs.cpp"
// clang-format on

s32 main(void) {
    srand(time(NULL));
    Cube cube;
    Init(cube);
    Cube goal = cube;
    for (s32 i = 0; i < 7; i++) {
        s32 r = rand() % 18;
        printf("%s ", kNames[r]);
        ApplyMove(cube, r);
    }
    cube.SetLastMoveIndex(0);
    PrettyPrint(cube);
    printf("\n");
    bool solved = Bfs(cube, goal);
    printf("solved=%d\n", solved);
    return 0;
}
