#pragma once

#include <stdint.h>

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
#define KiB(x) ((x)*1024ull)
#define MiB(x) ((x)*1024ull*1024ull)
#define GiB(x) ((x)*1024ull*1024ull*1024ull)
