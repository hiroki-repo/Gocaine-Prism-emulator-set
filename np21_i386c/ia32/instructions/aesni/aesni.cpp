/*
 * Copyright (c) 2024 Gocaine Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "compiler.h"

#include <math.h>
#include <float.h>
#include <stdint.h>

#define isnan(x) (_isnan(x))

#include "../../cpu.h"
#include "../../ia32.mcr"

#include "../sse/sse.h"
#include "../sse2/sse2.h"
#include "../sse3/sse3.h"
#include "../ssse3/ssse3.h"
#include "aesni.h"

#if defined(USE_AESNI) && defined(USE_SSSE3) && defined(USE_SSE3) && defined(USE_SSE2) && defined(USE_SSE) && defined(USE_FPU)

#define CPU_SSSE3WORKCLOCK	CPU_WORKCLOCK(2)

static INLINE void
AESNI_check_NM_EXCEPTION() {
	// SSE4.2 ?  ? UD(     I y R [ h  O) ??       
	if (!(i386cpuid.cpu_feature_ecx & CPU_FEATURE_ECX_AES)) {
		EXCEPTION(UD_EXCEPTION, 0);
	}
	//  G ~     [ V     ? UD(     I y R [ h  O) ??       
	if (CPU_CR0 & CPU_CR0_EM) {
		EXCEPTION(UD_EXCEPTION, 0);
	}
	//  ^ X N X C b `    NM( f o C X g p s ? O) ??       
	if (CPU_CR0 & CPU_CR0_TS) {
		EXCEPTION(NM_EXCEPTION, 0);
	}
}

static INLINE void
AESNI_setTag(void)
{
}

static INLINE void
PCLMULQDQ_check_NM_EXCEPTION() {
	// SSE4.2 ?  ? UD(     I y R [ h  O) ??       
	if (!(i386cpuid.cpu_feature_ecx & CPU_FEATURE_ECX_PCLMULDQ)) {
		EXCEPTION(UD_EXCEPTION, 0);
	}
	//  G ~     [ V     ? UD(     I y R [ h  O) ??       
	if (CPU_CR0 & CPU_CR0_EM) {
		EXCEPTION(UD_EXCEPTION, 0);
	}
	//  ^ X N X C b `    NM( f o C X g p s ? O) ??       
	if (CPU_CR0 & CPU_CR0_TS) {
		EXCEPTION(NM_EXCEPTION, 0);
	}
}

static INLINE void
PCLMULQDQ_setTag(void)
{
}

// mmx.c ? ??   
static INLINE void
MMX_setTag(void)
{
	int i;

	if (!FPU_STAT.mmxenable) {
		FPU_STAT.mmxenable = 1;
		//FPU_CTRLWORD = 0x27F;
		for (i = 0; i < FPU_REG_NUM; i++) {
			FPU_STAT.tag[i] = TAG_Valid;
#ifdef SUPPORT_FPU_DOSBOX2
			FPU_STAT.int_regvalid[i] = 0;
#endif
			FPU_STAT.reg[i].ul.ext = 0xffff;
		}
	}
	FPU_STAT_TOP = 0;
	FPU_STATUSWORD &= ~0x3800;
	FPU_STATUSWORD |= (FPU_STAT_TOP & 7) << 11;
}

/*
 * SSE4.2 interface
 */

 //  R [ h       ? ?? ?   ?  ? 
 // xmm/m128 -> xmm
static INLINE void SSE_PART_GETDATA1DATA2_PD(double** data1, double** data2, double* data2buf) {
	UINT32 op;
	UINT idx, sub;

	AESNI_check_NM_EXCEPTION();
	AESNI_setTag();
	CPU_SSSE3WORKCLOCK;
	GET_PCBYTE((op));
	idx = (op >> 3) & 7;
	sub = (op & 7);
	*data1 = (double*)(&(FPU_STAT.xmm_reg[idx]));
	if ((op) >= 0xc0) {
		*data2 = (double*)(&(FPU_STAT.xmm_reg[sub]));
	}
	else {
		UINT32 maddr;
		maddr = calc_ea_dst((op));
		*((UINT64*)(data2buf + 0)) = cpu_vmemoryread_q(CPU_INST_SEGREG_INDEX, maddr + 0);
		*((UINT64*)(data2buf + 1)) = cpu_vmemoryread_q(CPU_INST_SEGREG_INDEX, maddr + 8);
		*data2 = data2buf;
	}
}
static INLINE void SSE_PART_GETDATA1DATA2_PD_PCLMULQDQ(double** data1, double** data2, double* data2buf) {
	UINT32 op;
	UINT idx, sub;

	PCLMULQDQ_check_NM_EXCEPTION();
	PCLMULQDQ_setTag();
	CPU_SSSE3WORKCLOCK;
	GET_PCBYTE((op));
	idx = (op >> 3) & 7;
	sub = (op & 7);
	*data1 = (double*)(&(FPU_STAT.xmm_reg[idx]));
	if ((op) >= 0xc0) {
		*data2 = (double*)(&(FPU_STAT.xmm_reg[sub]));
	}
	else {
		UINT32 maddr;
		maddr = calc_ea_dst((op));
		*((UINT64*)(data2buf + 0)) = cpu_vmemoryread_q(CPU_INST_SEGREG_INDEX, maddr + 0);
		*((UINT64*)(data2buf + 1)) = cpu_vmemoryread_q(CPU_INST_SEGREG_INDEX, maddr + 8);
		*data2 = data2buf;
	}
}

