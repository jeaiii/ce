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

// ce::atomic specialized on std

#include <atomic>

namespace ce
{
    template<class T> struct atomic { std::atomic<T> atom; };
    template<> struct atomic<void> { };

    template<class T> using atomic_mem_t = decltype(T() == T(), T());
    template<class T> using atomic_val_t = decltype(*(T*)0 += T() - T(), T() - T());
    template<class T> using atomic_int_t = decltype(*(T*)0 ^= T() ^ T(), T() ^ T());

    template<class T> atomic_mem_t<T> atomic_load(atomic<T> const& a) { return a.atom.load(); }
    template<class T> void atomic_store(atomic<T>& a, atomic_mem_t<T> v) { return a.atom.store(v); }
    template<class T> T atomic_exchange(atomic<T>& a, atomic_mem_t<T> v) { return a.atom.exchange(v); }
    template<class T> bool atomic_compare_exchange(atomic<T>& a, T& expected, atomic_mem_t<T> v) { return a.atom.compare_exchange_strong(expected, v); }

    // polyfill fetch_add & fetch_sub if missing (e.g. floats before std c++20)
    template<class T> constexpr bool has_fetch_add(...) { return false; }
    template<class T> constexpr bool has_fetch_add(decltype(((std::atomic<T>*)0)->fetch_add(atomic_val_t<T>()), 0)) { return true; }

    template<class T> T atomic_fetch_add(atomic<T>& a, atomic_val_t<T> v)
    {
        if constexpr (has_fetch_add<T>(0))
            return a.atom.fetch_add(v);
        else
            for (T e = a.atom.load(std::memory_order_relaxed);;)
                if (T n = e; a.atom.compare_exchange_weak(e, n += v))
                    return e;
    }

    template<class T> T atomic_fetch_sub(atomic<T>& a, atomic_val_t<T> v)
    {
        if constexpr (has_fetch_add<T>(0))
            return a.atom.fetch_sub(v);
        else
            for (T e = a.atom.load(std::memory_order_relaxed);;)
                if (T n = e; a.atom.compare_exchange_weak(e, n -= v))
                    return e;
    }


    template<class T> T atomic_fetch_xor(atomic<T>& a, atomic_int_t<T> v) { return a.atom.fetch_xor(v); }
    template<class T> T atomic_fetch_and(atomic<T>& a, atomic_int_t<T> v) { return a.atom.fetch_and(v); }
    template<class T> T atomic_fetch_or(atomic<T>& a, atomic_int_t<T> v) { return a.atom.fetch_or(v); }
}
