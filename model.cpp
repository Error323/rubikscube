/// A Rubik's Cube model (3x3x3) based on Richard E. Korf's paper.
///
///   Finding Optimal Solutions to Rubik's Cube
///          Using Pattern Databases
///
/// A cubie is an 1x1x1 cube on the rubiks cube with an index
/// and orientation. In a solved state the cube is facing you with
/// the orange side in front and yellow side on top. Equal to
/// https://ruwix.com/online-rubiks-cube-solver-program/
///
///                    Up
///
///                   Y Y Y
///                   Y Y Y
///                   Y Y Y
///
///            G G G  O O O  B B B  R R R
///      Left  G G G  O O O  B B B  R R R  Back
///            G G G  O O O  B B B  R R R
///
///                   W W W
///                   W W W
///                   W W W
///
///                   Down
///
/// Edge indices (12):
///
///       0  1  2  3  4  5  6  7  8  9  10 11
/// pos   UB UR UF UL FR FL BL BR DF DL DB DR
/// color YR YB YO YG OB OG RG RB WO WG WR WB
///
///
/// Corner indices (8):
///
///       0   1   2   3   4   5   6   7
/// pos   ULB URB URF ULF DLF DLB DRB DRF
/// color YGR YBR YBO YGO WGO WGR WBR WBO
/// world YXZ YXZ YXZ YXZ YXZ YXZ YXZ YXZ
///
/// U=Up, B=Back, L=Left, R=Right, F=Front, D=Down
/// R=Red, G=Green, B=Blue, W=White, Y=Yellow, O=Orange

// edges
enum Edge { UB, UR, UF, UL, FR, FL, BL, BR, DF, DL, DB, DR };

// corners
enum Corner { ULB, URB, URF, ULF, DLF, DLB, DRB, DRF };

// colors
enum Color { RED, GREEN, BLUE, WHITE, ORANGE, YELLOW };

struct Cube {
    // (3 + 3) * 8 + 5 = 53 bits
    //  |   |    |   |
    //  |   |    |   last move index
    //  |   |    nof corners
    //  |   orientation
    //  position
    u64 corners;

    // (4 + 1) * 12 = 60 bits
    //  |   |    |
    //  |   |    nof edges
    //  |   orientation
    //  position
    u64 edges;

    // 6 bits per corner: position(3), orientation(3)
    static constexpr u32 CORNER_BITS = 6;
    static constexpr u32 CORNER_POSITION = 0;
    static constexpr u32 CORNER_ORIENTATION = 3;
    // Last 5 bits after 8 corners (bits [48..52])
    static constexpr u32 LAST_MOVE_SHIFT = 48;
    static constexpr u64 LAST_MOVE_MASK = 0b11111ull;
    // 5 bits per edge: position(4), orientation(1)
    static constexpr u32 EDGE_BITS = 5;
    static constexpr u32 EDGE_POSITION = 0;
    static constexpr u32 EDGE_ORIENTATION = 4;

    // Helpers to extract corner sub-fields
    inline u32 GetCornerCubie(Corner corner) const {
        const u32 shift = corner * CORNER_BITS;
        constexpr u64 mask = 0b111111ull;
        return static_cast<u32>((corners >> shift) & mask);
    }

    inline void SetCornerCubie(Corner corner, u32 cubie) {
        const u32 shift = corner * CORNER_BITS;
        constexpr u64 mask = 0b111111ull;
        corners &= ~(mask << shift);
        corners |= (static_cast<u64>(cubie) & mask) << shift;
    }

    inline u32 GetCornerPos(Corner corner) const {
        const u32 shift = corner * CORNER_BITS + CORNER_POSITION;
        constexpr u64 mask = 0b111ull;
        return static_cast<u32>((corners >> shift) & mask);
    }

    inline void SetCornerPos(Corner corner, u32 pos) {
        const u32 shift = corner * CORNER_BITS + CORNER_POSITION;
        constexpr u64 mask = 0b111ull;
        corners &= ~(mask << shift);
        corners |= (static_cast<u64>(pos) & mask) << shift;
    }

