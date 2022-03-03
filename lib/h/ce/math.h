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

#if CE_CPU_X86
#include <emmintrin.h>
#endif

namespace ce
{
    namespace math
    {
#ifdef _MSC_VER
        extern "C" __int64 _div128(__int64 hi_n, __int64 lo_n, __int64 d, __int64* r);
        extern "C" __int64 _mul128(__int64, __int64, __int64*); // should return unsigned __int64 :-(
#pragma intrinsic(_mul128)
        inline bool ab_less_cd(int64_t a, int64_t b, int64_t c, int64_t d)
        {

            int64_t abhi;
            uint64_t ablo = _mul128(a, b, &abhi);

            int64_t cdhi;
            uint64_t cdlo = _mul128(c, d, &cdhi);

            return abhi < cdhi + (ablo < cdlo ? 1 : 0);
        }

#else
        inline bool ab_less_cd(int64_t a, int64_t b, int64_t c, int64_t d)
        {
            return __int128(a) * b < __int128(c)* d;
        }
#endif

#ifdef _MSC_VER
        namespace msvc
        {
            struct int128_t
            {
                uint64_t lo;
                int64_t hi;
            };

            inline int128_t xmul(int64_t a, int64_t b)
            {
                __int64 hi;
                unsigned __int64 lo = _mul128(a, b, &hi);
                return { lo, hi };
            }

            inline int64_t xdiv(int128_t n, int64_t d)
            {
                int64_t r;
                return _div128(n.hi, int64_t(n.lo), d, &r);
            }
        }
#endif
        inline vec2<int32_t> muldiv(vec2<int32_t> const& b, int64_t n, int64_t d)
        {
            int64_t d16 = d >> 16;

            if (d16 == 0)
                return b;

            int64_t t = (n << 16) / d16;

            //int64_t t = msvc::xdiv(msvc::xmul(n, 0x0000000100000000), d);
            return { int32_t((b.x * t) >> 32), int32_t((b.y * t) >> 32) };
        }

        inline vec2<int32_t> muldiv(vec2<int32_t> const& b, int32_t n, int32_t d)
        {
            return { int32_t(b.x * int64_t(n) / d), int32_t(b.y * int64_t(n) / d) };
        }

        // inside(a, b) - given 2 vectors a and b return true if a X b < 0

        // simulation of simplicity
        // if the vectors are co-linear (i.e. the cross product is 0)
        // we simulate adding a tiny amount first in x and then in y
        // to break the tie such that inside(a, b) -> true then inside(b, a) -> false
        // if a == b then always return false
        // given a planar triangle mesh without degenerate triangles:
        // for a given triangle with verts `a`, `b`, `c` and a point `p`
        // `inside(a - p, b - p) && inside(b - p, c - p) && inside(c - p, a - p)` is true for one and only one triangle
        // basically any point is only ever considered "inside" one triangle in the planar mesh
        // even if the point is on an edge or vertex

        template<class...U> constexpr bool inside(vec2<U...> const& a, vec2<U...> const& b)
        {
            using E = decltype(dotx(a.x, b.y) - dotx(a.y, b.x));

            auto const axby = E(a.x) * b.y;
            auto const aybx = E(a.y) * b.x;

            if (axby != aybx)
                return axby < aybx;

            // if we add 1 to ax and bx then we really add (by - ay) to the cross product
            if (a.y != b.y)
                return b.y < a.y;

            // if we add 1 to ay and by then we really add (ax - bx) to the cross product
            return a.x < b.x;
        }

        inline bool in_range_rr(int32_t x, int32_t y, int64_t rr) { return int64_t(x) * x + int64_t(y) * y < rr; }


        // { px, py } in range of line through { 0, 0 } and { dx, dy }
        inline bool in_range_rr_line(int32_t px, int32_t py, int64_t rr, int32_t dx, int32_t dy)
        {
            auto pp = int64_t(px) * px + int64_t(py) * py;
            if (pp < rr)
                return true;

            auto dp = int64_t(px) * dx + int64_t(py) * dy;

            auto dd = int64_t(dx) * dx + int64_t(dy) * dy;
            return ab_less_cd(dd, pp - rr, dp, dp);
        };

        // { px, py } in range of edge from { 0, 0 } to { dx, dy } (projection of point must be on edge)
        inline bool in_range_rr_edge(int32_t px, int32_t py, int64_t rr, int32_t dx, int32_t dy)
        {
            auto dp = int64_t(px) * dx + int64_t(py) * dy;
            if (dp < 0)
                return false;

            auto pp = int64_t(px) * px + int64_t(py) * py;
            if (pp < rr)
                return true;

            auto dd = int64_t(dx) * dx + int64_t(dy) * dy;
            if (dp > dd)
                return false;

            return ab_less_cd(dd, pp - rr, dp, dp);
        };

