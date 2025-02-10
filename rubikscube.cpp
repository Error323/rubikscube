#include <sys/mman.h>

#include <cassert>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#define PICKED 6

// clang-format off
#include "utils.cpp"
#include "database.cpp"
#include "indexer.cpp"
#include "model.cpp"
#include "deque.cpp"
#include "bfs.cpp"
// clang-format on

s32 main(void) {
    u64 csize = Factorial(8) * Power(3, 7);
    u64 esize = (Factorial(12) / Factorial(12 - PICKED)) * Power(2, PICKED);
    u64 psize = Factorial(12);
    printf("2x 12P%d edge db size = %lluMiB\n", PICKED, esize / MiB(1) / 2);
    printf("8! * 3^7 corner db size = %lluMiB\n", csize / MiB(1) / 2);
    printf("12! permutation db size = %lluMiB\n", psize / MiB(1) / 2);
    edge1db.Init(esize);
    edge2db.Init(esize);
    cornerdb.Init(csize);
    permdb.Init(psize);

    if (!Bfs(Max(permdb.num_entries, Max(edge1db.num_entries, cornerdb.num_entries)))) {
        fprintf(stderr, "not enough memory\n");
        return 1;
    }

    u64 sum = 0;
    for (u32 i = 0; i < cornerdb.num_entries; i++) {
        u8 v = cornerdb.Get(i);
        sum += v;
    }
    printf("corner mean = %0.3f\n", sum / double(cornerdb.num_entries));

    sum = 0;
    for (u32 i = 0; i < edge1db.num_entries; i++) {
        u8 v = edge1db.Get(i);
        sum += v;
    }
    printf("edge1 mean = %0.3f\n", sum / double(edge1db.num_entries));

    sum = 0;
    for (u32 i = 0; i < edge2db.num_entries; i++) {
        u8 v = edge2db.Get(i);
        sum += v;
    }
    printf("edge2 mean = %0.3f\n", sum / double(edge2db.num_entries));

    sum = 0;
    for (u32 i = 0; i < permdb.num_entries; i++) {
        u8 v = permdb.Get(i);
        sum += v;
    }
    printf("perm mean = %0.3f\n", sum / double(permdb.num_entries));

    auto cornerpath = "data/corner.db";
    auto edge1path = "data/edge1.db";
    auto edge2path = "data/edge2.db";
    auto permpath = "data/perm.db";
    cornerdb.Write(cornerpath);
    edge1db.Write(edge1path);
    edge2db.Write(edge2path);
    permdb.Write(permpath);

    return 0;
}
