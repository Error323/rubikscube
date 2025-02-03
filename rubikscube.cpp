#include <sys/mman.h>

#include <cassert>
#include <csignal>
#include <cstdio>
#include <ctime>
#include <vector>

// clang-format off
#include "defines.hpp"
#include "model.cpp"
#include "search.cpp"
// clang-format on

void* Allocate(u64 size) {
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    assert(ptr != MAP_FAILED);
    return ptr;
}

void Deallocate(void *addr, u64 size) {
    if (munmap(addr, size) == -1) {
        assert(false);
    }
}

s32 main(void) {
    srand((u32)time(NULL));
    Cube cube;
    Init(cube);
    for (int i = 0; i < 20; i++) {
        u32 r = rand() % 18;
        printf("%s ", kNames[r]);
        kMoves[r](cube);
    }
    printf("\n");
    PrettyPrint(cube);
    return 0;
}
