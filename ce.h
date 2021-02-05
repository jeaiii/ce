#pragma once
/*
MIT License

Copyright(c) 2021 James Edward Anhalt III - https://github.com/jeaiii/ce

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/


namespace ce
{
    namespace core
    {
        using uint = unsigned int;

        using size_t = decltype(sizeof(0));

        using ptrdiff_t = decltype("" - "");

        using uint8_t = unsigned char;
        using uint16_t = unsigned short;
        using uint32_t = decltype(0xffffffff);
        using uint64_t = decltype(0xffffffffffffffff);

        using int8_t = signed char;
        using int16_t = short;
        using int32_t = decltype(2147483647);
        using int64_t = decltype(9223372036854775807);
    }

    using namespace core;

    namespace detail
    {
#ifdef _MSC_VER
        extern "C" unsigned __int64 __rdtsc();
        extern "C" unsigned int __cdecl _rotl(unsigned int, int);
        extern "C" size_t __cdecl strlen(const char*);
        extern "C" void  _ReadWriteBarrier();
        #pragma intrinsic(strlen)
#endif
        template<size_t N, class T> char(&countof_prototype(T const (&)[N]))[N];

        inline uint32_t rotl32(uint32_t x, int i)
        {
            return (x << (i & 31)) | (x >> (32 - (i & 31)));
        }
    }
}

#if defined(_MSC_VER)
#define CE_DEBUG_BREAK() (ce::detail::_ReadWriteBarrier(), __debugbreak())
#define CE_TIME_STAMP() static_cast<ce::uint64_t>(ce::detail::__rdtsc())
#define CE_ROTL32(...) ce::detail::_rotl(__VA_ARGS__)
#define CE_STRLEN(...) ce::detail::strlen(__VA_ARGS__)
#elif defined(__clang__)
#define CE_DEBUG_BREAK() __builtin_debugtrap()
#define CE_TIME_STAMP() static_cast<ce::uint64_t>(__builtin_ia32_rdtsc())
#define CE_STRLEN(...) __builtin_strlen(__VA_ARGS__)
#define CE_ROTL32(...) __builtin_rotateleft32(__VA_ARGS__)
#elif defined(__GNUC__)
#define CE_DEBUG_BREAK() ({ __asm__ volatile("int $0x03"); })
#define CE_TIME_STAMP() static_cast<ce::uint64_t>(__builtin_ia32_rdtsc())
#define CE_STRLEN(...) __builtin_strlen(__VA_ARGS__)
#define CE_ROTL32(...) ce::detail::rotl32(__VA_ARGS__)
#endif

#define CE_ERROR(...) CE_DEBUG_BREAK()
#define CE_ASSERT(...) void(bool{ __VA_ARGS__ } || (CE_ERROR(__VA_ARGS__), false))
#define CE_VERIFY(...) (bool{ __VA_ARGS__ } || (CE_ERROR(__VA_ARGS__), false))
#define CE_FAILED(...) (bool{ __VA_ARGS__ } && (CE_ERROR(__VA_ARGS__), true))

#define CE_COUNTOF(...) sizeof(ce::detail::countof_prototype(__VA_ARGS__))

#if __cpp_fold_expressions
#define CE_FOLD_LEFT_COMMA(...) (__VA_ARGS__, ...)
#else
#define CE_FOLD_LEFT_COMMA(...) void(ce::identity_t<int[]>{ (void(__VA_ARGS__), 0)... });
#endif

namespace ce
{
    template<class T> using identity_t = T;

    template<class T, T...Is> struct items { enum : size_t { count = sizeof...(Is) }; };
    template<class T, T I> struct items<T, I> { enum : size_t { count = 1 }; enum : T { value = I }; };

    template<class...> struct types;
    template<> struct types<> : items<size_t, 0> { enum : size_t { count = 0 }; using car_t = types<>; using cdr_t = types<>; };
    template<class T, class...Ts> struct types<T, Ts...> { enum : size_t { count = 1 + sizeof...(Ts) }; using car_t = T; using cdr_t = types<Ts...>; };

    template<class...Ts> using car_t = typename types<Ts...>::car_t;
    template<class...Ts> using cdr_t = typename types<Ts...>::cdr_t;

    template<class T, T...Ns> constexpr auto& items_v = identity_t<T const []>{ Ns... };

    //--------

    template<size_t, class...> struct select { };
    template<size_t N, class T, class...Ts> struct select<N, T, Ts...> : select<N - 1, Ts...> { };
    template<class T, class...Ts> struct select<0, T, Ts...> { using type = T; };

    template<size_t N, class...Ts> using select_t = typename select<N, Ts...>::type;
    template<bool E, class...Ts> using cond_t = typename select<E ? 0 : 1, Ts...>::type;
    template<bool E, class T = void> using enable_t = typename select<E ? 0 : 1, T>::type;

    template<class...> struct are_same : items<bool, false> { };
    template<class T> struct are_same<T> : items<bool, true> { };
    template<class T, class...Ts> struct are_same<T, T, Ts...> : are_same<T, Ts...> { };
    template<class...Ts> constexpr bool are_same_v = are_same<Ts...>::value;

    template<class T, class U> using is_same = are_same<T, U>;
    template<class T, class U> constexpr bool is_same_v = are_same<T, U>::value;

    template<class T, class...Ts> using enable_same_t = enable_t<are_same_v<T, Ts...>, T>;

    //--------

    template<class T> struct remove_reference { using type = T; };
    template<class T> struct remove_reference<T&> { using type = T; };
    template<class T> struct remove_reference<T&&> { using type = T; };
    template<class T> using remove_reference_t = typename remove_reference<T>::type;

    template<class T> struct remove_const { using type = T; };
    template<class T> struct remove_const<const T> { using type = T; };
    template<class T> using remove_const_t = typename remove_const<T>::type;

    template<class T> struct remove_volatile { using type = T; };
    template<class T> struct remove_volatile<volatile T> { using type = T; };
    template<class T> using remove_volatile_t = typename remove_volatile<T>::type;

    template<class T> struct remove_cv { using type = T; };
    template<class T> struct remove_cv<const T> { using type = T; };
    template<class T> struct remove_cv<volatile T> { using type = T; };
    template<class T> struct remove_cv<const volatile T> { using type = T; };
    template<class T> using remove_cv_t = typename remove_cv<T>::type;

    template<class T> struct remove_cvref { using type = remove_cv_t<remove_reference_t<T>>; };
    template<class T> using remove_cvref_t = typename remove_cvref<T>::type;

    template<class T> struct remove_pointer { using type = T; };
    template<class T> struct remove_pointer<T*> { using type = T; };
    template<class T> struct remove_pointer<T* const> { using type = T; };
    template<class T> struct remove_pointer<T* volatile> { using type = T; };
    template<class T> struct remove_pointer<T* const volatile> { using type = T; };
    template<class T> using remove_pointer_t = typename remove_pointer<T>::type;

    //--------

    namespace detail
    {
        template<class T, size_t, size_t, class, class> struct sequence;
        template<class T, size_t N, size_t...As, size_t...Is> struct sequence<T, N, 0, items<size_t, As...>, items<size_t, Is...>> : sequence<T, N / 2, N % 2, items<size_t, As..., (sizeof...(As) + As)...>, items<size_t, Is...>> { };
        template<class T, size_t N, size_t...As, size_t...Is> struct sequence<T, N, 1, items<size_t, As...>, items<size_t, Is...>> : sequence<T, N / 2, N % 2, items<size_t, As..., (sizeof...(As) + As)...>, items<size_t, Is..., (sizeof...(Is) + As)...>> { };
        template<class T, size_t...As, size_t...Is> struct sequence<T, 0, 0, items<size_t, As...>, items<size_t, Is...>> { using type = items<T, T{ Is }... > ; };
        template<class T, size_t...As, size_t...Is> struct sequence<T, 0, 1, items<size_t, As...>, items<size_t, Is...>> { using type = items<T, T{ Is }..., T{ sizeof...(Is) + As }... > ; };
    }
    template<class T, size_t N> using sequence = detail::sequence<T, N / 2, N % 2, items<size_t, 0>, items<size_t>>;
    template<class T, size_t N> using sequence_t = typename detail::sequence<T, N / 2, N % 2, items<size_t, 0>, items<size_t>>::type;

    //--------

    template<class...> struct tuple;
    template<> struct tuple<> { enum : size_t { count = 0 }; };

    template<class T0> struct tuple<T0> { enum : size_t { count = 1 }; T0 _0; };
    template<class T0, class T1> struct tuple<T0, T1> { enum : size_t { count = 2 }; T0 _0; T1 _1; };
    template<class T0, class T1, class T2> struct tuple<T0, T1, T2> { enum : size_t { count = 3 }; T0 _0; T1 _1; T2 _2; };
    template<class T0, class T1, class T2, class T3> struct tuple<T0, T1, T2, T3> { enum : size_t { count = 4 }; T0 _0; T1 _1; T2 _2; T3 _3; };

    template<class T> using box = tuple<T>;
    template<class T, class U> using pair = tuple<T, U>;

    namespace detail
    {
        struct any { any(...);  template<class T> operator T() const; };

#if __cpp_structured_bindings
        template<class T, size_t...Ns> constexpr bool test_arrity(...) { return false; }
        template<class T, size_t...Ns> constexpr decltype(T{ any(Ns)... }, true) test_arrity(int) { return true; }

        template<class, class> struct arrity_test;
        template<class T, size_t...Ns> struct arrity_test<T, items<size_t, Ns...>> { static constexpr bool test() { return test_arrity<T, Ns...>(0); } };

        template<class T, size_t N> constexpr auto has_arrity = arrity_test<T, sequence_t<size_t, N>>::test();
        template<class T, size_t N> constexpr auto is_arrity = items<bool, has_arrity<T, N> && !has_arrity<T, N + 1>>::value;

        template<class, class> struct arrity_check;
        template<class T, size_t...Ns> struct arrity_check<T, items<size_t, Ns...>> { using type_t = items<bool, has_arrity<T, Ns + 1>...>; };

        template<class, class> struct count_leading_t : items<size_t, 0> { };
        template<class T, T I, T...Is> struct count_leading_t<items<T, I>, items<T, I, Is...>> : items<size_t, 1 + count_leading_t<items<T, I>, items<T, Is...>>::value> { };

        template<class T> using arrity_count = count_leading_t<items<bool, true>, typename arrity_check<T, sequence_t<size_t, 5>>::type_t>;

        template<class T, class...Ts> T get(items<size_t, 0>, T t, Ts...) { return t; }
        template<size_t N, class T, class...Ts> auto get(items<size_t, N>, T t, Ts...ts) { return get(items<size_t, N - 1>{}, ts...); }

        template<class T, size_t N, size_t = arrity_count<T>::value> struct crack { static detail::any get(T) = delete; };
        template<class T, size_t N> struct crack<T, N, 1> { static auto get(T t) { auto [_0] = t; return get(items<size_t, N>{}, _0); } };
        template<class T, size_t N> struct crack<T, N, 2> { static auto get(T t) { auto [_0, _1] = t; return get(items<size_t, N>{}, _0, _1); } };
        template<class T, size_t N> struct crack<T, N, 3> { static auto get(T t) { auto [_0, _1, _2] = t; return get(items<size_t, N>{}, _0, _1, _2); } };
        template<class T, size_t N> struct crack<T, N, 4> { static auto get(T t) { auto [_0, _1, _2, _3] = t; return get(items<size_t, N>{}, _0, _1, _2, _3); } };
#else
        template<class T, size_t N> struct crack { static detail::any get(T) = delete; };
        template<class T> struct crack<T, 0> { static auto get(T t) { return t._0; } };
        template<class T> struct crack<T, 1> { static auto get(T t) { return t._1; } };
        template<class T> struct crack<T, 2> { static auto get(T t) { return t._2; } };
        template<class T> struct crack<T, 3> { static auto get(T t) { return t._3; } };
#endif
    }

    template<size_t N, class T> auto get(T const& t) { return detail::crack<T, N>::get(t); }

    //--------

    template<class T, size_t N> struct list
    {
        size_t size;
        alignas(T) char data[N][sizeof(T)];

        T* begin() { return reinterpret_cast<T*>(&data[0]); }
        T* end() { return reinterpret_cast<T*>(&data[size]); }

        T const* begin() const { return reinterpret_cast<T const*>(&data[0]); }
        T const* end() const { return reinterpret_cast<T const*>(&data[size]); }

        T& operator[](size_t i) { return *reinterpret_cast<T*>(&data[i]); }
        T const& operator[](size_t i) const { return *reinterpret_cast<T const*>(&data[i]); }

        template<class...Us>
        void append(Us&&...us) { new (reinterpret_cast<T*>(&data[size])) T{ us... }; ++size; }
    };

    template<class T> struct span
    {
        size_t size;
        T* data;

        T* begin() { return data + 0; }
        T* end() { return data + size; }

        T const* begin() const { return data + 0; }
        T const* end() const { return data + size; }
    };

    //--------

    namespace detail
    {
        template<uint32_t P> struct crc32
        {           
            enum class type : uint32_t { initial = 0, polynomial = P };

            static constexpr uint32_t crc(uint32_t i) { return i / 2 ^ i % 2 * P; }
            template<size_t I> static constexpr uint32_t x8 = crc(crc(crc(crc(crc(crc(crc(crc(I))))))));

            template<size_t, class> struct table;
            template<size_t N, size_t...Is> struct table<N, items<size_t, Is...>>
            {
                using p = table<N - 1, items<size_t, Is...>>;
                static constexpr uint32_t vs[sizeof...(Is)] = { p::vs[Is] / 256 ^ x8<p::vs[Is] % 256>... };
            };
            template<size_t...Is> struct table<0, items<size_t, Is...>>
            {
                static constexpr uint32_t vs[sizeof...(Is)] = { x8<Is>... };
            };
        };

        template<class T, size_t N> using crc32_table = typename crc32<uint32_t(T::polynomial)>::template table<N, sequence_t<size_t, 256>>;
    }

    template<uint32_t P> using crc32_t = typename detail::crc32<P>::type;

    using crc32c_t = typename detail::crc32<0x82f63b78>::type;

    template<class T, T P = T::polynomial> constexpr T crc32(T c, size_t n, uint64_t const p[])
    {
        uint32_t v = ~uint32_t(c);

        for (size_t i = 0; i < n; ++i)
        {
            auto q = p[i];
            v ^= uint32_t(q);
            uint32_t w = uint32_t(q >> 32);

            v = detail::crc32_table<T, 0>::vs[w / 16777216] ^
                detail::crc32_table<T, 1>::vs[w / 65536 % 256] ^
                detail::crc32_table<T, 2>::vs[w / 256 % 256] ^
                detail::crc32_table<T, 3>::vs[w % 256] ^
                detail::crc32_table<T, 4>::vs[v / 16777216] ^
                detail::crc32_table<T, 5>::vs[v / 65536 % 256] ^
                detail::crc32_table<T, 6>::vs[v / 256 % 256] ^
                detail::crc32_table<T, 7>::vs[v % 256];
        }

        return T(~v);
    }

    template<class T, T P = T::polynomial> constexpr T crc32(T c, size_t n, uint32_t const p[])
    {
        uint32_t v = ~uint32_t(c);

        for (size_t i = 0; i < n; ++i)
        {
            v ^= p[i];
            v = detail::crc32_table<T, 0>::vs[v / 16777216] ^
                detail::crc32_table<T, 1>::vs[v / 65536 % 256] ^
                detail::crc32_table<T, 2>::vs[v / 256 % 256] ^
                detail::crc32_table<T, 3>::vs[v % 256];
        }

        return T(~v);
    }

    template<class T, T P = T::polynomial> constexpr T crc32(T c, size_t n, uint8_t const p[])
    {
        uint32_t v = ~uint32_t(c);

        for (size_t i = 0; i < n; ++i)
            v = v / 256 ^ detail::crc32_table<T, 0>::vs[v % 256 ^ p[i]];

        return T(~v);
    }

    template<size_t N>
    constexpr uint32_t crc32c(char const (&s)[N])
    {
        uint32_t v = ~uint32_t(crc32c_t::initial);

        for (size_t i = 0; i < N - 1; ++i)
            v = v / 256 ^ detail::crc32_table<crc32c_t, 0>::vs[v % 256 ^ uint8_t(s[i])];

        return ~v;
    }

    //--------

    namespace math
    {
        template<class, size_t, class...> struct vec;
        template<class T, class...Ks> struct vec<T, 0, Ks...> { enum : size_t { count = 0 }; using type = T; };
        template<class T, class...Ks> struct vec<T, 1, Ks...> { enum : size_t { count = 1 }; using type = T; T x; };
        template<class T, class...Ks> struct vec<T, 2, Ks...> { enum : size_t { count = 2 }; using type = T; T x; T y; };
        template<class T, class...Ks> struct vec<T, 3, Ks...> { enum : size_t { count = 3 }; using type = T; T x; T y; T z; };
        template<class T, class...Ks> struct vec<T, 4, Ks...> { enum : size_t { count = 4 }; using type = T; T x; T y; T z; T w; };

        inline int64_t sqrx(int32_t a) { return int64_t(a) * a; }
        inline int64_t dotx(int32_t a, int32_t b) { return int64_t(a) * b; }

        inline float sqrx(float a) { return a * a; }
        inline float dotx(float a, float b) { return a * b; }

        template<class T, class...Ks> auto sqrx(const vec<T, 2, Ks...>& a) -> decltype(sqrx(a.x))
        {
            using E = decltype(sqrx(a.x));
            return { E(a.x) * a.x + E(a.y) * a.y };
        }

        template<class T, class...Ks> auto dotx(const vec<T, 2, Ks...>& a, const vec<T, 2, Ks...>& b) -> decltype(dotx(a.x, b.x))
        {
            using E = decltype(dotx(a.x, b.x));
            return { E(a.x) * b.x + E(a.y) * b.y };
        }

        template<class T, class...Ks> auto dotx(const vec<T, 3, Ks...>& a, const vec<T, 3, Ks...>& b) -> decltype(dotx(a.x, b.x))
        {
            using E = decltype(dotx(a.x, b.x));
            return { E(a.x) * b.x + E(a.y) * b.y + E(a.z) * b.z };
        }

        template<class T, class...Ks> auto crossx(const vec<T, 2, Ks...>& a, const vec<T, 2, Ks...>& b)-> decltype(dotx(a.x, b.x))
        {
            using E = decltype(dotx(a.x, b.x));
            return E(a.x) * b.y - E(a.y) * b.x;
        };

        template<class T, class...Ks> auto crossx(const vec<T, 3, Ks...>& a, const vec<T, 3, Ks...>& b) -> vec<decltype(dotx(a.x, b.x)), 3, Ks...>
        {
            using E = decltype(dotx(a.x, b.x));
            return { E(a.y) * b.z - E(a.z) * b.y, E(a.z) * b.x - E(a.x) * b.z, E(a.x) * b.y - E(a.y) * b.x };
        }

        template<class T, class...Ks> bool operator==(const vec<T, 2, Ks...>& a, const vec<T, 2, Ks...>& b) { return a.x == b.x && a.y == b.y; }
        template<class T, class...Ks> bool operator==(const vec<T, 2, Ks...>& a, T b) { return a.x == b && a.y == b; }
        template<class T, class...Ks> bool operator!=(const vec<T, 2, Ks...>& a, const vec<T, 2, Ks...>& b) { return a.x != b.x || a.y != b.y; }
        template<class T, class...Ks> bool operator!=(const vec<T, 2, Ks...>& a, T b) { return a.x != b || a.y != b; }

        template<class T, class...Ks> vec<T, 2, Ks...> operator+(const vec<T, 2, Ks...>& a, const vec<T, 2, Ks...>& b) { return { a.x + b.x, a.y + b.y }; }
        template<class T, class...Ks> vec<T, 2, Ks...>& operator+=(vec<T, 2, Ks...>& a, const vec<T, 2, Ks...>& b) { a.x += b.x; a.y += b.y; return a; }
        template<class T, class...Ks> vec<T, 2, Ks...> operator-(const vec<T, 2, Ks...>& a, const vec<T, 2, Ks...>& b) { return { a.x - b.x, a.y - b.y }; }

        template<class T, class...Ks> vec<T, 2, Ks...> operator*(const vec<T, 2, Ks...>& a, int32_t b) { return { a.x * b, a.y * b }; }
        template<class T, class...Ks> vec<T, 2, Ks...>& operator*=(vec<T, 2, Ks...>& a, int32_t b) { a.x *= b; a.y *= b; return a; }

        template<class T, class...Ks> vec<T, 2, Ks...> operator/(const vec<T, 2, Ks...>& a, int32_t b) { return { a.x / b, a.y / b }; }
        template<class T, class...Ks> vec<T, 2, Ks...>& operator/=(vec<T, 2, Ks...>& a, int32_t b) { a.x /= b; a.y /= b; return a; }


        template<class T, class...Ks> vec<T, 3, Ks...> operator-(const vec<T, 3, Ks...>& a, const vec<T, 3, Ks...>& b) { return { a.x - b.x, a.y - b.y, a.z - b. z }; }
    }

    using namespace math;   
    
    namespace random
    {
        // http://prng.di.unimi.it/xoroshiro64starstar.c
        struct xoroshiro64ss
        {
            uint32_t a;
            uint32_t b;
            
            friend void seed(xoroshiro64ss& g, size_t size, uint8_t data[])
            {
                auto a = crc32(crc32c_t::initial, size / 2, data);
                auto b = crc32(a, size - size / 2, data + size / 2);

                g.a = uint32_t(a);
                g.b = uint32_t(b);
            }

            friend uint32_t next32(xoroshiro64ss& g)
            {
                const uint32_t a = g.a;
                const uint32_t b = g.b ^ a;
                g.a = CE_ROTL32(a, 26) ^ b ^ (b << 9);
                g.b = CE_ROTL32(b, 13);
                return CE_ROTL32(a * 0x9e3779bb, 5) * 5;
            }
        };
    }

    using namespace random;

#if defined(CE_USER_CHECKED)
    static_assert(detail::crc32_table<crc32c_t, 0>::vs[0] == 0, "invalid crc32c value");
    static_assert(detail::crc32_table<crc32c_t, 0>::vs[1] == 0xf26b8303, "invalid crc32c value");
    static_assert(detail::crc32_table<crc32c_t, 0>::vs[255] == 0xad7d5351, "invalid crc32c value");
    static_assert(detail::crc32_table<crc32c_t, 1>::vs[0] == 0, "invalid crc32c value");
    static_assert(detail::crc32_table<crc32c_t, 1>::vs[1] == 0x13a29877, "invalid crc32c value");
    static_assert(detail::crc32_table<crc32c_t, 1>::vs[255] == 0xa3e32483, "invalid crc32c value");
    static_assert(detail::crc32_table<crc32c_t, 15>::vs[255] == 0x8fda7dfa, "invalid crc32c value");

    static_assert(crc32(crc32c_t::initial, 1, items_v<uint64_t, 0x1234567812345678ull>) ==
        crc32(crc32c_t::initial, 2, items_v<uint32_t, 0x12345678u, 0x12345678u>), "");

    static_assert(crc32(crc32c_t::initial, 1, items_v<uint64_t, 0x1234567812345678ull>) ==
        crc32(crc32c_t::initial, 8, items_v<uint8_t, 0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12>), "");

    static_assert(crc32c("The quick brown fox jumps over the lazy dog") == 0x22620404, "crc32c failed");
    static_assert(crc32c("123456789") == 0xe3069283, "crc32c failed");
#endif
}
