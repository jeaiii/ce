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
#if CE_MUTEX_STD

namespace ce
{
    void construct_mutex(thread_mutex& q) { auto m = reinterpret_cast<std::mutex*>(q.opaque_data); CE_CONSTRUCT_AT(m) std::mutex(); }
    void destroy_mutex(thread_mutex& q) { auto m = reinterpret_cast<std::mutex*>(q.opaque_data); CE_DESTROY_AT(m) ~mutex(); }
    bool try_acquire_mutex(thread_mutex& q) { auto m = reinterpret_cast<std::mutex*>(q.opaque_data); return m->try_lock(); }
    void acquire_mutex(thread_mutex& q) { auto m = reinterpret_cast<std::mutex*>(q.opaque_data); m->lock(); }
    void release_mutex(thread_mutex& q) { auto m = reinterpret_cast<std::mutex*>(q.opaque_data); m->unlock(); }

    void construct_mutex(thread_shared_mutex& q) { auto m = reinterpret_cast<std::shared_mutex*>(q.opaque_data); CE_CONSTRUCT_AT(m) std::shared_mutex(); }
    void destroy_mutex(thread_shared_mutex& q) { auto m = reinterpret_cast<std::shared_mutex*>(q.opaque_data); CE_DESTROY_AT(m) ~shared_mutex(); }
    bool try_acquire_mutex(thread_shared_mutex& q) { auto m = reinterpret_cast<std::shared_mutex*>(q.opaque_data); return m->try_lock(); }
    void acquire_mutex(thread_shared_mutex& q) { auto m = reinterpret_cast<std::shared_mutex*>(q.opaque_data); m->lock(); }
    void release_mutex(thread_shared_mutex& q) { auto m = reinterpret_cast<std::shared_mutex*>(q.opaque_data); m->unlock(); }
    bool try_acquire_mutex_shared(thread_shared_mutex& q) { auto m = reinterpret_cast<std::shared_mutex*>(q.opaque_data); return m->try_lock_shared(); }
    void acquire_mutex_shared(thread_shared_mutex& q) { auto m = reinterpret_cast<std::shared_mutex*>(q.opaque_data); m->lock_shared(); }
    void release_mutex_shared(thread_shared_mutex& q) { auto m = reinterpret_cast<std::shared_mutex*>(q.opaque_data); m->unlock_shared(); }
}
#endif