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

#include "ce.h"

#if CE_CPU_X86 && defined(_MSC_VER)

#include <emmintrin.h>

namespace ce
{
    namespace detail
    {
        // MSVC intrinsics

        extern "C" __int8 __iso_volatile_load8(const volatile __int8* Location);
        extern "C" __int16 __iso_volatile_load16(const volatile __int16* Location);
        extern "C" __int32 __iso_volatile_load32(const volatile __int32* Location);
        extern "C" __int64 __iso_volatile_load64(const volatile __int64* Location);

        extern "C" void __iso_volatile_store8(const volatile __int8* Location, __int8);
        extern "C" void __iso_volatile_store16(const volatile __int16* Location, __int32);
        extern "C" void __iso_volatile_store32(const volatile __int32* Location, __int32);
        extern "C" void __iso_volatile_store64(const volatile __int64* Location, __int64);

        extern "C" char _InterlockedExchange8(char volatile* Addend, char Value);
        extern "C" short _InterlockedExchange16(short volatile* Addend, short Value);
        extern "C" long _InterlockedExchange(long volatile* Addend, long Value);
        extern "C" __int64 _InterlockedExchange64(__int64 volatile* Addend, __int64 Value);
        extern "C" void* _InterlockedExchangePointer(void* volatile* Target, void* Value);

        extern "C" char _InterlockedExchangeAdd8(char volatile* Addend, char Value);
        extern "C" short _InterlockedExchangeAdd16(short volatile* Addend, short Value);
        extern "C" long _InterlockedExchangeAdd(long volatile* Addend, long Value);
        extern "C" __int64 _InterlockedExchangeAdd64(__int64 volatile* Addend, __int64 Value);

        extern "C" char _InterlockedCompareExchange8(char volatile* Destination, char Exchange, char Comperand);
        extern "C" short _InterlockedCompareExchange16(short volatile* Destination, short Exchange, short Comperand);
        extern "C" long _InterlockedCompareExchange(long volatile* Destination, long Exchange, long Comperand);
        extern "C" __int64 _InterlockedCompareExchange64(__int64 volatile* Destination, __int64 Exchange, __int64 Comperand);
        extern "C" void* _InterlockedCompareExchangePointer(void* volatile* Target, void* Exchange, void* Comperand);

        extern "C" void _ReadWriteBarrier();

        auto scope_spin_lock(void volatile const* p)
        {
            static void* locks[256];

            struct scope_lock
            {
                void* volatile& lock;
                ~scope_lock()
                {
                    _ReadWriteBarrier();
                    lock = 0;
                }
            };

            void* volatile* spin = &locks[255 & (static_cast<void**>(const_cast<void*>(p)) - static_cast<void**>(0))];
            while (_InterlockedCompareExchangePointer(spin, const_cast<void**>(spin), 0) != 0)
                do _mm_pause();
                while (*spin != 0);

            return scope_lock{ *spin };
        }

        // map sizes to MSVC interlocked types
        template<unsigned N> struct atom { using type = char[N]; };
        template<> struct atom<1> { using type = char; };
        template<> struct atom<2> { using type = short; };
        template<> struct atom<4> { using type = long; };
        template<> struct atom<8> { using type = __int64; };
        template<unsigned N> using atom_t = typename atom<N>::type;

        // tests for native types (consdier in ce.h)
        template<class U> static constexpr bool test_can_sub(...) { return false; }
        template<class U> static constexpr bool test_can_sub(decltype(U() - U(), 0)) { return true; }
        template<class U> static constexpr bool test_can_user_sub(...) { return false; }
        template<class U> static constexpr bool test_can_user_sub(decltype(operator-(U(), U()), 0)) { return true; }

        template<class U> static constexpr bool test_can_xor(...) { return false; }
        template<class U> static constexpr bool test_can_xor(decltype(U() ^ U(), 0)) { return true; }
        template<class U> static constexpr bool test_can_user_xor(...) { return false; }
        template<class U> static constexpr bool test_can_user_xor(decltype(operator^(U(), U()), 0)) { return true; }

        template<class T> constexpr bool can_cpu_sub = test_can_sub<T>(0) && !test_can_user_sub<T>(0);
        template<class T> constexpr bool can_cpu_xor = test_can_xor<T>(0) && !test_can_user_xor<T>(0);

