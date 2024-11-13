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
#include "cpu.h"
#include "jit.h"
#include "inst_table.h"
#include "ia32.mcr"
#ifdef _M_ARM64
#include "arm64_emitter.h"
#endif
#include "instructions/sse/sse.h"
#include "instructions/bin_arith.h"

UINT64 CPU_EIP_VAK = 0;

void __fastcall JIT_CPU_COUNT(UINT32 prm_0) { CPU_REMCLOCK -= (prm_0); return; }

UINT32 __fastcall JIT_ADD_DWORD(UINT32 prm_0, UINT32 prm_1) { UINT32 prm_0x = prm_0; _ADD_DWORD(prm_0x, prm_0, prm_1); return prm_0x; }
UINT16 __fastcall  JIT_ADD_WORD(UINT16 prm_0, UINT16 prm_1) { UINT16 prm_0x = prm_0; _ADD_WORD(prm_0x, prm_0, prm_1); return prm_0x; }
UINT8  __fastcall  JIT_ADD_BYTE(UINT8  prm_0, UINT8  prm_1) { UINT8  prm_0x = prm_0; _ADD_BYTE(prm_0x, prm_0, prm_1); return prm_0x; }

UINT32 __fastcall JIT_ADC_DWORD(UINT32 prm_0, UINT32 prm_1) { UINT32 prm_0x = prm_0; _ADC_DWORD(prm_0x, prm_0, prm_1); return prm_0x; }
UINT16 __fastcall  JIT_ADC_WORD(UINT16 prm_0, UINT16 prm_1) { UINT16 prm_0x = prm_0; _ADC_WORD(prm_0x, prm_0, prm_1); return prm_0x; }
UINT8  __fastcall  JIT_ADC_BYTE(UINT8  prm_0, UINT8  prm_1) { UINT8  prm_0x = prm_0; _ADC_BYTE(prm_0x, prm_0, prm_1); return prm_0x; }

UINT32 __fastcall JIT_SUB_DWORD(UINT32 prm_0, UINT32 prm_1) { UINT32 prm_0x = prm_0; _DWORD_SUB(prm_0x, prm_0, prm_1); return prm_0x; }
UINT16 __fastcall  JIT_SUB_WORD(UINT16 prm_0, UINT16 prm_1) { UINT16 prm_0x = prm_0; _WORD_SUB(prm_0x, prm_0, prm_1); return prm_0x; }
UINT8  __fastcall  JIT_SUB_BYTE(UINT8  prm_0, UINT8  prm_1) { UINT8  prm_0x = prm_0; _BYTE_SUB(prm_0x, prm_0, prm_1); return prm_0x; }

UINT32 __fastcall JIT_SBB_DWORD(UINT32 prm_0, UINT32 prm_1) { UINT32 prm_0x = prm_0; _DWORD_SBB(prm_0x, prm_0, prm_1); return prm_0x; }
UINT16 __fastcall  JIT_SBB_WORD(UINT16 prm_0, UINT16 prm_1) { UINT16 prm_0x = prm_0; _WORD_SBB(prm_0x, prm_0, prm_1); return prm_0x; }
UINT8  __fastcall  JIT_SBB_BYTE(UINT8  prm_0, UINT8  prm_1) { UINT8  prm_0x = prm_0; _BYTE_SBB(prm_0x, prm_0, prm_1); return prm_0x; }

UINT32 __fastcall JIT_OR_DWORD(UINT32 prm_0, UINT32 prm_1) { UINT32 prm_0x = prm_0; _OR_DWORD(prm_0x, prm_1); return prm_0x; }
UINT16 __fastcall  JIT_OR_WORD(UINT16 prm_0, UINT16 prm_1) { UINT16 prm_0x = prm_0; _OR_WORD(prm_0x, prm_1); return prm_0x; }
UINT8  __fastcall  JIT_OR_BYTE(UINT8  prm_0, UINT8  prm_1) { UINT8  prm_0x = prm_0; _OR_BYTE(prm_0x, prm_1); return prm_0x; }

UINT32 __fastcall JIT_AND_DWORD(UINT32 prm_0, UINT32 prm_1) { UINT32 prm_0x = prm_0; _AND_DWORD(prm_0x, prm_1); return prm_0x; }
UINT16 __fastcall  JIT_AND_WORD(UINT16 prm_0, UINT16 prm_1) { UINT16 prm_0x = prm_0; _AND_WORD(prm_0x, prm_1); return prm_0x; }
UINT8  __fastcall  JIT_AND_BYTE(UINT8  prm_0, UINT8  prm_1) { UINT8  prm_0x = prm_0; _AND_BYTE(prm_0x, prm_1); return prm_0x; }

UINT32 __fastcall JIT_XOR_DWORD(UINT32 prm_0, UINT32 prm_1) { UINT32 prm_0x = prm_0; _DWORD_XOR(prm_0x, prm_1); return prm_0x; }
UINT16 __fastcall  JIT_XOR_WORD(UINT16 prm_0, UINT16 prm_1) { UINT16 prm_0x = prm_0; _WORD_XOR(prm_0x, prm_1); return prm_0x; }
UINT8  __fastcall  JIT_XOR_BYTE(UINT8  prm_0, UINT8  prm_1) { UINT8  prm_0x = prm_0; _BYTE_XOR(prm_0x, prm_1); return prm_0x; }

UINT32 __fastcall JIT_CMP_DWORD(UINT32 prm_0, UINT32 prm_1) { UINT32 prm_0x = prm_0; _DWORD_SUB(prm_0x, prm_0, prm_1); return prm_0; }
UINT16 __fastcall  JIT_CMP_WORD(UINT16 prm_0, UINT16 prm_1) { UINT16 prm_0x = prm_0; _DWORD_SUB(prm_0x, prm_0, prm_1); return prm_0; }
UINT8  __fastcall  JIT_CMP_BYTE(UINT8  prm_0, UINT8  prm_1) { UINT8  prm_0x = prm_0; _DWORD_SUB(prm_0x, prm_0, prm_1); return prm_0; }

//PREPART_REG8_EA(op, src8, out8, 2, 5);
#ifdef _M_ARM64 || _M_ARM32

