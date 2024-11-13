/*
 * Copyright (c) 2002-2003 NONAKA Kimihiro
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

//#include "compiler.h"
#include "cpu.h"
#include "ia32.mcr"

//#include "pccore.h"
//#include "iocore.h"
//#include "dmax86.h"
//#include "bios/bios.h"

#include "instructions/fpu/fp.h"

#if defined(SUPPORT_IA32_HAXM)
#include "i386hax/haxfunc.h"
#include "i386hax/haxcore.h"
#endif

void
ia32_initreg(void)
{
	int i;

	CPU_STATSAVE.cpu_inst_default.seg_base = (UINT32)-1;

	CPU_EDX = (CPU_FAMILY << 8) | (CPU_MODEL << 4) | CPU_STEPPING;
	CPU_EFLAG = 2;
	CPU_CR0 = CPU_CR0_CD | CPU_CR0_NW;
#if defined(USE_FPU)
	if(i386cpuid.cpu_feature & CPU_FEATURE_FPU){
		CPU_CR0 |= CPU_CR0_ET;	/* FPU present */
		CPU_CR0 &= ~CPU_CR0_EM;
	}else{
		CPU_CR0 |= CPU_CR0_EM | CPU_CR0_NE;
		CPU_CR0 &= ~(CPU_CR0_MP | CPU_CR0_ET);
	}
#else
	CPU_CR0 |= CPU_CR0_EM | CPU_CR0_NE;
	CPU_CR0 &= ~(CPU_CR0_MP | CPU_CR0_ET);
#endif
	CPU_MXCSR = 0x1f80;
	
#if defined(USE_TSC)
	CPU_MSR_TSC = 0;
#endif

	CPU_GDTR_BASE = 0x0;
	CPU_GDTR_LIMIT = 0xffff;
	CPU_IDTR_BASE = 0x0;
	CPU_IDTR_LIMIT = 0xffff;
	CPU_LDTR_BASE = 0x0;
	CPU_LDTR_LIMIT = 0xffff;
	CPU_TR_BASE = 0x0;
	CPU_TR_LIMIT = 0xffff;

	CPU_STATSAVE.cpu_regs.dr[6] = 0xffff1ff0;

	for (i = 0; i < CPU_SEGREG_NUM; ++i) {
		segdesc_init(i, 0, &CPU_STAT_SREG(i));
	}
	LOAD_SEGREG(CPU_CS_INDEX, 0xf000);
	CPU_STAT_CS_BASE = 0xffff0000;
	CPU_EIP = 0xfff0;
//	CPU_ADRSMASK = 0x000fffff;
	CPU_ADRSMASK = ~0;

	tlb_init();
	fpu_initialize();
}

void
ia32reset(void)
{

	memset(&i386core.s, 0, sizeof(i386core.s));
	ia32_initreg();
}

void
ia32shut(void)
{

	memset(&i386core.s, 0, offsetof(I386STAT, cpu_type));
	ia32_initreg();
}

void
ia32a20enable(BOOL enable)
{

//	CPU_ADRSMASK = (enable)?0xffffffff:0x00ffffff;
	CPU_ADRSMASK = (enable)?(~0):(~(1 << 20));
}

//#pragma optimize("", off)
void
ia32(void)
{
#ifdef __cplusplus
	try {
#else
	switch (sigsetjmp(exec_1step_jmpbuf, 1)) {
	case 0:
		break;

	case 1:
		VERBOSE(("ia32: return from exception"));
		break;

	case 2:
		VERBOSE(("ia32: return from panic"));
		return;

	default:
		VERBOSE(("ia32: return from unknown cause"));
		break;
	}
#endif
/*
	if (!CPU_TRAP && !dmac.working) {
		exec_allstep();
	}else 
*/
	if (!CPU_TRAP) {
		do {
			exec_1step();
			dmax86();
		} while (CPU_REMCLOCK > 0);
	}else{
		do {
			exec_1step();
			if (CPU_TRAP) {
				CPU_DR6 |= CPU_DR6_BS;
				INTERRUPT(1, INTR_TYPE_EXCEPTION);
			}
			dmax86();
		} while (CPU_REMCLOCK > 0);
	}
#ifdef __cplusplus
	} catch (int e) {
		switch (e) {
		case 0:
			break;

		case 1:
			VERBOSE(("ia32: return from exception"));
			break;

		case 2:
			VERBOSE(("ia32: return from panic"));
			return;

		default:
			VERBOSE(("ia32: return from unknown cause"));
			break;
		}
	}
#endif
}