static INLINE void SSE_PART_GETDATA1DATA2_P_UINT32(UINT32** data1, UINT32** data2, UINT32* data2buf) {
	SSE_PART_GETDATA1DATA2_PD((double**)data1, (double**)data2, (double*)data2buf);
}
static INLINE void SSE_PART_GETDATA1DATA2_PD_UINT64(UINT64** data1, UINT64** data2, UINT64* data2buf) {
	SSE_PART_GETDATA1DATA2_PD((double**)data1, (double**)data2, (double*)data2buf);
}

static INLINE void MMX_PART_GETDATA1DATA2_PD(float** data1, float** data2, float* data2buf) {
	UINT32 op;
	UINT idx, sub;

	AESNI_check_NM_EXCEPTION();
	AESNI_setTag();
	CPU_SSSE3WORKCLOCK;
	GET_PCBYTE((op));
	idx = (op >> 3) & 7;
	sub = (op & 7);
	*data1 = (float*)(&(FPU_STAT.reg[idx]));
	if ((op) >= 0xc0) {
		*data2 = (float*)(&(FPU_STAT.reg[sub]));
	}
	else {
		UINT32 maddr;
		maddr = calc_ea_dst((op));
		*((UINT32*)(data2buf + 0)) = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, maddr);
		*((UINT32*)(data2buf + 1)) = cpu_vmemoryread_d(CPU_INST_SEGREG_INDEX, maddr + 4);
		*data2 = data2buf;
	}
}
// below constants and routine is based on Box64 thanks for ptitSeb
 // AES opcodes constants
                         //   A0 B1 C2 D3 E4 F5 G6 H7 I8 J9 Ka Lb Mc Nd Oe Pf
                         //   A  F  K  P  E  J  O  D  I  N  C  H  M  B  G  L
const uint8_t shiftrows[] = { 0, 5,10,15, 4, 9,14, 3, 8,13, 2, 7,12, 1, 6,11 };
const uint8_t subbytes[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16,
};
//   A0 B1 C2 D3 E4 F5 G6 H7 I8 J9 Ka Lb Mc Nd Oe Pf
//   A  N  K  H  E  B  O  L  I  F  C  P  M  J  G  D
const uint8_t invshiftrows[] = { 0,13,10, 7, 4, 1,14,11, 8, 5, 2,15,12, 9, 6, 3 };
const uint8_t invsubbytes[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d,
};

static uint8_t ff_mult(uint8_t a, uint8_t b)
{
	int retval = 0;

	for (int i = 0; i < 8; i++) {
		if ((b & 1) == 1)
			retval ^= a;

		if ((a & 0x80)) {
			a <<= 1;
			a ^= 0x1b;
		}
		else {
			a <<= 1;
		}

		b >>= 1;
	}

	return retval;
}

void AESNI_AESIMC(void) {
	int i;
	UINT32 tmp;

	UINT32 op;

	UINT8 databuf[16];
	UINT8 data2buf[16];
	UINT8* data1, * data2;
	SSE_PART_GETDATA1DATA2_PD((double**)(&data1), (double**)(&data2), (double*)data2buf);
	for (i = 0; i < 16; i++) {
		databuf[i] = data2[i];
	}
	for (i = 0; i < 4; i++) {
		data1[(i * 4) + 0] = ff_mult(0x0E, databuf[(i * 4) + 0]) ^ ff_mult(0x0B, databuf[(i * 4) + 1]) ^ ff_mult(0x0D, databuf[(i * 4) + 2]) ^ ff_mult(0x09, databuf[(i * 4) + 3]);
		data1[(i * 4) + 1] = ff_mult(0x09, databuf[(i * 4) + 0]) ^ ff_mult(0x0E, databuf[(i * 4) + 1]) ^ ff_mult(0x0B, databuf[(i * 4) + 2]) ^ ff_mult(0x0D, databuf[(i * 4) + 3]);
		data1[(i * 4) + 2] = ff_mult(0x0D, databuf[(i * 4) + 0]) ^ ff_mult(0x09, databuf[(i * 4) + 1]) ^ ff_mult(0x0E, databuf[(i * 4) + 2]) ^ ff_mult(0x0B, databuf[(i * 4) + 3]);
		data1[(i * 4) + 3] = ff_mult(0x0B, databuf[(i * 4) + 0]) ^ ff_mult(0x0D, databuf[(i * 4) + 1]) ^ ff_mult(0x09, databuf[(i * 4) + 2]) ^ ff_mult(0x0E, databuf[(i * 4) + 3]);
	}
	TRACEOUT(("AESNI_AESIMC"));
}

