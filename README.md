# ce - Common Environment for c++

- supports clang, gcc and msvc
- no dependancies
- header only
- non-allocating
- good code generation in non-optimized builds

## Features
- basic types
- critical type traits
- `types<Ts...>` - list of types
- `items<T, T...Ns>` - list of scalar values
- `tuple<Ts...>` - basic tuple support, `box<T>`, `pair<T, U>`
- `vec<T, N>` - small simd style vectors
- `list<T, N>` - sized c-style array
- `span<T>` - contiguous range of `T*`
- `crc32_t<P>` - constexpr type safe crc32 on Polynomial `P`
- `crc32c_t` crc32 using ISO crc32c polynomial
- `xoroshiro64ss` - easy, good, small random number generator xoroshiro64**
