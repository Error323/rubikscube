internal bool Bfs(Cube root, Cube goal) {
    static const u64 maxsize = GiB(6) / sizeof(Cube);
    Deque<Cube> q(maxsize);
    q.Push(root);
    s64 depth = 0;
    s64 explored = 1;
    while (q.Size() > 0) {
        depth++;
        u64 size = q.Size();

        for (u64 i = 0; i < size; i++) {
            Cube &cube = q.Pop();
            u32 valid = kValidMoves[cube.GetLastMoveIndex()];
            while (valid) {
                explored++;
                s32 move = __builtin_ffs(valid) - 1;
                valid &= valid - 1;
                Cube next = cube;
                next.from = &cube;
                ApplyMove(next, move);
                if (next == goal) {
                    printf("solution(%lu): ", depth);
                    while (next.from) {
                        printf("%s ", kNames[next.GetLastMoveIndex() - 1]);
                        next = *next.from;
                    }
                    printf("\n");
                    printf("%lu/%lu\n", explored, maxsize);
                    return true;
                }
                if (q.Size() == maxsize) {
                    return false;
                }
                q.Push(next);
            }
        }
    }
    return false;
}