        template<class T> struct info
        {
            static constexpr auto size = sizeof(T);
            static constexpr bool is_cpu = can_cpu_sub<T>;
            static constexpr bool is_fpu = can_cpu_sub<T> && !can_cpu_xor<T> && !is_ptr<T>::value;
            static constexpr bool is_mem = (size & size - 1) == 0 && size <= 16;

            static constexpr bool is_m8 = size == 1;
            static constexpr bool is_m16 = size == 2;
            static constexpr bool is_m32 = size == 4;
            static constexpr bool is_m64 = size == 8;
            static constexpr bool is_m128 = size == 16;

            static constexpr bool is_i8 = is_m8 && is_cpu && !is_fpu;
            static constexpr bool is_i16 = is_m16 && is_cpu && !is_fpu;
            static constexpr bool is_i32 = is_m32 && is_cpu && !is_fpu;
            static constexpr bool is_i64 = is_m64 && is_cpu && !is_fpu;
            static constexpr bool is_i128 = is_m128 && is_cpu && !is_fpu;
        };

        // MSVC does really well with using a union to bit cast between types
        // except for float <-> int where it always goes through memory (__builtin_bit_cast has this same problem)
        // so use sse intrinsics to make it use register <-> sse register instructions
        // could do this with function overloads easily, but then extra function calls in debug...
        // could build this into ce::as_cast, but then need emmintrin.h everywhere and maybe MSVC will improve this

        template<class As, class From> union atomic_as_cast { From from; As as; };

        // bit cast as long from float
        template<> union atomic_as_cast<long, float>
        {
            long as;
            atomic_as_cast(float from) : as{ _mm_cvtsi128_si32(_mm_castps_si128(_mm_set_ss(from))) } { }
        };

        // bit cast as float from long
        template<> union atomic_as_cast<float, long>
        {
            float as;
            atomic_as_cast(long from) : as{ _mm_cvtss_f32(_mm_castsi128_ps(_mm_cvtsi32_si128(from))) } { }
        };

        template<class U> using as_atom = atomic_as_cast<atom_t<sizeof(U)>, U>;
        template<class U> using as_data = atomic_as_cast<U, atom_t<sizeof(U)>>;

        template<class T> using arg_t = cond_t<sizeof(T) <= 8, T, T const&>;
        template<class T> using alu_t = arg_t<decltype(T() - T())>;
        template<class T> using bit_t = arg_t<decltype(T() ^ T())>;

        namespace public_atomic
        {
            template<class T> union atomic
            {
                T data;
                atom_t<sizeof(T)> volatile atom;

                atomic(const T& v) : data{ v } { };
                atomic() = default;
                atomic(const atomic&) = delete;
                atomic& operator=(const atomic&) = delete;
            };

            template<class T> inline T atomic_load(atomic<T> const& a) noexcept
            {
                if constexpr (info<T>::is_cpu)
                {
                    T u = reinterpret_cast<T const volatile&>(a.data);
                    _ReadWriteBarrier();
                    return u;
                }
                else
                {
                    auto spin = scope_spin_lock(&a);
                    auto u = a.data;
                    return u;
                }
            }

            template<class T> inline void atomic_store(atomic<T>& a, arg_t<T> b) noexcept
            {
                if constexpr (info<T>::is_m8)
                    _InterlockedExchange8(&a.atom, as_atom<T>{ b }.as);
                else if constexpr (info<T>::is_m16)
                    _InterlockedExchange16(&a.atom, as_atom<T>{ b }.as);
                else if constexpr (info<T>::is_m32)
                    _InterlockedExchange(&a.atom, as_atom<T>{ b }.as);
                else if constexpr (info<T>::is_m64)
                    _InterlockedExchange64(&a.atom, as_atom<T>{ b }.as);
                else
                {
                    auto spin = scope_spin_lock(&a);
                    a.data = b;
                }
            }

            template<class T> inline T atomic_exchange(atomic<T>& a, arg_t<T> b) noexcept
            {
                if constexpr (info<T>::is_m8)
                    return as_data<T>{ _InterlockedExchange8(&a.atom, as_atom<T>{ b }.as) }.as;
                else if constexpr (info<T>::is_m16)
                    return as_data<T>{ _InterlockedExchange16(&a.atom, as_atom<T>{ b }.as) }.as;
                else if constexpr (info<T>::is_m32)
                    return as_data<T>{ _InterlockedExchange(&a.atom, as_atom<T>{ b }.as) }.as;
                else if constexpr (info<T>::is_m64)
                    return as_data<T>{ _InterlockedExchange64(&a.atom, as_atom<T>{ b }.as) }.as;
                else
                {
                    auto spin = scope_spin_lock(&a);
                    auto u = a.data;
                    a.data = b;
                    return u;
                }
            }