#define genarithcode(inst,instcode)\
case instcode+0:\
GET_PCBYTE(op);\
GEN_IMM(&instcache4gen, 0, (UINT64)(&CPU_EIP));\
GEN_LDR_MEM(&instcache4gen, 1, 0, 2);\
GEN_ADD_IMM8(&instcache4gen, 1, 1);\
GEN_STR_MEM(&instcache4gen, 1, 0, 2);\
if ((op) >= 0xc0) {\
	GEN_IMM(&instcache4gen,0,2);\
	GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(&(reg8_b20[op]))));\
	GEN_LDR_MEM(&instcache4gen, 0, 0, 0);\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(reg8_b53[op]))));\
	GEN_LDR_MEM(&instcache4gen, 1, 1, 0);\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(& JIT_##inst##_BYTE));\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(reg8_b20[op]))));\
	GEN_STR_MEM(&instcache4gen, 0, 1, 0);\
}\
else {\
	GEN_IMM(&instcache4gen,0,7);\
	GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(op)));\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(&calc_ea_dst));\
	GEN_MOV(&instcache4gen, 1, 0);\
	CPU_EIP_VAK=CPU_EIP;\
	calc_ea_dst(op);\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(CPU_INST_SEGREG_INDEX)));\
	GEN_IMM(&instcache4gen, 2, ((UINT64)(& JIT_##inst##_BYTE)));\
	GEN_IMM(&instcache4gen, 3, ((UINT64)(&(reg8_b53[op]))));\
	GEN_LDR_MEM(&instcache4gen, 3, 3, 0);\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(&cpu_vmemory_RMW_b));\
}\
break;\
case instcode+1:\
GET_PCBYTE(op);\
GEN_IMM(&instcache4gen, 0, (UINT64)(&CPU_EIP));\
GEN_LDR_MEM(&instcache4gen, 1, 0, 2);\
GEN_ADD_IMM8(&instcache4gen, 1, 1);\
GEN_STR_MEM(&instcache4gen, 1, 0, 2);\
if (CPU_INST_OP32){\
if ((op) >= 0xc0) {\
	GEN_IMM(&instcache4gen,0,2);\
	GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(&(reg32_b20[op]))));\
	GEN_LDR_MEM(&instcache4gen, 0, 0, 2);\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(reg32_b53[op]))));\
	GEN_LDR_MEM(&instcache4gen, 1, 1, 2);\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(& JIT_##inst##_DWORD));\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(reg32_b20[op]))));\
	GEN_STR_MEM(&instcache4gen, 0, 1, 2);\
}\
else {\
	GEN_IMM(&instcache4gen,0,7);\
	GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(op)));\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(&calc_ea_dst));\
	GEN_MOV(&instcache4gen, 1, 0);\
	CPU_EIP_VAK=CPU_EIP;\
	calc_ea_dst(op);\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(CPU_INST_SEGREG_INDEX)));\
	GEN_IMM(&instcache4gen, 2, ((UINT64)(& JIT_##inst##_DWORD)));\
	GEN_IMM(&instcache4gen, 3, ((UINT64)(&(reg32_b53[op]))));\
	GEN_LDR_MEM(&instcache4gen, 3, 3, 2);\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(&cpu_vmemory_RMW_d));\
}\
}else{\
if ((op) >= 0xc0) {\
	GEN_IMM(&instcache4gen,0,2);\
	GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(&(reg16_b20[op]))));\
	GEN_LDR_MEM(&instcache4gen, 0, 0, 1);\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(reg16_b53[op]))));\
	GEN_LDR_MEM(&instcache4gen, 1, 1, 1);\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(& JIT_##inst##_WORD));\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(reg16_b20[op]))));\
	GEN_STR_MEM(&instcache4gen, 0, 1, 1);\
}\
else {\
	GEN_IMM(&instcache4gen,0,7);\
	GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(op)));\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(&calc_ea_dst));\
	GEN_MOV(&instcache4gen, 1, 0);\
	CPU_EIP_VAK=CPU_EIP;\
	calc_ea_dst(op);\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(CPU_INST_SEGREG_INDEX)));\
	GEN_IMM(&instcache4gen, 2, ((UINT64)(& JIT_##inst##_WORD)));\
	GEN_IMM(&instcache4gen, 3, ((UINT64)(&(reg16_b53[op]))));\
	GEN_LDR_MEM(&instcache4gen, 3, 3, 1);\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(&cpu_vmemory_RMW_w));\
}\
}\
break;\
case instcode+2:\
GET_PCBYTE(op);\
GEN_IMM(&instcache4gen, 0, (UINT64)(&CPU_EIP));\
GEN_LDR_MEM(&instcache4gen, 1, 0, 2);\
GEN_ADD_IMM8(&instcache4gen, 1, 1);\
GEN_STR_MEM(&instcache4gen, 1, 0, 2);\
if ((op) >= 0xc0) {\
	GEN_IMM(&instcache4gen,0,2);\
	GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(&(reg8_b53[op]))));\
	GEN_LDR_MEM(&instcache4gen, 0, 0, 0);\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(reg8_b20[op]))));\
	GEN_LDR_MEM(&instcache4gen, 1, 1, 0);\
}\
else {\
	GEN_IMM(&instcache4gen,0,7);\
	GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(&calc_ea_dst));\
	GEN_MOV(&instcache4gen, 1, 0);\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(CPU_INST_SEGREG_INDEX)));\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(&cpu_vmemoryread_b));\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(op)));\
	CPU_EIP_VAK=CPU_EIP;\
	calc_ea_dst(op);\
}\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(& JIT_##inst##_BYTE));\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(reg8_b53[op]))));\
	GEN_STR_MEM(&instcache4gen, 0, 1, 0);\
break;\
case instcode+3:\
GET_PCBYTE(op);\
GEN_IMM(&instcache4gen, 0, (UINT64)(&CPU_EIP));\
GEN_LDR_MEM(&instcache4gen, 1, 0, 2);\
GEN_ADD_IMM8(&instcache4gen, 1, 1);\
GEN_STR_MEM(&instcache4gen, 1, 0, 2);\
if (CPU_INST_OP32){\
if ((op) >= 0xc0) {\
	GEN_IMM(&instcache4gen,0,2);\
	GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(&(reg32_b53[op]))));\
	GEN_LDR_MEM(&instcache4gen, 0, 0, 2);\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(reg32_b20[op]))));\
	GEN_LDR_MEM(&instcache4gen, 1, 1, 2);\
}\
else {\
	GEN_IMM(&instcache4gen,0,7);\
	GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(&calc_ea_dst));\
	GEN_MOV(&instcache4gen, 1, 0);\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(CPU_INST_SEGREG_INDEX)));\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(&cpu_vmemoryread_d));\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(op)));\
	CPU_EIP_VAK=CPU_EIP;\
	calc_ea_dst(op);\
}\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(& JIT_##inst##_DWORD));\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(reg32_b53[op]))));\
	GEN_STR_MEM(&instcache4gen, 0, 1, 2);\
}else{\
if ((op) >= 0xc0) {\
	GEN_IMM(&instcache4gen,0,2);\
	GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(&(reg16_b53[op]))));\
	GEN_LDR_MEM(&instcache4gen, 0, 0, 1);\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(reg16_b20[op]))));\
	GEN_LDR_MEM(&instcache4gen, 1, 1, 1);\
}\
else {\
	GEN_IMM(&instcache4gen,0,7);\
	GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(&calc_ea_dst));\
	GEN_MOV(&instcache4gen, 1, 0);\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(CPU_INST_SEGREG_INDEX)));\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(&cpu_vmemoryread_w));\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(op)));\
	CPU_EIP_VAK=CPU_EIP;\
	calc_ea_dst(op);\
}\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(& JIT_##inst##_WORD));\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(reg16_b53[op]))));\
	GEN_STR_MEM(&instcache4gen, 0, 1, 1);\
}\
break;\
case instcode+4:\
GET_PCBYTE(op);\
GEN_IMM(&instcache4gen,0,2);\
GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
GEN_IMM(&instcache4gen, 0, (UINT64)(&CPU_EIP));\
GEN_LDR_MEM(&instcache4gen, 1, 0, 2);\
GEN_ADD_IMM8(&instcache4gen, 1, 1);\
GEN_STR_MEM(&instcache4gen, 1, 0, 2);\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(&(CPU_AL))));\
	GEN_LDR_MEM(&instcache4gen, 0, 0, 0);\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(op)));\
	GEN_LDR_MEM(&instcache4gen, 1, 1, 0);\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(& JIT_##inst##_BYTE));\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(CPU_AL))));\
	GEN_STR_MEM(&instcache4gen, 0, 1, 0);\
break;\
case instcode+5:\
GEN_IMM(&instcache4gen,0,2);\
GEN_CALL_ADDR(&instcache4gen, ((UINT64)(&(JIT_CPU_COUNT))));\
if (CPU_INST_OP32){\
GET_PCDWORD(op);\
GEN_IMM(&instcache4gen, 0, (UINT64)(&CPU_EIP));\
GEN_LDR_MEM(&instcache4gen, 1, 0, 2);\
GEN_ADD_IMM8(&instcache4gen, 1, 5);\
GEN_STR_MEM(&instcache4gen, 1, 0, 2);\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(&(CPU_EAX))));\
	GEN_LDR_MEM(&instcache4gen, 0, 0, 2);\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(op)));\
	GEN_LDR_MEM(&instcache4gen, 1, 1, 2);\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(& JIT_##inst##_DWORD));\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(CPU_EAX))));\
	GEN_STR_MEM(&instcache4gen, 0, 1, 2);\
}else{\
GET_PCWORD(op);\
GEN_IMM(&instcache4gen, 0, (UINT64)(&CPU_EIP));\
GEN_LDR_MEM(&instcache4gen, 1, 0, 2);\
GEN_ADD_IMM8(&instcache4gen, 1, 3);\
GEN_STR_MEM(&instcache4gen, 1, 0, 2);\
	GEN_IMM(&instcache4gen, 0, ((UINT64)(&(CPU_AX))));\
	GEN_LDR_MEM(&instcache4gen, 0, 0, 1);\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(op)));\
	GEN_LDR_MEM(&instcache4gen, 1, 1, 1);\
	GEN_CALL_ADDR(&instcache4gen, (UINT64)(& JIT_##inst##_WORD));\
	GEN_IMM(&instcache4gen, 1, ((UINT64)(&(CPU_AX))));\
	GEN_STR_MEM(&instcache4gen, 0, 1, 1);\
}\
break;

#else
#endif


void GEN_RET(void* codetmp) {
#ifdef _M_ARM64
	* (UINT32*)(*(UINT64*)(codetmp)) = 0xd65f03c0;
	*(UINT64*)(codetmp) += 4;
#else
#ifdef _M_ARM32
	* (UINT16*)(*(UINT32*)(codetmp)) = 0x46f7;
	*(UINT32*)(codetmp) += 2;
#else
#ifdef _M_AMD64
	* (UINT8*)(*(UINT64*)(codetmp)) = 0xc3;
	*(UINT64*)(codetmp) += 1;
#else
	* (UINT8*)(*(UINT32*)(codetmp)) = 0xc3;
	*(UINT32*)(codetmp) += 1;
#endif
#endif
#endif
}
void GEN_NOP(void* codetmp) {
#ifdef _M_ARM64
	* (UINT32*)(*(UINT64*)(codetmp)) = 0xd503201f;
	*(UINT64*)(codetmp) += 4;
#else
#ifdef _M_ARM32
	* (UINT16*)(*(UINT32*)(codetmp)) = 0xbf00;
	*(UINT32*)(codetmp) += 2;
#else
#ifdef _M_AMD64
	* (UINT8*)(*(UINT64*)(codetmp)) = 0x90;
	*(UINT64*)(codetmp) += 1;
#else
	* (UINT8*)(*(UINT32*)(codetmp)) = 0x90;
	*(UINT32*)(codetmp) += 1;
#endif
#endif
#endif
}
void GEN_IMM(void* codetmp,UINT8 regid,UINT64 imm64) {
#ifdef _M_ARM64
	if (((imm64 >> (16 * 0)) & 0xFFFF) == 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xd2800000 | (((regid) & 0x1F) << 0)); *(UINT64*)(codetmp) += 4; }
	if (((imm64 >> (16 * 0)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xd2800000 | (((regid) & 0x1F) << 0) | (((imm64 >> (16 * 0)) & 0xFFFF) << 5)); *(UINT64*)(codetmp) += 4; }
	if (((imm64 >> (16 * 1)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xf2a00000 | (((regid) & 0x1F) << 0) | (((imm64 >> (16 * 1)) & 0xFFFF) << 5)); *(UINT64*)(codetmp) += 4; }
	if (((imm64 >> (16 * 2)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xf2c00000 | (((regid) & 0x1F) << 0) | (((imm64 >> (16 * 2)) & 0xFFFF) << 5)); *(UINT64*)(codetmp) += 4; }
	if (((imm64 >> (16 * 3)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xf2e00000 | (((regid) & 0x1F) << 0) | (((imm64 >> (16 * 3)) & 0xFFFF) << 5)); *(UINT64*)(codetmp) += 4; }
#else
#ifdef _M_ARM32
	if (((imm64 >> (16 * 0)) & 0xFFFF) == 0) { *(UINT32*)(*(UINT32*)(codetmp)) = (0x0000F04F | (((regid + 0) & 0xF) << 24)); *(UINT32*)(codetmp) += 4; }
	if (((imm64 >> (16 * 0)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT32*)(codetmp)) = (0x0000F240 | (((regid + 0) & 0xF) << 24) | ((((imm64 >> (16 * 0)) >> (4 * 0)) & 0xFF) << (16)) | ((((imm64 >> (16 * 0)) >> (4 * 2)) & 0xF) << (28)) | ((((imm64 >> (16 * 0)) >> (4 * 3)) & 0xF) << (0))); *(UINT32*)(codetmp) += 4; }
	if (((imm64 >> (16 * 1)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT32*)(codetmp)) = (0x0000F2C0 | (((regid + 0) & 0xF) << 24) | ((((imm64 >> (16 * 1)) >> (4 * 0)) & 0xFF) << (16)) | ((((imm64 >> (16 * 1)) >> (4 * 2)) & 0xF) << (28)) | ((((imm64 >> (16 * 1)) >> (4 * 3)) & 0xF) << (0))); *(UINT32*)(codetmp) += 4; }
	if (((imm64 >> (16 * 2)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT32*)(codetmp)) = (0x0000F240 | (((regid + 1) & 0xF) << 24) | ((((imm64 >> (16 * 2)) >> (4 * 0)) & 0xFF) << (16)) | ((((imm64 >> (16 * 2)) >> (4 * 2)) & 0xF) << (28)) | ((((imm64 >> (16 * 2)) >> (4 * 3)) & 0xF) << (0))); *(UINT32*)(codetmp) += 4; }
	if (((imm64 >> (16 * 3)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT32*)(codetmp)) = (0x0000F2C0 | (((regid + 1) & 0xF) << 24) | ((((imm64 >> (16 * 3)) >> (4 * 0)) & 0xFF) << (16)) | ((((imm64 >> (16 * 3)) >> (4 * 2)) & 0xF) << (28)) | ((((imm64 >> (16 * 3)) >> (4 * 3)) & 0xF) << (0))); *(UINT32*)(codetmp) += 4; }
#else
#ifdef _M_AMD64
	* (UINT8*)(*(UINT64*)(codetmp)) = 0x48 | ((regid >> 3) & 1); *(UINT64*)(codetmp) += 1;
	* (UINT8*)(*(UINT64*)(codetmp)) = 0xb8 + (regid & 7); *(UINT64*)(codetmp) += 1;
	* (UINT64*)(*(UINT64*)(codetmp)) = imm64; *(UINT64*)(codetmp) += 8;
#else
	* (UINT8*)(*(UINT32*)(codetmp)) = 0xb8 + (regid & 7); *(UINT32*)(codetmp) += 1;
	*(UINT32*)(*(UINT32*)(codetmp)) = imm64; *(UINT32*)(codetmp) += 4;
#endif
#endif
#endif
}
void GEN_CALL(void* codetmp, UINT8 regid) {
#ifdef _M_ARM64
	* (UINT32*)(*(UINT64*)(codetmp)) = (0xa9bf7be0 | ((regid & 0x1f) << 0));
	*(UINT64*)(codetmp) += 4;
	* (UINT32*)(*(UINT64*)(codetmp)) = (0xd63f0000 | ((regid & 0x1f) << 5));
	*(UINT64*)(codetmp) += 4;
	* (UINT32*)(*(UINT64*)(codetmp)) = (0xa8c17be0 | ((regid & 0x1f) << 0));
	*(UINT64*)(codetmp) += 4;
#else
#ifdef _M_ARM32
	* (UINT32*)(*(UINT32*)(codetmp)) = 0xe92d4000;
	*(UINT32*)(codetmp) += 4;
	* (UINT16*)(*(UINT32*)(codetmp)) = (0x4780 | ((regid & 0xf) << 3));
	codetmp += 2;
	* (UINT32*)(*(UINT32*)(codetmp)) = 0xe8bd4000;
	*(UINT32*)(codetmp) += 4;
#else
#ifdef _M_AMD64
	* (UINT8*)(*(UINT64*)(codetmp)) = 0x48 | (((regid >> 3) & 1) << 2); *(UINT64*)(codetmp) += 1;
	* (UINT8*)(*(UINT64*)(codetmp)) = 0x89; *(UINT64*)(codetmp) += 1;
	* (UINT8*)(*(UINT64*)(codetmp)) = 0xc0 | ((regid & 7) << 3); *(UINT64*)(codetmp) += 1;
	* (UINT8*)(*(UINT64*)(codetmp)) = 0xff; *(UINT64*)(codetmp) += 1;
	* (UINT8*)(*(UINT64*)(codetmp)) = 0x10; *(UINT64*)(codetmp) += 1;
#else
	* (UINT8*)(*(UINT32*)(codetmp)) = 0x89; *(UINT32*)(codetmp) += 1;
	*(UINT8*)(*(UINT32*)(codetmp)) = 0xc0 | ((regid & 7) << 3); *(UINT32*)(codetmp) += 1;
	*(UINT8*)(*(UINT32*)(codetmp)) = 0xff; *(UINT32*)(codetmp) += 1;
	*(UINT8*)(*(UINT32*)(codetmp)) = 0x10; *(UINT32*)(codetmp) += 1;
#endif
#endif
#endif
}
void GEN_JMP(void* codetmp, UINT8 regid) {
#ifdef _M_ARM64
	* (UINT32*)(*(UINT64*)(codetmp)) = (0xd61f0000 | ((regid & 0x1f) << 5));
	*(UINT64*)(codetmp) += 4;
#else
#ifdef _M_ARM32
	* (UINT16*)(*(UINT32*)(codetmp)) = (0x4700 | ((regid & 0xf) << 3));
	*(UINT32*)(codetmp) += 2;
#else
#ifdef _M_AMD64
	* (UINT8*)(*(UINT64*)(codetmp)) = 0x48 | (((regid >> 3) & 1) << 2); *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0x89; *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xc0 | ((regid & 7) << 3); *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xff; *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0x20; *(UINT64*)(codetmp) += 1;
#else
	* (UINT8*)(*(UINT32*)(codetmp)) = 0x89; *(UINT32*)(codetmp) += 1;
	*(UINT8*)(*(UINT32*)(codetmp)) = 0xc0 | ((regid & 7) << 3); *(UINT32*)(codetmp) += 1;
	*(UINT8*)(*(UINT32*)(codetmp)) = 0xff; *(UINT32*)(codetmp) += 1;
	*(UINT8*)(*(UINT32*)(codetmp)) = 0x20; *(UINT32*)(codetmp) += 1;
#endif
#endif
#endif
}

void GEN_CALL_ADDR(void* codetmp, UINT64 imm64) {
#ifdef _M_ARM64
	* (UINT32*)(*(UINT64*)(codetmp)) = (0xa9bf7be0 | ((9 & 0x1f) << 0));
	*(UINT64*)(codetmp) += 4;
	if (((imm64 >> (16 * 0)) & 0xFFFF) == 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xd2800000 | (((9) & 0x1F) << 0)); *(UINT64*)(codetmp) += 4; }
	if (((imm64 >> (16 * 0)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xd2800000 | (((9) & 0x1F) << 0) | (((imm64 >> (16 * 0)) & 0xFFFF) << 5)); *(UINT64*)(codetmp) += 4; }
	if (((imm64 >> (16 * 1)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xf2a00000 | (((9) & 0x1F) << 0) | (((imm64 >> (16 * 1)) & 0xFFFF) << 5)); *(UINT64*)(codetmp) += 4; }
	if (((imm64 >> (16 * 2)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xf2c00000 | (((9) & 0x1F) << 0) | (((imm64 >> (16 * 2)) & 0xFFFF) << 5)); *(UINT64*)(codetmp) += 4; }
	if (((imm64 >> (16 * 3)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xf2e00000 | (((9) & 0x1F) << 0) | (((imm64 >> (16 * 3)) & 0xFFFF) << 5)); *(UINT64*)(codetmp) += 4; }
	*(UINT32*)(*(UINT64*)(codetmp)) = (0xd63f0000 | ((9 & 0x1f) << 5));
	*(UINT64*)(codetmp) += 4;
	*(UINT32*)(*(UINT64*)(codetmp)) = (0xa8c17be0 | ((9 & 0x1f) << 0));
	*(UINT64*)(codetmp) += 4;
#else
#ifdef _M_ARM32
	* (UINT32*)(*(UINT32*)(codetmp)) = 0xe92d4000;
	*(UINT32*)(codetmp) += 4;
	if (((imm64 >> (16 * 0)) & 0xFFFF) == 0) { *(UINT32*)(*(UINT32*)(codetmp)) = (0x0000F04F | (((3 + 0) & 0xF) << 24)); *(UINT32*)(codetmp) += 4; }
	if (((imm64 >> (16 * 0)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT32*)(codetmp)) = (0x0000F240 | (((3 + 0) & 0xF) << 24) | ((((imm64 >> (16 * 0)) >> (4 * 0)) & 0xFF) << (16)) | ((((imm64 >> (16 * 0)) >> (4 * 2)) & 0xF) << (28)) | ((((imm64 >> (16 * 0)) >> (4 * 3)) & 0xF) << (0))); *(UINT32*)(codetmp) += 4; }
	if (((imm64 >> (16 * 1)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT32*)(codetmp)) = (0x0000F2C0 | (((3 + 0) & 0xF) << 24) | ((((imm64 >> (16 * 1)) >> (4 * 0)) & 0xFF) << (16)) | ((((imm64 >> (16 * 1)) >> (4 * 2)) & 0xF) << (28)) | ((((imm64 >> (16 * 1)) >> (4 * 3)) & 0xF) << (0))); *(UINT32*)(codetmp) += 4; }
	*(UINT16*)(*(UINT32*)(codetmp)) = (0x4780 | ((3 & 0xf) << 3));
	codetmp += 2;
	*(UINT32*)(*(UINT32*)(codetmp)) = 0xe8bd4000;
	*(UINT32*)(codetmp) += 4;
#else
#ifdef _M_AMD64
	* (UINT8*)(*(UINT64*)(codetmp)) = 0x48; *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xb8; *(UINT64*)(codetmp) += 1;
	*(UINT64*)(*(UINT64*)(codetmp)) = imm64; *(UINT64*)(codetmp) += 8;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xff; *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xd0; *(UINT64*)(codetmp) += 1;
#else
	* (UINT8*)(*(UINT64*)(codetmp)) = 0xb8; *(UINT64*)(codetmp) += 1;
	*(UINT32*)(*(UINT64*)(codetmp)) = imm64; *(UINT64*)(codetmp) += 4;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xff; *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xd0; *(UINT64*)(codetmp) += 1;
#endif
#endif
#endif
}
void GEN_JMP_ADDR(void* codetmp, UINT64 imm64) {
#ifdef _M_ARM64
	if (((imm64 >> (16 * 0)) & 0xFFFF) == 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xd2800000 | (((9) & 0x1F) << 0)); *(UINT64*)(codetmp) += 4; }
	if (((imm64 >> (16 * 0)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xd2800000 | (((9) & 0x1F) << 0) | (((imm64 >> (16 * 0)) & 0xFFFF) << 5)); *(UINT64*)(codetmp) += 4; }
	if (((imm64 >> (16 * 1)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xf2a00000 | (((9) & 0x1F) << 0) | (((imm64 >> (16 * 1)) & 0xFFFF) << 5)); *(UINT64*)(codetmp) += 4; }
	if (((imm64 >> (16 * 2)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xf2c00000 | (((9) & 0x1F) << 0) | (((imm64 >> (16 * 2)) & 0xFFFF) << 5)); *(UINT64*)(codetmp) += 4; }
	if (((imm64 >> (16 * 3)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT64*)(codetmp)) = (0xf2e00000 | (((9) & 0x1F) << 0) | (((imm64 >> (16 * 3)) & 0xFFFF) << 5)); *(UINT64*)(codetmp) += 4; }
	* (UINT32*)(*(UINT64*)(codetmp)) = (0xd61f0000 | ((9 & 0x1f) << 5));
	*(UINT64*)(codetmp) += 4;
#else
#ifdef _M_ARM32
	if (((imm64 >> (16 * 0)) & 0xFFFF) == 0) { *(UINT32*)(*(UINT32*)(codetmp)) = (0x0000F04F | (((3 + 0) & 0xF) << 24)); *(UINT32*)(codetmp) += 4; }
	if (((imm64 >> (16 * 0)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT32*)(codetmp)) = (0x0000F240 | (((3 + 0) & 0xF) << 24) | ((((imm64 >> (16 * 0)) >> (4 * 0)) & 0xFF) << (16)) | ((((imm64 >> (16 * 0)) >> (4 * 2)) & 0xF) << (28)) | ((((imm64 >> (16 * 0)) >> (4 * 3)) & 0xF) << (0))); *(UINT32*)(codetmp) += 4; }
	if (((imm64 >> (16 * 1)) & 0xFFFF) != 0) { *(UINT32*)(*(UINT32*)(codetmp)) = (0x0000F2C0 | (((3 + 0) & 0xF) << 24) | ((((imm64 >> (16 * 1)) >> (4 * 0)) & 0xFF) << (16)) | ((((imm64 >> (16 * 1)) >> (4 * 2)) & 0xF) << (28)) | ((((imm64 >> (16 * 1)) >> (4 * 3)) & 0xF) << (0))); *(UINT32*)(codetmp) += 4; }
	* (UINT16*)(*(UINT32*)(codetmp)) = (0x4700 | ((3 & 0xf) << 3));
	*(UINT32*)(codetmp) += 2;
#else
#ifdef _M_AMD64
	* (UINT8*)(*(UINT64*)(codetmp)) = 0x48; *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xb8; *(UINT64*)(codetmp) += 1;
	*(UINT64*)(*(UINT64*)(codetmp)) = imm64; *(UINT64*)(codetmp) += 8;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xff; *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xe0; *(UINT64*)(codetmp) += 1;
#else
	* (UINT8*)(*(UINT64*)(codetmp)) = 0xb8; *(UINT64*)(codetmp) += 1;
	*(UINT32*)(*(UINT64*)(codetmp)) = imm64; *(UINT64*)(codetmp) += 4;
	*(UINT8*)(*(UINT32*)(codetmp)) = 0xff; *(UINT32*)(codetmp) += 1;
	*(UINT8*)(*(UINT32*)(codetmp)) = 0xe0; *(UINT32*)(codetmp) += 1;
#endif
#endif
#endif
}

void GEN_LDR_MEM(void* codetmp, UINT8 regid, UINT8 regid2, UINT8 size) {
#ifdef _M_ARM64
	* (UINT32*)(*(UINT64*)(codetmp)) = (0x39400000 | ((size & 3) << 30) | ((regid2 & 0x1f) << 5) | ((regid & 0x1f) << 0));
	*(UINT64*)(codetmp) += 4;
#else
#ifdef _M_ARM32
	switch (size & 3) {
	case 0:
		*(UINT32*)(*(UINT32*)(codetmp)) = (0x0000f890 | ((regid2 & 0xf) << 4) | ((regid & 0xf) << 28));
		break;
	case 1:
		*(UINT32*)(*(UINT32*)(codetmp)) = (0x0000f8b0 | ((regid2 & 0xf) << 4) | ((regid & 0xf) << 28));
		break;
	case 2:
		*(UINT32*)(*(UINT32*)(codetmp)) = (0x0000f8d0 | ((regid2 & 0xf) << 4) | ((regid & 0xf) << 28));
		break;
	case 3:
		*(UINT32*)(*(UINT32*)(codetmp)) = (0x0000f8d0 | ((regid2 & 0xf) << 4) | ((regid & 0xf) << 28));
		*(UINT32*)(codetmp) += 4;
		*(UINT32*)(*(UINT32*)(codetmp)) = (0x0004f8d0 | ((regid2 & 0xf) << 4) | (((regid + 1) & 0xf) << 28));
		break;
	}
	*(UINT32*)(codetmp) += 4;
#else
#ifdef _M_AMD64
	if (size == 1) {
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x66; *(UINT64*)(codetmp) += 1;
	}
	if ((((regid >> 3) & 1)) || (size == 0 ? ((regid2 >> 2) & 1) : ((regid2 >> 3) & 1))) {
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x40 | (((regid >> 3) & 1) << 0) | (((regid2 >> 3) & 1) << 3) | (size == 3 ? 8 : 0) | (size == 0 ? (((regid2 >> 2) & 1) << 2) : 0); *(UINT64*)(codetmp) += 1;
	}
	switch (size & 3) {
	case 0:
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x8a; *(UINT64*)(codetmp) += 1;
		*(UINT8*)(*(UINT64*)(codetmp)) = ((regid & 7) << 0) | ((regid2 & 3) << 3); *(UINT64*)(codetmp) += 1;
		break;
	case 1:
	case 2:
	case 3:
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x8b; *(UINT64*)(codetmp) += 1;
		*(UINT8*)(*(UINT64*)(codetmp)) = ((regid & 7) << 0) | ((regid2 & 7) << 3); *(UINT64*)(codetmp) += 1;
		break;
	}
#else
	if (size == 1) {
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x66; *(UINT64*)(codetmp) += 1;
	}
	switch (size & 3) {
	case 0:
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x8a; *(UINT64*)(codetmp) += 1;
		*(UINT8*)(*(UINT64*)(codetmp)) = ((regid & 7) << 0) | ((regid2 & 3) << 3); *(UINT64*)(codetmp) += 1;
		break;
	case 1:
	case 2:
	case 3:
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x8b; *(UINT64*)(codetmp) += 1;
		*(UINT8*)(*(UINT64*)(codetmp)) = ((regid & 7) << 0) | ((regid2 & 7) << 3); *(UINT64*)(codetmp) += 1;
		break;
	}
#endif
#endif
#endif
}

void GEN_STR_MEM(void* codetmp, UINT8 regid, UINT8 regid2, UINT8 size) {
#ifdef _M_ARM64
	* (UINT32*)(*(UINT64*)(codetmp)) = (0x39000000 | ((size & 3) << 30) | ((regid2 & 0x1f) << 5) | ((regid & 0x1f) << 0));
	*(UINT64*)(codetmp) += 4;
#else
#ifdef _M_ARM32
	switch (size & 3) {
	case 0:
		*(UINT32*)(*(UINT32*)(codetmp)) = (0x0000f880 | ((regid2 & 0xf) << 4) | ((regid & 0xf) << 28));
		break;
	case 1:
		*(UINT32*)(*(UINT32*)(codetmp)) = (0x0000f8a0 | ((regid2 & 0xf) << 4) | ((regid & 0xf) << 28));
		break;
	case 2:
		*(UINT32*)(*(UINT32*)(codetmp)) = (0x0000f8c0 | ((regid2 & 0xf) << 4) | ((regid & 0xf) << 28));
		break;
	case 3:
		*(UINT32*)(*(UINT32*)(codetmp)) = (0x0000f8c0 | ((regid2 & 0xf) << 4) | ((regid & 0xf) << 28));
		*(UINT32*)(codetmp) += 4;
		*(UINT32*)(*(UINT32*)(codetmp)) = (0x0004f8c0 | ((regid2 & 0xf) << 4) | (((regid + 1) & 0xf) << 28));
		break;
	}
	*(UINT32*)(codetmp) += 4;
#else
#ifdef _M_AMD64
	if (size == 1) {
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x66; *(UINT64*)(codetmp) += 1;
	}
	if ((((regid >> 3) & 1)) || (size == 0 ? ((regid2 >> 2) & 1) : ((regid2 >> 3) & 1))) {
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x40 | (((regid >> 3) & 1) << 0) | (((regid2 >> 3) & 1) << 3) | (size == 3 ? 8 : 0) | (size == 0 ? (((regid2 >> 2) & 1) << 2) : 0); *(UINT64*)(codetmp) += 1;
	}
	switch (size & 3) {
	case 0:
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x88; *(UINT64*)(codetmp) += 1;
		*(UINT8*)(*(UINT64*)(codetmp)) = ((regid & 7) << 0) | ((regid2 & 3) << 3); *(UINT64*)(codetmp) += 1;
		break;
	case 1:
	case 2:
	case 3:
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x89; *(UINT64*)(codetmp) += 1;
		*(UINT8*)(*(UINT64*)(codetmp)) = ((regid & 7) << 0) | ((regid2 & 7) << 3); *(UINT64*)(codetmp) += 1;
		break;
	}
#else
	if (size == 1) {
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x66; *(UINT64*)(codetmp) += 1;
	}
	switch (size & 3) {
	case 0:
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x88; *(UINT64*)(codetmp) += 1;
		*(UINT8*)(*(UINT64*)(codetmp)) = ((regid & 7) << 0) | ((regid2 & 3) << 3); *(UINT64*)(codetmp) += 1;
		break;
	case 1:
	case 2:
	case 3:
		*(UINT8*)(*(UINT64*)(codetmp)) = 0x89; *(UINT64*)(codetmp) += 1;
		*(UINT8*)(*(UINT64*)(codetmp)) = ((regid & 7) << 0) | ((regid2 & 7) << 3); *(UINT64*)(codetmp) += 1;
		break;
	}
#endif
#endif
#endif
}

void GEN_MOV(void* codetmp, UINT8 regid, UINT8 regid2) {
#ifdef _M_ARM64
	* (UINT32*)(*(UINT64*)(codetmp)) = (0xaa0003e0 | ((regid2 & 0x1f) << 24) | ((regid & 0x1f) << 0));
	*(UINT64*)(codetmp) += 4;
#else
#ifdef _M_ARM32
	* (UINT16*)(*(UINT32*)(codetmp)) = (0x4600 | ((regid2 & 0xf) << 3) | ((regid & 0x7) << 0) | ((regid & 0x8) << 4));
	*(UINT32*)(codetmp) += 2;
#else
#ifdef _M_AMD64
	*(UINT8*)(*(UINT64*)(codetmp)) = 0x48 | (((regid >> 3) & 1) << 0) | (((regid2 >> 3) & 1) << 2); *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0x89; *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xc0 | ((regid & 7) << 0) | ((regid2 & 7) << 3); *(UINT64*)(codetmp) += 1;
#else
	*(UINT8*)(*(UINT64*)(codetmp)) = 0x89; *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xc0 | ((regid & 7) << 0) | ((regid2 & 7) << 3); *(UINT64*)(codetmp) += 1;
#endif
#endif
#endif
}

void GEN_ADD_IMM8(void* codetmp, UINT8 regid, UINT8 imm8) {
#ifdef _M_ARM64
	* (UINT32*)(*(UINT64*)(codetmp)) = (0x91000000 | ((regid & 0x1f) << 5) | ((regid & 0x1f) << 0) | ((imm8 & 255) << 10));
	*(UINT64*)(codetmp) += 4;
#else
#ifdef _M_ARM32
	* (UINT32*)(*(UINT32*)(codetmp)) = (0x0000f100 | ((regid & 0xf) << 24) | ((regid & 0xf) << 0) | ((imm8 & 255) << 16));
	*(UINT32*)(codetmp) += 4;
#else
#ifdef _M_AMD64
	* (UINT8*)(*(UINT64*)(codetmp)) = 0x48 | (((regid >> 3) & 1) << 0); *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0x83; *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xc0 | ((regid & 7) << 0); *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = imm8; *(UINT64*)(codetmp) += 1;
#else
	* (UINT8*)(*(UINT64*)(codetmp)) = 0x83; *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = 0xc0 | ((regid & 7) << 0); *(UINT64*)(codetmp) += 1;
	*(UINT8*)(*(UINT64*)(codetmp)) = imm8; *(UINT64*)(codetmp) += 1;
#endif
#endif
#endif
}

void GEN_NE_RET(void* codetmp, UINT8 regid, UINT8 regid2, bool sizeofinst) {
#ifdef _M_ARM64
	* (UINT32*)(*(UINT64*)(codetmp)) = (0x6b000000 | ((regid2 & 0x1f) << 16) | ((regid & 0x1f) << 0) | ((sizeofinst ? 1 : 0) << 31));
	*(UINT64*)(codetmp) += 4;
	*(UINT32*)(*(UINT64*)(codetmp)) = 0x54000040;
	*(UINT64*)(codetmp) += 4;
	GEN_RET(codetmp);
#endif
}

void GEN_EQ_RET(void* codetmp, UINT8 regid, UINT8 regid2, bool sizeofinst) {
#ifdef _M_ARM64
	* (UINT32*)(*(UINT64*)(codetmp)) = (0x6b000000 | ((regid2 & 0x1f) << 16) | ((regid & 0x1f) << 0) | ((sizeofinst ? 1 : 0) << 31));
	*(UINT64*)(codetmp) += 4;
	*(UINT32*)(*(UINT64*)(codetmp)) = 0x54000041;
	*(UINT64*)(codetmp) += 4;
	GEN_RET(codetmp);
#endif
}

void GEN_CC_RET(void* codetmp, UINT8 regid, UINT8 regid2, bool sizeofinst, UINT8 cond) {
#ifdef _M_ARM64
	* (UINT32*)(*(UINT64*)(codetmp)) = (0x6b000000 | ((regid2 & 0x1f) << 16) | ((regid & 0x1f) << 0) | ((sizeofinst ? 1 : 0) << 31));
	*(UINT64*)(codetmp) += 4;
	*(UINT32*)(*(UINT64*)(codetmp)) = (0x54000040 | ((cond & 0xF) << 0));
	*(UINT64*)(codetmp) += 4;
	GEN_RET(codetmp);
#endif
}

void __fastcall addeip_addr(UINT32 prm_0) {
	CPU_EIP += prm_0;
	return;
}

void setfirstate4x86() {
	CPU_PREV_EIP = CPU_EIP;
	CPU_STATSAVE.cpu_inst = CPU_STATSAVE.cpu_inst_default;
}
void __fastcall gencpufaultinjit(int prm_0,int prm_1) {
	EXCEPTION(prm_0,prm_1);
	return;
}

UINT64 codefinishedptr;
UINT64 transedeip = 0;

UINT8* instcache;
UINT8* instcacheexec;
UINT8* instcache4gen;
UINT8* instcache4genfirst;
UINT8* instcache4genfirsttmp;
UINT8* instcache4genold;
UINT8* instcache4genret;
UINT64 cpueipbak;
UINT8 prefixamountx = 0;
UINT32 instcacheaddr[128];
UINT8 instcachex[128][4096];
UINT8 cachepos = 0;

UINT64 execinstcode(UINT32 op) {
	CPU_EIP++;
	if (insttable_info[op] & INST_PREFIX) {
		(*insttable_1byte[0][op])();
		prefixamountx++;
		if (prefixamountx == MAX_PREFIX) {
			EXCEPTION(UD_EXCEPTION, 0);
			prefixamountx = 0;
		}
		return CPU_REMCLOCK;
	}
	else {
		prefixamountx = 0;
		if (!(insttable_info[op] & INST_STRING) || !CPU_INST_REPUSE) {
			(*insttable_1byte[CPU_INST_OP32][op])();
		}
		else {
			/* rep */
			CPU_WORKCLOCK(5);
			if (!CPU_INST_AS32) {
				if (CPU_CX != 0) {
					if (!(insttable_info[op] & REP_CHECKZF)) {
						/* rep */
						if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
							(*insttable_1byte_repfunc[CPU_INST_OP32][op])(0);
						}
						else {
							for (;;) {
								(*insttable_1byte[CPU_INST_OP32][op])();
								if (--CPU_CX == 0) {
									break;
								}
								if (CPU_REMCLOCK <= 0) {
									CPU_EIP = CPU_PREV_EIP;
									break;
								}
							}
						}
					}
					else if (CPU_INST_REPUSE != 0xf2) {
						/* repe */
						if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
							(*insttable_1byte_repfunc[CPU_INST_OP32][op])(1);
						}
						else {
							for (;;) {
								(*insttable_1byte[CPU_INST_OP32][op])();
								if (--CPU_CX == 0 || CC_NZ) {
									break;
								}
								if (CPU_REMCLOCK <= 0) {
									CPU_EIP = CPU_PREV_EIP;
									break;
								}
							}
						}
					}
					else {
						/* repne */
						if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
							(*insttable_1byte_repfunc[CPU_INST_OP32][op])(2);
						}
						else {
							for (;;) {
								(*insttable_1byte[CPU_INST_OP32][op])();
								if (--CPU_CX == 0 || CC_Z) {
									break;
								}
								if (CPU_REMCLOCK <= 0) {
									CPU_EIP = CPU_PREV_EIP;
									break;
								}
							}
						}
					}
				}
			}
			else {
				if (CPU_ECX != 0) {
					if (!(insttable_info[op] & REP_CHECKZF)) {
						/* rep */
						if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
							(*insttable_1byte_repfunc[CPU_INST_OP32][op])(0);
						}
						else {
							for (;;) {
								(*insttable_1byte[CPU_INST_OP32][op])();
								if (--CPU_ECX == 0) {
									break;
								}
								if (CPU_REMCLOCK <= 0) {
									CPU_EIP = CPU_PREV_EIP;
									break;
								}
							}
						}
					}
					else if (CPU_INST_REPUSE != 0xf2) {
						/* repe */
						if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
							(*insttable_1byte_repfunc[CPU_INST_OP32][op])(1);
						}
						else {
							for (;;) {
								(*insttable_1byte[CPU_INST_OP32][op])();
								if (--CPU_ECX == 0 || CC_NZ) {
									break;
								}
								if (CPU_REMCLOCK <= 0) {
									CPU_EIP = CPU_PREV_EIP;
									break;
								}
							}
						}
					}
					else {
						/* repne */
						if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
							(*insttable_1byte_repfunc[CPU_INST_OP32][op])(2);
						}
						else {
							for (;;) {
								(*insttable_1byte[CPU_INST_OP32][op])();
								if (--CPU_ECX == 0 || CC_Z) {
									break;
								}
								if (CPU_REMCLOCK <= 0) {
									CPU_EIP = CPU_PREV_EIP;
									break;
								}
							}
						}
					}
				}
			}
		}
	}
	/*CPU_PREV_EIP = CPU_EIP;
	CPU_STATSAVE.cpu_inst = CPU_STATSAVE.cpu_inst_default;*/

	return CPU_REMCLOCK;
}

UINT64 __fastcall getptrforexec() {
	UINT64 tmpptx4jmp = (((UINT64)((void*)instcacheexec)) + 4096);
	if ((transedeip & 0xFFFFF000) == (CPU_EIP & 0xFFFFF000)) {
		for (int cnt = 0; cnt < ((UINT64)(CPU_EIP & 0xFFF)); cnt++) {
#ifdef _M_ARM64
			tmpptx4jmp += (((UINT64)instcache[cnt]) * 4);
#else
#ifdef _M_ARM32
			tmpptx4jmp += (((UINT64)instcache[cnt]) * 2);
#else
#ifdef _M_AMD64
			tmpptx4jmp += ((UINT64)instcache[cnt]);
#else
			tmpptx4jmp += ((UINT64)instcache[cnt]);
#endif
#endif
#endif
		}
#ifdef _M_ARM32
		return ((UINT64)tmpptx4jmp | 1);
#else
		return (UINT64)tmpptx4jmp;
#endif
	}
	else {
#ifdef _M_ARM32
		return ((UINT64)codefinishedptr | 1);
#else
		return (UINT64)codefinishedptr;
#endif
	}
}

void __fastcall repgenx_16(UINT32 op) {
	CPU_WORKCLOCK(5);
		(*insttable_1byte[CPU_INST_OP32][op])();
		--CPU_CX;
}
void __fastcall rep_16(UINT32 op) {
	CPU_WORKCLOCK(5);
	for (;;) {
		(*insttable_1byte[CPU_INST_OP32][op])();
		if (--CPU_CX == 0) {
			break;
		}
	}
}
void __fastcall repe_16(UINT32 op) {
	CPU_WORKCLOCK(5);
	for (;;) {
		(*insttable_1byte[CPU_INST_OP32][op])();
		if (--CPU_CX == 0 || CC_NZ) {
			break;
		}
	}
}
void __fastcall repne_16(UINT32 op) {
	CPU_WORKCLOCK(5);
	for (;;) {
		(*insttable_1byte[CPU_INST_OP32][op])();
		if (--CPU_CX == 0 || CC_Z) {
			break;
		}
	}
}

void __fastcall repgenx_32(UINT32 op) {
	CPU_WORKCLOCK(5);
	(*insttable_1byte[CPU_INST_OP32][op])();
	--CPU_ECX;
}
void __fastcall rep_32(UINT32 op) {
	CPU_WORKCLOCK(5);
	for (;;) {
		(*insttable_1byte[CPU_INST_OP32][op])();
		if (--CPU_ECX == 0) {
			break;
		}
	}
}
void __fastcall repe_32(UINT32 op) {
	CPU_WORKCLOCK(5);
	for (;;) {
		(*insttable_1byte[CPU_INST_OP32][op])();
		if (--CPU_ECX == 0 || CC_NZ) {
			break;
		}
	}
}
void __fastcall repne_32(UINT32 op) {
	CPU_WORKCLOCK(5);
	for (;;) {
		(*insttable_1byte[CPU_INST_OP32][op])();
		if (--CPU_ECX == 0 || CC_Z) {
			break;
		}
	}
}

UINT64 exec_allstepfast() {
	int prefix;
	UINT32 op;

	do {

		CPU_PREV_EIP = CPU_EIP;
		CPU_STATSAVE.cpu_inst = CPU_STATSAVE.cpu_inst_default;

		for (prefix = 0; prefix < MAX_PREFIX; prefix++) {
			GET_PCBYTE(op);

			/* prefix */
			if (insttable_info[op] & INST_PREFIX) {
				(*insttable_1byte[0][op])();
				continue;
			}
			break;
		}
		if (prefix == MAX_PREFIX) {
			EXCEPTION(UD_EXCEPTION, 0);
		}

		/* normal / rep, but not use */
		if (!(insttable_info[op] & INST_STRING) || !CPU_INST_REPUSE) {
			(*insttable_1byte[CPU_INST_OP32][op])();
			goto cpucontinue;
		}

		/* rep */
		CPU_WORKCLOCK(5);
		if (!CPU_INST_AS32) {
			if (CPU_CX != 0) {
				if (!(insttable_info[op] & REP_CHECKZF)) {
					/* rep */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(0);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_CX == 0) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
				else if (CPU_INST_REPUSE != 0xf2) {
					/* repe */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(1);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_CX == 0 || CC_NZ) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
				else {
					/* repne */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(2);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_CX == 0 || CC_Z) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
			}
		}
		else {
			if (CPU_ECX != 0) {
				if (!(insttable_info[op] & REP_CHECKZF)) {
					/* rep */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(0);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_ECX == 0) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
				else if (CPU_INST_REPUSE != 0xf2) {
					/* repe */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(1);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_ECX == 0 || CC_NZ) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
				else {
					/* repne */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(2);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_ECX == 0 || CC_Z) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
			}
		}
	cpucontinue:;
	} while (CPU_REMCLOCK > 0);
	return CPU_REMCLOCK;
}

UINT64 exec_allstepfast2() {
	int prefix;
	UINT32 op;

	UINT8 cacheid = 0;
	UINT32 transcoderem = 0;

	do {

		CPU_PREV_EIP = CPU_EIP;
		CPU_STATSAVE.cpu_inst = CPU_STATSAVE.cpu_inst_default;

		for (prefix = 0; prefix < MAX_PREFIX; prefix++) {
			GET_PCBYTE(op);

			/* prefix */
			if (insttable_info[op] & INST_PREFIX) {
				(*insttable_1byte[0][op])();
				continue;
			}
			break;
		}
		if (prefix == MAX_PREFIX) {
			EXCEPTION(UD_EXCEPTION, 0);
		}

		/* normal / rep, but not use */
		if (!(insttable_info[op] & INST_STRING) || !CPU_INST_REPUSE) {
			if (op == 0x0F) {
				GET_PCBYTE(op);
#ifdef USE_SSE
					if (insttable_2byte660F_32[op] && CPU_INST_OP32 == !CPU_STATSAVE.cpu_inst_default.op_32) {
						(*insttable_2byte660F_32[op])();
					}
					else if (insttable_2byteF20F_32[op] && CPU_INST_REPUSE == 0xf2) {
						(*insttable_2byteF20F_32[op])();
					}
					else if (insttable_2byteF30F_32[op] && CPU_INST_REPUSE == 0xf3) {
						(*insttable_2byteF30F_32[op])();
					}
					else {
						(*insttable_2byte[CPU_INST_AS32][op])();
					}
#else
					(*insttable_2byte[CPU_INST_AS32][op])();
#endif
			}
			else {
				(*insttable_1byte[CPU_INST_OP32][op])();
			}
			goto cpucontinue;
		}

		/* rep */
		CPU_WORKCLOCK(5);
		if (!CPU_INST_AS32) {
			if (CPU_CX != 0) {
				if (!(insttable_info[op] & REP_CHECKZF)) {
					/* rep */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(0);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_CX == 0) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
				else if (CPU_INST_REPUSE != 0xf2) {
					/* repe */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(1);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_CX == 0 || CC_NZ) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
				else {
					/* repne */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(2);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_CX == 0 || CC_Z) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
			}
		}
		else {
			if (CPU_ECX != 0) {
				if (!(insttable_info[op] & REP_CHECKZF)) {
					/* rep */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(0);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_ECX == 0) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
				else if (CPU_INST_REPUSE != 0xf2) {
					/* repe */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(1);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_ECX == 0 || CC_NZ) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
				else {
					/* repne */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(2);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_ECX == 0 || CC_Z) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
			}
		}
	cpucontinue:;
	} while (CPU_REMCLOCK > 0);
	return CPU_REMCLOCK;
}

UINT64 exec_1stepfast() {
	exec_1step();
	return CPU_REMCLOCK;
}

void* getopcodeptr() {
	int prefix;
	UINT32 op;

	CPU_PREV_EIP = CPU_EIP;
	CPU_STATSAVE.cpu_inst = CPU_STATSAVE.cpu_inst_default;
	for (prefix = 0; prefix < MAX_PREFIX; prefix++) {
		GET_PCBYTE(op);
		/* prefix */
		if (insttable_info[op] & INST_PREFIX) {
			(*insttable_1byte[0][op])();
			continue;
		}
		break;
	}
	if (prefix == MAX_PREFIX) {
		EXCEPTION(UD_EXCEPTION, 0);
	}
	/* normal / rep, but not use */
	if (!(insttable_info[op] & INST_STRING) || !CPU_INST_REPUSE) {
		//(*insttable_1byte[CPU_INST_OP32][op])();
		return (void*)(insttable_1byte[CPU_INST_OP32][op]);
	}
	CPU_EIP = CPU_PREV_EIP;
	CPU_STATSAVE.cpu_inst = CPU_STATSAVE.cpu_inst_default;
	return (void*)(&exec_1step);
}

void firstinsttrans() {
	CPU_PREV_EIP = CPU_EIP;
	CPU_STATSAVE.cpu_inst = CPU_STATSAVE.cpu_inst_default;
	return;
}
void firstinsttrans_null() {
	return;
}

void eiptransaction(UINT32 prm_0) { CPU_EIP += prm_0; }

bool writeddebugfile = false;
typedef UINT64 typeofcachedcode();

struct jitinfo{
#ifdef _WIN64
	void (*codeptr)(void);
#else
	void(*codeptr)(void);
	UINT32 codeptrh;
#endif
	bool isfirstop = false;
	bool isprefix = false;
	bool is1step = false;
	UINT8 reptype = 0;
	UINT8 op = 0;
};

UINT64 jitcachedeip = 0xffffffff00000000;
jitinfo jitcache[4096 + MAX_PREFIX];

void genud(void) {
	EXCEPTION(UD_EXCEPTION, 0);
	return;
}

void repops(void) {
	UINT32 op = jitcache[(CPU_EIP) & 0xFFF].op;
	/* rep */
	CPU_WORKCLOCK(5);
	if (!CPU_INST_AS32) {
		if (CPU_CX != 0) {
			if (!(insttable_info[op] & REP_CHECKZF)) {
				/* rep */
				if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
					(*insttable_1byte_repfunc[CPU_INST_OP32][op])(0);
				}
				else {
					for (;;) {
						(*insttable_1byte[CPU_INST_OP32][op])();
						if (--CPU_CX == 0) {
							break;
						}
						if (CPU_REMCLOCK <= 0) {
							CPU_EIP = CPU_PREV_EIP;
							break;
						}
					}
				}
			}
			else if (CPU_INST_REPUSE != 0xf2) {
				/* repe */
				if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
					(*insttable_1byte_repfunc[CPU_INST_OP32][op])(1);
				}
				else {
					for (;;) {
						(*insttable_1byte[CPU_INST_OP32][op])();
						if (--CPU_CX == 0 || CC_NZ) {
							break;
						}
						if (CPU_REMCLOCK <= 0) {
							CPU_EIP = CPU_PREV_EIP;
							break;
						}
					}
				}
			}
			else {
				/* repne */
				if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
					(*insttable_1byte_repfunc[CPU_INST_OP32][op])(2);
				}
				else {
					for (;;) {
						(*insttable_1byte[CPU_INST_OP32][op])();
						if (--CPU_CX == 0 || CC_Z) {
							break;
						}
						if (CPU_REMCLOCK <= 0) {
							CPU_EIP = CPU_PREV_EIP;
							break;
						}
					}
				}
			}
		}
	}
	else {
		if (CPU_ECX != 0) {
			if (!(insttable_info[op] & REP_CHECKZF)) {
				/* rep */
				if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
					(*insttable_1byte_repfunc[CPU_INST_OP32][op])(0);
				}
				else {
					for (;;) {
						(*insttable_1byte[CPU_INST_OP32][op])();
						if (--CPU_ECX == 0) {
							break;
						}
						if (CPU_REMCLOCK <= 0) {
							CPU_EIP = CPU_PREV_EIP;
							break;
						}
					}
				}
			}
			else if (CPU_INST_REPUSE != 0xf2) {
				/* repe */
				if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
					(*insttable_1byte_repfunc[CPU_INST_OP32][op])(1);
				}
				else {
					for (;;) {
						(*insttable_1byte[CPU_INST_OP32][op])();
						if (--CPU_ECX == 0 || CC_NZ) {
							break;
						}
						if (CPU_REMCLOCK <= 0) {
							CPU_EIP = CPU_PREV_EIP;
							break;
						}
					}
				}
			}
			else {
				/* repne */
				if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
					(*insttable_1byte_repfunc[CPU_INST_OP32][op])(2);
				}
				else {
					for (;;) {
						(*insttable_1byte[CPU_INST_OP32][op])();
						if (--CPU_ECX == 0 || CC_Z) {
							break;
						}
						if (CPU_REMCLOCK <= 0) {
							CPU_EIP = CPU_PREV_EIP;
							break;
						}
					}
				}
			}
		}
	}
}

