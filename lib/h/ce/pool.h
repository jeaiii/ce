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
    template<size_t N, class T, class IndexT = uint16_t>
    struct pool
    {
        using index_t = IndexT;

        static_assert(index_t(N - 1) == N - 1, "");

        // should allocate orders with a bitmap so they stay together for the most part?
        // or is there a way to keep them contigous?
        // data[0] is unused

        enum class ptr_t : index_t { nil };

        static constexpr auto nil = ptr_t::nil;

        // links to the next node (used for the free list and to link nodes together externally, e.g. a queue of nodes)
        // next[0] is the free list
        index_t next[N];

        // this could be a ce::list...hmmm
        index_t size;
        T data[N];

        static size_t capacity() { return N; }

        void reset()
        {
            size = 0;
            next[0] = 0;
        }

        index_t alloc_index()
        {
            auto node = next[0];
            if (node != 0)
            {
                next[0] = next[node];
                return node;
            }

            if (size < N - 1)
                node = ++size;

            return node;
        }

        void free_index(index_t node)
        {
            if (node != 0)
            {
                next[node] = next[0];
                next[0] = node;
            }
        }

        struct queue_t
        {
            ptr_t tail = ptr_t::nil;
            void reset()
            {
                tail = ptr_t::nil;
            }
        };

        T& head(queue_t q) { return data[next[index_t(q.tail)]]; }
        T& tail(queue_t q) { return data[index_t(q.tail)]; }

        bool is_empty(queue_t const& q) const { return q.tail == ptr_t::nil; }

        bool remove_head(queue_t& q)
        {
            if (q.tail == ptr_t::nil)
                return false;

            auto tail = index_t(q.tail);
            auto head = next[tail];

            // skip the head
            next[tail] = next[head];

            // link free list to head
            next[head] = next[0];

            // make head the free list
            next[0] = head;

            // is queue now empty?
            if (head == tail)
                q.tail = ptr_t::nil;

            return true;
        }

        bool insert_head(queue_t& q, T const& item)
        {
            auto node = alloc_index();
            if (node == 0)
                return false;

            data[node] = item;

            auto tail = index_t(q.tail);

            if (tail == 0)
            {
                // emtpy case, link to self
                next[node] = node;

                // node is the new tail (and head) so set q.tail to node
                q.tail = ptr_t(node);
            }
            else
            {
                // the tail links to the head
                auto head = next[tail];

                // link the new node between the tail and head
                next[node] = head;
                next[tail] = node;
            }

            return true;
        }

        bool append_tail(queue_t& q, T const& item)
        {
            auto node = alloc_index();
            if (node == 0)
                return false;

            data[node] = item;

            auto tail = index_t(q.tail);

            if (tail == 0)
            {
                // emtpy case, link to self
                next[node] = node;
            }
            else
            {
                // the tail links to the head
                auto head = next[tail];

                // link the new node between the tail and head
                next[node] = head;
                next[tail] = node;
            }

            // node is the new tail so set q.tail to node
            q.tail = ptr_t(node);

            return true;
        }
    };
}
