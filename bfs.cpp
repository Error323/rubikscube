internal Database cornerdb;
internal Database edge1db;
internal Database edge2db;
internal Database permdb;

using Indexer = u64 (*)(Cube &c);

internal bool Bfs(Database &db, Indexer indexer) {
    timespec start, end;
    Deque<Cube> q(db.hdr.num_entries);
    Cube root;
    Init(root);
    q.Push(root);
    s64 depth = 0;
    s64 nodes = 1;
    s64 todo = 1;

    db.Update(indexer(root), depth);
    while (q.Size() > 0 && todo > 0) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        depth++;
        u64 size = q.Size();

        for (u64 i = 0; i < size; i++) {
            Cube &cube = q.Pop();
            u32 valid = kValidMoves[cube.GetLastMoveIndex()];
            while (valid) {
                if (q.Size() == db.hdr.num_entries) {
                    return false;
                }
                s32 move = __builtin_ffs(valid) - 1;
                valid &= valid - 1;
                Cube next = cube;
                ApplyMove(next, move);
                if (db.Update(indexer(next), depth)) {
                    q.Push(next);
                }
            }
        }

        nodes += size;
        clock_gettime(CLOCK_MONOTONIC, &end);

        todo = 0;
        for (u32 j = 0; j < db.hdr.num_entries; j++) {
            todo += db.Get(j) == 0xf;
        }

        double elapsed = Timespec2Sec(&end) - Timespec2Sec(&start);
        printf(
            "Depth:%02lu MiB:%04lu Time:%0.3f Todo:%'lu Nodes:%'lu Nps:%'0lu\n",
            depth, q.Size() * sizeof(Cube) / u32(MiB(1)), elapsed, todo, nodes,
            u64(size / elapsed));
    }
    return true;
}
