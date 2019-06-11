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

#if defined(_WIN32) || defined(__CYGWIN__)
#define ARG0 %rcx
#define ARG1 %rdx
#define ARG2 %r8
#define ARG3 %r9
#define NOARG0 %rsi
#define NOARG1 %rdi
#else
#define ARG0 %rdi
#define ARG1 %rsi
#define ARG2 %rdx
#define ARG3 %rcx
#define NOARG0 %r8
#define NOARG1 %r9
#endif

#define Data ARG0
#define Size ARG1
#define Seed ARG2
#define S0 %rax // the retrun value
#define S1 ARG2 // initialized with the seed
#define S2 ARG1 // length gets clobbered
#define S3 ARG3
#define S4 NOARG0
#define S5 NOARG1
#define S6 %rbx
#define S7 %rbp
#define S8 %r10
#define S9 %r11
#define S10 %r12
#define S11 %r13
#define Last %r14

#define FUNC(name)  \
	.global    name; \
	.hidden    name; \
	.type      name,@function; \
	.align     32; \
	name:

#define ENDF(name) \
	.size      name,.-name

#define Mix(i0, i1, i2, i10, i11, k) \
	xor  S##i10,S##i2     ; \
	xor  S##i0,S##i11     ; \
	rol  $k,S##i0         ; \
	add  S##i1,S##i11

#define Mix0  Mix( 0,  1,  2, 10, 11, 11)
#define Mix1  Mix( 1,  2,  3, 11,  0, 32)
#define Mix2  Mix( 2,  3,  4,  0,  1, 43)
#define Mix3  Mix( 3,  4,  5,  1,  2, 31)
#define Mix4  Mix( 4,  5,  6,  2,  3, 17)
#define Mix5  Mix( 5,  6,  7,  3,  4, 28)
#define Mix6  Mix( 6,  7,  8,  4,  5, 39)
#define Mix7  Mix( 7,  8,  9,  5,  6, 57)
#define Mix8  Mix( 8,  9, 10,  6,  7, 55)
#define Mix9  Mix( 9, 10, 11,  7,  8, 54)
#define Mix10 Mix(10, 11,  0,  8,  9, 22)
#define Mix11 Mix(11,  0,  1,  9, 10, 46)

#define Mix11a \
	xor  S9,S1   ; \
	xor  S11,S10 ; \
	rol  $46,S11
#define Mix11b \
	add  S0,S10

#define EndMix(i1, i2, i11, k) \
	add  S##i1,S##i11     ; \
	xor  S##i11,S##i2     ; \
	rol  $k,S##i1

#define EndMix1  EndMix( 1,  2, 11, 44)
#define EndMix2  EndMix( 2,  3,  0, 15)
#define EndMix3  EndMix( 3,  4,  1, 34)
#define EndMix4  EndMix( 4,  5,  2, 21)
#define EndMix5  EndMix( 5,  6,  3, 38)
#define EndMix6  EndMix( 6,  7,  4, 33)
#define EndMix7  EndMix( 7,  8,  5, 10)
#define EndMix8  EndMix( 8,  9,  6, 13)
#define EndMix9  EndMix( 9, 10,  7, 38)
#define EndMix10 EndMix(10, 11,  8, 53)
#define EndMix11 EndMix(11,  0,  9, 42)
#define EndMix0  EndMix( 0,  1, 10, 54)

#define EndMixA \
	EndMix1; EndMix2; EndMix3; EndMix4; \
	EndMix5; EndMix6; EndMix7; EndMix8; \
	EndMix9; EndMix10; EndMix11; EndMix0

#define EndMix10b \
	add  S10,S8; \
	xor  S8,S11
#define EndMix11b \
	add  S11,S9; \
	xor  S9,S0
#define EndMix0b \
	rol  $54,S0

#define EndMixB \
	EndMix1; EndMix2; EndMix3; EndMix4; \
	EndMix5; EndMix6; EndMix7; EndMix8; \
	EndMix9; EndMix10b; EndMix11b; EndMix0b

FUNC(spooky64v2long)
	push %rbx
	push %rbp
	push %r12
	push %r13
	push %r14

	lea  -96(Data,Size,1),Last

	mov  S1,S0
	movabs $0xdeadbeefdeadbeef,S2

	mov  S1,S4
	mov  S1,S7
	mov  S1,S10

	mov  S0,S3
	mov  S0,S6
	mov  S0,S9

	mov  S2,S5
	mov  S2,S8
	mov  S2,S11

	.align 32
1:	add   0*8(Data),S0;    Mix0
	add   1*8(Data),S1;    Mix1
	add   2*8(Data),S2;    Mix2
	add   3*8(Data),S3;    Mix3
	add   4*8(Data),S4;    Mix4
	add   5*8(Data),S5;    Mix5
	add   6*8(Data),S6;    Mix6
	add   7*8(Data),S7;    Mix7
	add   8*8(Data),S8;    Mix8
	add   9*8(Data),S9;    Mix9
	add  10*8(Data),S10;   Mix10
	add  11*8(Data),S11;   Mix11a
	add  $96,Data;         Mix11b
	cmp  Last,Data
	jbe  1b

	movups 64(Last),%xmm0
	movups 80(Last),%xmm1
	pxor %xmm2,%xmm2
	movaps %xmm0,-64(%rsp)
	movaps %xmm1,-48(%rsp)
	add  $64,Last // Last32
	movaps %xmm2,-32(%rsp)
	movaps %xmm2,-16(%rsp)

	cmp  Last,Data
	jae  2f

	add  0*8(Data),S0
	add  1*8(Data),S1
	sub  $32,Last // Last64
	add  2*8(Data),S2
	add  3*8(Data),S3
	cmp  Last,Data
	jae  3f

	add  4*8(Data),S4
	add  5*8(Data),S5
	sub  $32,Last // Last96
	add  6*8(Data),S6
	add  7*8(Data),S7

	sub  Last,Data // Data = off
	mov  $96,Last // Last = left
	sub  Data,Last
	add  %rsp,Data
	shl  $56,Last
	add  Last,S11

	add  -8*8(Data),S8
	add  -7*8(Data),S9
	add  -6*8(Data),S10
	add  -5*8(Data),S11
	jmp  4f

2:	sub  Last,Data
	mov  $32,Last
	sub  Data,Last
	add  %rsp,Data
	shl  $56,Last
	add  Last,S11

	add  -8*8(Data),S0
	add  -7*8(Data),S1
	add  -6*8(Data),S2
	add  -5*8(Data),S3
	jmp  4f

3:	sub  Last,Data
	mov  $64,Last
	sub  Data,Last
	add  %rsp,Data
	shl  $56,Last
	add  Last,S11

	add  -8*8(Data),S4
	add  -7*8(Data),S5
	add  -6*8(Data),S6
	add  -5*8(Data),S7

4:	EndMixA
	EndMixA
	EndMixB

	pop  %r14
	pop  %r13
	pop  %r12
	pop  %rbp
	pop  %rbx
	ret
ENDF(spooky64v2long)