    inline u32 GetCornerOri(Corner corner) const {
        const u32 shift = corner * CORNER_BITS + CORNER_ORIENTATION;
        constexpr u64 mask = 0b111ull;
        return static_cast<u32>((corners >> shift) & mask);
    }

    inline void SetCornerOri(Corner corner, u32 ori) {
        const u32 shift = corner * CORNER_BITS + CORNER_ORIENTATION;
        constexpr u64 mask = 0b111ull;
        corners &= ~(mask << shift);
        corners |= (static_cast<u64>(ori) & mask) << shift;
    }

    inline u32 GetLastMoveIndex() const {
        return static_cast<u32>((corners >> LAST_MOVE_SHIFT) & LAST_MOVE_MASK);
    }

    inline void SetLastMoveIndex(u32 move) {
        corners &= ~(LAST_MOVE_MASK << LAST_MOVE_SHIFT);
        corners |= (static_cast<u64>(move) & LAST_MOVE_MASK) << LAST_MOVE_SHIFT;
    }

    inline u32 GetEdgeCubie(Edge edge) const {
        const u32 shift = edge * EDGE_BITS;
        constexpr u64 mask = 0b11111ull;
        return static_cast<u32>((edges >> shift) & mask);
    }

    inline void SetEdgeCubie(Edge edge, u32 cubie) {
        const u32 shift = edge * EDGE_BITS;
        constexpr u64 mask = 0b11111ull;
        edges &= ~(mask << shift);
        edges |= (static_cast<u64>(cubie) & mask) << shift;
    }

    inline u32 GetEdgePos(Edge edge) const {
        const u32 shift = edge * EDGE_BITS + EDGE_POSITION;
        constexpr u64 mask = 0b1111ull;
        return static_cast<u32>((edges >> shift) & mask);
    }

    inline void SetEdgePos(Edge edge, u32 pos) {
        const u32 shift = edge * EDGE_BITS + EDGE_POSITION;
        constexpr u64 mask = 0b1111ull;
        edges &= ~(mask << shift);
        edges |= (static_cast<u64>(pos) & mask) << shift;
    }

    inline u32 GetEdgeOri(Edge edge) const {
        const u32 shift = edge * EDGE_BITS + EDGE_ORIENTATION;
        constexpr u64 mask = 0b1ull;
        return static_cast<u32>((edges >> shift) & mask);
    }

    inline void SetEdgeOri(Edge edge, u32 ori) {
        const u32 shift = edge * EDGE_BITS + EDGE_ORIENTATION;
        constexpr u64 mask = 0b1ull;
        edges &= ~(mask << shift);
        edges |= (static_cast<u64>(ori) & mask) << shift;
    }

    inline void EdgeFlipOri(Edge edge) {
        const u32 shift = edge * EDGE_BITS + EDGE_ORIENTATION;
        edges ^= 1ull << shift;
    }

    inline void Update(Corner corner, u32 n) {
        auto ori = GetCornerOri(corner);
        ori += n;
        ori %= 3;
        SetCornerOri(corner, ori);
    }

    bool operator==(const Cube &other) const {
        static const u64 edge_mask = 0xfffffffffffffff;
        static const u64 corner_mask = 0xffffffffffff;
        return (edges & edge_mask) == (other.edges & edge_mask) &&
               (corners & corner_mask) == (other.corners & corner_mask);
    }
};

internal void Init(Cube &c) {
    c.corners = c.edges = 0ull;

    for (s32 i = 0; i < 8; i++) {
        c.SetCornerPos(Corner(i), i);
    }

    for (s32 i = 0; i < 12; i++) {
        c.SetEdgePos(Edge(i), i);
    }
}

