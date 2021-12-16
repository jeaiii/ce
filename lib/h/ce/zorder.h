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
	inline uint64_t z_expand(uint32_t n)
	{
		const uint64_t m4 = n;
		const uint64_t m3 = 0x0000ffff0000ffff & (m4 | (m4 << 16));
		const uint64_t m2 = 0x00ff00ff00ff00ff & (m3 | (m3 << 8));
		const uint64_t m1 = 0x0f0f0f0f0f0f0f0f & (m2 | (m2 << 4));
		const uint64_t m0 = 0x3333333333333333 & (m1 | (m1 << 2));
		const uint64_t mn = 0x5555555555555555 & (m0 | (m0 << 1));
		return mn;
	}

	inline uint64_t z_encode(int32_t x, int32_t y)
	{
		auto u = z_expand(uint32_t(x) ^ 0x80000000);
		auto v = z_expand(uint32_t(y) ^ 0x80000000);
		return v + v + u;
	}

	inline bool z_split(uint64_t lo, uint64_t hi, uint64_t& jo, uint64_t& ji)
	{
		uint64_t d = lo ^ hi;
		uint64_t dx = d & 0x5555555555555555;
		uint64_t dy = d & 0xaaaaaaaaaaaaaaaa;
		uint64_t m = dx < dy ? dy : dx;
		// smear
		m |= m >> 2;
		m |= m >> 4;
		m |= m >> 8;
		m |= m >> 16;
		m |= m >> 32;

		// replace bits in hi with 01....1 for jump out
		// new box is [lo, jo]
		jo = (hi & ~m) | (m >> 2);

		// replace bits in lo with 10....0 for jump in
		// new box is [ji, hi]
		ji = (lo | m) & ~(m >> 2);

		//CE_ASSERT(jo <= ji);
		jo &= ~0x0ffff;
		ji &= ~0x0ffff;

		return ji - jo > 0x010000;
	}

	inline uint64_t const* z_lower_bound(uint64_t const* a, uint64_t const* b, uint64_t z)
	{
		for (size_t n = b - a; n > 0;)
		{
			size_t step = n / 2;
			uint64_t const* i = a + step;
			if (*i < z) {
				a = ++i;
				n -= step + 1;
			}
			else
				n = step;
		}
		return a;
	}

	inline bool z_inside(uint64_t lo, uint64_t hi, uint64_t /*zn*/)
	{
		uint64_t zn_x = lo & 0x5555555555555555;
		uint64_t zn_y = hi & 0xaaaaaaaaaaaaaaaa;

		uint64_t lo_x = lo & 0x5555555555555555;
		uint64_t lo_y = lo & 0xaaaaaaaaaaaaaaaa;

		uint64_t hi_x = hi & 0x5555555555555555;
		uint64_t hi_y = hi & 0xaaaaaaaaaaaaaaaa;

		if (zn_x < lo_x || zn_x > hi_x)
			return false;

		if (zn_y < lo_y || zn_y > hi_y)
			return false;

		return true;
	}

	inline size_t z_query(uint64_t* out, uint64_t lo, uint64_t hi, size_t size, uint64_t const* data)
	{
		auto last = data + size;
		size = 0;

		//data = z_lower_bound(data, last, lo);
		last = z_lower_bound(data, last, hi + 0x0ffff);

		uint64_t spans[32 * 2];
		auto next = &spans[0];

		while (data < last)
		{
			uint64_t zn = *data++;

			while (zn > hi + 0x0ffff)
			{
				// pop
				if (next <= &spans[0])
					return size;

				next -= 2;
				lo = next[0];
				hi = next[1];
			}

		test_lo:
			if (zn < lo)
			{
				data = z_lower_bound(data, last, lo);
				continue;
			}

			// zn in [lo,hi]
			
			uint64_t mo;
			uint64_t mi;
			while (z_split(lo, hi, mo, mi))
			{
				if (zn > mo + 0x0ffff)
				{
					lo = mi;
					goto test_lo;
				}
				else
				{
					//CE_ASSERT(next < &spans[32 * 2]);
					// push
					next[0] = mi;
					next[1] = hi;
					next += 2;
					hi = mo;
				}
			}

			out[size++] = zn;
		}

		return size;
	}
}