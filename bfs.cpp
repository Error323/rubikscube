internal Database cornerdb;
internal Database edge1db;
internal Database edge2db;
internal Database permdb;

using Indexer = u64 (*)(Cube &c);
internal bool Bfs(Database &db, Indexer indexer) {
    timespec start, end;
    Deque<Cube> q(db.num_entries);
    Cube root;
    Init(root);
    q.Push(root);
    s64 depth = 0;
    s64 nodes = 1;
    s64 todo = 1;
    u64 index = 0;

    index = indexer(root);
    db.Update(index, depth);
    while (q.Size() > 0 && todo > 0) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        depth++;
        u64 size = q.Size();

        for (u64 i = 0; i < size; i++) {
            Cube &cube = q.Pop();
            u32 valid = kValidMoves[cube.GetLastMoveIndex()];
            while (valid) {
                if (q.Size() == db.num_entries) {
                    return false;
                }
                nodes++;
                s32 move = __builtin_ffs(valid) - 1;
                valid &= valid - 1;
                Cube next = cube;
                ApplyMove(next, move);
                if (db.Update(indexer(next), depth)) {
                    q.Push(next);
                }
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &end);

        todo = 0;
        for (u32 j = 0; j < db.num_entries; j++) {
            todo += db.Get(j) == 0xf;
        }

        double elapsed = Timespec2Sec(&end) - Timespec2Sec(&start);
        printf(
            "Depth:%02lu MiB:%04lu Todo:%lu Nodes:%lu Time:%0.3f Nps:%0.1f\n",
            depth, q.Size() * sizeof(Cube) / u32(MiB(1)), todo, nodes, elapsed,
            size / elapsed);
    }
    return true;
}
