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

        template<class T>
        void swap(T& a, T& b)
        {
            T c(a);
            a = b;
            b = c;
        }
    }

    using namespace core;

    namespace detail
    {
        constexpr bool static_assert_helper(bool condition, char const* = nullptr) { return condition; }

#if defined(_MSC_VER)
        extern "C" void _ReadWriteBarrier();
        extern "C" void __cdecl __debugbreak();
        extern "C" unsigned __int64 __rdtsc();
        extern "C" void* __cdecl memcpy(void* a, void const*, size_t);
        extern "C" void* __cdecl memset(void* a, int, size_t);
        extern "C" size_t __cdecl strlen(const char*);
        extern "C" unsigned int __cdecl _rotl(unsigned int, int);
#pragma intrinsic(memcpy)
#pragma intrinsic(memset)
#pragma intrinsic(strlen)
#endif
        template<size_t N, class T> char (&countof_helper(T const (&)[N]))[N];
        template<class...Ts> char (&countof_args_helper(Ts const&...))[sizeof...(Ts) + 1];

        inline uint32_t rotl32(uint32_t x, int i)
        {
            return (x << (i & 31)) | (x >> (32 - (i & 31)));
        }

        class new_tag;
    }


    enum class api
    {
        unknown,
        windows,
        linux,
        wasm
    };

    enum class cpu
    {
        unknown,
        x86_32,
        x86_64,
        arm_32,
        arm_64,
        wasm_32,
        wasm_64
    };
}

inline void* operator new(ce::size_t, ce::detail::new_tag* p) noexcept { return p; }

//--------

#define CE_API_UNKNOWN 0
#define CE_API_WIN32 0
#define CE_API_POSIX 0

#if defined(_WIN32)
#undef CE_API_WIN32
#define CE_API_WIN32 1
#elif defined(__unix__)
#undef CE_API_POSIX
#define CE_API_POSIX 1
#else
#undef CE_API_UNKNOWN
#define CE_API_UNKNOWN 1
#endif

#define CE_CPU_X86_32 0
#define CE_CPU_X86_64 0
#define CE_CPU_X86 0

#define CE_CPU_WASM_32 0
#define CE_CPU_WASM_64 0
#define CE_CPU_WASM 0

#if defined(__wasm32)
#undef CE_CPU_WASM_32
#define CE_CPU_WASM_32 1
#endif

#if defined(__wasm64)
#undef CE_CPU_WASM_64
#define CE_CPU_WASM_64 1
#endif

#if CE_CPU_WASM_32 || CE_CPU_WASM_64
#undef CE_CPU_WASM
#define CE_CPU_WASM 1
#endif

#if defined(__x86_64__) || defined(_M_X64)
#undef CE_CPU_X86_64
#define CE_CPU_X86_64 1
#endif

#if defined(__i386__) || defined(_M_IX86)
#undef CE_CPU_X86_32
#define CE_CPU_X86_32 1
#endif

#if CE_CPU_X86_32 || CE_CPU_X86_64
#undef CE_CPU_X86
#define CE_CPU_X86 1
#endif

//--------

#if defined(__clang__)

#define CE_DEBUG_BREAK() __builtin_debugtrap()
#define CE_TIME_STAMP() static_cast<ce::uint64_t>(__builtin_readcyclecounter())
#define CE_MEMCPY(...) __builtin_memcpy(__VA_ARGS__)
#define CE_MEMSET(...) __builtin_memset(__VA_ARGS__)
#define CE_STRLEN(...) __builtin_strlen(__VA_ARGS__)
#define CE_ROTL32(...) __builtin_rotateleft32(__VA_ARGS__)
#define CE_NOINLINE __attribute__((noinline))

#elif defined(_MSC_VER)

#define CE_DEBUG_BREAK() (ce::detail::_ReadWriteBarrier(), ce::detail::__debugbreak())
#if CE_CPU_X86
#define CE_TIMESTAMP() static_cast<ce::uint64_t>(ce::detail::__rdtsc())
#else
#define CE_TIMESTAMP() ce::uint64_t(0)
#endif
#define CE_MEMCPY(...) ce::detail::memcpy(__VA_ARGS__)
#define CE_MEMSET(...) ce::detail::memset(__VA_ARGS__)
#define CE_STRLEN(...) ce::detail::strlen(__VA_ARGS__)
#define CE_ROTL32(...) ce::detail::_rotl(__VA_ARGS__)
#define CE_NOINLINE __declspec(noinline)

#elif defined(__GNUC__)

#if CE_CPU_X86
#define CE_DEBUG_BREAK() ({ __asm__ volatile("int $0x03"); })
#define CE_TIMESTAMP() static_cast<ce::uint64_t>(__builtin_ia32_rdtsc())
#else
#define CE_DEBUG_BREAK() void(0)
#define CE_TIMESTAMP() ce::uint64_t(0)
#endif
#define CE_MEMCPY(...) __builtin_memcpy(__VA_ARGS__)
#define CE_MEMSET(...) __builtin_memset(__VA_ARGS__)
#define CE_STRLEN(...) __builtin_strlen(__VA_ARGS__)
#define CE_ROTL32(...) ce::detail::rotl32(__VA_ARGS__)
#define CE_NOINLINE __attribute__((noinline))

#endif

#define CE_TIME_STAMP() CE_TIMESTAMP()

//--------

#ifdef CE_USER_INCLUDE
#include CE_USER_INCLUDE
#endif