internal void CornerColor(Cube &c, Corner corner, s32 colors[3]) {
    // see scripts/corners.py
    static const u8 CO[2][3][2][3] = {{{{0, 1, 2}, {0, 2, 1}},
                                       {{1, 2, 0}, {1, 0, 2}},
                                       {{2, 0, 1}, {2, 1, 0}}},
                                      {{{0, 1, 2}, {0, 2, 1}},
                                       {{2, 0, 1}, {2, 1, 0}},
                                       {{1, 2, 0}, {1, 0, 2}}}};
    // see scripts/manhatten.py
    static const u64 kFlipped = 0x55aa55aa55aa55aaull;

    s32 tmp[] = {0, 0, 0};
    auto pos = c.GetCornerPos(corner);
    auto ori = c.GetCornerOri(corner);
    auto flipped = (kFlipped >> (pos * 8 + corner)) & 1;

    u8 o0 = CO[pos & 1][ori][flipped][0];
    u8 o1 = CO[pos & 1][ori][flipped][1];
    u8 o2 = CO[pos & 1][ori][flipped][2];

    // clang-format off
    switch (pos) {
        case ULB: tmp[0] = YELLOW; tmp[1] = GREEN; tmp[2] = RED; break;
        case URB: tmp[0] = YELLOW; tmp[1] =  BLUE; tmp[2] = RED; break;
        case URF: tmp[0] = YELLOW; tmp[1] =  BLUE; tmp[2] = ORANGE; break;
        case ULF: tmp[0] = YELLOW; tmp[1] = GREEN; tmp[2] = ORANGE; break;
        case DLF: tmp[0] =  WHITE; tmp[1] = GREEN; tmp[2] = ORANGE; break;
        case DLB: tmp[0] =  WHITE; tmp[1] = GREEN; tmp[2] = RED; break;
        case DRB: tmp[0] =  WHITE; tmp[1] =  BLUE; tmp[2] = RED; break;
        case DRF: tmp[0] =  WHITE; tmp[1] =  BLUE; tmp[2] = ORANGE; break;
        default: tmp[0] = tmp[1] = tmp[2] = WHITE; break;
    }
    // clang-format on

    colors[0] = tmp[o0];
    colors[1] = tmp[o1];
    colors[2] = tmp[o2];
}

internal void EdgeColor(Cube &c, Edge edge, s32 colors[2]) {
    // clang-format off
    switch (c.GetEdgePos(edge)) {
        case UB: colors[0] = YELLOW; colors[1] = RED;    break;
        case UR: colors[0] = YELLOW; colors[1] = BLUE;   break;
        case UF: colors[0] = YELLOW; colors[1] = ORANGE; break;
        case UL: colors[0] = YELLOW; colors[1] = GREEN;  break;
        case FR: colors[0] = ORANGE; colors[1] = BLUE;   break;
        case FL: colors[0] = ORANGE; colors[1] = GREEN;  break;
        case BL: colors[0] = RED;    colors[1] = GREEN;  break;
        case BR: colors[0] = RED;    colors[1] = BLUE;   break;
        case DF: colors[0] = WHITE;  colors[1] = ORANGE; break;
        case DL: colors[0] = WHITE;  colors[1] = GREEN;  break;
        case DB: colors[0] = WHITE;  colors[1] = RED;    break;
        case DR: colors[0] = WHITE;  colors[1] = BLUE;   break;
        default: colors[0] = colors[1] = WHITE; break;
    }
    // clang-format on

    if (c.GetEdgeOri(edge)) {
        Swap(colors[0], colors[1]);
    }
}

