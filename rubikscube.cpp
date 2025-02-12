#include <sys/mman.h>
#include <sys/stat.h>

#include <cassert>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#define PICKED 7

// clang-format off
#include "utils.cpp"
#include "database.cpp"
#include "indexer.cpp"
#include "model.cpp"
#include "deque.cpp"
#include "bfs.cpp"
#include "search.cpp"
// clang-format on

s32 main(s32 argc, char *argv[]) {
    s32 n = 10;
    if (argc > 1) {
        n = atoi(argv[1]);
    }
    srand(1337);

    auto cornerpath = "data/corner.db";
    auto edge1path = "data/edge1.db";
    auto edge2path = "data/edge2.db";
    auto permpath = "data/perm.db";
    u64 csize = Factorial(8) * Power(3, 7);
    u64 esize = (Factorial(12) / Factorial(12 - PICKED)) * Power(2, PICKED);
    u64 psize = Factorial(12);

    if (access("data", R_OK|W_OK|X_OK) != 0) {
        fprintf(stderr, "Directory 'data' must exist and be accessible\n");
        return 1;
    }

    if (access(cornerpath, F_OK) == 0 && access(edge1path, F_OK) == 0 &&
        access(edge2path, F_OK) == 0 && access(permpath, F_OK) == 0) {
        Cube root;
        Init(goal);
        Init(root);
        edge1db.Init(esize, Database::EDGE1);
        edge2db.Init(esize, Database::EDGE2);
        cornerdb.Init(csize, Database::CORNER);
        permdb.Init(psize, Database::PERMUTATION);
        bool loaded = false;
        loaded &= edge1db.Load(edge1path);
        loaded &= edge2db.Load(edge2path);
        loaded &= cornerdb.Load(cornerpath);
        loaded &= permdb.Load(permpath);
        if (!loaded) {
            return 1;
        }
        // scramble the cube
        for (s32 i = 0; i < n; i++) {
            s32 move = rand() % 18;
            printf("%s ", kNames[move]);
            kMoves[move](root);
        }
        printf("\n");
        PrettyPrint(root);
        // solve the cube
        IDAStar(root);
    } else {
        printf("2x 12P%d edge db size = %lluMiB\n", PICKED, esize / MiB(1) / 2);
        printf("8! * 3^7 corner db size = %lluMiB\n", csize / MiB(1) / 2);
        printf("12! permutation db size = %lluMiB\n", psize / MiB(1) / 2);

        Database db[] = {cornerdb, edge1db, edge2db, permdb};
        u64 sz[] = {csize, esize, esize, psize};
        const char *names[] = {"corner", "edge1", "edge2", "permutation"};
        const char *paths[] = {cornerpath, edge1path, edge2path, permpath};
        Indexer indexer[] = {
            [](Cube &c) { return CornerIndex(c); },
            [](Cube &c) { return EdgeIndex<PICKED>(c, 0); },
            [](Cube &c) { return EdgeIndex<PICKED>(c, 12 - PICKED); },
            [](Cube &c) { return PermutationIndex(c); }};

        for (s32 i = 0; i < 4; i++) {
            printf("Generating '%s'\n", names[i]);
            db[i].Init(sz[i], Database::Type(i));
            if (!Bfs(db[i], indexer[i])) {
                fprintf(stderr, "not enough memory\n");
                return 1;
            }

            u64 sum = 0;
            for (u32 j = 0; j < db[i].hdr.num_entries; j++) {
                u8 v = db[i].Get(j);
                sum += v;
            }
            printf("%s mean = %0.3f\n", names[i],
                   sum / double(db[i].hdr.num_entries));
            db[i].Write(paths[i]);
        }
    }

    return 0;
}
