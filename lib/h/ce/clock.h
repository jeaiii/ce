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
    template<uint64_t Fn, uint64_t Fd = 1>
    struct clock
    {
        uint64_t frequency;
        uint64_t timestamp;
        uint64_t residual;
        uint64_t tick;

        void reset()
        {
            frequency = ce::os::monotonic_frequency();
            timestamp = ce::os::monotonic_timestamp();
            residual = 0;
            tick = 0;
        }

        auto elapsed()
        {
            auto current_ts = ce::os::monotonic_timestamp();
            auto elapsed_ts = current_ts - timestamp;
            timestamp = current_ts;

            auto fd = frequency * Fd;

            auto whole = elapsed_ts / fd * Fn;
            auto fract = elapsed_ts % fd * Fn + residual;

            residual = fract % fd;

            auto dt = whole + fract / fd;
            tick += dt;

            return dt;
        }

        auto current()
        {
            (void)elapsed();
            return tick;
        }
    };
}