void AESNI_AESENC(void) {
	int i;
	UINT32 tmp;

	UINT32 op;

	UINT8 databuf[16];
	UINT8 databuf2[16];
	UINT8 data2buf[16];
	UINT8* data1, * data2;
	SSE_PART_GETDATA1DATA2_PD((double**)(&data1), (double**)(&data2), (double*)data2buf);
	for (i = 0; i < 16; i++) {
		databuf[i] = subbytes[data1[i]];
	}
	for (i = 0; i < 4; i++) {
		databuf2[(i * 4) + 0] = ff_mult(0x02, databuf[(i * 4) + 0]) ^ ff_mult(0x03, databuf[(i * 4) + 1]) ^        (      databuf[(i * 4) + 2]) ^        (      databuf[(i * 4) + 3]);
		databuf2[(i * 4) + 1] =        (      databuf[(i * 4) + 0]) ^ ff_mult(0x02, databuf[(i * 4) + 1]) ^ ff_mult(0x03, databuf[(i * 4) + 2]) ^        (      databuf[(i * 4) + 3]);
		databuf2[(i * 4) + 2] =        (      databuf[(i * 4) + 0]) ^        (      databuf[(i * 4) + 1]) ^ ff_mult(0x02, databuf[(i * 4) + 2]) ^ ff_mult(0x03, databuf[(i * 4) + 3]);
		databuf2[(i * 4) + 3] = ff_mult(0x03, databuf[(i * 4) + 0]) ^        (      databuf[(i * 4) + 1]) ^        (      databuf[(i * 4) + 2]) ^ ff_mult(0x02, databuf[(i * 4) + 3]);
	}
	for (i = 0; i < 16; i++) {
		data1[i] = databuf2[i] ^ data2[i];
	}
	TRACEOUT(("AESNI_AESENC"));
}

void AESNI_AESENCLAST(void) {
	int i;
	UINT32 tmp;

	UINT32 op;

	UINT8 databuf[16];
	UINT8 data2buf[16];
	UINT8* data1, * data2;
	SSE_PART_GETDATA1DATA2_PD((double**)(&data1), (double**)(&data2), (double*)data2buf);
	for (i = 0; i < 16; i++) {
		databuf[i] = subbytes[data1[shiftrows[i]]];
	}
	for (i = 0; i < 16; i++) {
		data1[i] = databuf[i] ^ data2[i];
	}
	TRACEOUT(("AESNI_AESENCLAST"));
}

void AESNI_AESDEC(void) {
	int i;
	UINT32 tmp;

	UINT32 op;

	UINT8 databuf[16];
	UINT8 databuf2[16];
	UINT8 data2buf[16];
	UINT8* data1, * data2;
	SSE_PART_GETDATA1DATA2_PD((double**)(&data1), (double**)(&data2), (double*)data2buf);
	for (i = 0; i < 16; i++) {
		databuf[i] = invsubbytes[data1[invshiftrows[i]]];
	}
	for (i = 0; i < 4; i++) {
		databuf2[(i * 4) + 0] = ff_mult(0x0E, databuf[(i * 4) + 0]) ^ ff_mult(0x0B, databuf[(i * 4) + 1]) ^ ff_mult(0x0D, databuf[(i * 4) + 2]) ^ ff_mult(0x09, databuf[(i * 4) + 3]);
		databuf2[(i * 4) + 1] = ff_mult(0x09, databuf[(i * 4) + 0]) ^ ff_mult(0x0E, databuf[(i * 4) + 1]) ^ ff_mult(0x0B, databuf[(i * 4) + 2]) ^ ff_mult(0x0D, databuf[(i * 4) + 3]);
		databuf2[(i * 4) + 2] = ff_mult(0x0D, databuf[(i * 4) + 0]) ^ ff_mult(0x09, databuf[(i * 4) + 1]) ^ ff_mult(0x0E, databuf[(i * 4) + 2]) ^ ff_mult(0x0B, databuf[(i * 4) + 3]);
		databuf2[(i * 4) + 3] = ff_mult(0x0B, databuf[(i * 4) + 0]) ^ ff_mult(0x0D, databuf[(i * 4) + 1]) ^ ff_mult(0x09, databuf[(i * 4) + 2]) ^ ff_mult(0x0E, databuf[(i * 4) + 3]);
	}
	for (i = 0; i < 16; i++) {
		data1[i] = databuf2[i] ^ data2[i];
	}
	TRACEOUT(("AESNI_AESDEC"));
}

