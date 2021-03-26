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
- `span<T>` - contiguous range of `T*`
- `list<T, N>` - sized c-style array
- `crc32_t<P>` - constexpr type safe crc32 on Polynomial `P`
- `crc32c_t` - crc32 using ISO crc32c polynomial
- `vec<N, T, Ks...>` - small simd style vectors `N` = [0, 4] with optional unit tags `Ks...`
- `sqrx()`, `dotx()`, and `crossx()` - operations on scalars and `vec`s which expand integer types by 2x bits
- `in_polygon_xy()` and `in_polygon()` - generic point in polygon test (winding number) for types with `.x` & `.y` members
- `xoroshiro64ss` - easy, good, small random number generator **xoroshiro64\*\*** http://prng.di.unimi.it/xoroshiro64starstar.c
## Macros
- `CE_STATIC_ASSERT()` - pre c++17 static_assert with optional and better error messges
- `CE_DEBUG_BREAK()` - break to debugger
- `CE_TIME_STAMP()` - high resolution time stamp counter
- `CE_ROTL32()` - rotate left intrinsic
- `CE_STRLEN()` - strlen intrinsic
- `CE_ERROR()`, `CE_ASSERT()`, `CE_VERIFY()`, and `CE_FAILED()` - runtime error checking
- `CE_COUNTOF()` - compile time array extent
- `CE_FOLD_LEFT_COMMA()` - pre c++17 left comma fold expression
- `CE_CPU_X86`, `CE_CPU_X86_32`, `CE_CPU_X86_64` - x86 compile time cpu detection
- `CE_CPU_WASM`, `CE_CPU_WASM_32`, `CE_CPU_WASM_64` - WebAssembly compile time cpu detection
