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

#define CE_MUTEX_STD 0
#define CE_MUTEX_WIN32 0

#if defined(CE_USER_MUTEX_STD)
#undef CE_MUTEX_STD
#define CE_MUTEX_STD 1
#elif CE_API_WIN32
#undef CE_MUTEX_WIN32
#define CE_MUTEX_WIN32 1
#else
#undef CE_MUTEX_STD
#define CE_MUTEX_STD 1
#endif

#if CE_MUTEX_STD
#include <mutex>
#include <shared_mutex>
#endif

namespace ce
{
    namespace detail
    {
#if CE_MUTEX_WIN32
        constexpr auto thread_mutex_sizeof = sizeof(void*); // WIN32 SRWLOCK 
        constexpr auto thread_shared_mutex_sizeof = sizeof(void*); // WIN32 SRWLOCK
#elif CE_MUTEX_STD
        constexpr auto thread_mutex_sizeof = sizeof(std::mutex);
        constexpr auto thread_shared_mutex_sizeof = sizeof(std::shared_mutex);
#endif
        static_assert(thread_mutex_sizeof % sizeof(void*) == 0);
        static_assert(thread_shared_mutex_sizeof % sizeof(void*) == 0);
    };

    struct thread_mutex { void* opaque_data[detail::thread_mutex_sizeof / sizeof(void*)]; };
    struct thread_shared_mutex { void* opaque_data[detail::thread_shared_mutex_sizeof / sizeof(void*)]; };

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
