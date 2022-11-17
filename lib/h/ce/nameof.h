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

// TODO add CE_PRETTY_FUNCTION and move useful stuff like same, trim, and const_text to ce.h
#if defined(_MSC_VER)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

namespace ce
{
    namespace nameof_detail
    {
        template<class T, auto... N> static constexpr auto& raw_name() { return __PRETTY_FUNCTION__; }

        static constexpr auto same(char const a[], char const b[])
        {
            for (unsigned i = 0;; ++i)
                if (a[i] != b[i] || a[i] == '\0')
                    return i;
        }

        static constexpr auto raw_type_skip = same(raw_name<void>(), raw_name<int>());
        static constexpr auto raw_type_size = sizeof(raw_name<void>()) - 4;
        static constexpr auto raw_item_skip = same(raw_name<void, 0>(), raw_name<void>());
        static constexpr auto raw_item_size = sizeof(raw_name<void>());

        static constexpr auto trim(char const a[], char const b[])
        {
            auto n = same(a, b);
            return a[n] == '\0' ? n : 0;
        }

        template<unsigned N> struct const_text
        {
            static constexpr unsigned size = N;
            char data[N + 1]{ };

            constexpr const_text(char const text[])
            {
                for (auto& c : data) c = text[&c - data];
                data[N] = 0;
            }
        };

        template<class, auto...> struct nameof;
        template<class T> struct nameof<T>
        {
            static constexpr auto& raw = raw_name<T>();
            static constexpr auto raw_size = sizeof(raw) - raw_type_size;
            static constexpr auto raw_text = raw + raw_type_skip;

            static constexpr auto skip = trim("enum ", raw_text) + trim("class ", raw_text) + trim("struct ", raw_text);
            static constexpr auto size = raw_size - skip;
            static constexpr auto text = const_text<size>{ raw_text + skip };
        };

        template<class U, auto N> struct nameof<U, N>
        {
            static constexpr auto& raw = raw_name<void, N>();
            static constexpr auto size = sizeof(raw) - raw_item_size;
            static constexpr auto text = const_text<size>{ raw + raw_item_skip };

            static constexpr bool is_known = text.data[nameof<decltype(N)>::size] == ':';
            static constexpr auto type_skip = is_known ? nameof<decltype(N)>::size + 2 : 0;
        };

        template<class T, unsigned L, unsigned H> static constexpr char const* nameof_enum(T n)
        {
            if constexpr (H - L > 64)
            {
                constexpr auto M = L + (H - L) / 2;
                return unsigned(n) < M ? nameof_enum<T, L, M>(n) : nameof_enum<T, M, H>(n);
            }
            else if constexpr (L < H)
            {
                if (unsigned(n) == L)
                {
                    using N = nameof<void, T{ L }>;
                    if constexpr (N::is_known)
                        return N::text.data + N::type_skip;
                    else
                        return "???";

                }
                return nameof_enum<T, L + 1, H>(n);
            }
            else
            {
                return "???";
            }
        }
    }

    template<class T> char const* nameof(T n) { return nameof_detail::nameof_enum<T, 0, 256>(n); }
    template<class T> constexpr auto& nameof() { return nameof_detail::nameof<T>::text.data; }
}
