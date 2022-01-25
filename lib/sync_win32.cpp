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

#include "ce/sync.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace ce
{
    namespace os
    {
        void construct_sync(lock& q) { InitializeSRWLock(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
        void terminate_sync(lock& q) { InitializeSRWLock(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
        void acquire_sync(lock& q) { AcquireSRWLockExclusive(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
        void try_acquire_sync(lock& q) { TryAcquireSRWLockExclusive(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
        void release_sync(lock& q) { ReleaseSRWLockExclusive(reinterpret_cast<SRWLOCK*>(q.opaque_values)); }
    }
}