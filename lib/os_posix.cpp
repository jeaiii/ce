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
namespace ce
{
    namespace os
    {
        void debug_out(char const text[])
        {
        }

        uint64_t monotonic_timestamp()
        {
            return 0;
        }

        uint64_t monotonic_frequency()
        {
            return 0;
        }

        file_t open_file(char const path[])
        {
            return { };
        }

        bool open_file(file_t& file, char const path[])
        {
            return false;
        }

        bool close_file(file_t& file)
        {
            return false;
        }

        // read only
        bool map_span(span<uint8_t const>& span, file_t file)
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

        uint8_t* virtual_alloc(size_t size)
        {
            return nullptr;
        }

        void log(int level, int argc, char const* argv[][2])
        {
            char const* prefix = " ";
            char header[5]{ "FEWNIDSM"[level < 0 ? 0 : level > 7 ? 7 : level], ':', ' ', '{' };

            debug_out(header);
            for (int i = 0; i < argc; ++i)
            {
                debug_out(prefix);
                debug_out("\"");
                debug_out(argv[i][0]);
                debug_out("\": \"");
                debug_out(argv[i][1]);
                debug_out("\"");
                prefix = ", ";
            }
            debug_out(" }\n");
        }
    }
}
#endif
