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

#ifndef __cplusplus
#include <stdint.h>
#include <string.h>
#else
#include <cstdint>
#include <cstring>
extern "C" {
#endif

// Not to be re-exported on behalf of a library.
#ifdef __GNUC__
#pragma GCC visibility push(hidden)
#endif

uint64_t spooky64v2short(const void *data, size_t size, uint64_t seed);
uint64_t spooky64v2long(const void *data, size_t size, uint64_t seed);

static inline uint64_t spooky64v2(const void *data, size_t size, uint64_t seed)
{
    if (size < 192)
	return spooky64v2short(data, size, seed);
    return spooky64v2long(data, size, seed);
}

#ifdef __GNUC__
#pragma GCC visibility pop
#endif

#ifdef __cplusplus
}
#endif
