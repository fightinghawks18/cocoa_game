#pragma once

#include <cstdint>
#include <limits>

using i64 = int64_t;
using u64 = uint64_t;

using f32 = float;
using i32 = int32_t;
using u32 = uint32_t;

using i16 = int16_t;
using u16 = uint16_t;

using i8 = int8_t;
using u8 = uint8_t;

using usize = size_t;

constexpr i64 i64Max = std::numeric_limits<i64>::max();
constexpr u64 u64Max = std::numeric_limits<u64>::max();

constexpr f32 f32Max = std::numeric_limits<f32>::max();
constexpr i32 i32Max = std::numeric_limits<i32>::max();
constexpr u32 u32Max = std::numeric_limits<u32>::max();

constexpr i16 i16Max = std::numeric_limits<i16>::max();
constexpr u16 u16Max = std::numeric_limits<u16>::max();

constexpr i8 i8Max = std::numeric_limits<i8>::max();
constexpr u8 u8Max = std::numeric_limits<u8>::max();