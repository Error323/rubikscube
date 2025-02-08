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
//#include "dfs.cpp"
#include "deque.cpp"
#include "bfs.cpp"
// clang-format on

s32 main(void) {
    u64 csize = Factorial(8) * Power(3, 7);
    u64 esize = (Factorial(12) / Factorial(12 - PICKED)) * Power(2, PICKED);
    printf("2x 12P%d edge db size = %lluMiB\n", PICKED, esize/MiB(1)/2);
    printf("8! * 3^7 corner db size = %lluMiB\n", csize/MiB(1)/2);
    //cornerdb.Init(csize);
    edge1db.Init(esize);
    //edge2db.Init(esize);

    if (!Bfs()) {
        printf("out of memory\n");
        return 1;
    }

    //Iddfs();
    u64 sum = 0;
    for (u32 i = 0; i < cornerdb.num_entries; i++) {
        //sum += cornerdb.Get(i);
    }
    //printf("corner mean = %0.3f\n", sum / double(cornerdb.num_entries));
    sum = 0;
    for (u32 i = 0; i < edge1db.num_entries; i++) {
        u8 v = edge1db.Get(i);
        if (v == 0xf) {
            printf("error val\n");
        }
        sum += v;
    }
    printf("edge1 mean =  %0.3f\n", sum / double(edge1db.num_entries));
    sum = 0;
    for (u32 i = 0; i < edge2db.num_entries; i++) {
        //sum += edge2db.Get(i);
    }
    //printf("edge2 mean =  %0.3f\n", sum / double(edge2db.num_entries));

    //auto cornerpath = "data/corner.db";
    auto edge1path = "data/edge1.db";
    //auto edge2path = "data/edge2.db";
    //cornerdb.Write(cornerpath);
    edge1db.Write(edge1path);
    //edge2db.Write(edge2path);

    return 0;
}