void
ia32_step(void)
{
#ifdef __cplusplus
	try {
#else
	switch (sigsetjmp(exec_1step_jmpbuf, 1)) {
	case 0:
		break;

	case 1:
		VERBOSE(("ia32: return from exception"));
		break;

	case 2:
		VERBOSE(("ia32: return from panic"));
		return;

	default:
		VERBOSE(("ia32: return from unknown cause"));
		break;
	}
#endif
	do {
		exec_1step();
		if (CPU_TRAP) {
			CPU_DR6 |= CPU_DR6_BS;
			INTERRUPT(1, INTR_TYPE_EXCEPTION);
		}
		//if (dmac.working) {
			dmax86();
		//}
	} while (CPU_REMCLOCK > 0);
#ifdef __cplusplus
	} catch (int e) {
		switch (e) {
		case 0:
			break;

		case 1:
			VERBOSE(("ia32: return from exception"));
			break;

		case 2:
			VERBOSE(("ia32: return from panic"));
			return;

		default:
			VERBOSE(("ia32: return from unknown cause"));
			break;
		}
	}
#endif
}
//#pragma optimize("", on)

void CPUCALL
ia32_interrupt(int vect, int soft)
{

//	TRACEOUT(("int (%x, %x) PE=%d VM=%d",  vect, soft, CPU_STAT_PM, CPU_STAT_VM86));
#if defined(SUPPORT_IA32_HAXM)
	if(np2hax.enable && !np2hax.emumode && np2hax.hVCPUDevice){
		np2haxcore.hltflag = 0;
		if(!soft){
			HAX_TUNNEL *tunnel;
			tunnel = (HAX_TUNNEL*)np2hax.tunnel.va;
			if(np2haxstat.irq_reqidx_end - np2haxstat.irq_reqidx_cur < 250){
				np2haxstat.irq_req[np2haxstat.irq_reqidx_end] = vect;
				np2haxstat.irq_reqidx_end++;
			}
			//i386haxfunc_vcpu_interrupt(vect);
		}
	}else
#endif
	{
		if (!soft) {
			INTERRUPT(vect, INTR_TYPE_EXTINTR);
		} else {
			if (CPU_STAT_PM && CPU_STAT_VM86 && CPU_STAT_IOPL < CPU_IOPL3) {
				VERBOSE(("ia32_interrupt: VM86 && IOPL < 3 && INTn"));
				EXCEPTION(GP_EXCEPTION, 0);
			}
			INTERRUPT(vect, INTR_TYPE_SOFTINTR);
		}
	}
}


/*
 * error function
 */
void
ia32_panic(const char *str, ...)
{
	extern char *cpu_reg2str(void);
	char buf[2048];
	va_list ap;

	va_start(ap, str);
	vsnprintf(buf, sizeof(buf), str, ap);
	va_end(ap);
	strcat(buf, "\n");
	strcat(buf, cpu_reg2str());
	VERBOSE(("%s", buf));

	msgbox("ia32_panic", buf);

#if defined(IA32_REBOOT_ON_PANIC)
	VERBOSE(("ia32_panic: reboot"));
	//kbd_reset();
#ifdef __cplusplus
	throw(2);
#else
	siglongjmp(exec_1step_jmpbuf, 2);
#endif
#else
	__ASSERT(0);
	exit(1);
#endif
}

void
ia32_warning(const char *str, ...)
{
	char buf[1024];
	va_list ap;
	va_start(ap, str);
	vsnprintf(buf, sizeof(buf), str, ap);
	va_end(ap);
	strcat(buf, "\n");

	msgbox("ia32_warning", buf);
}