        // { px, py } in range of ray from { 0, 0 } through { dx, dy } 
        inline bool in_range_rr_ray(int32_t px, int32_t py, int64_t rr, int32_t dx, int32_t dy)
        {
            auto pp = int64_t(px) * px + int64_t(py) * py;
            if (pp < rr)
                return true;

            auto dp = int64_t(px) * dx + int64_t(py) * dy;
            if (dp < 0)
                return false;

            auto dd = int64_t(dx) * dx + int64_t(dy) * dy;

            return ab_less_cd(dd, pp - rr, dp, dp);
        };

        // { px, py } in range of segment from { 0, 0 } to { dx, dy } 
        inline bool in_range_rr_segment(int32_t px, int32_t py, int64_t rr, int32_t dx, int32_t dy)
        {
            auto pp = int64_t(px) * px + int64_t(py) * py;
            if (pp < rr)
                return true;

            auto dp = int64_t(px) * dx + int64_t(py) * dy;
            if (dp < 0)
                return false;

            auto ux = int64_t(px) - dx;
            auto uy = int64_t(py) - dy;
            if (ux * ux + uy * uy < rr)
                return true;

            auto dd = int64_t(dx) * dx + int64_t(dy) * dy;
            if (dp > dd)
                return false;

            return ab_less_cd(dd, pp - rr, dp, dp);
        };

        inline bool ball_hits_edge(vec2<int32_t> const& p, int64_t rr, vec2<int32_t> const& a, vec2<int32_t> const& b)
        {
            return in_range_rr_edge(p.x - a.x, p.y - a.y, rr, b.x - a.x, b.y - a.y);
        }

        inline bool ball_hits_ray(vec2<int32_t> const& p, int64_t rr, vec2<int32_t> const& a, vec2<int32_t> const& b)
        {
            return in_range_rr_ray(p.x - a.x, p.y - a.y, rr, b.x - a.x, b.y - a.y);
        }

#if CE_CPU_X86

        inline int32_t isqrt(int64_t v)
        {
            return _mm_cvtsd_si32(_mm_sqrt_sd(_mm_setzero_pd(), _mm_cvtsi64_sd(_mm_setzero_pd(), v)));
            //return _mm_cvtsd_si32(_mm_sqrt_sd(_mm_setzero_pd(), _mm_set1_pd(double(v))));
            //return _mm_cvtss_si32(_mm_sqrt_ss(_mm_cvtsi64_ss(_mm_setzero_ps(), v)));
        }

        inline int32_t hypot(int32_t x, int32_t y)
        {
            return _mm_cvtsd_si32(_mm_sqrt_sd(_mm_setzero_pd(), _mm_cvtsi64_sd(_mm_setzero_pd(), int64_t(x) * x + int64_t(y) * y)));
            //return _mm_cvtsd_si32(_mm_sqrt_sd(_mm_setzero_pd(), _mm_set1_pd(x * double(x) + y * double(y))));
            //return _mm_cvtss_si32(_mm_sqrt_ss(_mm_cvtsi64_ss(_mm_setzero_ps(), int64_t(x) * x + int64_t(y) * y)));
        }

        template<class...U>
        inline vec3<float, U...> normal(vec3<float, U...> const& v)
        {
            float m = v.x * v.x + v.y * v.y + v.z * v.z;
            float r = _mm_cvtss_f32(_mm_rsqrt_ss(_mm_set_ss(m)));
            return { v.x * r, v.y * r, v.z * r };
        }

#else
        namespace detail
        {
            extern "C" double sqrt(double);
            extern "C" float sqrtf(float);
        }

        inline int32_t hypot(int32_t x, int32_t y)
        {
            return int(detail::sqrt(x * double(x) + y * double(y)));
        }

        template<class...U>
        inline vec3<float, U...> normal(vec3<float, U...> const& v)
        {
            float m = v.x * v.x + v.y * v.y + v.z * v.z;
            float r = 1.0f / sqrt(m);
            return { v.x * r, v.y * r, v.z * r };
        }


#endif    

        template<class T>
        bool intersect(vec2<T> const& p0, vec2<T> const& p1, vec2<T> const& q0, vec2<T> const& q1, vec2<T>& result)
        {
            result = p0;

            auto p = p1 - p0;
            auto q = q1 - q0;
            auto d = crossx(p, q);

            // lines are ||, no intersection
            if (d == 0)
                return false;

            auto t = crossx(q0 - p0, q);
            result = p0 + muldiv(p, t, d);
            return true;
        }

    }

