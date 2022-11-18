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
        static constexpr auto same(char const a[], char const b[])
        {
            for (unsigned i = 0;; ++i)
                if (a[i] != b[i] || a[i] == '\0')
                    return i;
        }

        static constexpr unsigned skip(char what, char const text[])
        {
            for (unsigned n = 0; text[n] != 0; ++n)
                if (text[n] == what && text[n + 1] != what)
                    return n + 1;
            return 0;
        };

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

        template<class T, auto... N> static constexpr auto& name() { return __PRETTY_FUNCTION__; }
        static constexpr auto& name_void = name<void>();
        static constexpr auto skip_type = same(name_void, name<int>());
        static constexpr auto skip_item = same(name<void, 0>(), name_void);
        template<class T> static constexpr auto name_type = const_text<sizeof(name<T>()) - sizeof(name_void) + 4>(name<T>() + skip_type);
        template<auto N> static constexpr auto name_item = const_text<sizeof(name<void, N>()) - sizeof(name_void)>(name<void, N>() + skip_item);

        template<class T, T...> struct nameof;
        template<class T> struct nameof<T>
        {
            static constexpr auto _skip = skip(' ', name_type<T>.data);
            static constexpr auto text = const_text<name_type<T>.size - _skip>{ name_type<T>.data + _skip };
        };

        template<class T, T N> struct nameof<T, N>
        {
            static constexpr bool is_known = name_item<N>.data[0] != '(';
            static constexpr auto _skip = skip(':', name_item<N>.data);
            static constexpr auto text = const_text<is_known ? name_item<N>.size - _skip : 3>{ is_known ? name_item<N>.data + _skip : "???" };
        };

        template<class T> struct result
        {
            T data;
            bool good;
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
                    using N = detail::nameof<T, T{ L }>;
                    if constexpr (N::is_known)
                        return N::text.data;
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

        template<class T, unsigned L, unsigned H> static constexpr result<T> as_enum(char const name[])
        {
            if constexpr (H - L > 64)
            {
                constexpr auto M = L + (H - L) / 2;
                auto n = as_enum<T, L, M>(name);
                return n.good ? n : as_enum<T, M, H>(name);
            }
            else if constexpr (L < H)
            {
                using N = detail::nameof<T, T{ L }>;
                if constexpr (N::is_known)
                {
                    if (same(N::text.data, name) == N::text.size && name[N::text.size] == '\0')
                        return { T{ L }, true };
                }
                return as_enum<T, L + 1, H>(name);
            }
            else
            {
                return { T{ }, false };
            }
        }
    }

    template<class T> char const* nameof(T n) { return nameof_detail::nameof_enum<T, 0, 256>(n); }
    template<class T> constexpr auto& nameof() { return nameof_detail::nameof<T>::text.data; }
    template<class T> T as_enum(char const name[], T unknown)
    { 
        auto n = nameof_detail::as_enum<T, 0, 256>(name);
        return n.good ? n.data : unknown;
    }
}