void
ia32_printf(const char *str, ...)
{
	char buf[1024];
	va_list ap;
	va_start(ap, str);
	vsnprintf(buf, sizeof(buf), str, ap);
	va_end(ap);
	strcat(buf, "\n");

	msgbox("ia32_printf", buf);
}


/*
 * bios call interface
 */
#if 0
void
ia32_bioscall(void)
{
	UINT32 adrs;

	if (!CPU_STAT_PM || CPU_STAT_VM86) {
#if 1
		adrs = CPU_PREV_EIP + (CPU_CS << 4);
#else
		adrs = CPU_PREV_EIP + CPU_STAT_CS_BASE;
#endif
		if ((adrs >= 0xf8000) && (adrs < 0x100000)) {
			if (biosfunc(adrs)) {
				/* Nothing to do */
			}
			LOAD_SEGREG(CPU_ES_INDEX, CPU_ES);
			LOAD_SEGREG(CPU_CS_INDEX, CPU_CS);
			LOAD_SEGREG(CPU_SS_INDEX, CPU_SS);
			LOAD_SEGREG(CPU_DS_INDEX, CPU_DS);
		}
	}else{
#ifdef SUPPORT_PCI
		adrs = CPU_EIP;
		if (bios32func(adrs)) {
			/* Nothing to do */
		}
#endif
	}
}
#endif

void
ia32smi(void) {
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7ff8, CPU_CR0);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7ff0, CPU_CR3);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7fe8, CPU_EFLAG);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7fd8, CPU_EIP);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7fd0, CPU_DR6);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7fc8, CPU_DR7);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7fc4, CPU_TR);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7fc0, CPU_LDTR);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7fbc, CPU_GS);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7fb8, CPU_FS);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7fb4, CPU_DS);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7fb0, CPU_SS);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7fac, CPU_CS);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7fa8, CPU_ES);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7f94, CPU_EDI);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7f8c, CPU_ESI);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7f84, CPU_EBP);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7f7c, CPU_ESP);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7f74, CPU_EBX);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7f6c, CPU_EDX);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7f64, CPU_ECX);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7f5c, CPU_EAX);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7ef8, i386msr.reg.ia32_smbase_msr);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7e9c, CPU_LDTR_BASE);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7e94, CPU_IDTR_BASE);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7e8c, CPU_GDTR_BASE);
	cpu_memorywrite_d(i386msr.reg.ia32_smbase_msr + 0x7e40, CPU_CR4);
	CPU_INST_OP32 = CPU_INST_AS32 =
		CPU_STATSAVE.cpu_inst_default.op_32 =
		CPU_STATSAVE.cpu_inst_default.as_32 = 0;
	CPU_STAT_SS32 = 0;
	set_cpl(0);
	CPU_STAT_PM = 0;
	change_pg(0);
	CS_BASE = i386msr.reg.ia32_smbase_msr;
	CPU_STAT_SREGLIMIT(CPU_CS_INDEX) = 0xffffffff;
	DS_BASE = 0;
	CPU_STAT_SREGLIMIT(CPU_DS_INDEX) = 0xffffffff;
	ES_BASE = 0;
	CPU_STAT_SREGLIMIT(CPU_ES_INDEX) = 0xffffffff;
	FS_BASE = 0;
	CPU_STAT_SREGLIMIT(CPU_FS_INDEX) = 0xffffffff;
	GS_BASE = 0;
	CPU_STAT_SREGLIMIT(CPU_GS_INDEX) = 0xffffffff;
	SS_BASE = 0;
	CPU_STAT_SREGLIMIT(CPU_SS_INDEX) = 0xffffffff;
	CPU_STAT_CPL = 0;
	CPU_STAT_USER_MODE = (CPU_STAT_CPL == 3) ? CPU_MODE_USER : CPU_MODE_SUPERVISER;
	CPU_EIP = 0x8000;
	i386core.smm_mode = 1;
	return;
}
