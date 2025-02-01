struct Cube {
    // (3 + 3 + 1) * 8 + 5 = 61 bits
    //  |   |   |    |   |
    //  |   |   |    |   last move index
    //  |   |   |    nof corners
    //  |   |   flipped
    //  |   orientation
    //  position
    u64 corners;

    // (4 + 1) * 12 = 60 bits
    //  |   |    |
    //  |   |    nof edges
    //  |   orientation
    //  position
    u64 edges;

    bool operator==(const Cube &other) const {
        static const u64 edge_mask = 0xfffffffffffffff;
        static const u64 corner_mask = 0xffffffffffffff;
        return (edges & edge_mask) == (other.edges & edge_mask) &&
               (corners & corner_mask) == (other.corners & corner_mask);
    }
};

internal void Init(Cube &c) {
    c.corners = c.edges = 0ull;

    for (s32 i = 0; i < 8; i++) {
        c.corners |= (i << i * 8);
    }

    for (s32 i = 0; i < 12; i++) {
        c.edges |= (i << i * 5);
    }
}

/*
internal void Print(Cube &c) {}

internal void L(Cube &c) {}

internal void R(Cube &c) {}

internal void U(Cube &c) {}

internal void D(Cube &c) {}

internal void F(Cube &c) {}

internal void B(Cube &c) {}

internal void Li(Cube &c) {}

internal void Ri(Cube &c) {}

internal void Ui(Cube &c) {}

internal void Di(Cube &c) {}

internal void Fi(Cube &c) {}

internal void Bi(Cube &c) {}

internal void LL(Cube &c) {}

internal void RR(Cube &c) {}

internal void UU(Cube &c) {}

internal void DD(Cube &c) {}

internal void FF(Cube &c) {}

internal void BB(Cube &c) {}
*/