internal void PrettyPrint(Cube &c) {
    static const char *colors[] = {"\033[38;2;255;0;0m\uF0C8\033[0m",
                                   "\033[38;2;0;255;0m\uF0C8\033[0m",
                                   "\033[38;2;0;0;255m\uF0C8\033[0m",
                                   "\033[38;2;255;255;255m\uF0C8\033[0m",
                                   "\033[38;2;255;150;0m\uF0C8\033[0m",
                                   "\033[38;2;255;255;0m\uF0C8\033[0m"};

    s32 cc[3];
    u8 faces[54];

    for (s32 i = 0; i < 12; i++) {
        EdgeColor(c, Edge(i), cc);
        faces[i * 2 + 0] = cc[0];
        faces[i * 2 + 1] = cc[1];
    }

    for (s32 i = 0; i < 8; i++) {
        CornerColor(c, Corner(i), cc);
        faces[24 + i * 3 + 0] = cc[0];
        faces[24 + i * 3 + 1] = cc[1];
        faces[24 + i * 3 + 2] = cc[2];
    }

    for (s32 i = 0; i < 6; i++) {
        faces[48 + i] = i;
    }

    // 2d mapping of the cube, 54 faces, see scripts/map.py
    // clang-format off
    static const u8 map[] = {
                     24,  0, 27,
                     6, 53,  2,
                     33,  4, 30,

        25,  7, 34,  35,  5, 32,  31,  3, 28,  29,  1, 26,
        13, 49, 11,  10, 52,  8,   9, 50, 15,  14, 48, 12,
        40, 19, 37,  38, 17, 47,  46, 23, 43,  44, 21, 41,

                     36, 16, 45,
                     18, 51, 22,
                     39, 20, 42
    };
    // clang-format on

    printf("\n");
    for (s32 i = 0; i < 3; i++) {
        printf("        ");
        for (s32 j = 0; j < 3; j++) {
            printf("%s ", colors[faces[map[i * 3 + j]]]);
        }
        printf("\n");
    }
    printf("\n");
    for (s32 i = 0; i < 3; i++) {
        printf(" ");
        for (s32 j = 0; j < 12; j++) {
            if (j % 3 == 0 and j > 0) {
                printf(" %s ", colors[faces[map[9 + i * 12 + j]]]);
            } else {
                printf("%s ", colors[faces[map[9 + i * 12 + j]]]);
            }
        }
        printf("\n");
    }
    printf("\n");
    for (s32 i = 0; i < 3; i++) {
        printf("        ");
        for (s32 j = 0; j < 3; j++) {
            printf("%s ", colors[faces[map[9 + 3 * 12 + i * 3 + j]]]);
        }
        printf("\n");
    }
    printf("\n");
}

internal inline void L(Cube &c) {
    auto tmp = c.GetCornerCubie(DLB);
    c.SetCornerCubie(DLB, c.GetCornerCubie(DLF));
    c.SetCornerCubie(DLF, c.GetCornerCubie(ULF));
    c.SetCornerCubie(ULF, c.GetCornerCubie(ULB));
    c.SetCornerCubie(ULB, tmp);

    c.Update(DLB, 2);
    c.Update(DLF, 1);
    c.Update(ULF, 2);
    c.Update(ULB, 1);

    tmp = c.GetEdgeCubie(BL);
    c.SetEdgeCubie(BL, c.GetEdgeCubie(DL));
    c.SetEdgeCubie(DL, c.GetEdgeCubie(FL));
    c.SetEdgeCubie(FL, c.GetEdgeCubie(UL));
    c.SetEdgeCubie(UL, tmp);
}

internal inline void L2(Cube &c) {
    L(c);
    L(c);
}

internal inline void Lprime(Cube &c) {
    L(c);
    L(c);
    L(c);
}

internal inline void R(Cube &c) {
    auto tmp = c.GetCornerCubie(DRB);
    c.SetCornerCubie(DRB, c.GetCornerCubie(URB));
    c.SetCornerCubie(URB, c.GetCornerCubie(URF));
    c.SetCornerCubie(URF, c.GetCornerCubie(DRF));
    c.SetCornerCubie(DRF, tmp);

    c.Update(DRB, 1);
    c.Update(URB, 2);
    c.Update(URF, 1);
    c.Update(DRF, 2);

    tmp = c.GetEdgeCubie(BR);
    c.SetEdgeCubie(BR, c.GetEdgeCubie(UR));
    c.SetEdgeCubie(UR, c.GetEdgeCubie(FR));
    c.SetEdgeCubie(FR, c.GetEdgeCubie(DR));
    c.SetEdgeCubie(DR, tmp);
}

