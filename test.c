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

#undef NDEBUG
#include <assert.h>
#include "spooky64v2.h"

int main()
{
    // The example is from en.wikipedia.org/wiki/Jenkins_hash_function
#define STR "The quick brown fox jumps over the lazy dog"
    const char s0[] = STR;
    uint64_t h0 = spooky64v2(s0, sizeof s0 - 1, 0);
    assert(h0 == UINT64_C(0x2b12e846aa0693c7));

    const char s1[] = STR " " STR " " STR " " STR " " STR;
    uint64_t h1 = spooky64v2(s1, sizeof s1 - 1, 0);
    assert(h1 == UINT64_C(0xf1b71c6ac5af39e7));

    // This test is somewhat similar to the one provided by Bob Jenkins
    // in burtleburtle.net/bob/c/TestSpookyV2.cpp
    unsigned char buf[512];
    uint64_t hash = UINT64_C(0xe220a8397b1dcdaf);
    for (size_t i = 0; i < sizeof buf; i++) {
	buf[i] = i + 128;
	hash = spooky64v2(buf, i, hash);
    }
    // The value was obtained with the original SpookyHash::Hash64
    // from burtleburtle.net/bob/c/SpookyV2.cpp
    assert(hash == UINT64_C(0xc32a0b61ff7678c1));

    return 0;
}