#ifdef CE_USER_LOG_LEVEL
#define CE_LOG_LEVEL CD_USER_LOG_LEVEL
#else
#define CE_LOG_LEVEL 1
#endif

#ifdef CE_USER_LOG_HOOK
#define CE_LOG_HOOK(LVL, ARGC, ...) CE_USER_LOG_HOOK(LVL, ARGC, __VA_ARGS__)
#else
#define CE_LOG_HOOK(LVL, ARGC, ...) ce::os::log(LVL, ARGC, __VA_ARGS__)
#endif

//--------
// pre c++17 static_assert with optional and better error message
#define CE_STATIC_ASSERT(...) static_assert(ce::detail::static_assert_helper(__VA_ARGS__), "static_assert(" #__VA_ARGS__ ") failed")

#define CE_ERROR(...) CE_DEBUG_BREAK()
#define CE_ASSERT(...) void(bool{ __VA_ARGS__ } || (CE_ERROR(__VA_ARGS__), false))
#define CE_VERIFY(...) (bool{ __VA_ARGS__ } || (CE_ERROR(__VA_ARGS__), false))
#define CE_FAILED(...) (bool{ __VA_ARGS__ } && (CE_ERROR(__VA_ARGS__), true))

#define CE_COUNTOF(...) sizeof(ce::detail::countof_helper(__VA_ARGS__))
#define CE_COUNTOF_ARGS(...) (sizeof(decltype(ce::detail::countof_args_helper(__VA_ARGS__))) - 1)

#define CE_REF(...) (void)CE_COUNTOF_ARGS(__VA_ARGS__)

#define CE_PP_JOIN(_0, ...) _0 ## __VA_ARGS__
#define CE_PP_PASS(...) __VA_ARGS__
#define CE_PP_KILL(...)
#define CE_PP_ARGS(...) (__VA_ARGS__)
#define CE_PP_CALL(F, ...) F(__VA_ARGS__)
#define CE_PP_TEXT(...) #__VA_ARGS__

#define CE_PP_GET_0(_0, ...) _0
#define CE_PP_GET_1(_0, _1, ...) _1
#define CE_PP_GET_2(_0, _1, _2, ...) _2
#define CE_PP_GET_3(_0, _1, _2, _3, ...) _3
#define CE_PP_GET_4(_0, _1, _2, _3, _4, ...) _4
#define CE_PP_GET_5(_0, _1, _2, _3, _4, _5, ...) _5
#define CE_PP_GET_6(_0, _1, _2, _3, _4, _5, _6, ...) _6
#define CE_PP_GET_7(_0, _1, _2, _3, _4, _5, _6, _7, ...) _7
#define CE_PP_MAP(M, _0, _1, _2, _3, _4, _5, _6, ...) M(_0) M(_1) M(_2) M(_3) M(_4) M(_5) M(_6) M(__VA_ARGS__)

#define CE_PP_WHEN_0(...) CE_PP_PASS
#define CE_PP_WHEN_1(...) __VA_ARGS__ CE_PP_KILL
#define CE_PP_WHEN(COND) CE_PP_JOIN(CE_PP_WHEN_, COND)

#ifdef __cpp_fold_expressions
#define CE_FOLD_LEFT_COMMA(...) (__VA_ARGS__, ...)
#else
#define CE_FOLD_LEFT_COMMA(...) void(ce::identity_t<int[]>{ (void(__VA_ARGS__), 0)... });
#endif

//--------

namespace ce
{
    template<class T> using identity_t = T;

    template<class T, T...Is> struct items { static constexpr size_t count = sizeof...(Is); };
    template<class T, T I> struct items<T, I> { static constexpr size_t count = 1; static constexpr T value = I; };

    template<class...> struct types;
    template<> struct types<> { static constexpr size_t count = 0; using car_t = void; using cdr_t = void; };
    template<class T, class...Ts> struct types<T, Ts...> { static constexpr size_t count = 1 + sizeof...(Ts); using car_t = T; using cdr_t = types<Ts...>; };

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

    template<class...> struct has : items<bool, false> { };
    template<class T, class...Us> struct has<T, T, Us...> : items<bool, true> { };
    template<class T, class U, class...Us> struct has<T, U, Us...> : has<T, Us...> { };
    template<class...Ts> constexpr bool has_v = has<Ts...>::value;

    template<class T> using is_enum = items<bool, __is_enum(T)>;
    template<class T> using underlying_type_t = __underlying_type(T);

    template<class T, class U> union as_cast
    {
        U from;
        T as;
    };

    template<class U> as_cast(U) -> as_cast<underlying_type_t<U>, U>;

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

    template<class T> struct is_unqualified_ptr : items<bool, false> { };
    template<class T> struct is_unqualified_ptr<T*> : items<bool, true> { };

    template<class T> using is_ptr = items<bool, is_unqualified_ptr<remove_cvref_t<T>>::value>;

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
    template<> struct tuple<> { static constexpr size_t count = 0; };

    template<class T0> struct tuple<T0> { static constexpr size_t count = 1; T0 _0; };
    template<class T0, class T1> struct tuple<T0, T1> { static constexpr size_t count = 2; T0 _0; T1 _1; };
    template<class T0, class T1, class T2> struct tuple<T0, T1, T2> { static constexpr size_t count = 3; T0 _0; T1 _1; T2 _2; };
    template<class T0, class T1, class T2, class T3> struct tuple<T0, T1, T2, T3> { static constexpr size_t count = 4; T0 _0; T1 _1; T2 _2; T3 _3; };

