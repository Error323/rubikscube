internal PermutationIndexer<8> ci;
internal PermutationIndexer<12, PICKED> ei;
internal Database cornerdb;
internal Database edge1db;
internal Database edge2db;
internal u64 nodes{0};

internal u64 Dfs(Cube cube, u8 depth, u8 maxdepth) {
    nodes++;
    u32 i1 = CornerIndex(ci, cube);
    u32 i2 = EdgeIndex<PICKED>(ei, cube, 0);
    u32 i3 = EdgeIndex<PICKED>(ei, cube, 12 - PICKED);
    auto b1 = cornerdb.Update(i1, depth);
    auto b2 = edge1db.Update(i2, depth);
    auto b3 = edge2db.Update(i3, depth);

    if (depth == maxdepth || b1 + b2 + b3 == 0) {
        return 1;
    }

    u32 valid = kValidMoves[cube.GetLastMoveIndex()];
    u64 sum = 0;
    Cube tmp = cube;
    while (valid) {
        s32 move = __builtin_ffs(valid) - 1;
        valid &= valid - 1;
        ApplyMove(cube, move);
        sum += Dfs(cube, depth + 1, maxdepth);
        cube = tmp;
    }

    return sum;
}

internal void Iddfs() {
    Cube root;
    Init(root);
    for (u8 depth = 0; depth < 12; depth++) {
        Dfs(root, 0, depth);
        u64 sum = 0;
        for (u32 j = 0; j < cornerdb.num_entries; j++) {
            sum += cornerdb.Get(j) == 0xf;
        }
        for (u32 j = 0; j < edge1db.num_entries; j++) {
            sum += edge1db.Get(j) == 0xf;
        }
        for (u32 j = 0; j < edge2db.num_entries; j++) {
            sum += edge2db.Get(j) == 0xf;
        }
        printf("D:%02u TODO:%lu N:%lu\n", depth, sum, nodes);
        if (sum == 0) {
            break;
        }
    }
}
