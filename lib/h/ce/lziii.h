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
    namespace lziii
    {

        // lzss variant
        // a stream of control bytes followed by literals or spans
        // u8 : control - each bit from low to high signals a literal (1) or span (0)
        // u8 : literal - copy literal to decoded data
        // [2]u8 : span - copy from previously decoded data to decoded data
        //      length = span[1] / 16 + 3
        //      offset = span[1] % 16 * 256 + span[0] - 4096

        constexpr size_t size(size_t encoded_size, uint8_t const data[])
        {
            size_t decoded_size = 0;

            unsigned c = 1;
            for (size_t i = 0; i < encoded_size;)
            {
                // need another control byte?
                // there must be data after a control byte,
                // so no need to continue to top of loop after fetch
                if (c == 1)
                    c = data[i++] | 0x100;

                // always consume 1 encoded byte (must exist), a literal or half of a span
                ++i;

                // always produce 1 decoded byte (note +2 below for span length)
                ++decoded_size;

                // it is a span?
                // consume another byte (must exist)
                // (note +2 for a span legnth, spans are [3, 18] bytes)
                if (c % 2 == 0)
                    decoded_size += size_t(2) + data[i++] / 16;

                c /= 2;
            }
            return decoded_size;
        }

        constexpr void decode(size_t dst_size, uint8_t dst_data[], uint8_t const src[])
        {
            size_t s = 0;

            unsigned c = 1;
            for (size_t i = 0; i < dst_size;)
            {
                if (c == 1)
                    c = src[s++] | 0x100;

                uint8_t p = src[s++];

                if (c % 2 == 0)
                    for (size_t q = src[s++], b = i + q % 16 * 256 + p - 4096, e = b + q / 16 + 3; b < e; ++b)
                        dst_data[i++] = dst_data[b];
                else
                    dst_data[i++] = p;

                c /= 2;
            }
        }

        template<class T> struct decoder
        {
            static constexpr T source{};
            static constexpr auto size = lziii::size(source.size, source.data);
            uint8_t data[size];

            constexpr decoder() : data{ } { lziii::decode(size, data, source.data); }
        };

    }
}
