#pragma once

#include <cstdint>

using f64 = double;
using f32 = float;
using s64 = int64_t;
using s32 = int32_t;
using s16 = int16_t;
using s8 = int8_t;
using u64 = uint64_t;
using u32 = uint32_t;
using u16 = uint16_t;
using u8 = uint8_t;

#define internal static

#define KiB(x) ((x) * 1024ull)
#define MiB(x) ((x) * 1024ull * 1024ull)
#define GiB(x) ((x) * 1024ull * 1024ull * 1024ull)

template <typename T>
internal void Swap(T& a, T& b) {
    T tmp = a;
    a = b;
    b = tmp;
}

template <typename T>
internal T RoundUp(T x, T y) {
    if (x % y == T(0)) {
        return x;
    }
    return (x / y) * y + y;
}

template <typename T>
internal T Min(T a, T b) {
    return a < b ? a : b;
}

template <typename T>
internal T Max(T a, T b) {
    return a > b ? a : b;
}

internal inline f64 Timespec2Sec(const struct timespec* ts) {
    return (f64)ts->tv_sec + (f64)ts->tv_nsec / 1.0e9;
}