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

#include "to_text_from_integer.h"
#include <charconv>

namespace ce
{
    char* to_text(char text[], float value) { return std::to_chars(text, text + 32, value).ptr; }
    char* to_text(char text[], double value) { return std::to_chars(text, text + 32, value).ptr; }

    char* to_text(char text[], unsigned char i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], unsigned short i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], unsigned int i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], unsigned long i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], long long i) { return jeaiii::to_text_from_integer(text, i); }

    char* to_text(char text[], signed char i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], short i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], int i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], long i) { return jeaiii::to_text_from_integer(text, i); }
    char* to_text(char text[], unsigned long long i) { return jeaiii::to_text_from_integer(text, i); }
}

