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

namespace ce
{
    namespace os
    {
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

        inline bool error(int level, int argc, char const* argv[][2])
        {
            auto out = [](char const text[]) { if (text != nullptr) debug_out(text); };

            if (argc < 2 || argv == nullptr)
                return level != 0;

            out("******** FAILED ********\n");

            out(argv[0][1]);
            out(": ");
            out(argv[0][0]);
            out(": ");
            out(argv[1][0]);
            out(": (");
            out(argv[1][1]);
            out(") = (");
            char const* prefix = "";
            for (int i = 2; i < argc; ++i)
            {
                out(prefix);
                prefix = ", ";
                //out(argv[i][0]);
                //out(" = ");
                out(argv[i][1]);
            }
            out(")\n");
            out("************************\n");

            return level != 0;
        }
    }
}
