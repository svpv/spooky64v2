// Copyright (c) 2019 Alexey Tourbin
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "spooky64v2.h"

static inline uint64_t fetch64le(const void *p)
{
    uint64_t x;
    memcpy(&x, p, 8);
    return x;
}

static inline uint32_t fetch32le(const void *p)
{
    uint32_t x;
    memcpy(&x, p, 4);
    return x;
}

static inline uint16_t fetch16le(const void *p)
{
    uint16_t x;
    memcpy(&x, p, 2);
    return x;
}

static inline uint64_t tail64le(const void *end, int nbytes)
{
    uint64_t x = fetch64le((const char *) end - 8);
    int k = (8 - nbytes) * 8;
    return x >> k;
}

static inline uint32_t tail32le(const void *end, int nbytes)
{
    uint32_t x = fetch32le((const char *) end - 4);
    int k = (4 - nbytes) * 8;
    return x >> k;
}

static inline uint64_t rotl64(uint64_t x, int k)
{
    return x << k | x >> (64 - k);
}

uint64_t spooky64v2veryshort(const void *data, size_t size, uint64_t seed)
{
    uint64_t a, b, c, d;
    a = b = seed;
    c = d = UINT64_C(0xdeadbeefdeadbeef);

#define ShortMix() \
    do { \
	c = rotl64(c, 50);  c += d;  a ^= c;  \
	d = rotl64(d, 52);  d += a;  b ^= d;  \
	a = rotl64(a, 30);  a += b;  c ^= a;  \
	b = rotl64(b, 41);  b += c;  d ^= b;  \
	c = rotl64(c, 54);  c += d;  a ^= c;  \
	d = rotl64(d, 48);  d += a;  b ^= d;  \
	a = rotl64(a, 38);  a += b;  c ^= a;  \
	b = rotl64(b, 37);  b += c;  d ^= b;  \
	c = rotl64(c, 62);  c += d;  a ^= c;  \
	d = rotl64(d, 34);  d += a;  b ^= d;  \
	a = rotl64(a, 5);   a += b;  c ^= a;  \
	b = rotl64(b, 36);  b += c;  d ^= b;  \
    } while (0)
#define ShortEndMix() \
    do { \
	d ^= c;  c = rotl64(c, 15);  d += c;  \
	a ^= d;  d = rotl64(d, 52);  a += d;  \
	b ^= a;  a = rotl64(a, 26);  b += a;  \
	c ^= b;  b = rotl64(b, 51);  c += b;  \
	d ^= c;  c = rotl64(c, 28);  d += c;  \
	a ^= d;  d = rotl64(d, 9);   a += d;  \
	b ^= a;  a = rotl64(a, 47);  b += a;  \
	c ^= b;  b = rotl64(b, 54);  c += b;  \
	d ^= c;  c = rotl64(c, 32);  d += c;  \
	a ^= d;  d = rotl64(d, 25);  a += d;  \
	b ^= a;  a = rotl64(a, 63);  b += a;  \
    } while (0)

    d += (uint64_t) size << 56;

    switch (size) {
    default:
	d += tail64le((const char *) data + size, size - 8);
	// fall through
    case 8:
	c += fetch64le(data);
	break;

    case 7:
    case 6:
    case 5:
	c += (uint64_t) tail32le((const char *) data + size, size - 4) << 32;
	// fall through
    case 4:
	c += fetch32le(data);
	break;

    case 3:
	c += ((const unsigned char *) data)[2] << 16;
	// fall through
    case 2:
	c += fetch16le(data);
	break;

    case 1:
	c += ((const unsigned char *) data)[0];
	break;

    case 0:
	c += UINT64_C(0xdeadbeefdeadbeef);
	d += UINT64_C(0xdeadbeefdeadbeef);
    }

    ShortEndMix();
    return a;
}

uint64_t spooky64v2short(const void *data, size_t size, uint64_t seed)
{
    uint64_t a, b, c, d;
    a = b = seed;
    c = d = UINT64_C(0xdeadbeefdeadbeef);

#define Load(i) fetch64le((const char *) data + 8 * i)

    const void *last32 = (const char *) data + size - 32;
    while (data <= last32) {
	c += Load(0);
	d += Load(1);
	ShortMix();
	a += Load(2);
	b += Load(3);
	data = (const char *) data + 32;
    }

    const void *last16 = (const char *) last32 + 16;
    if (data <= last16) {
	c += Load(0);
	d += Load(1);
	ShortMix();
	data = (const char *) data + 16;
    }

    d += (uint64_t) size << 56;

    if (size % 16 == 0) {
	c += UINT64_C(0xdeadbeefdeadbeef);
	d += UINT64_C(0xdeadbeefdeadbeef);
    }
    else {
	char tail[32];
	memcpy(tail, last16, 16);
	memset(tail + 16, 0, 16);
	size_t off = (const char *) data - (const char *) last16;
	data = tail + off;
	c += Load(0);
	d += Load(1);
    }

    ShortEndMix();
    return a;
}

