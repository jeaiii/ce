# ce - Common Environment for c++
![windows](https://github.com/jeaiii/ce/actions/workflows/build-windows.yml/badge.svg)
![linux](https://github.com/jeaiii/ce/actions/workflows/build-linux.yml/badge.svg)

- supports clang, msvc, and gcc
- supports x86, WebAssembly, and ARM (TODO)
- non-allocating
- good code generation in non-optimized builds

## Features
### ce.h
- 0 depenacies
- header only
#### basic types compatible with `std`
- `uint`, `size_t`, `ptrdiff_t`
- `uint8_t`, `uint16_t`, `uint32_t`, `uint64_t`
- `int8_t`, `int16_t`, `int32_t`, `int64_t` 
#### critical type traits and templates
- `types<Ts...>` - list of types
- `items<T, T...Ns>` - list of scalar values
- `sequence_t<T, N>` - the type `items<T, T{ 0 }, T{ 1 }, ..., T{ N - 1 }>` using log N compile time recursion
#### fixed size containers and view
- `tuple<Ts...>` - basic tuple support, `box<T>`, `pair<T, U>`
- `span<T>` - contiguous range of `T*`
- `bulk<N, T>` - fixed sized container of [0, `N`) `T`'s
#### simd style vectors and math utlities
- `vec<N, T, Ks...>` - small simd style vectors `N` = [0, 4] with optional unit tags `Ks...`
- `sqrx()`, `dotx()`, and `crossx()` - operations on scalars and `vec`'s which expand types by 2x bits
- `in_polygon_xy()` and `in_polygon()` - generic point in polygon test (winding number) for types with `.x` & `.y` members
- `crc32` - `constexpr` fast, header only, type safe  32 bit cyclic redundancy check
- `fnv1a` - `constexpr` fnv hashing
- `xoroshiro64ss` - easy, good, compact random number generator http://prng.di.unimi.it/xoroshiro64starstar.c
- `base64` - `constexpr` base64 decode
#### type to text conversion
- `to_text(char *, Ts...)` - convert any typed value to text (non terminated utf-8)
- `as_string` - form a fixed size null terminated array of utf-8 chars "inline"
#### basic os functions
- `debug_out()` - output to the debugger
- `monotonic_timestamp()` - monotonic non stop timestamp
- `monotonic_frequency()` - frequency of monotonic non stop timestamp
- `open_file()`
- `close_file()`
- `map_span()` - memory map a file as a `span`
- `unmap_span()`
- `virtual_alloc()`
- `log()` - default logging hook to `debug_out`

### Macros
- `CE_STATIC_ASSERT()` - pre c++17 static_assert with optional and better error messges
- `CE_CPU_X86`, `CE_CPU_X86_32`, `CE_CPU_X86_64` - x86 compile time cpu detection
- `CE_CPU_WASM`, `CE_CPU_WASM_32`, `CE_CPU_WASM_64` - WebAssembly compile time cpu detection
- `CE_DEBUG_BREAK()` - break to debugger
- `CE_TIMESTAMP()` - high resolution timestamp counter (e.g. rdtsc on x86)
- `CE_MEMCPY()` - memcpy intrinsic
- `CE_MEMSET()` - memset intrinsic
- `CE_ROTL32()` - rotate left intrinsic
- `CE_STRLEN()` - strlen intrinsic
- `CE_ERROR()`, `CE_ASSERT()`, `CE_VERIFY()`, and `CE_FAILED()` - runtime error checking
- `CE_COUNTOF()` - compile time array extent
- `CE_FOLD_LEFT_COMMA()` - pre c++17 left comma fold expression
- `CE_LOG(level, expressions...)` - user hookable structured logging
- `CE_LOG_MSG(level, values...)` - user hookable message logging
```
    int time = 42;
    int money = 1000000;
    CE_LOG(fail, time, money);
    CE_LOG_MSG(info, "The answer is ", 6 * 7, "!");
```
```
    F: { "$": "fail", "time": "42", "money": "1000000" }
    I: { "$": "info", "$msg": "The answer is 42!" }
```
### atomic.h
- ultra low overhead atomics similar to `std::atomic<T>`, but using reference parameters and minimal function call depth for optimal debug and release codegen
### cdt.h
- fully deterministic, integer based, constrained delaunay triangulation with low memory footprint
### dictionary.h
- hash table
### io.h
- reflection for struct serialization/deserialization
### lziii.h
- `constexpr` lzss style compression
### math.h
- more math...
### mutex.h
- `thread_mutex` and `thread_shared_mutex`
### nameof.h
- type -> string: `char (&nameof<T>())[N]`
- enum value <-> string: `char const* nameof(T n)` `T as_enum(char const name[], T unknown = T{ })`
### pool.h
- object pool 
### sort.h
- intro sort
### zorder.h
- Morton Z ordering https://en.wikipedia.org/wiki/Z-order_curve