    template<class T> using box = tuple<T>;
    template<class T, class U> using pair = tuple<T, U>;

    namespace detail
    {
        struct any { any(...);  template<class T> operator T() const; };

#ifdef __cpp_structured_bindings
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

    template<class T> struct span
    {
        size_t size;
        T* data;

        T* begin() { return data + 0; }
        T* end() { return data + size; }

        T const* begin() const { return data + 0; }
        T const* end() const { return data + size; }

        friend span skip(span s, size_t n)
        {
            return n < s.size ? span{ s.size - n, s.data + n } : span{ 0, s.data };
        }

        friend span trim(span s, size_t n)
        {
            return n < s.size ? span{ s.size - n, s.data } : span{ 0, s.data };
        }
    };

#define CE_CONSTRUCT_AT(...) new (reinterpret_cast<ce::detail::new_tag*>(__VA_ARGS__))
#define CE_DESTROY_AT(...) (__VA_ARGS__)->

    template<class T> size_t construct_at(T data[], size_t head, size_t tail)
    {
        if constexpr (!__has_trivial_constructor(T))
            while (head < tail)
                CE_CONSTRUCT_AT(data + head) T, ++head;

        return tail;
    }

    template<class T> size_t destroy_at(T data[], size_t head, size_t tail)
    {
        if constexpr (!__has_trivial_destructor(T))
            while (head < tail)
                --tail, CE_DESTROY_AT(data + tail) ~T();

        return head;
    }

    template<class T, class...Ts> struct is_constructible_with
    {
        template<class U, class...Us> static constexpr bool test(...) { return false; };
        template<class U, class...Us> static constexpr bool test(decltype(U{ (*(Us*)0)... }, 0)) { return false; }
        static constexpr bool value = test<T, Ts...>(0);
    };

    template<class T> struct is_constructible_with<T>
    {
        template<class U> static constexpr bool test(...) { return false; }
        template<class U> static constexpr bool test(decltype(U{ }, 0)) { return true; }
        static constexpr bool value = test<T>(0);
    };

    template<class T> inline void construct_copy(T& dst, T const& src)
    {
        if constexpr (is_constructible_with<T, T>::value)
        {
            CE_CONSTRUCT_AT(&dst) T{ src };
        }
        else if constexpr (is_constructible_with<T>::value)
        {
            CE_CONSTRUCT_AT(&dst) T, dst = src;
        }
        else
        {
            dst = src;
        }
    }

    template<class T> size_t assign_items(size_t dst_size, T dst_data[], size_t src_size, T const src_data[])
    {
        if (dst_size < src_size)
        {
            for (size_t i = 0; i < dst_size; ++i)
                dst_data[i] = src_data[i];
            for (size_t i = dst_size; i < src_size; ++i)
                construct_copy(dst_data[i], src_data[i]);
        }
        else
        {
            for (size_t i = 0; i < src_size; ++i)
                dst_data[i] = src_data[i];
            destroy_at(dst_data, src_size, dst_size);
        }
        return src_size;
    }

    template<size_t N, class T> struct bulk
    {
        size_t size;
        union { char d0; T data[N]; };

        bulk() : size{ }, d0{ } { }
        ~bulk() { destroy_at(data, 0, size); }

        bulk(bulk const& b)
        {
            CE_ASSERT(b.size <= N);

            for (size_t i = 0; i < b.size; ++i)
                construct_copy(data[i], b.data[i]);
            size = b.size;
        }

        bulk& operator=(bulk const& b)
        {
            CE_ASSERT(size <= N && b.size <= N);
            size = assign_items(size, data, b.size, b.data);
            return *this;
        }
        
        //bulk(bulk&&) = delete;
        //bulk& operator=(bulk&&) = delete;

        static size_t capacity() { return N; }

        void clear() { size = destroy_at(data, 0, size); }

        // tries to set the size to `n`
        // fails returning false if `n' > capacity
        bool resize(size_t n)
        {
            if (n > N)
                return false;

            size = n <= size ? destroy_at(data, n, size) : construct_at(data, size, n);
            return true;
        }


        size_t room() const { return N - size; }

        T* begin() { return &data[0]; }
        T* end() { return &data[size]; }

        T const* begin() const { return &data[0]; }
        T const* end() const { return &data[size]; }

        T const& back() const { return size > 0 ? data[size - 1] : *reinterpret_cast<T const*>(nullptr); }
        T& back() { return size > 0 ? data[size - 1] : *reinterpret_cast<T*>(nullptr); }

        T& operator[](size_t i) { return data[i]; }
        T const& operator[](size_t i) const { return data[i]; }

        bool append_n(size_t n = 1)
        {
            if (N - size < n)
                return false;

            size = construct_at(data, size, size + n);
            return true;
        }

        template<class...Us>
        bool append(Us&&...us)
        {
            if (size < N)
                return CE_CONSTRUCT_AT(&data[size]) T{ static_cast<Us&&>(us)... }, ++size, true;
            return false;
        }

        template<bool Ordered = false>
        bool remove_at(size_t i)
        {
            if (i >= size)
                return false;

            --size;
            if constexpr (Ordered)
            {
                for (; i < size; ++i)
                    data[i] = static_cast<T&&>(data[i + 1]);
            }
            else
            {
                if (i < size)
                    data[i] = static_cast<T&&>(data[size]);
            }

            CE_DESTROY_AT(data + size) ~T();

            return true;
        }

        friend span<T> make_span(bulk& c) { return { c.size, &c.data[0] }; }
        friend span<T const> make_span(bulk const& c) { return { c.size, &c.data[0] }; }
    };