uint64_t spooky64v2long(const void *data, size_t size, uint64_t seed)
{
    uint64_t s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11;
    s0 = s3 = s6 = s9 = seed;
    s1 = s4 = s7 = s10 = seed;
    s2 = s5 = s8 = s11 = UINT64_C(0xdeadbeefdeadbeef);

#define LongMix() \
    do { \
	s0  += Load(0);   s2  ^= s10;  s11 ^= s0;   s0  = rotl64(s0,  11);  s11 += s1;  \
	s1  += Load(1);   s3  ^= s11;  s0  ^= s1;   s1  = rotl64(s1,  32);  s0  += s2;  \
	s2  += Load(2);   s4  ^= s0;   s1  ^= s2;   s2  = rotl64(s2,  43);  s1  += s3;  \
	s3  += Load(3);   s5  ^= s1;   s2  ^= s3;   s3  = rotl64(s3,  31);  s2  += s4;  \
	s4  += Load(4);   s6  ^= s2;   s3  ^= s4;   s4  = rotl64(s4,  17);  s3  += s5;  \
	s5  += Load(5);   s7  ^= s3;   s4  ^= s5;   s5  = rotl64(s5,  28);  s4  += s6;  \
	s6  += Load(6);   s8  ^= s4;   s5  ^= s6;   s6  = rotl64(s6,  39);  s5  += s7;  \
	s7  += Load(7);   s9  ^= s5;   s6  ^= s7;   s7  = rotl64(s7,  57);  s6  += s8;  \
	s8  += Load(8);   s10 ^= s6;   s7  ^= s8;   s8  = rotl64(s8,  55);  s7  += s9;  \
	s9  += Load(9);   s11 ^= s7;   s8  ^= s9;   s9  = rotl64(s9,  54);  s8  += s10; \
	s10 += Load(10);  s0  ^= s8;   s9  ^= s10;  s10 = rotl64(s10, 22);  s9  += s11; \
	s11 += Load(11);  s1  ^= s9;   s10 ^= s11;  s11 = rotl64(s11, 46);  s10 += s0;  \
    } while (0)
#define LongEndMix() \
    do { \
	s11 += s1;   s2  ^= s11;  s1  = rotl64(s1,  44);  \
	s0  += s2;   s3  ^= s0;   s2  = rotl64(s2,  15);  \
	s1  += s3;   s4  ^= s1;   s3  = rotl64(s3,  34);  \
	s2  += s4;   s5  ^= s2;   s4  = rotl64(s4,  21);  \
	s3  += s5;   s6  ^= s3;   s5  = rotl64(s5,  38);  \
	s4  += s6;   s7  ^= s4;   s6  = rotl64(s6,  33);  \
	s5  += s7;   s8  ^= s5;   s7  = rotl64(s7,  10);  \
	s6  += s8;   s9  ^= s6;   s8  = rotl64(s8,  13);  \
	s7  += s9;   s10 ^= s7;   s9  = rotl64(s9,  38);  \
	s8  += s10;  s11 ^= s8;   s10 = rotl64(s10, 53);  \
	s9  += s11;  s0  ^= s9;   s11 = rotl64(s11, 42);  \
	s10 += s0;   s1  ^= s10;  s0  = rotl64(s0,  54);  \
    } while (0)

    const void *last96 = (const char *) data + size - 96;
    do {
	LongMix();
	data = (const char *) data + 96;
    } while (data <= last96);

    unsigned char tail[2*96];
    memcpy(tail, last96, 96);
    memset(tail + 96, 0, 96);

    size_t off = (const char *) data - (const char *) last96;
    tail[off+95] = 96 - off;
    data = tail + off;

    s0 += Load(0); s1 += Load(1); s2 += Load(2); s3 += Load(3);
    s4 += Load(4); s5 += Load(5); s6 += Load(6); s7 += Load(7);
    s8 += Load(8); s9 += Load(9); s10 += Load(10); s11 += Load(11);

    LongEndMix();
    LongEndMix();
    LongEndMix();
    return s0;
}
