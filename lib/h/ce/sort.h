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

namespace ce
{
    template<class T>
    void insertion_sort(T* a, T* b)
    {
        for (T* p = a; p < b; ++p)
        {
            auto k = *p;
            auto q = p;
            for (; q > a && k < *(q - 1); --q)
                *q = *(q - 1);
            *q = k;
        }
    }

    // median in b
    template<class T>
    void median(T& a, T& b, T& c)
    {
        if (a < b)
        {
            if (b < c)
                return;  // a < b < c
            if (a < c)
                return swap(b, c); // a < c < b
            swap(b, a); // c < a < b
        }
        else
        {
            if (c < b)
                return; // c < b < a;
            if (a < c)
                return swap(b, a); // b < a < c
            swap(b, c); // b < c < a
        }
    }

    template<class T>
    T* partition(T* a, T* b)
    {
        size_t n = b - a;
        T* e = b - 1;
        median(*a, *e, *(n / 2 + a));
        T k = *e;
        T* p = a;

        for (T* q = a; q < e; ++q)
            if (k < *q)
                swap(*p, *q), ++p;
        swap(*p, *e);
        return p;
    }

    template<class T>
    void intro_sort(T* a, T* b, size_t intro)
    {
        size_t n = b - a;
        if (n < 128)
            return insertion_sort(a, b);

        //if (limit == 0)
        //    return heap_sort();

        T* p = partition(a, b);
        size_t limit = intro / 2;
        intro_sort<T>(p + 1, b, limit);
        intro_sort<T>(a, p, limit);
    }

    template<class T>
    void intro_sort(T* a, T* b)
    {
        if (a < b)
        {
            size_t n = b - a;
            if (n < 128)
                return insertion_sort(a, b);

            T* p = partition(a, b);
            intro_sort<T>(a, p, n);
            intro_sort<T>(p + 1, b, n);
        }
    }

    template<class T>
    T const* lower_bound(T const* a, T const* b, T k)
    {
        for (size_t n = b - a; n > 0;)
        {
            size_t step = n / 2;
            T const* i = a + step;
            if (*i < k) {
                a = ++i;
                n -= step + 1;
            }
            else
                n = step;
        }
        return a;
    }

    template<class T>
    T const* upper_bound(T const* a, T const* b, T k)
    {
        for (size_t n = b - a; n > 0;)
        {
            size_t step = n / 2;
            T const* i = a + step;
            if (!(k < *i)) {
                a = ++i;
                n -= step + 1;
            }
            else
                n = step;
        }
        return a;
    }
}
