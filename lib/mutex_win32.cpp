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

#include "ce/mutex.h"

#if CE_API_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace ce
{
    void construct_mutex(thread_mutex& q) { InitializeSRWLock(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
    void destroy_mutex(thread_mutex& q) { InitializeSRWLock(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
    bool try_acquire_mutex(thread_mutex& q) { return TryAcquireSRWLockExclusive(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
    void acquire_mutex(thread_mutex& q) { AcquireSRWLockExclusive(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
    void release_mutex(thread_mutex& q) { ReleaseSRWLockExclusive(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }

    void construct_mutex(thread_shared_mutex& q) { InitializeSRWLock(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
    void destroy_mutex(thread_shared_mutex& q) { InitializeSRWLock(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
    bool try_acquire_mutex(thread_shared_mutex& q) { return TryAcquireSRWLockExclusive(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
    void acquire_mutex(thread_shared_mutex& q) { AcquireSRWLockExclusive(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
    void release_mutex(thread_shared_mutex& q) { ReleaseSRWLockExclusive(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
    bool try_acquire_mutex_shared(thread_shared_mutex& q) { return TryAcquireSRWLockShared(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
    void acquire_mutex_shared(thread_shared_mutex& q) { AcquireSRWLockShared(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
    void release_mutex_shared(thread_shared_mutex& q) { ReleaseSRWLockShared(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
}
#endif