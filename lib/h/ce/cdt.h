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

#include "math.h"

namespace ce
{
    inline bool is_edge(uint n) { return n % 4 != 3; }
    inline bool is_face(uint n) { return n % 4 == 3; }

    template<size_t Q = 0, size_t N = 8 * 1024> struct cdt
    {

        enum class edge_id_t : uint16_t { nil = 0 };
        enum class face_id_t : uint16_t { nil = 0 };

        static constexpr int32_t m2w32 = 1 << Q;

        using index_t = uint16_t;

        static constexpr index_t c_limit = N < 8 * 1024 ? N : 8 * 1024;

        // every face is made out of 4 nodes
        // f + 0, f + 1, f + 2 are the 3 edges of the face 
        // f + 3 is the face info
        // 1 face is 4 nodes, every point creates 2 faces
        // 1 point : 8 nodes

        static constexpr size_t c_faces_per_vert = 2;
        static constexpr size_t c_nodes_per_face = 4;
        static constexpr size_t c_nodes_per_vert = 8;

        // per edge data - `c`onstrained, `p`oint index, `e`dge twin

        struct edge_data
        {
            uint16_t c : 3;
            uint16_t p : 13;
            uint16_t e;
        };

        // per vert data - for every vert { x, y } there are two faces
        struct vert_data
        {
            uint16_t a_c0 : 3;
            uint16_t a_p0 : 13;
            uint16_t a_e0;
            uint16_t a_c1 : 3;
            uint16_t a_p1 : 13;
            uint16_t a_e1;
            uint16_t a_c2 : 3;
            uint16_t a_p2 : 13;
            uint16_t a_e2;

            uint16_t a_fi;

            int16_t x;

            uint16_t b_c0 : 3;
            uint16_t b_p0 : 13;
            uint16_t b_e0;
            uint16_t b_c1 : 3;
            uint16_t b_p1 : 13;
            uint16_t b_e1;
            uint16_t b_c2 : 3;
            uint16_t b_p2 : 13;
            uint16_t b_e2;

            uint16_t b_fi;

            int16_t y;
        };

        // per face data - 3 edges, user `fi` face info, 1 cordinate from a vert
        struct face_data
        {
            uint16_t c0 : 3;
            uint16_t p0 : 13;
            uint16_t e0;

            uint16_t c1 : 3;
            uint16_t p1 : 13;
            uint16_t e1;

            uint16_t c2 : 3;
            uint16_t p2 : 13;
            uint16_t e2;

            uint16_t fi;

            int16_t xy;
        };

        CE_STATIC_ASSERT(sizeof(edge_data) == sizeof(uint32_t));
        CE_STATIC_ASSERT(sizeof(face_data) == sizeof(edge_data) * 4);
        CE_STATIC_ASSERT(sizeof(vert_data) == sizeof(edge_data) * 8);

        size_t size;
        union
        {
            vert_data data[c_limit];
            vert_data vs[1 * c_limit];
            face_data fs[2 * c_limit];
            edge_data es[8 * c_limit];
        };

        struct face
        {
            vec2<int32_t> ps[3];
            edge_id_t es[3];
            bool cs[3];
            uint16_t data;
        };

        face operator[](face_id_t f) const
        {
            auto n = size_t(f) / 4;
            return
            {
                {
                    { vs[fs[n].p0].x * m2w32, vs[fs[n].p0].y * m2w32 },
                    { vs[fs[n].p1].x * m2w32, vs[fs[n].p1].y * m2w32 },
                    { vs[fs[n].p2].x * m2w32, vs[fs[n].p2].y * m2w32 },
                },
                { edge_id_t(fs[n].e0), edge_id_t(fs[n].e1), edge_id_t(fs[n].e2) },
                { fs[n].c0 != 0, fs[n].c1 != 0, fs[n].c2 != 0 },
                fs[n].fi,
            };
        }

        struct edge
        {
            vec2<int32_t> u;
            vec2<int32_t> v;
            bool primary;
            bool constrained;
        };

        edge operator[](edge_id_t e) const
        {
            auto en = size_t(e);
            auto ei = en % 4;
            auto ea = ei < 2 ? en + 1 : en - 2;
            auto eb = ei > 0 ? en - 1 : en + 2;
            auto& u = vs[es[ea].p];
            auto& v = vs[es[eb].p];
            return
            {
                { u.x * m2w32, u.y * m2w32 },
                { v.x * m2w32, v.y * m2w32 },
                es[en].e < en,
                es[en].c != 0
            };
        }

        struct edge_view
        {
            cdt const& mesh;

            struct iterator
            {
                cdt const& mesh;
                size_t en;

                edge operator*() const
                {
                    auto ei = en % 4;
                    auto e1 = ei < 2 ? en + 1 : en - 2;
                    auto e2 = ei > 0 ? en - 1 : en + 2;

                    auto& a = mesh.vs[mesh.es[e1].p];
                    auto& b = mesh.vs[mesh.es[e2].p];
                    return
                    {
                        { a.x * m2w32, a.y * m2w32 },
                        { b.x * m2w32, b.y * m2w32 },
                        mesh.es[en].e < en,
                        mesh.es[en].c != 0
                    };
                }

                iterator& operator++()
                {
                    en = en % 4 < 2 ? en + 1 : en + 2;
                    return *this;
                }

                bool operator!=(iterator b) { return en != b.en; }
            };

            iterator begin() { return { mesh, 24 }; }
            iterator end() { return { mesh, mesh.size < 4 ? 24 : 4 * 2 * mesh.size }; }
        };

        edge_view edges() const { return { *this }; }

        struct face_view
        {
            cdt const& mesh;

            struct iterator
            {
                cdt const& mesh;
                size_t fn;

                face operator*() const { return mesh[face_id_t(fn)]; }

                iterator& operator++()
                {
                    fn += 4;
                    return *this;
                }

                bool operator!=(iterator b) { return fn != b.fn; }
            };

            iterator begin() { return { mesh, 24 }; }
            iterator end() { return { mesh, mesh.size < 4 ? 24 : 4 * 2 * mesh.size }; }
        };

        face_view faces() const { return { *this }; }

        void reset()
        {
            constexpr uint16_t m = uint16_t(~0);
            constexpr uint16_t h = m >> 1;
            constexpr int16_t hi = int16_t(h);
            constexpr int16_t lo = int16_t(~h);

            size = 4;

            vs[0] = { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, lo, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, lo };
            vs[1] = { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, hi, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, lo };
            vs[2] = { 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, lo, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, hi };

            // vert 3 - face 6 & face 7- edges 24, 25, 26 and edges 28, 29, 30
            vs[3] =
            {
                // face 6
                // edge 24
                0, 0, 28,
                // edge 25
                1, 2, 0,
                // edge 26
                1, 1, 0,
                0,
                hi,

                //face 7
                // edge 28
                0, 3, 24,
                // edge 29
                1, 1, 0,
                // edge 30
                1, 2, 0,
                0,
                hi
            };

        }

        void validate_face(size_t n) const
        {
            auto& f = fs[n / 4];

            CE_ASSERT(f.p0 != f.p1);
            CE_ASSERT(f.p1 != f.p2);
            CE_ASSERT(f.p2 != f.p0);

            CE_ASSERT(f.e0 != f.e1);
            CE_ASSERT(f.e1 != f.e2);
            CE_ASSERT(f.e2 != f.e0);

            CE_ASSERT(es[es[f.e0].e].e == f.e0);
            CE_ASSERT(es[es[f.e1].e].e == f.e1);
            CE_ASSERT(es[es[f.e2].e].e == f.e2);

            CE_ASSERT(es[f.e0].c == f.c0);
            CE_ASSERT(es[f.e1].c == f.c1);
            CE_ASSERT(es[f.e2].c == f.c2);

            auto a = vec2<int32_t>{ vs[f.p0].x, vs[f.p0].y };
            auto b = vec2<int32_t>{ vs[f.p1].x, vs[f.p1].y };
            auto c = vec2<int32_t>{ vs[f.p2].x, vs[f.p2].y };
            CE_ASSERT(crossx(b - a, c - a) < 0);
            CE_ASSERT(crossx(c - b, a - b) < 0);
            CE_ASSERT(crossx(a - c, b - c) < 0);
        }

        bool incircle(size_t f, size_t v) const
        {
            CE_ASSERT(v < size);
            CE_ASSERT(f < size * 2);

            int64_t x = vs[v].x;
            int64_t y = vs[v].y;

            auto ax = vs[fs[f].p0].x - x;
            auto ay = vs[fs[f].p0].y - y;
            auto bx = vs[fs[f].p1].x - x;
            auto by = vs[fs[f].p1].y - y;
            auto cx = vs[fs[f].p2].x - x;
            auto cy = vs[fs[f].p2].y - y;

            auto aa = ax * ax + ay * ay;
            auto bb = bx * bx + by * by;
            auto cc = cx * cx + cy * cy;
            auto bc = bx * cy - by * cx;
            auto ca = cx * ay - cy * ax;
            auto ab = ax * by - ay * bx;

            auto aabc = aa * bc;
            auto bbca = bb * ca;
            auto ccab = cc * ab;

            return aabc + bbca + ccab < 0;
        }

        template<bool More>
        void do_flip(size_t n)
        {
            auto ae = index_t(n);

            CE_ASSERT(es[ae].c == 0);
            auto be = es[ae].e;
            CE_ASSERT(es[be].e == ae);
            CE_ASSERT(es[be].c == 0);

            index_t ano = ae % 4;
            CE_ASSERT(ano < 3);
            index_t a1 = ano < 2 ? ae + 1 : ae - 2;
            index_t a2 = ano > 0 ? ae - 1 : ae + 2;

            index_t bno = be % 4;
            CE_ASSERT(bno < 3);
            index_t b1 = bno < 2 ? be + 1 : be - 2;
            index_t b2 = bno > 0 ? be - 1 : be + 2;

            es[a1].p = es[be].p;
            es[b1].p = es[ae].p;

            auto u2 = es[a2].e;
            auto v2 = es[b2].e;

            es[ae].e = v2;
            if (v2 != 0)
                es[v2].e = ae;

            es[be].e = u2;
            if (u2 != 0)
                es[u2].e = be;

            es[ae].c = es[b2].c;
            es[b2].c = 0; // [be] must have been false

            es[be].c = es[a2].c;
            es[a2].c = 0; // [ae] must have been false

            es[a2].e = b2;
            es[b2].e = a2;

            validate_face(ae);
            validate_face(be);

            if (More)
            {
                flip(ae);
                flip(a1);
                flip(be);
                flip(b1);
            }
        }

        bool can_flip(size_t e) const
        {
            if (es[e].c != 0)
                return false;

            int64_t x = vs[es[e].p].x;
            int64_t y = vs[es[e].p].y;

            auto& p1 = vs[es[e % 4 < 2 ? e + 1 : e - 2].p];

            auto ax = p1.x - x;
            auto ay = p1.y - y;

            auto t = es[e].e;
            auto& q0 = vs[es[t].p];
            auto& q1 = vs[es[t % 4 < 2 ? t + 1 : t - 2].p];

            auto bx = q0.x - x;
            auto by = q0.y - y;

            auto cx = q1.x - x;
            auto cy = q1.y - y;

            return (ax * by < ay* bx&& bx* cy < by* cx);
        }

        bool try_flip(size_t e)
        {
            if (!can_flip(e))
                return false;

            do_flip<false>(e);

            return true;
        }

        bool flip(size_t ae)
        {
            CE_ASSERT(ae % 4 != 3);

            if (es[ae].c != 0)
                return false;

            index_t be = es[ae].e;
            CE_ASSERT(be % 4 != 3);
            CE_ASSERT(es[be].e == ae);

            if (!incircle(ae / 4, es[be].p))
                return false;

            do_flip<true>(ae);
            return true;
        }

        index_t refine(int64_t x, int64_t y, index_t n) const
        {
            vec2<int64_t> a{ vs[es[n + 0].p].x - x, vs[es[n + 0].p].y - y };
            vec2<int64_t> b{ vs[es[n + 1].p].x - x, vs[es[n + 1].p].y - y };
            vec2<int64_t> c{ vs[es[n + 2].p].x - x, vs[es[n + 2].p].y - y };

            int e = (b.x * c.y < b.y* c.x ? 1 : 0) + (c.x * a.y < c.y* a.x ? 2 : 0) + (a.x * b.y < a.y* b.x ? 4 : 0);

            switch (e)
            {
            case 2 + 1: return n + 2;
            case 4 + 1: return n + 1;
            case 4 + 2: return n + 0;
            case 4 + 2 + 1: return n + 3;
            default: return 0;
            }
        }

        static bool in(vec2<int64_t> const& a, vec2<int64_t> const& b)
        {
            return a.x * b.y <= a.y * b.x;
        }

        static bool on(int64_t ax, int64_t ay, int64_t bx, int64_t by)
        {
            return ax * by == ay * bx;
        }

        static bool collinear(int64_t ax, int64_t ay, int64_t bx, int64_t by, int64_t cx, int64_t cy)
        {
            return on(bx - ax, by - ay, cx - ax, cy - ay);
        }

        // from inside the face the edge's `a` vert is on the left
        vec2<int32_t> edge_vert_a(uint e) const
        {
            auto p = es[e % 4 < 2 ? e + 1 : e - 2].p;
            return { vs[p].x * m2w32, vs[p].y * m2w32 };
        }

        // from inside the face the edge's `b` vert is on the right
        vec2<int32_t> edge_vert_b(uint e) const
        {
            auto p = es[e % 4 > 0 ? e - 1 : e + 2].p;
            return { vs[p].x * m2w32, vs[p].y * m2w32 };
        }

        // from inside the face the edge's `o` vert is opposite it
        vec2<int32_t> edge_vert_o(uint e) const
        {
            auto p = es[e].p;
            return { vs[p].x * m2w32, vs[p].y * m2w32 };
        }

        static size_t next_edge(size_t e)
        {
            CE_ASSERT(e % 4 != 3);
            e += 1;
            return e % 4 == 3 ? e & ~3 : e;
        }

        size_t locate_insertion_node(int64_t px, int64_t py, size_t hint) const
        {
#if 0
            for (size_t f = size * 2; --f >= 6;)
            {
                vec2<int64_t> a{ vs[fs[f].p0].x - px, vs[fs[f].p0].y - py };
                vec2<int64_t> b{ vs[fs[f].p1].x - px, vs[fs[f].p1].y - py };
                vec2<int64_t> c{ vs[fs[f].p2].x - px, vs[fs[f].p2].y - py };

                if (in(a, b) && in(b, c) && in(c, a))
                    return f * 4;
            }
            return 0;
#else
            if (hint < 24 || hint >= size * 8)
                hint = size * 8 - 4;

            auto e = hint & ~3;

            vec2<int64_t> p1{ vs[es[e + 1].p].x - px, vs[es[e + 1].p].y - py };
            vec2<int64_t> p2{ vs[es[e + 2].p].x - px, vs[es[e + 2].p].y - py };
            vec2<int64_t> t;

            // make sure we are inside e0
            if (!in(p1, p2))
                e = es[e].e, t = p1, p1 = p2, p2 = t;

            while (e != 0)
            {
                vec2<int64_t> p0{ vs[es[e].p].x - px, vs[es[e].p].y - py };

                bool in1 = in(p2, p0);
                bool in2 = in(p0, p1);

                if (in1 && in2)
                    return e;
                else if (in1 && !in2)
                    // exit through e2
                    e = es[e % 4 > 0 ? e - 1 : e + 2].e, p2 = p0;
                else if (!in1 && in2)
                    // exit through e1
                    e = es[e % 4 < 2 ? e + 1 : e - 2].e, p1 = p0;
                else
                {
                    // exit through e1
                    e = es[e % 4 < 2 ? e + 1 : e - 2].e, p1 = p0;

                    // spin around p1 until a is inside e0 and e2 (it is already inside e0)
                    for (;;)
                    {
                        CE_ASSERT(in(p1, p2));
                        if (e == 0)
                            return 0;

                        p0.x = vs[es[e].p].x - px;
                        p0.y = vs[es[e].p].y - py;

                        if (in(p0, p1))
                            break;

                        // exit through e2
                        e = es[e % 4 > 0 ? e - 1 : e + 2].e, p2 = p0;
                    }

                    // are we also inside e1?
                    if (in(p2, p0))
                        return e;

                    e = es[e % 4 < 2 ? e + 1 : e - 2].e, p1 = p0;
                }
            }
            return 0;
#endif
        }

        size_t insert_point_at_node(int64_t px, int64_t py, size_t n)
        {
            if (n == 0)
                return 0;

            auto un = index_t(n & ~3);
            un &= ~3;

            {
                vec2<int64_t> a{ vs[es[un + 0].p].x - px, vs[es[un + 0].p].y - py };
                vec2<int64_t> b{ vs[es[un + 1].p].x - px, vs[es[un + 1].p].y - py };
                vec2<int64_t> c{ vs[es[un + 2].p].x - px, vs[es[un + 2].p].y - py };

                CE_ASSERT(in(a, b) && in(b, c) && in(c, a));

                int e = (b.x * c.y < b.y* c.x ? 1 : 0) + (c.x * a.y < c.y* a.x ? 2 : 0) + (a.x * b.y < a.y* b.x ? 4 : 0);

                CE_ASSERT(e != 0);

                switch (e)
                {
                case 2 + 1: un += 2; break;
                case 4 + 1: un += 1; break;
                case 4 + 2: un += 0; break;
                case 4 + 2 + 1: un += 3; break;
                case 0:
                    CE_ASSERT(false);
                    return 0;
                default:
                    return un; // point is a vert...maybe do something else
                }

                CE_ASSERT(a != 0 && b != 0 && c != 0);
            }

            auto pn = index_t(size);
            if (pn >= c_limit)
                return 0;

            // 3 -> in face, 0 1 2 -> on edge
            auto uno = un % 4;

#if 0
            // validate point doesn't already exist
            for (size_t i = 0; i < size; ++i)
            {
                CE_ASSERT(vs[i].x != px || vs[i].y != py);
            }
#endif

            // in face
            if (uno == 3)
            {
                size = pn + 1;

                auto const u0 = un - 3;
                auto const v0 = pn * 8;
                auto const w0 = v0 + 4;

                auto& uf = fs[un / 4];

                vs[pn].a_c0 = 0;
                vs[pn].a_p0 = uf.p0;
                vs[pn].a_e0 = (uint16_t)u0 + 1;

                vs[pn].a_c1 = uf.c1;
                vs[pn].a_p1 = pn;
                vs[pn].a_e1 = uf.e1;
                if (uf.e1 != 0)
                    es[uf.e1].e = (uint16_t)v0 + 1;

                vs[pn].a_c2 = 0;
                vs[pn].a_p2 = uf.p2;
                vs[pn].a_e2 = (uint16_t)w0 + 1;

                vs[pn].a_fi = uf.fi;

                vs[pn].x = int16_t(px);

                vs[pn].b_c0 = 0;
                vs[pn].b_p0 = uf.p0;
                vs[pn].b_e0 = (uint16_t)u0 + 2;

                vs[pn].b_c1 = 0;
                vs[pn].b_p1 = uf.p1;
                vs[pn].b_e1 = (uint16_t)v0 + 2;

                vs[pn].b_c2 = uf.c2;
                vs[pn].b_p2 = pn;
                vs[pn].b_e2 = uf.e2;
                if (uf.e2 != 0)
                    es[uf.e2].e = (uint16_t)w0 + 2;

                vs[pn].b_fi = uf.fi;

                vs[pn].y = int16_t(py);

                uf.p0 = pn;

                uf.c1 = 0;
                uf.e1 = (uint16_t)v0 + 0;

                uf.c2 = 0;
                uf.e2 = (uint16_t)w0 + 0;

                validate_face(u0);
                validate_face(v0);
                validate_face(w0);

                flip(u0 + 0);
                flip(v0 + 1);
                flip(w0 + 2);
            }
            // on edge
            else
            {
                auto ue = un;
                auto ve = es[ue].e;
                if (ve == 0)
                    return false;

                CE_ASSERT(es[ve].e == ue);

                if (ve < ue)
                    swap(ve, ue);

                size = pn + 1;

                index_t u1 = ue % 4 < 2 ? ue + 1 : ue - 2;
                index_t u2 = ue % 4 > 0 ? ue - 1 : ue + 2;

                index_t v1 = ve % 4 < 2 ? ve + 1 : ve - 2;
                index_t v2 = ve % 4 > 0 ? ve - 1 : ve + 2;

                auto const xn = pn * 8;
                auto const yn = pn * 8 + 4;

                vs[pn].a_c0 = es[ue].c;
                vs[pn].a_p0 = es[ue].p;
                vs[pn].a_e0 = (uint16_t)yn + 0;

                vs[pn].a_c1 = es[u1].c;
                vs[pn].a_p1 = pn;
                vs[pn].a_e1 = es[u1].e;
                if (es[u1].e != 0)
                    es[es[u1].e].e = (uint16_t)xn + 1;

                vs[pn].a_c2 = 0;
                vs[pn].a_p2 = es[u2].p;
                vs[pn].a_e2 = u1;

                vs[pn].a_fi = fs[ue / 4].fi;

                vs[pn].x = int16_t(px);

                es[u1].e = (uint16_t)xn + 2;
                es[u1].c = 0;
                es[u2].p = pn;

                vs[pn].b_c0 = es[ve].c;
                vs[pn].b_p0 = es[ve].p;
                vs[pn].b_e0 = (uint16_t)xn + 0;

                vs[pn].b_c1 = 0;
                vs[pn].b_p1 = es[v1].p;
                vs[pn].b_e1 = v2;

                vs[pn].b_c2 = es[v2].c;
                vs[pn].b_p2 = pn;
                vs[pn].b_e2 = es[v2].e;
                if (es[v2].e != 0)
                    es[es[v2].e].e = (uint16_t)yn + 2;

                vs[pn].b_fi = fs[ve / 4].fi;

                vs[pn].y = int16_t(py);

                es[v2].e = (uint16_t)yn + 1;
                es[v2].c = 0;
                es[v1].p = pn;

                validate_face(ue);
                validate_face(ve);
                validate_face(xn);
                validate_face(yn);

                flip(u2);
                flip(xn + 1);
                flip(v1);
                flip(yn + 2);
            }

            return un;
        }

        size_t insert_point(int64_t px, int64_t py, size_t hint)
        {
            return insert_point_at_node(px, py, locate_insertion_node(px, py, hint));
        }

        void constrain_edge(size_t en)
        {
            es[en].c = es[es[en].e].c = 1;
#if 1
            // not sure if this does anything
            // but the idea is we did a bunch of flips to get an
            // edge we want to constrain show up and we maybe able to undo
            // some of those?

            auto e1 = next_edge(en);
            auto e2 = next_edge(e1);

            flip(next_edge(e1));
            flip(next_edge(e2));

            e1 = next_edge(es[en].e);
            e2 = next_edge(e1);

            flip(next_edge(e1));
            flip(next_edge(e2));
#endif
        }

        template<class T>
        static vec2<T> intersect_edges(vec2<T> q, vec2<T> a, vec2<T> b)
        {
            auto u = b - a;
            auto d = crossx(u, q);

            // can't happen, q, a, b can't be a degenerate triangle
            CE_ASSERT(d > 0);

            auto t = crossx(q, a);

            CE_ASSERT(t >= 0 && t <= d);

            return { a.x + T(dotx(u.x, t) / d), a.y + T(dotx(u.y, t) / d) };
        }

        bool split_constraint(int64_t px, int64_t py, vec2<int64_t> q, vec2<int64_t> a, vec2<int64_t> b, size_t hint)
        {
            vec2<int64_t> r = intersect_edges(q, a, b);

            if (r == 0 || r == q)
            {
                //CE_DEBUG_BREAK();
                return false;
            }

            // this first part basically does the work of insert_point_at_node (on edge)
            // I think if we refactor that work we can call that directly here and it should also solve the off edge interesection failure case
            // i.e. we assume the intersection is on the edge even if it isn't (which will deform the constrained edge) but that's okay
            auto pr = insert_edge(px, py, px + r.x, py + r.y, hint);

            auto rq = insert_edge(px + r.x, py + r.y, px + q.x, py + q.y, hint);

            return pr && rq;
        }

        bool insert_edge(int64_t px, int64_t py, int64_t qx, int64_t qy, size_t hint)
        {
            auto pn = insert_point_at_node(px, py, locate_insertion_node(px, py, hint));
            if (pn == 0)
                return false;

            if (px == qx && py == qy)
                return true;

            auto qn = insert_point_at_node(qx, qy, locate_insertion_node(qx, qy, pn));
            if (qn == 0)
                return false;

            // re-locate...flips may have changed things
            pn = locate_insertion_node(px, py, pn);

            for (;;)
            {
                auto n = pn / 4 * 4;

                vec2<int64_t> ps[] = { {vs[es[n + 0].p].x - px, vs[es[n + 0].p].y - py }, { vs[es[n + 1].p].x - px, vs[es[n + 1].p].y - py }, { vs[es[n + 2].p].x - px, vs[es[n + 2].p].y - py } };
                vec2<int64_t> q{ qx - px, qy - py };

                if (q == 0)
                    return true;

                CE_ASSERT(ps[0] != ps[1]);
                CE_ASSERT(ps[1] != ps[2]);
                CE_ASSERT(ps[2] != ps[0]);

                // 0
                if (ps[0] == 0)
                {
                    auto a = crossx(ps[1], q);
                    auto b = crossx(ps[2], q);
                    if (a < 0 && b >= 0)
                    {
                        if (b == 0)
                        {
                            constrain_edge(n + 1);
                            pn = es[n + 1].e;
                            px = px + ps[2].x;
                            py = py + ps[2].y;
                        }
                        else if (!try_flip(n + 0))
                            return split_constraint(px, py, q, ps[1], ps[2], pn);
                    }
                    else
                        pn = es[n + 2].e;
                }
                else if (ps[1] == 0)
                {
                    auto a = crossx(ps[2], q);
                    auto b = crossx(ps[0], q);
                    if (a < 0 && b >= 0)
                    {
                        if (b == 0)
                        {
                            constrain_edge(n + 2);
                            pn = es[n + 2].e;
                            px = px + ps[0].x;
                            py = py + ps[0].y;
                        }
                        else if (!try_flip(n + 1))
                            return split_constraint(px, py, q, ps[2], ps[0], pn);
                    }
                    else
                        pn = es[n + 0].e;
                }
                else
                {
                    CE_ASSERT(ps[2] == 0);

                    auto a = crossx(ps[0], q);
                    auto b = crossx(ps[1], q);
                    if (a < 0 && b >= 0)
                    {
                        if (b == 0)
                        {
                            constrain_edge(n + 0);
                            pn = es[n + 0].e;
                            px = px + ps[1].x;
                            py = py + ps[1].y;
                        }
                        else if (!try_flip(n + 2))
                            return split_constraint(px, py, q, ps[0], ps[1], pn);
                    }
                    else
                        pn = es[n + 1].e;
                }
            }
        }

        bool insert_point(vec2<int32_t> const& p, size_t hint)
        {
            // we want to floor when we divide so use >> Q
            // user points are only at even locations so we have extra resolution to find intersections
            return insert_point(p.x >> Q & ~1, p.y >> Q & ~1, hint) != 0;
        }

        bool insert_edge(vec2<int32_t> const& p, vec2<int32_t> const& q, size_t hint)
        {
            // we want to floor when we divide so use >> Q
            // user points are only at even locations so we have extra resolution to find intersections
            return insert_edge(p.x >> Q & ~1, p.y >> Q & ~1, q.x >> Q & ~1, q.y >> Q & ~1, hint);
        }

        uint locate(int32_t px, int32_t py, uint hint) const
        {
#if 0
            for (size_t f = size * 2; --f >= 6;)
            {
                vec2<int32_t> a{ vs[fs[f].p0].x * m2w32 - px, vs[fs[f].p0].y * m2w32 - py };
                vec2<int32_t> b{ vs[fs[f].p1].x * m2w32 - px, vs[fs[f].p1].y * m2w32 - py };
                vec2<int32_t> c{ vs[fs[f].p2].x * m2w32 - px, vs[fs[f].p2].y * m2w32 - py };

                if (inside(a, b) && inside(b, c) && inside(c, a))
                    return f * 4;
            }
            return 0;
#else
            auto e = hint & ~3;
            if (e < 24 || e > size * 8 - 4)
                e = uint(size * 8 - 4);

            vec2<int32_t> p1{ vs[es[e + 1].p].x * m2w32 - px, vs[es[e + 1].p].y * m2w32 - py };
            vec2<int32_t> p2{ vs[es[e + 2].p].x * m2w32 - px, vs[es[e + 2].p].y * m2w32 - py };
            vec2<int32_t> t;

            // make sure we are inside e0
            if (!inside(p1, p2))
                e = es[e].e, t = p1, p1 = p2, p2 = t;

            while (e != 0)
            {
                vec2<int32_t> p0{ vs[es[e].p].x * m2w32 - px, vs[es[e].p].y * m2w32 - py };

                bool in1 = inside(p2, p0);
                bool in2 = inside(p0, p1);

                if (in1 && in2)
                    return e | 3;
                else if (in1 && !in2)
                    // exit through e2
                    e = es[e % 4 > 0 ? e - 1 : e + 2].e, p2 = p0;
                else if (!in1 && in2)
                    // exit through e1
                    e = es[e % 4 < 2 ? e + 1 : e - 2].e, p1 = p0;
                else
                {
                    // exit through e1
                    e = es[e % 4 < 2 ? e + 1 : e - 2].e, p1 = p0;

                    // spin around p1 until a is inside e0 and e2 (it is already inside e0)
                    for (;;)
                    {
                        if (e == 0)
                            return e;

                        p0.x = vs[es[e].p].x * m2w32 - px;
                        p0.y = vs[es[e].p].y * m2w32 - py;

                        if (inside(p0, p1))
                            break;

                        // exit through e2
                        e = es[e % 4 > 0 ? e - 1 : e + 2].e, p2 = p0;
                    }

                    // are we also inside e1?
                    if (inside(p2, p0))
                        return e | 3;

                    e = es[e % 4 < 2 ? e + 1 : e - 2].e, p1 = p0;
                }
            }
            return e;
#endif
        }

        uint locate(vec2<int32_t> const& p, uint hint) const { return locate(p.x, p.y, hint); }


        void fill_ex(uint f, uint16_t info)
        {
            // limited stack...if we need more than this we recure
            uint q[256];

            for (size_t i = 0;; f = q[--i])
            {
                if (fs[f].c0 == 0 && fs[fs[f].e0 / 4].fi != info)
                    fs[q[i++] = fs[f].e0 / 4].fi = info;
                if (fs[f].c1 == 0 && fs[fs[f].e1 / 4].fi != info)
                    fs[q[i++] = fs[f].e1 / 4].fi = info;
                if (fs[f].c2 == 0 && fs[fs[f].e2 / 4].fi != info)
                    fs[q[i++] = fs[f].e2 / 4].fi = info;

                if (i == 0)
                    return;

                while (i > 253)
                    fill_ex(q[--i], info);
            }
        }

        void fill(uint n, uint16_t info)
        {
            auto f = n / 4;

            if (f == 0)
                return;

            if (fs[f].fi == info)
                return;

            fs[f].fi = info;

            fill_ex(f, info);
        }

    };

}