            template<class T> inline bool atomic_compare_exchange(atomic<T>& a, T& b, arg_t<T> c)
            {
                if constexpr (info<T>::is_mem)
                {
                    atom_t<sizeof(T)> n = as_atom<T>{ c }.as;
                    atom_t<sizeof(T)> p = as_atom<T>{ b }.as;

                    if constexpr (info<T>::is_m8)
                        n = _InterlockedCompareExchange8(&a.atom, n, p);
                    else if constexpr (info<T>::is_m16)
                        n = _InterlockedCompareExchange16(&a.atom, n, p);
                    else if constexpr (info<T>::is_m32)
                        n = _InterlockedCompareExchange(&a.atom, n, p);
                    else if constexpr (info<T>::is_m64)
                        n = _InterlockedCompareExchange64(&a.atom, n, p);

                    b = as_data<T>{ n }.as;
                    return n == p;
                }
            }

            template<class T> inline T atomic_fetch_add(atomic<T>& a, alu_t<T> n) noexcept
            {
                if constexpr (info<T>::is_i8)
                    return as_data<T>{ _InterlockedExchangeAdd8(&a.atom, as_atom<T>{ T{ } + n }.as) }.as;
                else if constexpr (info<T>::is_i16)
                    return as_data<T>{ _InterlockedExchangeAdd16(&a.atom, as_atom<T>{ T{ } + n }.as) }.as;
                else if constexpr (info<T>::is_i32)
                    return as_data<T>{ _InterlockedExchangeAdd(&a.atom, as_atom<T>{ T{ } + n }.as) }.as;
                else if constexpr (info<T>::is_i64)
                    return as_data<T>{ _InterlockedExchangeAdd64(&a.atom, as_atom<T>{ T{ } + n }.as) }.as;
                else if constexpr (info<T>::is_mem)
                    for (auto b = a.atom;;)
                    {
                        auto p = b;
                        T u = as_data<T>{ b }.as;
                        b = as_atom<T>{ u += n }.as;

                        if constexpr (info<T>::is_m8)
                            b = _InterlockedCompareExchange8(&a.atom, b, p);
                        else if constexpr (info<T>::is_m16)
                            b = _InterlockedCompareExchange16(&a.atom, b, p);
                        else if constexpr (info<T>::is_m32)
                            b = _InterlockedCompareExchange(&a.atom, b, p);
                        else if constexpr (info<T>::is_m64)
                            b = _InterlockedCompareExchange64(&a.atom, b, p);

                        if (b == p)
                            return as_data<T>{ b }.as;
                    }
                else
                {
                    auto spin = scope_spin_lock(&a);
                    auto u = a.data;
                    a.data += n;
                    return u;
                }
            }

            template<class T> inline T atomic_fetch_sub(atomic<T>& a, alu_t<T> n) noexcept
            {
                if constexpr (info<T>::is_i8)
                    return as_data<T>{ _InterlockedExchangeAdd8(&a.atom, as_atom<T>{ T{ } - n }.as) }.as;
                else if constexpr (info<T>::is_i16)
                    return as_data<T>{ _InterlockedExchangeAdd16(&a.atom, as_atom<T>{ T{ } - n }.as) }.as;
                else if constexpr (info<T>::is_i32)
                    return as_data<T>{ _InterlockedExchangeAdd(&a.atom, as_atom<T>{ T{ } - n }.as) }.as;
                else if constexpr (info<T>::is_i64)
                    return as_data<T>{ _InterlockedExchangeAdd64(&a.atom, as_atom<T>{ T{ } - n }.as) }.as;
                else if constexpr (info<T>::is_mem)
                    for (auto b = a.atom;;)
                    {
                        auto p = b;
                        T u = as_data<T>{ b }.as;
                        b = as_atom<T>{ u -= n }.as;

                        if constexpr (info<T>::is_m8)
                            b = _InterlockedCompareExchange8(&a.atom, b, p);
                        else if constexpr (info<T>::is_m16)
                            b = _InterlockedCompareExchange16(&a.atom, b, p);
                        else if constexpr (info<T>::is_m32)
                            b = _InterlockedCompareExchange(&a.atom, b, p);
                        else if constexpr (info<T>::is_m64)
                            b = _InterlockedCompareExchange64(&a.atom, b, p);

                        if (b == p)
                            return as_data<T>{ b }.as;
                    }
                else
                {
                    auto spin = scope_spin_lock(&a);
                    auto u = a.data;
                    a.data -= n;
                    return u;
                }
            }
        }
    }
    using namespace detail::public_atomic;

}
#endif