    // angle is a signed 1.0.31 fixed point factor of 2 pi
    enum angle : int32_t
    {
        _0 = 0,
        _45 = 0x10000000,
        _90 = 0x20000000,
        _135 = 0x30000000,
        _180 = 0x40000000,
        _225 = 0x50000000,
        _270 = 0x60000000,
        _315 = 0x70000000,
        _360 = int32_t(0x80000000)
    };

    namespace detail
    {
        constexpr bool f_eq(double a, double b) { return a - b == 0; }

        constexpr double c_arctan(double a, double xx, double n, double d)
        {
            return f_eq(a, a + n / d) ? a + n / d : c_arctan(a + n / d, xx, n * xx, d + 2);
        }

        constexpr double c_arctan(double x)
        {
            return x == 1.0 ? 4 * c_arctan(.2) - c_arctan(1. / 239) : x - x * x * x / 3 + c_arctan(0, -x * x, x * x * x * x * x, 5);
        }

        constexpr double c_pi = c_arctan(1.0) * 4.0;

        constexpr int64_t dki(size_t n) { return int64_t(c_arctan(1. / (1ll << n)) * (1ll << 62) / c_pi + 0.5); }

        template<class> struct cordic_n;
        template<size_t...Is> struct cordic_n<items<size_t, Is...>>
        {
            static constexpr int64_t k = (int64_t)(0.6072529350088812561694 * (1ll << 32));
            static constexpr int64_t ks[]{ dki(Is)... };

            static vec2<int32_t> rotate(int32_t x0, int32_t y0, int32_t z0)
            {
                int64_t x = x0 * k;
                int64_t y = y0 * k;
                int64_t z = (z0 & 0x1fffffff) * 0x0000000100000000ll;

                for (size_t i = 0; i < 32; ++i)
                {
                    int64_t d = z < 0 ? -1 : 0;

                    int64_t dx = (y ^ d) - d;
                    int64_t dy = (x ^ d) - d;
                    int64_t dz = (ks[i] ^ d) - d;

                    x = x - (dx >> i);
                    y = y + (dy >> i);
                    z = z - dz;
                }

                x0 = int((x + 0x0000000080000000ll) >> 32);
                y0 = int((y + 0x0000000080000000ll) >> 32);
                // z should be 0

                switch (z0 & 0x60000000)
                {
                case 0:
                    return { +x0, +y0 };
                case 0x20000000:
                    return { -y0, +x0 };
                case 0x40000000:
                    return { -x0, -y0 };
                case 0x60000000:
                    return { +y0, -x0 };
                }

                CE_ERROR();
                return { 0, 0 };
            }

            static vec2<int32_t> vector(int32_t x0, int32_t y0, int32_t z0)
            {
                int64_t x = x0 * k;
                int64_t y = y0 * k;
                int64_t z = z0 * 0x0000000100000000ll;

                if (x < 0)
                {
                    // rotate 180
                    x = -x;
                    y = -y;


                    if (y < 0)
                        z += 0x4000000000000000ll;
                    else
                        z -= 0x4000000000000000ll;
                }

                for (size_t i = 0; i < 32; ++i)
                {
                    int64_t d = y < 0 ? -1 : 0;

                    int64_t dx = (y ^ d) - d;
                    int64_t dy = (x ^ d) - d;
                    int64_t dz = (ks[i] ^ d) - d;

                    x = x + (dx >> i);
                    y = y - (dy >> i);
                    z = z + dz;
                }

                x0 = int((x + 0x0000000080000000ll) >> 32);
                // y should be 0
                z0 = int((z + 0x0000000080000000ll) >> 32);

                return { x0, z0 };
            }

        };
    }
    using cordic = detail::cordic_n<sequence_t<size_t, 32>>;

    inline vec2<int32_t> to_cartesian(int32_t r, int32_t t) { return cordic::rotate(r, 0, t); }

    // r * cos(t * 2_pi)
    inline int32_t cos_2pi(int32_t r, int32_t t) { return cordic::rotate(r, 0, t).x; }

    // r * sin(t * 2_pi)
    inline int32_t sin_2pi(int32_t r, int32_t t) { return cordic::rotate(r, 0, t).y; }

    inline vec2<int32_t> to_polar(int32_t x, int32_t y) { return cordic::vector(x, y, 0); }

    inline int32_t polar_mag(int32_t x, int32_t y) { return cordic::vector(x, y, 0).x; };
    inline int32_t polar_ang(int32_t x, int32_t y) { return cordic::vector(x, y, 0).y; };

    // return atan2(y, x) / (2pi)
    inline int32_t atan2_2pi(int32_t y, int32_t x) { return cordic::vector(x, y, 0).y; };


    constexpr int32_t degrees_to_angle(int32_t d) { return int32_t(int64_t(d) * angle::_45 / 45); }

    using namespace math;
}
