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
    template<int N, int COUNT> struct names_t
    {
        char text[N]{ };
        char const* names[COUNT]{ };

        constexpr names_t(char const (&s)[N])
        {
            int j = 0;

            for (int i = 0, k = 0; i < COUNT; ++i)
            {
                names[i] = &text[j];

                while (s[k] != '\0' && s[k] <= ' ')
                    ++k;

                while (s[k] > ' ' && s[k] != ',')
                    text[j++] = s[k++];

                if (s[k] == ',')
                    ++k;

                text[j++] = '\0';
            }

            while (j < N)
                text[j++] = '\0';
        }
    };

    template<class T> struct reflection
    {
        template<class IO> void io_self(IO& io) { io.atom(*reinterpret_cast<T*>(this)); }
    };

    template<class IO>
    struct io
    {
        template<class T>
        void item(T& data)
        {
            reinterpret_cast<ce::reflection<T>*>(&data)->io_self(*static_cast<IO*>(this));
        }

        template<class T, size_t N>
        void item(T (&data)[N])
        {
            auto size = N;
            static_cast<IO*>(this)->enter_array(size, false);
            for (size_t i = 0; i < size; ++i)
                item(data[i]);
            static_cast<IO*>(this)->leave_array(size, false);
        }

        template<class T>
        void item(char const name[], T& data)
        {
            static_cast<IO*>(this)->name(name);
            item(data);
        }

        template<class... Ts>
        void items(char const* const names[], Ts&... data)
        {
            static_cast<IO*>(this)->enter_class();
            (item(*names++, data), ...);
            static_cast<IO*>(this)->leave_class();
        }

        template<class T>
        void items(size_t& size, T data[])
        {
            static_cast<IO*>(this)->enter_array(size, true);
            for (size_t i = 0; i < size; ++i)
                item(data[i]);
            static_cast<IO*>(this)->leave_array(size, true);
        }
    };

    // treat ce::list as an array of `size` items of type 'T', TODO jea make sure we have the hooks we need for deserialization
    template<size_t N, class T> struct reflection<list<T, N>> : list<T, N>
    {
        template<class IO> void io_self(IO& io) { io.items(this->size, this->data); }
    };

    // treat smid vecs as fixed size arrays
    template<size_t N, class T, class...Ks> struct reflection<vec<N, T, Ks...>> : vec<N, T, Ks...>
    {
        template<class IO> void io_self(IO& io) { io.item(*reinterpret_cast<identity_t<T[N]>*>(this)); }
    };

    template<class T, class...Ks> struct reflection<vec<2, T, Ks...>> : vec<2, T, Ks...>
    {
        template<class IO> void io_self(IO& io) { io.items(identity_t<char const* const[]>{ "x", "y" }, this->x, this->y); }
    };

}

#define CE_IO_BIND(TYPE, ...) template<> struct ce::reflection<TYPE> : TYPE { template<class IO> void io_self(IO& io) { static constexpr ce::names_t<sizeof(# __VA_ARGS__), CE_COUNTOF_ARGS(__VA_ARGS__)> _n{ #__VA_ARGS__}; io.items(_n.names, __VA_ARGS__); } }
#define CE_IO_AS(T, U) template<> struct ce::reflection<T> : T { template<class IO> void io_self(IO& io) { io.item(*(ce::identity_t<U>*)this); } };