internal inline void R2(Cube &c) {
    R(c);
    R(c);
}

internal inline void Rprime(Cube &c) {
    R(c);
    R(c);
    R(c);
}

internal inline void U(Cube &c) {
    auto tmp = c.GetCornerCubie(ULF);
    c.SetCornerCubie(ULF, c.GetCornerCubie(URF));
    c.SetCornerCubie(URF, c.GetCornerCubie(URB));
    c.SetCornerCubie(URB, c.GetCornerCubie(ULB));
    c.SetCornerCubie(ULB, tmp);

    tmp = c.GetEdgeCubie(UL);
    c.SetEdgeCubie(UL, c.GetEdgeCubie(UF));
    c.SetEdgeCubie(UF, c.GetEdgeCubie(UR));
    c.SetEdgeCubie(UR, c.GetEdgeCubie(UB));
    c.SetEdgeCubie(UB, tmp);
}

internal inline void U2(Cube &c) {
    U(c);
    U(c);
}

internal inline void Uprime(Cube &c) {
    U(c);
    U(c);
    U(c);
}

internal inline void D(Cube &c) {
    auto tmp = c.GetCornerCubie(DLB);
    c.SetCornerCubie(DLB, c.GetCornerCubie(DRB));
    c.SetCornerCubie(DRB, c.GetCornerCubie(DRF));
    c.SetCornerCubie(DRF, c.GetCornerCubie(DLF));
    c.SetCornerCubie(DLF, tmp);

    tmp = c.GetEdgeCubie(DB);
    c.SetEdgeCubie(DB, c.GetEdgeCubie(DR));
    c.SetEdgeCubie(DR, c.GetEdgeCubie(DF));
    c.SetEdgeCubie(DF, c.GetEdgeCubie(DL));
    c.SetEdgeCubie(DL, tmp);
}

internal inline void D2(Cube &c) {
    D(c);
    D(c);
}

internal inline void Dprime(Cube &c) {
    D(c);
    D(c);
    D(c);
}

internal inline void F(Cube &c) {
    auto tmp = c.GetCornerCubie(ULF);
    c.SetCornerCubie(ULF, c.GetCornerCubie(DLF));
    c.SetCornerCubie(DLF, c.GetCornerCubie(DRF));
    c.SetCornerCubie(DRF, c.GetCornerCubie(URF));
    c.SetCornerCubie(URF, tmp);

    c.Update(ULF, 1);
    c.Update(DLF, 2);
    c.Update(DRF, 1);
    c.Update(URF, 2);

    tmp = c.GetEdgeCubie(UF);
    c.SetEdgeCubie(UF, c.GetEdgeCubie(FL));
    c.SetEdgeCubie(FL, c.GetEdgeCubie(DF));
    c.SetEdgeCubie(DF, c.GetEdgeCubie(FR));
    c.SetEdgeCubie(FR, tmp);

    c.EdgeFlipOri(UF);
    c.EdgeFlipOri(FL);
    c.EdgeFlipOri(DF);
    c.EdgeFlipOri(FR);
}

internal inline void F2(Cube &c) {
    F(c);
    F(c);
}

internal inline void Fprime(Cube &c) {
    F(c);
    F(c);
    F(c);
}

