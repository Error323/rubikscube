internal Database cornerdb;
internal Database edge1db;
internal Database edge2db;
internal Database permdb;


internal bool Bfs(u64 maxsize) {
    timespec start, end;
    maxsize *= 2;
    Deque<Cube> q(maxsize);
    Cube root;
    Init(root);
    q.Push(root);
    s64 depth = 0;
    s64 nodes = 1;
    s64 todo = 1;
    u64 index = 0;

    index = EdgeIndex<PICKED>(root, 0);
    edge1db.Update(index, depth);
    index = EdgeIndex<PICKED>(root, 12 - PICKED);
    edge2db.Update(index, depth);
    index = CornerIndex(root);
    cornerdb.Update(index, depth);
    index = PermutationIndex(root);
    permdb.Update(index, depth);
    while (q.Size() > 0 && todo > 0) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        depth++;
        u64 size = q.Size();

        for (u64 i = 0; i < size; i++) {
            Cube &cube = q.Pop();
            u32 valid = kValidMoves[cube.GetLastMoveIndex()];
            while (valid) {
                if (q.Size() == maxsize) {
                    return false;
                }
                nodes++;
                s32 move = __builtin_ffs(valid) - 1;
                valid &= valid - 1;
                Cube next = cube;
                ApplyMove(next, move);
                bool updated = false;
                index = EdgeIndex<PICKED>(next, 0);
                updated |= edge1db.Update(index, depth);
                index = EdgeIndex<PICKED>(next, 12 - PICKED);
                updated |= edge2db.Update(index, depth);
                index = CornerIndex(next);
                updated |= cornerdb.Update(index, depth);
                index = PermutationIndex(next);
                updated |= permdb.Update(index, depth);
                if (updated) {
                    q.Push(next);
                }
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &end);

        todo = 0;
        for (u32 j = 0; j < edge1db.num_entries; j++) {
            todo += edge1db.Get(j) == 0xf;
        }
        for (u32 j = 0; j < edge2db.num_entries; j++) {
            todo += edge2db.Get(j) == 0xf;
        }
        for (u32 j = 0; j < cornerdb.num_entries; j++) {
            todo += cornerdb.Get(j) == 0xf;
        }
        for (u32 j = 0; j < permdb.num_entries; j++) {
            todo += permdb.Get(j) == 0xf;
        }

        double elapsed = Timespec2Sec(&end) - Timespec2Sec(&start);
        printf("Depth:%02lu MiB:%04lu Todo:%lu Nodes:%lu Time:%0.3f Nps:%0.1f\n", depth,
               q.Size() * sizeof(Cube) / u32(MiB(1)), todo, nodes, elapsed, size/elapsed);
    }
    return true;
}
