#define FOUND 0
#define NOT_FOUND 255
internal Cube goal;

internal u8 Heuristic(Cube root) {
    u64 index;
    u8 h = 0;
    index = EdgeIndex<PICKED>(ei, root, 0);
    h = Max(edge1db.Get(index), h);
    index = EdgeIndex<PICKED>(ei, root, 12 - PICKED);
    h = Max(edge2db.Get(index), h);
    index = CornerIndex(ci, root);
    h = Max(cornerdb.Get(index), h);
    index = PermutationIndex(pi, root);
    h = Max(permdb.Get(index), h);
    return h;
}

internal u8 Dfs(Cube *path, u8 g, u8 bound) {
    Cube cube = path[g];
    u8 f = g + Heuristic(cube);
    if (f > bound) {
        return f;
    }
    if (cube == goal) {
        return FOUND;
    }

    u8 min = NOT_FOUND, t = NOT_FOUND;
    u32 valid = kValidMoves[cube.GetLastMoveIndex()];
    while (valid) {
        s32 move = __builtin_ffs(valid) - 1;
        valid &= valid - 1;
        path[g + 1] = cube;
        ApplyMove(path[g + 1], move);
        t = Dfs(path, g + 1, bound);
        if (t == FOUND) {
            return FOUND;
        }
        min = Min(t, min);
    }
    return min;
}

internal bool IDAStar(Cube root) {
    Cube path[20];
    u8 bound = Heuristic(root);
    path[0] = root;
    for (s32 i = 0; i < 20; i++) {
        u8 t = Dfs(path, 0, bound);
        if (t == FOUND) {
            // print path, bound
            while (i) {
                auto move = path[i].GetLastMoveIndex();
                printf("%s ", kNames[move]);
                i--;
            }
            printf("\n");
            return true;
        }
        if (t == NOT_FOUND) {
            return false;
        }
        bound = t;
    }
    return false;
}