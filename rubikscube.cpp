#include <sys/mman.h>

#include <cassert>
#include <csignal>
#include <cstdio>
#include <vector>

// clang-format off
#include "defines.hpp"
#include "model.cpp"
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
    Cube cube1, cube2, cube3;
    Init(cube1);
    Init(cube3);
    printf("%lu %lu\n", cube1.corners, cube1.edges);
    printf("sizeof(Cube) = %lu\n", sizeof(Cube));
    assert(!(cube1 == cube2));
    assert(cube1 == cube3);
    return 0;
}
