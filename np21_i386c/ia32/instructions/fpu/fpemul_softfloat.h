#include "fpumem.h"

static INLINE UINT FPU_GET_TOP(void);

static INLINE void FPU_SET_TOP(UINT val);

static INLINE void FPU_SET_C0(UINT C);

static INLINE void FPU_SET_C1(UINT C);

static INLINE void FPU_SET_C2(UINT C);

static INLINE void FPU_SET_C3(UINT C);

static INLINE void FPU_SET_D(UINT C);

static INLINE void FPU_SetCW(UINT16 cword);

static void FPU_FLDCW(UINT32 addr);

static UINT16 FPU_GetTag(void);
static UINT8 FPU_GetTag8(void);

static INLINE void FPU_SetTag(UINT16 tag);
static INLINE void FPU_SetTag8(UINT8 tag);

static void FPU_FCLEX(void);

static void FPU_FNOP(void);

static void FPU_PUSH(floatx80 in);

static void FPU_PREP_PUSH(void);

static void FPU_FPOP(void);

static floatx80 FROUND(floatx80 in);

static void FPU_FLD80(UINT32 addr, UINT reg);

static void FPU_ST80(UINT32 addr, UINT reg);


static void FPU_FLD_F32(UINT32 addr, UINT store_to);

static void FPU_FLD_F64(UINT32 addr, UINT store_to);

static void FPU_FLD_F80(UINT32 addr);

static void FPU_FLD_I16(UINT32 addr, UINT store_to);

static void FPU_FLD_I32(UINT32 addr, UINT store_to);

static void FPU_FLD_I64(UINT32 addr, UINT store_to);

static void FPU_FBLD(UINT32 addr, UINT store_to);


static INLINE void FPU_FLD_F32_EA(UINT32 addr);
static INLINE void FPU_FLD_F64_EA(UINT32 addr);
static INLINE void FPU_FLD_I32_EA(UINT32 addr);
static INLINE void FPU_FLD_I16_EA(UINT32 addr);

static void FPU_FST_F32(UINT32 addr);

static void FPU_FST_F64(UINT32 addr);

static void FPU_FST_F80(UINT32 addr);

static void FPU_FST_I16(UINT32 addr);

static void FPU_FST_I32(UINT32 addr);

static void FPU_FST_I64(UINT32 addr);

static void FPU_FBST(UINT32 addr);
static void FPU_FADD(UINT op1, UINT op2);

static void FPU_FSIN(void);

static void FPU_FSINCOS(void);

static void FPU_FCOS(void);

static void FPU_FSQRT(void);
static void FPU_FPATAN(void);
static void FPU_FPTAN(void);
static void FPU_FDIV(UINT st, UINT other);

static void FPU_FDIVR(UINT st, UINT other);

static void FPU_FMUL(UINT st, UINT other);

static void FPU_FSUB(UINT st, UINT other);

static void FPU_FSUBR(UINT st, UINT other);

static void FPU_FXCH(UINT st, UINT other);

static void FPU_FST(UINT st, UINT other);

static void FPU_FCOM(UINT st, UINT other);
static void FPU_FCOMI(UINT st, UINT other);

//#define FPU_FUCOM FPU_FCOM
static void FPU_FUCOM(UINT st, UINT other);
//#define FPU_FUCOMI FPU_FCOMI
static void FPU_FUCOMI(UINT st, UINT other);

static void FPU_FCMOVB(UINT st, UINT other);
static void FPU_FCMOVE(UINT st, UINT other);
static void FPU_FCMOVBE(UINT st, UINT other);
static void FPU_FCMOVU(UINT st, UINT other);

static void FPU_FCMOVNB(UINT st, UINT other);
static void FPU_FCMOVNE(UINT st, UINT other);
static void FPU_FCMOVNBE(UINT st, UINT other);
static void FPU_FCMOVNU(UINT st, UINT other);

static void FPU_FRNDINT(void);

static void FPU_FPREM(void);

static void FPU_FPREM1(void);

static void FPU_FXAM(void);

static void FPU_F2XM1(void);

static void FPU_FYL2X(void);

static void FPU_FYL2XP1(void);

static void FPU_FSCALE(void);

static void FPU_FSTENV(UINT32 addr);

static void FPU_FLDENV(UINT32 addr);

static void FPU_FSAVE(UINT32 addr);

static void FPU_FRSTOR(UINT32 addr);

static void FPU_FXSAVE(UINT32 addr);
static void FPU_FXRSTOR(UINT32 addr);

void SF_FPU_FXSAVERSTOR(void);

static void FPU_FXTRACT(void);

static void FPU_FCHS(void);

static void FPU_FABS(void);

static void FPU_FTST(void);

static void FPU_FLD1(void);

static void FPU_FLDL2T(void);

static void FPU_FLDL2E(void);

static void FPU_FLDPI(void);

static void FPU_FLDLG2(void);

static void FPU_FLDLN2(void);

static void FPU_FLDZ(void);


static INLINE void FPU_FADD_EA(UINT op1);
static INLINE void FPU_FMUL_EA(UINT op1);
static INLINE void FPU_FSUB_EA(UINT op1);
static INLINE void FPU_FSUBR_EA(UINT op1);
static INLINE void FPU_FDIV_EA(UINT op1);
static INLINE void FPU_FDIVR_EA(UINT op1);
static INLINE void FPU_FCOM_EA(UINT op1);

/*
 * FPU interface
 */
 //int fpu_updateEmuEnv(void);
static void FPU_FINIT(void);
void SF_FPU_FINIT(void);

//char *
//fpu_reg2str(void);