internal inline void B(Cube &c) {
    auto tmp = c.GetCornerCubie(ULB);
    c.SetCornerCubie(ULB, c.GetCornerCubie(URB));
    c.SetCornerCubie(URB, c.GetCornerCubie(DRB));
    c.SetCornerCubie(DRB, c.GetCornerCubie(DLB));
    c.SetCornerCubie(DLB, tmp);

    c.Update(ULB, 2);
    c.Update(URB, 1);
    c.Update(DRB, 2);
    c.Update(DLB, 1);

    tmp = c.GetEdgeCubie(UB);
    c.SetEdgeCubie(UB, c.GetEdgeCubie(BR));
    c.SetEdgeCubie(BR, c.GetEdgeCubie(DB));
    c.SetEdgeCubie(DB, c.GetEdgeCubie(BL));
    c.SetEdgeCubie(BL, tmp);

    c.EdgeFlipOri(UB);
    c.EdgeFlipOri(BR);
    c.EdgeFlipOri(DB);
    c.EdgeFlipOri(BL);
}

internal inline void B2(Cube &c) {
    B(c);
    B(c);
}

internal inline void Bprime(Cube &c) {
    B(c);
    B(c);
    B(c);
}

using MoveFunc = void (*)(Cube &);
static const char *kNames[] = {"R", "R2", "R'", "L", "L2", "L'",
                               "U", "U2", "U'", "D", "D2", "D'",
                               "F", "F2", "F'", "B", "B2", "B'"};
static const MoveFunc kMoves[] = {&R, &R2, &Rprime, &L, &L2, &Lprime,
                                  &U, &U2, &Uprime, &D, &D2, &Dprime,
                                  &F, &F2, &Fprime, &B, &B2, &Bprime};

// after the first move we do not allow moves of the same kind. E.g. after move
// R, we disable R,R',2R. We only allow moves of the opposing faces
// in one strict order. See scripts/genmoves.py
static const u32 kValidMoves[] = {
    0b111111111111111111, 0b111111111111111000, 0b111111111111111000,
    0b111111111111111000, 0b111111111111000000, 0b111111111111000000,
    0b111111111111000000, 0b111111111000111111, 0b111111111000111111,
    0b111111111000111111, 0b111111000000111111, 0b111111000000111111,
    0b111111000000111111, 0b111000111111111111, 0b111000111111111111,
    0b111000111111111111, 0b000000111111111111, 0b000000111111111111,
    0b000000111111111111};

internal void ApplyMove(Cube &c, s32 move) {
    kMoves[move](c);
    c.SetLastMoveIndex(move + 1);
}

template <s32 K>
internal u64 EdgeIndex(Cube &c, u32 start) {
    static PermutationIndexer<12, PICKED> indexer;
    assert(start + K <= 12);

    u8 perm[K];
    s32 n = 0;
    u64 orientation = 0;
    if (start == 0) {
        for (u32 i = 0; i < 12 && n != PICKED; i++) {
            u32 index = c.GetEdgePos(Edge(i));
            if (index < PICKED) {
                perm[index] = i;
                orientation <<= 1;
                orientation += c.GetEdgeOri(Edge(i));
                n++;
            }
        }
    } else {
        for (u32 i = 0; i < 12 && n != PICKED; i++) {
            u32 index = c.GetEdgePos(Edge(i));
            if (index >= 12 - PICKED) {
                perm[index - (12 - PICKED)] = i;
                orientation <<= 1;
                orientation += c.GetEdgeOri(Edge(i));
                n++;
            }
        }
    }

    u64 position = indexer.Index(perm);
    return position * (1ull << K) + orientation;
}

internal u64 CornerIndex(Cube &c) {
    static PermutationIndexer<8> indexer;
    u8 perm[8];
    for (s32 i = 0; i < 8; i++) {
        perm[i] = c.GetCornerPos(Corner(i));
    }

    u32 position = indexer.Index(perm);
    u32 orientation = 0;
    u32 n = 1;
    for (s32 i = 0; i < 7; i++) {
        orientation += c.GetCornerOri(Corner(i)) * n;
        n *= 3;
    }

    return position * 2187 + orientation;
}

internal u64 PermutationIndex(Cube &c) {
    static PermutationIndexer<12> indexer;
    u8 perm[12];
    for (s32 i = 0; i < 12; i++) {
        perm[i] = c.GetEdgePos(Edge(i));
    }
    return indexer.Index(perm);
}