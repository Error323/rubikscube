/// This work is taken from Benjamin Botto's work on database indexing at:
/// https://medium.com/@benjamin.botto/sequentially-indexing-permutations-a-linear-algorithm-for-computing-lexicographic-rank-a22220ffd6e3
/// and adapted to suit our needs
u64 Factorial(s32 n) {
    if (n <= 1) {
        return 1;
    }

    s32 f = n;
    while (n > 1) {
        f *= --n;
    }

    return f;
}

u64 Pick(s32 n, s32 k) { return Factorial(n) / Factorial(n - k); }

u64 Power(s32 x, s32 y) {
    if (y == 0) {
        return 1;
    }

    s32 p = x;
    while (--y) {
        x *= p;
    }

    return x;
}

template <u32 N, u32 K = N>
class PermutationIndexer {
    u32 bitcount_[(1 << N) - 1]{};
    u32 factorials_[K]{};

   public:
    PermutationIndexer() {
        for (u32 i = 0; i < (1 << N) - 1; ++i) {
            bitcount_[i] = __builtin_popcount(i);
        }

        for (u32 i = 0; i < K; ++i) {
            factorials_[i] = Pick(N - 1 - i, K - 1 - i);
        }
    }

    u64 Index(const u8 perm[K]) const {
        // This will hold the Lehmer code (in a factorial number system).
        u32 lehmer[K];
        // Set of "seen" digits in the permutation.
        u32 seen = 0ul;
        // The first digit of the Lehmer code is always the first digit of
        // the permutation.
        lehmer[0] = perm[0];
        // Mark the digit as seen (bitset uses right-to-left indexing).
        seen |= 1u << (N - 1 - perm[0]);

        for (u32 i = 1; i < K; ++i) {
            seen |= 1u << (N - 1 - perm[i]);

            // The number of "seen" digits to the left of this digit is the
            // count of ones left of this digit.
            u32 count = bitcount_[seen >> (N - perm[i])];
            lehmer[i] = perm[i] - count;
        }

        // Convert the Lehmer code to base-10.
        u64 index = 0;

        for (u32 i = 0; i < K; ++i) {
            index += lehmer[i] * factorials_[i];
        }

        return index;
    }
};