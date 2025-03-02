#define FOUND 0
#define NOT_FOUND 255
internal Cube goal;
internal u64 nodes = 0;

internal u8 Heuristic(Cube cube, u8 g, u8 bound) {
    // we stop early if we exceed the bound. The databases are sorted from high
    // to low on their mean value
    u64 index;
    index = CornerIndex(cube);
    u8 h = cornerdb.Get(index);
    if (g + 1 + h > bound) {
        return h;
    }
    index = EdgeIndex<PICKED>(cube, 0);
    h = Max(edge1db.Get(index), h);
    if (g + 1 + h > bound) {
        return h;
    }
    index = EdgeIndex<PICKED>(cube, 12 - PICKED);
    h = Max(edge2db.Get(index), h);
    if (g + 1 + h > bound) {
        return h;
    }
    index = PermutationIndex(cube);
    h = Max(permdb.Get(index), h);
    return h;
}

internal void MoveBestToFront(u8 *moves, u8 *heuristic, s32 i, s32 n) {
    u8 *first_move = moves + i;
    u8 *first_h = heuristic + i;
    u8 *best_h = first_h;
    u8 *best_move = first_move;

    for (s32 j = i + 1; j < n; j++) {
        if (heuristic[j] < *best_h) {
            best_h = heuristic + j;
            best_move = moves + j;
        }
    }

    Swap(*first_move, *best_move);
    Swap(*first_h, *best_h);
}

internal u8 Dfs(Cube *path, u8 g, u8 bound) {
    if (path[g] == goal) {
        return FOUND;
    }

    u8 min = NOT_FOUND, t = NOT_FOUND;
    u32 valid = kValidMoves[path[g].GetLastMoveIndex()];
    u8 heuristic[18];
    u8 moves[18];
    s32 n = 0;

    // obtain all valid moves and their corresponding heuristic
    while (valid) {
        s32 move = __builtin_ffs(valid) - 1;
        moves[n] = move;
        path[g + 1] = path[g];
        ApplyMove(path[g + 1], move);
        heuristic[n] = Heuristic(path[g + 1], g, bound);
        valid &= valid - 1;
        n++;
    }

    nodes += n;

    // shift the best move to the front, best being the lowest h-cost move
    for (s32 i = 0; i < n; i++) {
        MoveBestToFront(moves, heuristic, i, n);
        if (g + 1 + heuristic[i] > bound) {
            return g + 1 + heuristic[i];
        }
        path[g + 1] = path[g];
        ApplyMove(path[g + 1], moves[i]);
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
    u8 bound = Heuristic(root, 0, 255);
    path[0] = root;
    while (true) {
        nodes = 0;
        clock_gettime(CLOCK_MONOTONIC, &start);
        u8 t = Dfs(path, 0, bound);
        clock_gettime(CLOCK_MONOTONIC, &end);
        f64 elapsed = Timespec2Sec(&end) - Timespec2Sec(&start);
        printf("T%5.3f B:%02u N/s:%'lu N:%'lu\n", elapsed, bound,
               u64(nodes / elapsed), nodes);
        if (t == FOUND) {
            // print path
            s32 depth = 1;
            printf("\n");
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