UINT64 exec_jit() {
	int prefix;
	UINT32 op;

	UINT8 cacheid = 0;
	UINT32 transcoderem = 0;

	do {

		CPU_PREV_EIP = CPU_EIP;
		CPU_STATSAVE.cpu_inst = CPU_STATSAVE.cpu_inst_default;

		for (prefix = 0; prefix < MAX_PREFIX; prefix++) {
			GET_PCBYTE(op);

			/* prefix */
			if (insttable_info[op] & INST_PREFIX) {
				(*insttable_1byte[0][op])();
				continue;
			}
			break;
		}
		if (prefix == MAX_PREFIX) {
			EXCEPTION(UD_EXCEPTION, 0);
		}

		/* normal / rep, but not use */
		if (!(insttable_info[op] & INST_STRING) || !CPU_INST_REPUSE) {
			if (op == 0x0F) {
				GET_PCBYTE(op);
#ifdef USE_SSE
				if (insttable_2byte660F_32[op] && CPU_INST_OP32 == !CPU_STATSAVE.cpu_inst_default.op_32) {
					(*insttable_2byte660F_32[op])();
				}
				else if (insttable_2byteF20F_32[op] && CPU_INST_REPUSE == 0xf2) {
					(*insttable_2byteF20F_32[op])();
				}
				else if (insttable_2byteF30F_32[op] && CPU_INST_REPUSE == 0xf3) {
					(*insttable_2byteF30F_32[op])();
				}
				else {
					(*insttable_2byte[CPU_INST_AS32][op])();
				}
#else
				(*insttable_2byte[CPU_INST_AS32][op])();
#endif
			}
			else {
				(*insttable_1byte[CPU_INST_OP32][op])();
			}
			goto cpucontinue;
		}

		/* rep */
		CPU_WORKCLOCK(5);
		if (!CPU_INST_AS32) {
			if (CPU_CX != 0) {
				if (!(insttable_info[op] & REP_CHECKZF)) {
					/* rep */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(0);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_CX == 0) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
				else if (CPU_INST_REPUSE != 0xf2) {
					/* repe */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(1);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_CX == 0 || CC_NZ) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
				else {
					/* repne */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(2);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_CX == 0 || CC_Z) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
			}
		}
		else {
			if (CPU_ECX != 0) {
				if (!(insttable_info[op] & REP_CHECKZF)) {
					/* rep */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(0);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_ECX == 0) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
				else if (CPU_INST_REPUSE != 0xf2) {
					/* repe */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(1);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_ECX == 0 || CC_NZ) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
				else {
					/* repne */
					if (insttable_1byte_repfunc[CPU_INST_OP32][op]) {
						(*insttable_1byte_repfunc[CPU_INST_OP32][op])(2);
					}
					else {
						for (;;) {
							(*insttable_1byte[CPU_INST_OP32][op])();
							if (--CPU_ECX == 0 || CC_Z) {
								break;
							}
							if (CPU_REMCLOCK <= 0) {
								CPU_EIP = CPU_PREV_EIP;
								break;
							}
						}
					}
				}
			}
		}
	cpucontinue:;
	} while (CPU_REMCLOCK > 0);
	return CPU_REMCLOCK;
}
