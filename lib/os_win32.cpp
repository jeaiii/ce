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

#include "ce/ce.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#pragma warning( push )
#pragma warning ( disable : 4127 ) // conditional expression is constant
#include "to_text_from_integer.h"
#pragma warning( pop )

#include <charconv>

namespace ce
{
    char16_t* to_utf16(char16_t* dst, size_t src_size, const char* src_data)
    {
        char16_t* d = dst;
        const unsigned char* s = (const unsigned char*)src_data;
        const unsigned char* f = (const unsigned char*)src_data + src_size;

        while (s < f)
        {
            if (s[0] < 0x80)
                *d++ = s[0], s += 1;
            else if (s[0] < 0xe0) // 2 units, 5 + 6 = 11  bits
                *d++ = s[0] * 64 + s[1] - 0xc0 * 64 - 0x80, s += 2;
            else if (s[0] < 0xf0) // 3 units, 4 + 6 + 6 = 16  bits
                *d++ = s[0] * 64 * 64 + s[1] * 64 + s[2] /*- 0xe0 * 64 * 64*/ - 0x80 * 64 - 0x80, s += 3;
            else // 4 units, 3 + 6 + 6 + 6 = 21 bits
                *d++ = 0xd800 - 0xf248 + s[0] * 256 + s[1] * 4 + s[2] / 16,
                *d++ = 0xdc00 - 0x0080 + s[2] % 16 * 64 + s[3],
                s += 4;
        }

        return d;
    }

    extern "C" void* _alloca(ce::size_t);
#pragma warning(disable: 6255)

    span<char const> as_span(char const* s) { return { CE_STRLEN(s) + 1, s }; }

#define CE_TO_UTF16(DST, SRC) wchar_t const* DST; { auto _src = as_span(SRC); auto _dst = _alloca(_src.size * 2); to_utf16(static_cast<char16_t*>(_dst), _src.size, _src.data); DST = static_cast<wchar_t const*>(_dst); }

	namespace os
	{
        void debug_out(char const text[])
        {
            CE_TO_UTF16(os_text, text);
            OutputDebugStringW(os_text);
        }

        file_t open_file(char const path[])
        {
            CE_TO_UTF16(os_path, path);
            HANDLE handle = CreateFileW(os_path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
            return { handle };
        }

        bool open_file(file_t& file, char const path[])
        {
            file = open_file(path);
            return file.os_handle != INVALID_HANDLE_VALUE;
        }

        bool close_file(file_t& file)
        {
            auto handle = file.os_handle;
            file.os_handle = INVALID_HANDLE_VALUE;
            return CloseHandle(handle);
        }

        // read only
        bool map_span(span<uint8_t const>& span, file_t file)
        {
            span.size = 0;
            span.data = nullptr;

            auto handle = file.os_handle;

            LARGE_INTEGER os_size;

            if (!GetFileSizeEx(handle, &os_size))
                return false;

            auto map = CreateFileMappingW(handle, NULL, PAGE_READONLY, os_size.HighPart, os_size.LowPart, NULL);
            if (map == NULL)
                return false;

            SIZE_T size = SIZE_T(os_size.QuadPart);

            auto data = static_cast<uint8_t const*>(MapViewOfFile(map, FILE_MAP_READ, 0, 0, size));
            CloseHandle(map);

            if (data == nullptr)
                return false;

            span.size = size;
            span.data = data;
            return true;
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
            auto data = span.data;
            span.size = 0;
            span.data = nullptr;
            return UnmapViewOfFile(data);
        }

        uint8_t* virtual_alloc(size_t size)
        {
            return static_cast<uint8_t*>(VirtualAlloc(nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
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

    char* to_text(char text[], char value) { text[0] = value; return text + 1; }

    char* to_text(char text[], bool value)
    {
        text[0] = '#';
        text[1] = '<';
        text[2] = "FT"[value ? 1 : 0];
        text[3] = '>';
        return text + 4;
    }

    char* to_text(char text[], float value) { return std::to_chars(text, text + 32, value).ptr; }
    char* to_text(char text[], double value) { return std::to_chars(text, text + 32, value).ptr; }

    char* to_text(char text[], signed char i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], unsigned char i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], short i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], unsigned short i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], int i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], unsigned int i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], long i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], unsigned long i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], long long i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], unsigned long long i) { return jeaiii::to_text_from_integer(text, i); }

    char* to_text(char text[], char const value[])
    {
        char* p = text;
        for (size_t i = 0; value[i] != 0; ++i)
            *p++ = value[i];
        return p;
    }
}

