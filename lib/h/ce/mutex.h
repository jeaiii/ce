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
#if CE_API_WIN32
    struct thread_mutex { void* opaque_values[1]{ }; }; // WIN32 SRWLOCK
    struct thread_shared_mutex { void* opaque_values[1]{ }; }; // WIN32 SRWLOCK
#elif CE_API_POSIX

#endif

    // non reentrant/recursive mutex for threads of a single process
    void construct_mutex(thread_mutex&);
    void destroy_mutex(thread_mutex&);

    bool try_acquire_mutex(thread_mutex&);
    void acquire_mutex(thread_mutex&);
    void release_mutex(thread_mutex&);

    // non reentrant/recursive shared mutex for threads of a single process ("reader/write lock")
    void construct_mutex(thread_shared_mutex&);
    void destroy_mutex(thread_shared_mutex&);

    bool try_acquire_mutex(thread_shared_mutex&);
    void acquire_mutex(thread_shared_mutex&);
    void release_mutex(thread_shared_mutex&);

    bool try_acquire_mutex_shared(thread_shared_mutex&);
    void acquire_mutex_shared(thread_shared_mutex&);
    void release_mutex_shared(thread_shared_mutex&);
}
