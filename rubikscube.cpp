#include <sys/mman.h>

#include <cassert>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <ctime>

// clang-format off
#include "utils.cpp"
#include "deque.cpp"
#include "model.cpp"
#include "dfs.cpp"
#include "bfs.cpp"
// clang-format on

s32 main(int argc, char *argv[]) {
    s32 depth = 6;
    if (argc > 1) {
        depth = atoi(argv[1]);
    }
    Cube cube;
    Init(cube);

    srand(time(NULL));
    Cube goal = cube;
    for (s32 i = 0; i < depth; i++) {
        s32 r = rand() % 18;
        printf("%s ", kNames[r]);
        kMoves[r](cube);
    }
    PrettyPrint(cube);
    printf("\n");
    bool solved = Bfs(cube, goal);
    printf("solved=%d\n", solved);

    return 0;
}
