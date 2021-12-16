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
    template<int N, class T, class K> struct dictionary
    {
        using data_t = T;
        using index_t = unsigned; //TODO jea determine index_t based on N
        enum class ptr_t : index_t { nil };

        using name_t = K;

        size_t size;
        data_t data[N];

        size_t heap; // this many names form a binary heap
        name_t names[N];

        void optimize() { } //TODO jea sort into binary heap

        ptr_t find(name_t n) const
        {
            // heap scan
            for (size_t i = 1; i < heap; i += i + (names[i] >= n))
                if (names[i] == n)
                    return ptr_t(i);

            // linear scan
            for (size_t i = heap; i < size; ++i)
                if (names[i] == n)
                    return ptr_t(i);

            return ptr_t::nil;
        }

        template<class...Ts>
        ptr_t bind(name_t n, Ts&&...ts)
        {
            auto i = size_t(find(n));
            if (i == 0)
            {
                if (size >= N)
                    return ptr_t::nil;

                i = size + !size; // add 1 if size == 0 so we don't allocate 0
                size = i + 1;

                names[i] = n;
            }

            // destruct the data here before we construct it, TODO jea ce::list also needs this
            data[i].~data_t();

            new (reinterpret_cast<detail::new_tag*>(&data[i])) data_t{ static_cast<Ts>(ts)... };

            return ptr_t(i);
        }

        data_t const& operator[](ptr_t p) const
        {
            auto i = size_t(p);
            CE_ASSERT(i != 0 && i < size);
            return data[i];
        }

        data_t& operator[](ptr_t p)
        {
            auto i = size_t(p);
            CE_ASSERT(i != 0 && i < size);
            return data[i];
        }

        data_t const& operator[](name_t n) const
        {
            auto i = size_t(find(n));
            CE_ASSERT(i != 0 && i < size);
            return data[i];
        }

        data_t& operator[](name_t n)
        {
            auto i = size_t(find(n));
            CE_ASSERT(i != 0 && i < size);
            return data[i];
        }
    };
}