void AESNI_AESDECLAST(void) {
	int i;
	UINT32 tmp;

	UINT32 op;

	UINT8 databuf[16];
	UINT8 data2buf[16];
	UINT8* data1, * data2;
	SSE_PART_GETDATA1DATA2_PD((double**)(&data1), (double**)(&data2), (double*)data2buf);
	for (i = 0; i < 16; i++) {
		databuf[i] = invsubbytes[data1[invshiftrows[i]]];
	}
	for (i = 0; i < 16; i++) {
		data1[i] = databuf[i] ^ data2[i];
	}
	TRACEOUT(("AESNI_AESDECLAST"));
}

void AESNI_AESKEYGENASSIST(void) {
	int i;
	UINT32 tmp;

	UINT32 op;

	UINT8 databuf[16];
	UINT8 data2buf[16];
	UINT8* data1, * data2;
	SSE_PART_GETDATA1DATA2_PD((double**)(&data1), (double**)(&data2), (double*)data2buf);
	GET_PCBYTE((op));
	if (op & 0x80) { op |= 0xFFFFFF00; }
	for (i = 4; i < 8; i++) {
		data1[i] = subbytes[data2[i]];
	}
	for (i = 12; i < 16; i++) {
		data1[i] = subbytes[data2[i]];
	}
	for (i = 0; i < 4; i++) {
		data1[i] = data1[i + 4];
	}
	tmp = data1[4];
	for (i = 0; i < 3; i++) {
		data1[i + 4] = data1[i + 5];
	}
	data1[7] = tmp;
	for (i = 0; i < 4; i++) {
		data1[4 + i] ^= ((op >> (i * 8)) & 0xFF);
	}
	tmp = data1[12];
	for (i = 0; i < 3; i++) {
		data1[i + 12] = data1[i + 13];
	}
	data1[15] = tmp;
	for (i = 0; i < 4; i++) {
		data1[12 + i] ^= ((op >> (i * 8)) & 0xFF);
	}
	TRACEOUT(("AESNI_AESKEYGENASSIST"));
}

void PCLMULDQ_PCLMULQDQ(void) {
	int i;
	UINT32 tmp;

	UINT32 op;

	UINT64 databuf[] = { 0,0 };
	UINT64 databuf2[] = { 0,0 };
	UINT64 data2buf[2];
	UINT64* data1, * data2;
	SSE_PART_GETDATA1DATA2_PD_PCLMULQDQ((double**)(&data1), (double**)(&data2), (double*)data2buf);
	GET_PCBYTE((op));
	databuf[0] = data2[(op & 16) ? 1 : 0];
	for (i = 0; i < 64; i++) { if (data1[op & 1] & (((UINT64)1) << i)) { databuf2[0] ^= (databuf[0] << i); if (i > 0) { databuf2[1] ^= (databuf[0] >> (64 - i)); } } }
	data1[0] = databuf2[0] & 0xffffffffffffffffLL;
	data1[1] = databuf2[1] & 0xffffffffffffffffLL;
	TRACEOUT(("PCLMULDQ_PCLMULQDQ"));
}

#else

void AESNI_AESIMC(void) {
	EXCEPTION(UD_EXCEPTION, 0);
}

void AESNI_AESENC(void) {
	EXCEPTION(UD_EXCEPTION, 0);
}

void AESNI_AESENCLAST(void) {
	EXCEPTION(UD_EXCEPTION, 0);
}

void AESNI_AESDEC(void) {
	EXCEPTION(UD_EXCEPTION, 0);
}

void AESNI_AESDECLAST(void) {
	EXCEPTION(UD_EXCEPTION, 0);
}

void AESNI_AESKEYGENASSIST(void) {
	EXCEPTION(UD_EXCEPTION, 0);
}

void PCLMULDQ_PCLMULQDQ(void) {
	EXCEPTION(UD_EXCEPTION, 0);
}

#endif