    template<class T, size_t N> using list = bulk<N, T>;
    
    template<class T> void heap_raise(T data[], T item, size_t n)
    {
        while (n > 0)
        {
            auto p = (n - 1) / 2;

            if (item < data[p])
                data[n] = data[p];
            else
                break;

            n = p;
        }
        data[n] = item;
    }

    template<class T> void heap_lower(size_t size, T data[], T item, size_t n)
    {
        for (;;)
        {
            auto p = n * 2 + 1;
            if (p >= size)
                break;

            if (p + 1 < size && data[p + 1] < data[p])
                ++p;

            if (item < data[p])
                break;

            data[n] = data[p];
            n = p;
        }
        data[n] = item;
    }

    template<size_t N, class T> struct min_priority_queue
    {
        size_t size;
        T data[N];

        T head() const { return data[0]; }

        bool empty() const { return size < 1; }

        bool enqueue(T item)
        {
            return size < N ? heap_raise(data, item, size++), true : false;
        }

        T dequeue()
        {
            T item = data[0];

            if (--size > 0)
                heap_lower(size, data, data[size], 0);

            return item;
        }
    };

    //--------

    template<size_t N> struct bitset
    {
        uint8_t data[(N + 7) >> 3];

        // operator[] can only be used to get, not to set.
        bool operator[](size_t index) const
        {
            CE_ASSERT(index < N);
            return (data[index >> 3] & ('\1' << (index % 8)));
        }

        void set()
        {
            CE_MEMSET(data, 0xff, (N + 7) >> 3);
        }

        void set(size_t index)
        {
            CE_ASSERT(index < N);
            data[index >> 3] |= ('\1' << (index % 8));
        }

        void reset()
        {
            CE_MEMSET(data, 0, (N + 7) >> 3);
        }

        void reset(size_t index)
        {
            CE_ASSERT(index < N);
            data[index >> 3] &= ~('\1' << (index % 8));
        }

        void set(size_t index, bool value)
        {
            CE_ASSERT(index < N);
            if (value)
            {
                data[index >> 3] |= 1 << (index % 8);
            }
            else
            {
                data[index >> 3] &= ~(1 << (index % 8));
            }
        }

        constexpr size_t get_size() const { return N; }
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

    namespace hash
    {
        namespace detail
        {
            template<class T> struct fnv1a;

            template<> struct fnv1a<uint32_t>
            {
                static constexpr uint32_t prime = 0x01000193;
                static constexpr uint32_t basis = 0x811c9dc5;
            };

            template<> struct fnv1a<uint64_t>
            {
                static constexpr uint64_t prime = 0x00000100000001B3;
                static constexpr uint64_t basis = 0xcbf29ce484222325;
            };
        }

        template<class T, class U, T Prime = detail::fnv1a<T>::prime>
        constexpr T fnv1a(size_t size, U const data[], T basis)
        {
            auto h = basis;

            for (size_t i = 0; i < size; ++i)
            {
                auto v = data[i];
                h = (h ^ uint8_t(v)) * Prime;

                if constexpr (sizeof(v) > 1)
                    for (size_t n = 1; n < sizeof(v); ++n)
                        h = (h ^ uint8_t(v >>= 8)) * Prime;
            }

            return h;
        }

        template<class U>
        constexpr uint32_t fnv1a32(size_t size, U const data[], uint32_t basis = detail::fnv1a<uint32_t>::basis) { return fnv1a(size, data, basis); }

        template<class U>
        constexpr uint64_t fnv1a64(size_t size, U const data[], uint64_t basis = detail::fnv1a<uint64_t>::basis) { return fnv1a(size, data, basis); }

        constexpr uint32_t fnv1a(char const text[])
        {
            auto h = detail::fnv1a<uint32_t>::basis;

            for (size_t i = 0; text[i] != '\0'; ++i)
                h = (h ^ uint8_t(text[i])) * detail::fnv1a<uint32_t>::prime;

            return h;
        }
    }

    namespace math
    {
        template<size_t, class...> struct vec;
        template<class T, class...Ks> struct vec<0, T, Ks...> { static constexpr size_t size = 0; using type = T; };
        template<class T, class...Ks> struct vec<1, T, Ks...> { static constexpr size_t size = 1; using type = T; T x; };
        template<class T, class...Ks> struct vec<2, T, Ks...> { static constexpr size_t size = 2; using type = T; T x; T y; };
        template<class T, class...Ks> struct vec<3, T, Ks...> { static constexpr size_t size = 3; using type = T; T x; T y; T z; };
        template<class T, class...Ks> struct vec<4, T, Ks...> { static constexpr size_t size = 4; using type = T; T x; T y; T z; T w; };

        template<class...Ts> using vec0 = vec<0, Ts...>;
        template<class...Ts> using vec1 = vec<1, Ts...>;
        template<class...Ts> using vec2 = vec<2, Ts...>;
        template<class...Ts> using vec3 = vec<3, Ts...>;
        template<class...Ts> using vec4 = vec<4, Ts...>;

        template<size_t H, size_t W, class T, class...Ks> using mat = vec<H, vec<W, T>, Ks...>;

        constexpr int64_t sqrx(int32_t a) { return int64_t(a) * a; }
        constexpr int64_t dotx(int32_t a, int32_t b) { return int64_t(a) * b; }

        constexpr int64_t sqrx(int64_t a) { return a * a; }
        constexpr int64_t dotx(int64_t a, int64_t b) { return a * b; }

        constexpr float sqrx(float a) { return a * a; }
        constexpr float dotx(float a, float b) { return a * b; }

        template<class...U> constexpr auto sqrx(const vec2<U...>& a) -> decltype(sqrx(a.x))
        {
            using E = decltype(sqrx(a.x));
            return E(a.x) * a.x + E(a.y) * a.y;
        }

        template<class...U> constexpr auto dotx(const vec2<U...>& a, const vec2<U...>& b) -> decltype(dotx(a.x, b.x))
        {
            using E = decltype(dotx(a.x, b.x));
            return E(a.x) * b.x + E(a.y) * b.y;
        }

        template<class...U> constexpr auto dotx(const vec<3, U...>& a, const vec<3, U...>& b) -> decltype(dotx(a.x, b.x))
        {
            using E = decltype(dotx(a.x, b.x));
            return E(a.x) * b.x + E(a.y) * b.y + E(a.z) * b.z;
        }

        template<class...U> constexpr auto crossx(const vec2<U...>& a, const vec2<U...>& b)-> decltype(dotx(a.x, b.x))
        {
            using E = decltype(dotx(a.x, b.x));
            return E(a.x) * b.y - E(a.y) * b.x;
        };

        template<class T, class...Ks> constexpr auto crossx(const vec<3, T, Ks...>& a, const vec<3, T, Ks...>& b) -> vec<3, decltype(dotx(a.x, b.x)), Ks...>
        {
            using E = decltype(dotx(a.x, b.x));
            return { E(a.y) * b.z - E(a.z) * b.y, E(a.z) * b.x - E(a.x) * b.z, E(a.x) * b.y - E(a.y) * b.x };
        }

        template<class...U> constexpr bool operator==(vec2<U...> const& a, vec2<U...> const& b) { return a.x == b.x && a.y == b.y; }
        template<class...U> constexpr bool operator==(vec2<U...> const& a, car_t<U...> const& b) { return a.x == b && a.y == b; }
        template<class...U> constexpr bool operator!=(vec2<U...> const& a, vec2<U...> const& b) { return a.x != b.x || a.y != b.y; }
        template<class...U> constexpr bool operator!=(vec2<U...> const& a, car_t<U...> const& b) { return a.x != b || a.y != b; }
        template<class...U> constexpr vec2<U...> operator+(const vec2<U...>& a, const vec2<U...>& b) { return { a.x + b.x, a.y + b.y }; }
        template<class...U> constexpr vec2<U...> operator-(const vec2<U...>& a, const vec2<U...>& b) { return { a.x - b.x, a.y - b.y }; }
        template<class...U> constexpr vec2<U...> operator*(const vec2<U...>& a, car_t<U...> const& b) { return { a.x * b, a.y * b }; }
        template<class...U> constexpr vec2<U...> operator/(const vec2<U...>& a, car_t<U...> const& b) { return { a.x / b, a.y / b }; }
        template<class...U> inline vec2<U...>& operator+=(vec2<U...>& a, const vec2<U...>& b) { a.x += b.x; a.y += b.y; return a; }
        template<class...U> inline vec2<U...>& operator-=(vec2<U...>& a, const vec2<U...>& b) { a.x -= b.x; a.y -= b.y; return a; }
        template<class...U> inline vec2<U...>& operator*=(vec2<U...>& a, car_t<U...> const& b) { a.x *= b; a.y *= b; return a; }
        template<class...U> inline vec2<U...>& operator/=(vec2<U...>& a, car_t<U...> const& b) { a.x /= b; a.y /= b; return a; }

        template<class...U> constexpr bool operator==(vec3<U...> const& a, vec3<U...> const& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
        template<class...U> constexpr bool operator==(vec3<U...> const& a, car_t<U...> const& b) { return a.x == b && a.y == b && a.z == b; }
        template<class...U> constexpr bool operator!=(vec3<U...> const& a, vec3<U...> const& b) { return a.x != b.x || a.y != b.y || a.z != b.z; }
        template<class...U> constexpr bool operator!=(vec3<U...> const& a, car_t<U...> const& b) { return a.x != b || a.y != b || a.z != b; }
        template<class...U> constexpr vec3<U...> operator+(const vec3<U...>& a, const vec3<U...>& b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
        template<class...U> constexpr vec3<U...> operator-(const vec3<U...>& a, const vec3<U...>& b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }
        template<class...U> constexpr vec3<U...> operator*(const vec3<U...>& a, car_t<U...> const& b) { return { a.x * b, a.y * b, a.z * b }; }
        template<class...U> constexpr vec3<U...> operator/(const vec3<U...>& a, car_t<U...> const& b) { return { a.x / b, a.y / b, a.z / b }; }
        template<class...U> inline vec3<U...>& operator+=(vec3<U...>& a, const vec3<U...>& b) { a.x += b.x; a.y += b.y; a.z += b.z; return a; }
        template<class...U> inline vec3<U...>& operator-=(vec3<U...>& a, const vec3<U...>& b) { a.x -= b.x; a.y -= b.y; a.z -= b.z; return a; }
        template<class...U> inline vec3<U...>& operator*=(vec3<U...>& a, car_t<U...> const& b) { a.x *= b; a.y *= b; a.z *= b; return a; }
        template<class...U> inline vec3<U...>& operator/=(vec3<U...>& a, car_t<U...> const& b) { a.x /= b; a.y /= b; a.z /= b; return a; }

        template<class T, class U> size_t in_polygon_xy(T x, T y, size_t n, U const ps[])
        {
            if (n < 1)
                return 0;

            auto bx = x - ps[n - 1].x;
            auto by = y - ps[n - 1].y;

            size_t w = 0;
            for (size_t i = 0; i < n; ++i)
            {
                auto ax = bx;
                auto ay = by;
                bx = x - ps[i].x;
                by = y - ps[i].y;

                // ay && by have different signs?
                if ((ay ^ by) < 0)
                {
                    auto ab = ax * by - ay * bx;

                    // winding number, by && ab have diffent signs? +/- 1 depending on which side of line we are on, or 0 if we are on the line
                    w += (by ^ ab) < 0 ? (ab > 0 ? 1 : 0) - (ab < 0 ? 1 : 0) : 0;

                    // crossing number
                    //w -= (by < 0 ? -d : d) < 0 ? -1 : 0;
                }
            }
            return w;
        }

        template<class T, class U> size_t in_polygon(T const& p, size_t n, U const ps[])
        {
            return in_polygon_xy<decltype(dotx(p.x - ps[0].x, p.y - ps[0].y))>(p.x, p.y, n, ps);
        }

    }

    using namespace math;

    namespace random
    {
        // 64 bit state - 32 bit result
        // http://prng.di.unimi.it/xoroshiro64starstar.c
        struct xoroshiro64ss
        {
            uint32_t a;
            uint32_t b;

            friend void seed(xoroshiro64ss& g, size_t size, uint8_t const data[])
            {
                auto a = crc32(crc32c_t::initial, size / 2, data);
                auto b = crc32(a, size - size / 2, data + size / 2);

                g.a = uint32_t(a);
                g.b = uint32_t(b);
            }

            friend uint32_t next(xoroshiro64ss& g)
            {
                const uint32_t a = g.a;
                const uint32_t b = g.b ^ a;
                g.a = CE_ROTL32(a, 26) ^ b ^ (b << 9);
                g.b = CE_ROTL32(b, 13);
                return CE_ROTL32(a * 0x9e3779bb, 5) * 5;
            }

            friend uint32_t next32(xoroshiro64ss& g) { return next(g); }
            friend uint64_t next64(xoroshiro64ss& g)
            {
                uint64_t a = next32(g);
                return (a << 32) | next32(g);
            }

        };

        // 128 bit state - 64 bit result
        // https://prng.di.unimi.it/xoroshiro128plusplus.c
        struct xoroshiro128pp
        {
            uint64_t a;
            uint64_t b;

            friend void seed(xoroshiro128pp& g, size_t size, uint8_t const data[])
            {
                g.a = ce::hash::fnv1a64(size, data);
                g.b = ce::hash::fnv1a64(size, data, g.a);
            }

            static inline uint64_t rotl(const uint64_t x, int k) {
                return (x << k) | (x >> (64 - k));
            }

            friend uint64_t next(xoroshiro128pp& g)
            {
                uint64_t a = g.a;
                uint64_t b = g.b;
                uint64_t c = a ^ b;
                g.a = rotl(a, 49) ^ c ^ (c << 21);
                g.b = rotl(c, 28);
                return rotl(a + b, 17) + a;
            }

            friend uint32_t next32(xoroshiro128pp& g) { return uint32_t(next(g)); }
            friend uint64_t next64(xoroshiro128pp& g) { return next(g); }

        };
    }

    using namespace random;

    namespace ascii
    {
        constexpr bool is_base10(char a) { return a >= '0' && a <= '9'; }
        constexpr bool is_base16(char a) { return a >= '0' && a <= '9' || a >= 'A' && a <= 'F' || a >= 'a' && a <= 'f'; }
        constexpr bool is_base64(char a) { return (a >= '0' && a <= '9') || (a >= 'A' && a <= 'Z') || (a >= 'a' && a <= 'z') || (a == '+') || (a == '/'); }
        constexpr bool is_alpha(char a) { return a >= 'A' && a <= 'Z' || a >= 'a' && a <= 'z'; }
        constexpr bool is_upper(char a) { return a >= 'A' && a <= 'Z'; }
        constexpr bool is_lower(char a) { return a >= 'a' && a <= 'z'; }
    }

    namespace base64
    {

        // non overflowing n * 3 / 4
        constexpr size_t size(size_t n) { return n / 4 * 3 + n % 4 * 3 / 4; }

        constexpr size_t size(char const src[])
        {
            for (size_t n = 0;; ++n)
                if (!ascii::is_base64(src[n]))
                    return size(n);
        }

        constexpr void decode(size_t dst_size, uint8_t dst_data[], char const src[])
        {
            uint8_t dec[256]{ };
            for (size_t i = 0; i < 64; ++i)
                dec["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = uint8_t(i);

            for (size_t i = 0; i < dst_size; ++i)
            {
                auto a = dec[src[i * 4 / 3 + 0]];
                auto b = dec[src[i * 4 / 3 + 1]];
                size_t n = i % 3 * 2 + 2;
                dst_data[i] = (a << n) + (b >> (6 - n));
            }
        }

        template<class T> struct decoder
        {
            static constexpr auto size = base64::size(T::data);
            uint8_t data[size];

            constexpr decoder() : data{ } { base64::decode(size, data, T::data); }
        };
    }

    namespace os
    {
        struct file_t { void* os_handle; };

        void debug_out(char const text[]);

        uint64_t monotonic_timestamp();
        uint64_t monotonic_frequency();

        bool open_file(file_t& file, char const path[]);
        bool close_file(file_t& file);
        bool map_span(span<uint8_t const>&, file_t);
        bool map_span(span<uint8_t const>&, char const path[]);
        bool unmap_span(span<uint8_t const>& span);

        template<class T, class...U> inline span<T const> map_span(U&&...u)
        {
            span<uint8_t const> view;
            map_span(view, static_cast<U>(u)...);
            return { view.size / sizeof(T), reinterpret_cast<T const*>(view.data) };
        }

        uint8_t* virtual_alloc(size_t size);

        void log(int level, int argc, char const* argv[][2]);
    }

    template<class T>
    struct rgba
    {
        T r;
        T g;
        T b;
        T a;

        using const_array_ref_t = T const (&)[4];
        operator const_array_ref_t () const { return reinterpret_cast<const_array_ref_t>(*this); }
        const_array_ref_t operator()() const { return reinterpret_cast<const_array_ref_t>(*this); }
    };

    //--------
    // unsafe conversion from value to text ((utf8, NOT null terminated)

    char* to_text(char[], float);
    char* to_text(char[], double);

    char* to_text(char[], unsigned char);
    char* to_text(char[], unsigned short);
    char* to_text(char[], unsigned int);
    char* to_text(char[], unsigned long);
    char* to_text(char[], unsigned long long);

    char* to_text(char[], signed char);
    char* to_text(char[], short);
    char* to_text(char[], int);
    char* to_text(char[], long);
    char* to_text(char[], long long);

    inline char* to_text(char text[], char value) { text[0] = value; return text + 1; }
    inline char* to_text(char text[], bool value)
    {
        text[0] = '#';
        text[1] = '<';
        text[2] = "FT"[value ? 1 : 0];
        text[3] = '>';
        return text + 4;
    }

    inline char* to_text_p(char text[], char const value[])
    {
        char* p = text;
        for (size_t i = 0; value[i] != 0; ++i)
            *p++ = value[i];
        return p;
    }

    template<class T> char* to_text_p(char text[], T const* value)
    {
        return to_text(to_text(to_text_p(text, "#<ptr "), size_t(value)), '>');
    }

    inline char* to_text_s(char text[], size_t n, char const value[])
    {
        char* p = text;
        for (size_t i = 0; i < n && value[i] != 0; ++i)
            *p++ = value[i];
        return p;
    }

    template<class...Ts> char* to_text(char text[], Ts const&...values) { return ((text = to_text(text, values)), ...); }

    template<class T> char* to_text(char text[], T const& value)
    {
        if constexpr (is_enum<T>::value)
        {
            // unknown enum types as underlying type
            return to_text(to_text_p(text, "#<enum "), as_cast{ value }.as, '>');
        }
        else if constexpr (is_same<T, decltype(nullptr)>::value)
        {
            // special case nullptr_t
            return to_text_p(text, "#<nil>");
        }
        else if constexpr (is_ptr<T>::value)
        {
            // pointer types
            return value == nullptr ? to_text_p(text, "#<nil>") : to_text_p(text, value);
        }
        else
        {
            // unknown types, show address 
            return to_text(to_text(to_text_p(text, "#<ref "), size_t(&value)), '>');
        }
    }

    template<class T> char* to_text_n(char text[], size_t n, T const values[])
    {
        text = to_text(text, '[');

        char const* p = "";
        for (size_t i = 0; i < n; p = ", ", ++i)
            text = to_text(text, p, values[i]);

        return to_text(text, ']');
    }

    template<size_t N, class T> char* to_text(char text[], T const (&values)[N])
    {
        if constexpr (is_same_v<T, char>)
            return to_text_s(text, N, &values[0]);
        else
            return to_text_n(text, N, &values[0]);
    }

    template<size_t N, class...U> char* to_text(char text[], vec<N, U...> const& value)
    {
        return to_text_n(text, N, &value.x);
    }

    //--------

    template<class...Ts> struct measure
    {
        static constexpr size_t value = (measure<Ts>::value + ...);
    };

    template<class T> struct measure<T&> : measure<T> { };
    template<class T> struct measure<T const> : measure<T> { };
    template<class T> struct measure<T volatile> : measure<T> { };

    template<class T> struct measure<T> { static constexpr size_t value = 1024; };
    template<size_t N> struct measure<char[N]> { static constexpr size_t value = N; };
    template<> struct measure<char> { static constexpr size_t value = 1; };
    template<> struct measure<signed char> { static constexpr size_t value = 4; };
    template<> struct measure<unsigned char> { static constexpr size_t value = 3; };
    template<> struct measure<short> { static constexpr size_t value = 6; };
    template<> struct measure<unsigned short> { static constexpr size_t value = 5; };
    template<> struct measure<int> { static constexpr size_t value = 11; };
    template<> struct measure<unsigned int> { static constexpr size_t value = 10; };
    template<> struct measure<long> { static constexpr size_t value = sizeof(long) > sizeof(int) ? 20 : 11; };
    template<> struct measure<unsigned long> { static constexpr size_t value = sizeof(unsigned long) > sizeof(unsigned int) ? 20 : 10; };
    template<> struct measure<long long> { static constexpr size_t value = 20; };          // -9223372036854775808
    template<> struct measure<unsigned long long> { static constexpr size_t value = 20; }; // 18??????????????????

    template<class...Ts> struct as_string
    {
        char as[measure<Ts...>::value + 1];
        as_string(Ts...ts) { *ce::to_text(as, ts...) = 0; }
    };

    template<size_t N> struct as_string<char const (&)[N]>
    {
        char const (&as)[N];
    };

    template<class...Ts> as_string(Ts const&...)->as_string<Ts const&...>;

    template<size_t N, size_t COUNT> struct names
    {
        static constexpr size_t size = N;
        char text[N]{ };
        char const* data[COUNT + !COUNT]{ };

        constexpr names(char const* src)
        {
            size_t i = 0;
            for (auto& name : data)
            {
                name = &text[i];

                // skip leading spaces
                while (*src <= ' ' && *src != '\0')
                    ++src;

                // if we are at the last data item, start unbalanced so we copy the whole string (
                int balance = &name - data + 1 == COUNT ? 1 : 0;
                char quote = 0;
                bool escape = false;
                size_t trim = i;

                // copy until comma (with balanced ()[]{} and not in a string or char) or eos
                while (*src != '\0')
                {    
                    if (quote == 0)
                    {
                        if (*src == ',' && balance == 0)
                            break;
                        else if (*src == '"' || *src == '\'')
                            quote = *src;
                        else if (*src == '(' || *src == '[' || *src == '{')
                            balance += 1;
                        else if (*src == ')' || *src == ']' || *src == '}')
                            balance -= 1;
                    }
                    else if (escape)
                        escape = false;
                    else if (*src == '\\')
                        escape = true;
                    else if (*src == quote)
                        quote = 0;

                    if ((text[i++] = *src++) > ' ')
                        trim = i;
                }

                // trim tailing spaces
                i = trim;

                text[i] = '\0';

                // skip the comma
                if (*src != '\0')
                    ++i, ++src;
            }
        }
    };

#define CE_AS_NAMES_EX(N, NAMES) []{ static constexpr ce::names<sizeof(NAMES ""), N> _{ NAMES "" }; return _.data; }()

    template<class...Ts> CE_NOINLINE void call_log_hook(int level, char const* const keys[], Ts const&...ts)
    {
        constexpr int argc = sizeof...(Ts);
        CE_LOG_HOOK(level, argc, ce::identity_t<char const* [argc + 1][2]>{ { *keys++, as_string{ ts }.as }..., { nullptr, nullptr } });
    }

}

#define _CE_LOG_fatal ~, 0
#define _CE_LOG_error ~, 1
#define _CE_LOG_warn ~, 2
#define _CE_LOG_note ~, 3
#define _CE_LOG_info ~, 4
#define _CE_LOG_debug ~, 5
#define _CE_LOG_spam ~, 6
#define _CE_LOG_more ~, 7

#define _CE_LOG_LVL(LEVEL) CE_PP_GET_1 CE_PP_ARGS(_CE_LOG_ ## LEVEL, 7)

#define CE_LOG_EX(LVL, NAMES, ...) CE_PP_WHEN(CE_LOG_LEVEL)(ce::call_log_hook(LVL, CE_AS_NAMES_EX(CE_COUNTOF_ARGS(__VA_ARGS__), NAMES), __VA_ARGS__))(CE_REF(__VA_ARGS__))

#define CE_LOG(LEVEL, ...) CE_LOG_EX(_CE_LOG_LVL(LEVEL), "$," #__VA_ARGS__, #LEVEL, ## __VA_ARGS__)
#define CE_LOG_MSG(LEVEL, ...) CE_LOG_EX(_CE_LOG_LVL(LEVEL), "$,$msg", #LEVEL, ce::as_string{ __VA_ARGS__ }.as)

//#define CE_USER_CHECKED
#if defined(CE_USER_CHECKED)
CE_STATIC_ASSERT(ce::detail::crc32_table<ce::crc32c_t, 0>::vs[0] == 0, "bad crc32c table entry");
CE_STATIC_ASSERT(ce::detail::crc32_table<ce::crc32c_t, 0>::vs[1] == 0xf26b8303);
CE_STATIC_ASSERT(ce::detail::crc32_table<ce::crc32c_t, 0>::vs[255] == 0xad7d5351);
CE_STATIC_ASSERT(ce::detail::crc32_table<ce::crc32c_t, 1>::vs[0] == 0);
CE_STATIC_ASSERT(ce::detail::crc32_table<ce::crc32c_t, 1>::vs[1] == 0x13a29877);
CE_STATIC_ASSERT(ce::detail::crc32_table<ce::crc32c_t, 1>::vs[255] == 0xa3e32483);
CE_STATIC_ASSERT(ce::detail::crc32_table<ce::crc32c_t, 15>::vs[255] == 0x8fda7dfa);

CE_STATIC_ASSERT(ce::crc32(ce::crc32c_t::initial, 1, ce::items_v<ce::uint64_t, 0x1234567812345678ull>) == ce::crc32(ce::crc32c_t::initial, 2, ce::items_v<ce::uint32_t, 0x12345678u, 0x12345678u>));
CE_STATIC_ASSERT(ce::crc32(ce::crc32c_t::initial, 1, ce::items_v<ce::uint64_t, 0x1234567812345678ull>) ==
    ce::crc32(ce::crc32c_t::initial, 8, ce::items_v<ce::uint8_t, 0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12>));

CE_STATIC_ASSERT(ce::crc32c("The quick brown fox jumps over the lazy dog") == 0x22620404);
CE_STATIC_ASSERT(ce::crc32c("123456789") == 0xe3069283);

CE_STATIC_ASSERT(ce::hash::fnv1a("Mallori, Jimmy, Sydney") == 0xaaad3b41);
#endif
