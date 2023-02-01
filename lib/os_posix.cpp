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

#include "ce/ce.h"

#if CE_API_POSIX

#include <unistd.h>
#include <time.h>

namespace ce
{
    namespace os
    {
        void debug_out(char const text[])
        {
            write(STDERR_FILENO, text, __builtin_strlen(text));
        }

        uint64_t monotonic_timestamp()
        {
            timespec ts;
            if (clock_gettime(CLOCK_MONOTONIC_RAW, &ts))
                return 0;

            return ts.tv_nsec + ts.tv_sec * 1000000000;
        }

        uint64_t monotonic_frequency()
        {
            return 1000000000;
        }

        void sleep_ns(uint64_t ns)
        {
            timespec ts;
            if (ns < 1000000000u)
            {
                ts.tv_sec = 0;
                ts.tv_nsec = ns;
            }
            else
            {
                ts.tv_sec = ns / 1000000000u;
                ts.tv_nsec = ns % 1000000000u;
            }
            nanosleep(&ts, nullptr);
        }

        file_t open_file(char const [])
        {
            return { };
        }

        bool open_file(file_t&, char const [])
        {
            return false;
        }

        bool close_file(file_t&)
        {
            return false;
        }

        // read only
        bool map_span(span<uint8_t const>& span, file_t)
        {
            span.size = 0;
            span.data = nullptr;
            return false;
        }

        bool map_span(span<uint8_t const>& span, char const path[])
        {
            auto file = open_file(path);
            auto okay = map_span(span, file);
            close_file(file);
            return okay;
        }

        bool unmap_span(span<uint8_t const>& span)
        {
            span.size = 0;
            span.data = nullptr;
            return false;
        }

        uint8_t* virtual_alloc(size_t)
        {
            return nullptr;
        }
    }
}
#endif
