#define FOUND 0
#define NOT_FOUND 255
internal Cube goal;
internal u64 nodes = 0;

internal u8 Heuristic(Cube root) {
    u64 index;
    u8 h = 0;
    index = EdgeIndex<PICKED>(root, 0);
    h = Max(edge1db.Get(index), h);
    index = EdgeIndex<PICKED>(root, 12 - PICKED);
    h = Max(edge2db.Get(index), h);
    index = CornerIndex(root);
    h = Max(cornerdb.Get(index), h);
    index = PermutationIndex(root);
    h = Max(permdb.Get(index), h);
    return h;
}

internal u8 Dfs(Cube *path, u8 g, u8 bound) {
    nodes++;
    u8 f = g + Heuristic(path[g]);
    if (f > bound) {
        return f;
    }
    if (path[g] == goal) {
        return FOUND;
    }

    u8 min = NOT_FOUND, t = NOT_FOUND;
    u32 valid = kValidMoves[path[g].GetLastMoveIndex()];
    while (valid) {
        s32 move = __builtin_ffs(valid) - 1;
        valid &= valid - 1;
        path[g + 1] = path[g];
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
    timespec start, end;
    Cube path[20];
    u8 bound = Heuristic(root);
    path[0] = root;
    while (true) {
        nodes = 0;
        clock_gettime(CLOCK_MONOTONIC, &start);
        u8 t = Dfs(path, 0, bound);
        clock_gettime(CLOCK_MONOTONIC, &end);
        f64 elapsed = Timespec2Sec(&end) - Timespec2Sec(&start);
        printf("T%0.3f N/s:%lu N:%lu\n", elapsed, u64(nodes/elapsed), nodes);
        if (t == FOUND) {
            // print path
            s32 depth = 1;
            while (true) {
                auto move = path[depth].GetLastMoveIndex() - 1;
                printf("%s ", kNames[move]);
                kMoves[move](root);
                if (path[depth] == goal) {
                    break;
                }
                depth++;
            }
            printf("(%d)\n", depth);
            PrettyPrint(root);
            return true;
        }
        if (t == NOT_FOUND) {
            return false;
        }
        bound = t;
    }
    return false;
}
