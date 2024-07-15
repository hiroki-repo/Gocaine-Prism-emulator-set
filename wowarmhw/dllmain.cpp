// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include "windows.h"
#include "winternl.h"
#include <stdio.h>
#include "stdlib.h"
#include "ntstatus.h"

#include "../ARMv7/ARM/ARMv7_cpu.h"

#pragma warning(disable:4996)

typedef WINBASEAPI
LPVOID
WINAPI
t_VirtualAlloc(
	_In_opt_ LPVOID lpAddress,
	_In_     SIZE_T dwSize,
	_In_     DWORD flAllocationType,
	_In_     DWORD flProtect
);
typedef WINBASEAPI
BOOL
WINAPI
t_VirtualFree(
	_Pre_notnull_ _When_(dwFreeType == MEM_DECOMMIT, _Post_invalid_) _When_(dwFreeType == MEM_RELEASE, _Post_ptr_invalid_) LPVOID lpAddress,
	_In_ SIZE_T dwSize,
	_In_ DWORD dwFreeType
);
struct t_Virtualallocandfree {
	t_VirtualAlloc* _VirtualAlloc;
	t_VirtualFree* _VirtualFree;
};

typedef t_Virtualallocandfree* t_getVirtualallocandfree();

extern char modulename4this[4096];
typedef NTSYSAPI NTSTATUS  WINAPI t_LdrLoadDll(LPCWSTR, DWORD, const UNICODE_STRING*, HMODULE*);
t_LdrLoadDll* LdrLoadDll = 0;

bool jit_enabled = false;

#ifdef __cplusplus
extern "C" {
#endif

	__declspec(dllexport) void* ULLoadLibraryA(char* prm_0) {
		char buff0[4096];
		char* buff4pe;
		char* buff4pecl;
		char* dll4prevdll = (char*)"::/\\";
		HANDLE fh;
		UINT64 baseaddr = 0;
		UINT64 cnt = 0;
		UINT64 cnt2 = 0;
		UINT64 reloc = 0;
		UINT64 relocsize = 0;
		UINT64 textaddr = 0;
		UINT64 textaddrsize = 0;
		DWORD tmp;
		if ((fh = CreateFileA(prm_0, GENERIC_READ, 3, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0)) != INVALID_HANDLE_VALUE) {
			ReadFile(fh, buff0, 512, &tmp, 0);
			SetFilePointer(fh, (*(UINT32*)(&buff0[0x3c])), 0, 0);
			ReadFile(fh, buff0, 4096, &tmp, 0);
			SetFilePointer(fh, 0, 0, 0);
			//buff4pe = (char*)malloc((*(UINT32*)(&buff0[0x1c])) + (*(UINT32*)(&buff0[0x20])) + (*(UINT32*)(&buff0[0x24])));
			if ((*(UINT16*)(&buff0[0x18])) == 0x10b) {
				baseaddr = (*(UINT32*)(&buff0[0x34]));
			}
			else if ((*(UINT16*)(&buff0[0x18])) == 0x20b) {
				baseaddr = (*(UINT64*)(&buff0[0x30]));
			}
			buff4pe = (char*)VirtualAlloc((LPVOID)baseaddr, (*(UINT32*)(&buff0[0x50])) + 4096, 0x3000, 0x40);//malloc((*(UINT32*)(&buff0[0x50]))+4096);
			if (buff4pe == 0) { buff4pe = (char*)VirtualAlloc(0, (*(UINT32*)(&buff0[0x50])) + 4096, 0x3000, 0x40); if (buff4pe == 0) { return 0; } }
			memcpy(buff4pe, buff0, 4096);
			if ((*(UINT16*)(&buff0[0x18])) == 0x10b) {
				//32bit
				baseaddr = (*(UINT32*)(&buff0[0x34]));
				cnt = 0;
				while (cnt < (*(UINT16*)(&buff0[0x6]))) {
					SetFilePointer(fh, (*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0x14])), 0, 0);
					ReadFile(fh, (void*)((UINT64)(buff4pe)+(*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0xc]))), (*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0x10])), &tmp, 0);
					if (strcmp(((char*)(&buff0[0xf8 + (0x28 * cnt)])), ".reloc") == 0) {
						reloc = ((UINT64)(buff4pe)+(*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0xc]))); relocsize = (*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0x8]));
					}
					if (strcmp(((char*)(&buff0[0xf8 + (0x28 * cnt)])), ".text") == 0) {
						textaddr = ((UINT64)(buff4pe)+(*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0xc]))); textaddrsize = (*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0x8]));
					}
					cnt++;
				}
			}
			else if ((*(UINT16*)(&buff0[0x18])) == 0x20b) {
				//64bit
				baseaddr = (*(UINT64*)(&buff0[0x30]));
				cnt = 0;
				while (cnt < (*(UINT16*)(&buff0[0x6]))) {
					SetFilePointer(fh, (*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0x14])), 0, 0);
					ReadFile(fh, (void*)((UINT64)(buff4pe)+(*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0xc]))), (*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0x10])), &tmp, 0);
					if (strcmp(((char*)(&buff0[0x108 + (0x28 * cnt)])), ".reloc") == 0) {
						reloc = (UINT64)(buff4pe)+(*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0xc])); relocsize = (*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0x8]));
					}
					if (strcmp(((char*)(&buff0[0x108 + (0x28 * cnt)])), ".text") == 0) {
						textaddr = (UINT64)(buff4pe)+(*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0xc])); textaddrsize = (*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0x8]));
					}
					cnt++;
				}
			}
			UINT64 delta = ((UINT64)(buff4pe - baseaddr));
			UINT64 tmp4relocptx = 8;
			UINT32 armhi = 0;
			UINT32 armlo = 0;
			UINT32 armhi_ = 0;
			UINT32 armlo_ = 0;
			UINT64 deltatmp;
			HMODULE HM = 0;
			//if (reloc == 0) { CloseHandle(fh); free(buff4pe); return 0; }

		loop4relocate:
			cnt = 0;
			for (cnt = 0; cnt < (((*(UINT32*)(reloc + (tmp4relocptx - 4))) - 8) / 2); cnt++) {
				switch (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) >> 12) & 0xF) {
				case 1:
					(*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) = (*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) + (UINT32)((delta >> 16) & 0xFFFF);
					break;
				case 2:
					(*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) = (*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) + (UINT32)((delta >> 0) & 0xFFFF);
					break;
				case 3:
					(*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) = (*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) + (UINT32)delta;
					break;
				case 7:
					armlo = (*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8))))));
					armlo_ = ((armlo << 1) & 0x0800) + ((armlo << 12) & 0xf000) + ((armlo >> 20) & 0x0700) + ((armlo >> 16) & 0x00ff);
					armhi = (*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))) + 4));
					armhi_ = ((armhi << 1) & 0x0800) + ((armhi << 12) & 0xf000) + ((armhi >> 20) & 0x0700) + ((armhi >> 16) & 0x00ff);
					deltatmp = (((armlo_ & 0xFFFF) << 0) | ((armhi_ & 0xFFFF) << 16)) + delta;
					armlo_ = (deltatmp >> 0) & 0xFFFF;
					armhi_ = (deltatmp >> 16) & 0xFFFF;
					armlo = (armlo & 0x8f00fbf0) + ((armlo_ >> 1) & 0x0400) + ((armlo_ >> 12) & 0x000f) + ((armlo_ << 20) & 0x70000000) + ((armlo_ << 16) & 0xff0000);
					armhi = (armhi & 0x8f00fbf0) + ((armhi_ >> 1) & 0x0400) + ((armhi_ >> 12) & 0x000f) + ((armhi_ << 20) & 0x70000000) + ((armhi_ << 16) & 0xff0000);
					(*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) = armlo;
					(*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))) + 4)) = armhi;
					break;
				case 10:
					(*(UINT64*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))) + 0)) = (*(UINT64*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))) + 0)) + delta;
					break;
				}
				//cnt++;
			}
			tmp4relocptx += (*(UINT32*)(reloc + (tmp4relocptx - 4)));
			if ((*(UINT32*)(reloc + (tmp4relocptx - 4))) != 0)
				goto loop4relocate;
			cnt = 0;
			if ((*(UINT16*)(&buff0[0x18])) == 0x10b) {
				//32bit
				while ((*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20))) != 0) {
					cnt2 = 0;
					if ((*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20) + 12)) != 0) {
						HM = 0;
						char fname64bit[4096];
						strcpy(fname64bit, prm_0);
						char* p = strrchr(fname64bit, '\\');
						if ((UINT64)p <= (UINT64)&fname64bit) {
							if (p) {
								if (p) { *++p = 0; }
								strncat(fname64bit, ((char*)(buff4pe + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20) + 12)))), strlen(((char*)(buff4pe + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20) + 12))))));
								HM = LoadLibraryA(fname64bit);
							}
						}
						if (HM == 0) {
							HM = LoadLibraryA(((char*)(buff4pe + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20) + 12)))));
						}
					}
					if (HM != 0) {
						while ((*(UINT32*)(buff4pe + (cnt2 * 4) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20) + 16)))) != 0) {//&& (*(UINT32*)(buff4pe + (cnt2 * 4) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20) + 0)))) != 0) {
							// + ((*(UINT16*)(buff4pe + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20) + 4)))) * 4)
							if ((*(UINT32*)(buff4pe + (cnt2 * 4) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20) + 0)))) & 0x80000000) {
								(*(UINT32*)(buff4pe + (cnt2 * 4) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20) + 16)))) = (UINT32)GetProcAddress(HM, ((LPCSTR)((*(UINT32*)(buff4pe + (cnt2 * 4) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20) + 0)))) & 0xFFFF)));
							}
							else {
								(*(UINT32*)(buff4pe + (cnt2 * 4) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20) + 16)))) = (UINT32)GetProcAddress(HM, ((char*)(buff4pe + 2 + (*(UINT32*)(buff4pe + (cnt2 * 4) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20) + 0)))))));
							}
							cnt2++;
						}
						//(*(UINT32*)(buff4pe + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x80])) + (cnt * 20) + 0)))) = 0 | 0x80000000;
					}
					cnt++;
				}
			}
			else if ((*(UINT16*)(&buff0[0x18])) == 0x20b) {
				//64bit
				while ((*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20))) != 0) {
					cnt2 = 0;
					if ((*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20) + 12)) != 0) {
						HM = 0;
						char fname64bit[4096];
						strcpy(fname64bit, prm_0);
						char* p = strrchr(fname64bit, '\\');
						if ((UINT64)p <= (UINT64)&fname64bit) {
							if (p) {
								if (p) { *++p = 0; }
								strncat(fname64bit, ((char*)(buff4pe + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20) + 12)))), strlen(((char*)(buff4pe + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20) + 12))))));
								HM = LoadLibraryA(fname64bit);
							}
						}
						if (HM == 0) {
							HM = LoadLibraryA(((char*)(buff4pe + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20) + 12)))));
						}
					}
					if (HM != 0) {
						while ((*(UINT32*)(buff4pe + (cnt2 * 4) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20) + 16)))) != 0 && (*(UINT32*)(buff4pe + (cnt2 * 4) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20) + 0)))) != 0) {
							// + ((*(UINT16*)(buff4pe + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20) + 4)))) * 8)
							if ((*(UINT64*)(buff4pe + (cnt2 * 4) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20) + 0)))) & 0x8000000000000000) {
								(*(UINT64*)(buff4pe + (cnt2 * 4) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20) + 16)))) = (UINT64)GetProcAddress(HM, ((LPCSTR)((*(UINT32*)(buff4pe + (cnt2 * 4) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20) + 0)))) & 0xFFFF)));
							}
							else {
								(*(UINT64*)(buff4pe + (cnt2 * 8) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20) + 16)))) = (UINT64)GetProcAddress(HM, ((char*)(buff4pe + 2 + (*(UINT32*)(buff4pe + (cnt2 * 4) + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20) + 0)))))));
							}
							cnt2++;
						}
						//(*(UINT64*)(buff4pe + (*(UINT32*)(buff4pe + (*(UINT32*)(&buff0[0x90])) + (cnt * 20) + 0)))) = 0 | 0x8000000000000000;
					}
					cnt++;
				}
			}
			if (textaddr != 0) {
				VirtualProtect((void*)(textaddr), textaddrsize, PAGE_EXECUTE_READWRITE, &tmp);
				FlushInstructionCache(GetCurrentProcess(), (void*)(textaddr), textaddrsize);
			}
			CloseHandle(fh);
			return buff4pe;
		}
		return 0;
	}
	__declspec(dllexport) void* ULCloneLibrary(HMODULE prm_0) {
		char buff0[4096];
		char* buff4pe;
		char* buff4pecl;
		char* dll4prevdll = (char*)"::/\\";
		HANDLE fh;
		UINT64 baseaddr = 0;
		UINT64 cnt = 0;
		UINT64 cnt2 = 0;
		UINT64 reloc = 0;
		UINT64 relocsize = 0;
		UINT64 textaddr = 0;
		UINT64 textaddrsize = 0;
		DWORD tmp;
		memcpy(buff0, (void*)prm_0, 4096);
		if ((buff0[0] != 'M' || buff0[1] != 'Z') && (buff0[0] != 'P' || buff0[1] != 'E')) { return 0; }
		if (buff0[0] != 'P' || buff0[1] != 'E') {
			memcpy(buff0, (void*)(prm_0 + ((*(UINT32*)(&buff0[0x3c])) / 4)), 4096 - (*(UINT32*)(&buff0[0x3c])));
		}
		if (buff0[0] != 'P' || buff0[1] != 'E') { return 0; }
		baseaddr = (UINT64)prm_0;
		buff4pe = (char*)VirtualAlloc(0, (*(UINT32*)(&buff0[0x50])) + 4096, 0x3000, 0x40);
		if (buff4pe == 0) { return 0; }
		memcpy(buff4pe, buff0, 4096);
		if ((*(UINT16*)(&buff0[0x18])) == 0x10b) {
			//32bit
			baseaddr = (*(UINT32*)(&buff0[0x34]));
			cnt = 0;
			while (cnt < (*(UINT16*)(&buff0[0x6]))) {
				//SetFilePointer(fh, (*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0x14])), 0, 0);
				memcpy((void*)((UINT64)(buff4pe)+(*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0xc]))), (void*)((UINT64)(prm_0)+(*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0xc]))), (*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0x10])));
				if (strcmp(((char*)(&buff0[0xf8 + (0x28 * cnt)])), ".reloc") == 0) {
					reloc = ((UINT64)(buff4pe)+(*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0xc]))); relocsize = (*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0x8]));
				}
				if (strcmp(((char*)(&buff0[0xf8 + (0x28 * cnt)])), ".text") == 0) {
					textaddr = ((UINT64)(buff4pe)+(*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0xc]))); textaddrsize = (*(UINT32*)(&buff0[0xf8 + (0x28 * cnt) + 0x8]));
				}
				cnt++;
			}
		}
		else if ((*(UINT16*)(&buff0[0x18])) == 0x20b) {
			//64bit
			baseaddr = (*(UINT64*)(&buff0[0x30]));
			cnt = 0;
			while (cnt < (*(UINT16*)(&buff0[0x6]))) {
				//SetFilePointer(fh, (*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0x14])), 0, 0);
				memcpy((void*)((UINT64)(buff4pe)+(*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0xc]))), (void*)((UINT64)(prm_0)+(*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0xc]))), (*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0x10])));
				if (strcmp(((char*)(&buff0[0x108 + (0x28 * cnt)])), ".reloc") == 0) {
					reloc = (UINT64)(buff4pe)+(*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0xc])); relocsize = (*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0x8]));
				}
				if (strcmp(((char*)(&buff0[0x108 + (0x28 * cnt)])), ".text") == 0) {
					textaddr = (UINT64)(buff4pe)+(*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0xc])); textaddrsize = (*(UINT32*)(&buff0[0x108 + (0x28 * cnt) + 0x8]));
				}
				cnt++;
			}
		}
		UINT64 delta = ((UINT64)(buff4pe - baseaddr));
		UINT64 tmp4relocptx = 8;
		UINT32 armhi = 0;
		UINT32 armlo = 0;
		UINT32 armhi_ = 0;
		UINT32 armlo_ = 0;
		UINT64 deltatmp;
		HMODULE HM = 0;
		if (reloc == 0) { VirtualFree(buff4pe, 0, 0x8000); return 0; }

	loop4relocate:
		cnt = 0;
		for (cnt = 0; cnt < (((*(UINT32*)(reloc + (tmp4relocptx - 4))) - 8) / 2); cnt++) {
			switch (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) >> 12) & 0xF) {
			case 1:
				(*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) = (*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) + (UINT32)((delta >> 16) & 0xFFFF);
				break;
			case 2:
				(*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) = (*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) + (UINT32)((delta >> 0) & 0xFFFF);
				break;
			case 3:
				(*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) = (*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) + (UINT32)delta;
				break;
			case 7:
				armlo = (*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8))))));
				armlo_ = ((armlo << 1) & 0x0800) + ((armlo << 12) & 0xf000) + ((armlo >> 20) & 0x0700) + ((armlo >> 16) & 0x00ff);
				armhi = (*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))) + 4));
				armhi_ = ((armhi << 1) & 0x0800) + ((armhi << 12) & 0xf000) + ((armhi >> 20) & 0x0700) + ((armhi >> 16) & 0x00ff);
				deltatmp = (((armlo_ & 0xFFFF) << 0) | ((armhi_ & 0xFFFF) << 16)) + delta;
				armlo_ = (deltatmp >> 0) & 0xFFFF;
				armhi_ = (deltatmp >> 16) & 0xFFFF;
				armlo = (armlo & 0x8f00fbf0) + ((armlo_ >> 1) & 0x0400) + ((armlo_ >> 12) & 0x000f) + ((armlo_ << 20) & 0x70000000) + ((armlo_ << 16) & 0xff0000);
				armhi = (armhi & 0x8f00fbf0) + ((armhi_ >> 1) & 0x0400) + ((armhi_ >> 12) & 0x000f) + ((armhi_ << 20) & 0x70000000) + ((armhi_ << 16) & 0xff0000);
				(*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))))) = armlo;
				(*(UINT32*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))) + 4)) = armhi;
				break;
			case 10:
				(*(UINT64*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))) + 0)) = (*(UINT64*)(buff4pe + (((*(UINT16*)(reloc + (tmp4relocptx)+(cnt * 2))) & 0xFFF) + (*(UINT32*)(reloc + (tmp4relocptx - 8)))) + 0)) + delta;
				break;
			}
			//cnt++;
		}
		tmp4relocptx += (*(UINT32*)(reloc + (tmp4relocptx - 4)));
		if ((*(UINT32*)(reloc + (tmp4relocptx - 4))) != 0)
			goto loop4relocate;
		cnt = 0;
		if (textaddr != 0) {
			VirtualProtect((void*)(textaddr), textaddrsize, PAGE_EXECUTE_READWRITE, &tmp);
			FlushInstructionCache(GetCurrentProcess(), (void*)(textaddr), textaddrsize);
		}
		return buff4pe;
	}
	typedef BOOL APIENTRY typeofDllMain(HMODULE hModule,
		DWORD  ul_reason_for_call,
		LPVOID lpReserved
	);
	__declspec(dllexport) BOOL ULExecDllMain(char* prm_0, UINT32 prm_1) {
		return ((typeofDllMain*)(prm_0 + (*(UINT32*)(prm_0 + 0x28))))((HMODULE)prm_0, prm_1, NULL);
	}
	__declspec(dllexport) void* ULGetProcAddress(char* prm_0, char* prm_1) {
		UINT64 cnt = 0;
		UINT64 AddrOfFunction = 0;
		void* AddrOfFunctionaly = 0;
		if ((*(UINT16*)(&prm_0[0x18])) == 0x10b) {
			while (cnt < (*(UINT32*)(&prm_0[0] + (*(UINT32*)(&prm_0[0x78])) + 24))) {
				//32bit
				AddrOfFunction = (UINT64)(&prm_0[0] + (*(UINT32*)(&prm_0[0] + (*(UINT32*)(&prm_0[0x78])) + 28)));
				if (strcmp((char*)(&prm_0[0] + (*(UINT32*)(&prm_0[0] + (4 * cnt) + (*(UINT32*)(&prm_0[0] + (*(UINT32*)(&prm_0[0x78])) + 32))))), prm_1) == 0) {
					AddrOfFunctionaly = (&prm_0[0] + (*(UINT32*)(AddrOfFunction + ((*(UINT16*)(&prm_0[0] + (2 * cnt) + (*(UINT32*)(&prm_0[0] + (*(UINT32*)(&prm_0[0x78])) + 36)))) * 4))));
				}
				cnt++;
			}
		}
		else if ((*(UINT16*)(&prm_0[0x18])) == 0x20b) {
			while (cnt < (*(UINT32*)(&prm_0[0] + (*(UINT32*)(&prm_0[0x88])) + 24))) {
				//64bit
				AddrOfFunction = (UINT64)(&prm_0[0] + (*(UINT32*)(&prm_0[0] + (*(UINT32*)(&prm_0[0x88])) + 28)));
				if (strcmp((char*)(&prm_0[0] + (*(UINT32*)(&prm_0[0] + (4 * cnt) + (*(UINT32*)(&prm_0[0] + (*(UINT32*)(&prm_0[0x88])) + 32))))), prm_1) == 0) {
					AddrOfFunctionaly = (&prm_0[0] + (*(UINT32*)(AddrOfFunction + ((*(UINT16*)(&prm_0[0] + (2 * cnt) + (*(UINT32*)(&prm_0[0] + (*(UINT32*)(&prm_0[0x88])) + 36)))) * 4))));
				}
				cnt++;
			}
		}
		return AddrOfFunctionaly;
	}
	__declspec(dllexport) DWORD ULFreeLibrary(void* prm_0) { return VirtualFree(prm_0, 0, 0x8000); }
#ifdef __cplusplus
}
#endif

typedef struct
{
	ULONG   version;
	ULONG   unknown1[3];
	ULONG64 unknown2;
	ULONG64 pLdrInitializeThunk;
	ULONG64 pKiUserExceptionDispatcher;
	ULONG64 pKiUserApcDispatcher;
	ULONG64 pKiUserCallbackDispatcher;
	ULONG64 pRtlUserThreadStart;
	ULONG64 pRtlpQueryProcessDebugInformationRemote;
	ULONG64 ntdll_handle;
	ULONG64 pLdrSystemDllInitBlock;
	ULONG64 pRtlpFreezeTimeBias;
} SYSTEM_DLL_INIT_BLOCK;

SYSTEM_DLL_INIT_BLOCK* pLdrSystemDllInitBlock = NULL;

typedef NTSYSAPI PVOID t_RtlAllocateHeap(PVOID, ULONG, SIZE_T);
t_RtlAllocateHeap* RtlAllocateHeap = 0;
typedef NTSYSCALLAPI NTSTATUS t_NtSetInformationThread(HANDLE, THREADINFOCLASS, PVOID, ULONG);
t_NtSetInformationThread* NtSetInformationThread_alternative = 0;
typedef NTSTATUS WINAPI t_RtlWow64GetCurrentCpuArea(USHORT, void**, void**);
t_RtlWow64GetCurrentCpuArea* RtlWow64GetCurrentCpuArea = 0;
typedef __kernel_entry NTSTATUS t_NtQueryInformationThread(HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG);
t_NtQueryInformationThread* NtQueryInformationThread_alternative = 0;

typedef NTSTATUS WINAPI t_Wow64SystemServiceEx(UINT, UINT*);
t_Wow64SystemServiceEx* Wow64SystemServiceEx = 0;

HMODULE hmhm4dll;

char modulename4this[4096];

typedef NTSYSAPI NTSTATUS  WINAPI t_LdrDisableThreadCalloutsForDll(HMODULE);
t_LdrDisableThreadCalloutsForDll* LdrDisableThreadCalloutsForDll;

static NTSTATUS(WINAPI* p__wine_unix_call)(UINT64, unsigned int, void*);
typedef NTSTATUS WINAPI t__wine_unix_call(UINT64, unsigned int, void*);


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	HMODULE hofntdll = 0;
	HMODULE HM = 0;
	HMODULE HM2 = 0;
	HMODULE HMHM = 0;
	switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		GetModuleFileNameA(hModule, modulename4this, sizeof(modulename4this));
		hmhm4dll = hModule;
		hofntdll = LoadLibraryA("C:\\Windows\\System32\\ntdll.dll");
		if (hofntdll == 0) { return false; }
		LdrLoadDll = (t_LdrLoadDll*)GetProcAddress(hofntdll, "LdrLoadDll");
		LdrDisableThreadCalloutsForDll = (t_LdrDisableThreadCalloutsForDll*)GetProcAddress(hofntdll, "LdrDisableThreadCalloutsForDll");
		if (LdrDisableThreadCalloutsForDll == 0) { return false; }
		RtlAllocateHeap = (t_RtlAllocateHeap*)GetProcAddress(hofntdll, "RtlAllocateHeap");
		NtSetInformationThread_alternative = (t_NtSetInformationThread*)GetProcAddress(hofntdll, "NtSetInformationThread");
		NtQueryInformationThread_alternative = (t_NtQueryInformationThread*)GetProcAddress(hofntdll, "NtQueryInformationThread");
		RtlWow64GetCurrentCpuArea = (t_RtlWow64GetCurrentCpuArea*)GetProcAddress(hofntdll, "RtlWow64GetCurrentCpuArea");
		LdrDisableThreadCalloutsForDll(hModule);
		pLdrSystemDllInitBlock = (SYSTEM_DLL_INIT_BLOCK*)GetProcAddress(hofntdll, "LdrSystemDllInitBlock");
		if (pLdrSystemDllInitBlock != 0) {
			if (pLdrSystemDllInitBlock->ntdll_handle == 0) { pLdrSystemDllInitBlock->ntdll_handle = (ULONG64)hofntdll; }
		}
		HM2 = LoadLibraryA("C:\\Windows\\Sysnative\\Wow64.dll");
		if (HM2 == 0) { HM2 = LoadLibraryA("C:\\Windows\\System32\\Wow64.dll"); }
		if (HM2 == 0) { return false; }
		Wow64SystemServiceEx = (t_Wow64SystemServiceEx*)GetProcAddress(HM2, "Wow64SystemServiceEx");
		if (!p__wine_unix_call) {
			p__wine_unix_call = (t__wine_unix_call*)GetProcAddress(hofntdll, "__wine_unix_call");
		}
	case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#define CONTEXT_EXCEPTION_ACTIVE    0x08000000
#define CONTEXT_SERVICE_ACTIVE      0x10000000
#define CONTEXT_EXCEPTION_REQUEST   0x40000000
#define CONTEXT_EXCEPTION_REPORTING 0x80000000

#define CONTEXT_ARM    0x0200000
#define CONTEXT_ARM_CONTROL         (CONTEXT_ARM | 0x00000001)
#define CONTEXT_ARM_INTEGER         (CONTEXT_ARM | 0x00000002)
#define CONTEXT_ARM_FLOATING_POINT  (CONTEXT_ARM | 0x00000004)
#define CONTEXT_ARM_DEBUG_REGISTERS (CONTEXT_ARM | 0x00000008)
#define CONTEXT_ARM_FULL (CONTEXT_ARM_CONTROL | CONTEXT_ARM_INTEGER)
#define CONTEXT_ARM_ALL  (CONTEXT_ARM_FULL | CONTEXT_ARM_FLOATING_POINT | CONTEXT_ARM_DEBUG_REGISTERS)

#define ARM_MAX_BREAKPOINTS     8
#define ARM_MAX_WATCHPOINTS     1

#if 0

typedef struct _IMAGE_ARM_RUNTIME_FUNCTION
{
	DWORD BeginAddress;
	union {
		DWORD UnwindData;
		struct {
			DWORD Flag : 2;
			DWORD FunctionLength : 11;
			DWORD Ret : 2;
			DWORD H : 1;
			DWORD Reg : 3;
			DWORD R : 1;
			DWORD L : 1;
			DWORD C : 1;
			DWORD StackAdjust : 10;
		} DUMMYSTRUCTNAME;
	} DUMMYUNIONNAME;
} IMAGE_ARM_RUNTIME_FUNCTION_ENTRY, * PIMAGE_ARM_RUNTIME_FUNCTION_ENTRY;

typedef struct _SCOPE_TABLE_ARM
{
	DWORD Count;
	struct
	{
		DWORD BeginAddress;
		DWORD EndAddress;
		DWORD HandlerAddress;
		DWORD JumpTarget;
	} ScopeRecord[1];
} SCOPE_TABLE_ARM, * PSCOPE_TABLE_ARM;
#endif

typedef struct _ARM_NEON128
{
	ULONGLONG Low;
	LONGLONG High;
} ARM_NEON128;

typedef struct _ARM_CONTEXT
{
	ULONG ContextFlags;             /* 000 */
	/* CONTEXT_INTEGER */
	ULONG R0;                       /* 004 */
	ULONG R1;                       /* 008 */
	ULONG R2;                       /* 00c */
	ULONG R3;                       /* 010 */
	ULONG R4;                       /* 014 */
	ULONG R5;                       /* 018 */
	ULONG R6;                       /* 01c */
	ULONG R7;                       /* 020 */
	ULONG R8;                       /* 024 */
	ULONG R9;                       /* 028 */
	ULONG R10;                      /* 02c */
	ULONG R11;                      /* 030 */
	ULONG R12;                      /* 034 */
	/* CONTEXT_CONTROL */
	ULONG Sp;                       /* 038 */
	ULONG Lr;                       /* 03c */
	ULONG Pc;                       /* 040 */
	ULONG Cpsr;                     /* 044 */
	/* CONTEXT_FLOATING_POINT */
	ULONG Fpscr;                    /* 048 */
	ULONG Padding;                  /* 04c */
	union
	{
		ARM_NEON128 Q[16];
		ULONGLONG D[32];
		ULONG S[32];
	} DUMMYUNIONNAME;               /* 050 */
	/* CONTEXT_DEBUG_REGISTERS */
	ULONG Bvr[ARM_MAX_BREAKPOINTS]; /* 150 */
	ULONG Bcr[ARM_MAX_BREAKPOINTS]; /* 170 */
	ULONG Wvr[ARM_MAX_WATCHPOINTS]; /* 190 */
	ULONG Wcr[ARM_MAX_WATCHPOINTS]; /* 194 */
	ULONG Padding2[2];              /* 198 */
} ARM_CONTEXT;

char bopcode[] = { 0x00,0x00,0x00,0xef,0x0e,0xf0,0xa0,0xe1 };
char unixbopcode[] = { 0x01,0x00,0x00,0xef,0x0e,0xf0,0xa0,0xe1 };
#ifndef ThreadWow64Context
#define ThreadWow64Context (THREADINFOCLASS)0x1d
#endif

#ifndef STATUS_INVALID_ADDRESS
#define STATUS_INVALID_ADDRESS -1073741503
#endif

#ifndef STATUS_SUCCESS
#define STATUS_SUCCESS 0
#endif

typedef struct _GDI_TEB_BATCH
{
	ULONG  Offset;
	HANDLE HDC;
	ULONG  Buffer[0x136];
} GDI_TEB_BATCH;

typedef struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME
{
	struct _RTL_ACTIVATION_CONTEXT_STACK_FRAME* Previous;
	struct _ACTIVATION_CONTEXT* ActivationContext;
	ULONG                                       Flags;
} RTL_ACTIVATION_CONTEXT_STACK_FRAME, * PRTL_ACTIVATION_CONTEXT_STACK_FRAME;

typedef struct _ACTIVATION_CONTEXT_STACK
{
	RTL_ACTIVATION_CONTEXT_STACK_FRAME* ActiveFrame;
	LIST_ENTRY                          FrameListCache;
	ULONG                               Flags;
	ULONG                               NextCookieSequenceNumber;
	ULONG_PTR                           StackId;
} ACTIVATION_CONTEXT_STACK, * PACTIVATION_CONTEXT_STACK;

typedef struct _TEB_ACTIVE_FRAME_CONTEXT
{
	ULONG       Flags;
	const char* FrameName;
} TEB_ACTIVE_FRAME_CONTEXT, * PTEB_ACTIVE_FRAME_CONTEXT;

typedef struct _TEB_ACTIVE_FRAME_CONTEXT_EX
{
	TEB_ACTIVE_FRAME_CONTEXT BasicContext;
	const char* SourceLocation;
} TEB_ACTIVE_FRAME_CONTEXT_EX, * PTEB_ACTIVE_FRAME_CONTEXT_EX;

typedef struct _TEB_ACTIVE_FRAME
{
	ULONG                     Flags;
	struct _TEB_ACTIVE_FRAME* Previous;
	TEB_ACTIVE_FRAME_CONTEXT* Context;
} TEB_ACTIVE_FRAME, * PTEB_ACTIVE_FRAME;

typedef struct _TEB_ACTIVE_FRAME_EX
{
	TEB_ACTIVE_FRAME BasicFrame;
	void* ExtensionIdentifier;
} TEB_ACTIVE_FRAME_EX, * PTEB_ACTIVE_FRAME_EX;

typedef struct _TEB_FLS_DATA
{
	LIST_ENTRY      fls_list_entry;
	void** fls_data_chunks[8];
} TEB_FLS_DATA, * PTEB_FLS_DATA;

typedef struct ___TEB
{                                                                 /* win32/win64 */
	NT_TIB                       Tib;                               /* 000/0000 */
	PVOID                        EnvironmentPointer;                /* 01c/0038 */
	CLIENT_ID                    ClientId;                          /* 020/0040 */
	PVOID                        ActiveRpcHandle;                   /* 028/0050 */
	PVOID                        ThreadLocalStoragePointer;         /* 02c/0058 */
	PPEB                         Peb;                               /* 030/0060 */
	ULONG                        LastErrorValue;                    /* 034/0068 */
	ULONG                        CountOfOwnedCriticalSections;      /* 038/006c */
	PVOID                        CsrClientThread;                   /* 03c/0070 */
	PVOID                        Win32ThreadInfo;                   /* 040/0078 */
	ULONG                        User32Reserved[26];                /* 044/0080 */
	ULONG                        UserReserved[5];                   /* 0ac/00e8 */
	PVOID                        WOW32Reserved;                     /* 0c0/0100 */
	ULONG                        CurrentLocale;                     /* 0c4/0108 */
	ULONG                        FpSoftwareStatusRegister;          /* 0c8/010c */
	PVOID                        ReservedForDebuggerInstrumentation[16]; /* 0cc/0110 */
#ifdef _WIN64
	PVOID                        SystemReserved1[30];               /*    /0190 */
#else
	PVOID                        SystemReserved1[26];               /* 10c/     used for krnl386 private data in Wine */
#endif
	char                         PlaceholderCompatibilityMode;      /* 174/0280 */
	char                         PlaceholderReserved[11];           /* 175/0281 */
	DWORD                        ProxiedProcessId;                  /* 180/028c */
	ACTIVATION_CONTEXT_STACK     ActivationContextStack;            /* 184/0290 */
	UCHAR                        WorkingOnBehalfOfTicket[8];        /* 19c/02b8 */
	LONG                         ExceptionCode;                     /* 1a4/02c0 */
	ACTIVATION_CONTEXT_STACK* ActivationContextStackPointer;     /* 1a8/02c8 */
	ULONG_PTR                    InstrumentationCallbackSp;         /* 1ac/02d0 */
	ULONG_PTR                    InstrumentationCallbackPreviousPc; /* 1b0/02d8 */
	ULONG_PTR                    InstrumentationCallbackPreviousSp; /* 1b4/02e0 */
#ifdef _WIN64
	ULONG                        TxFsContext;                       /*    /02e8 */
	BOOLEAN                      InstrumentationCallbackDisabled;   /*    /02ec */
#else
	BOOLEAN                      InstrumentationCallbackDisabled;   /* 1b8/     */
	BYTE                         SpareBytes1[23];                   /* 1b9/     */
	ULONG                        TxFsContext;                       /* 1d0/     */
#endif
	GDI_TEB_BATCH                GdiTebBatch;                       /* 1d4/02f0 used for ntdll private data in Wine */
	CLIENT_ID                    RealClientId;                      /* 6b4/07d8 */
	HANDLE                       GdiCachedProcessHandle;            /* 6bc/07e8 */
	ULONG                        GdiClientPID;                      /* 6c0/07f0 */
	ULONG                        GdiClientTID;                      /* 6c4/07f4 */
	PVOID                        GdiThreadLocaleInfo;               /* 6c8/07f8 */
	ULONG_PTR                    Win32ClientInfo[62];               /* 6cc/0800 used for user32 private data in Wine */
	PVOID                        glDispatchTable[233];              /* 7c4/09f0 */
	PVOID                        glReserved1[29];                   /* b68/1138 */
	PVOID                        glReserved2;                       /* bdc/1220 */
	PVOID                        glSectionInfo;                     /* be0/1228 */
	PVOID                        glSection;                         /* be4/1230 */
	PVOID                        glTable;                           /* be8/1238 */
	PVOID                        glCurrentRC;                       /* bec/1240 */
	PVOID                        glContext;                         /* bf0/1248 */
	ULONG                        LastStatusValue;                   /* bf4/1250 */
	UNICODE_STRING               StaticUnicodeString;               /* bf8/1258 */
	WCHAR                        StaticUnicodeBuffer[261];          /* c00/1268 */
	PVOID                        DeallocationStack;                 /* e0c/1478 */
	PVOID                        TlsSlots[64];                      /* e10/1480 */
	LIST_ENTRY                   TlsLinks;                          /* f10/1680 */
	PVOID                        Vdm;                               /* f18/1690 */
	PVOID                        ReservedForNtRpc;                  /* f1c/1698 */
	PVOID                        DbgSsReserved[2];                  /* f20/16a0 */
	ULONG                        HardErrorDisabled;                 /* f28/16b0 */
	PVOID                        Instrumentation[16];               /* f2c/16b8 */
	PVOID                        WinSockData;                       /* f6c/1738 */
	ULONG                        GdiBatchCount;                     /* f70/1740 */
	ULONG                        Spare2;                            /* f74/1744 */
	ULONG                        GuaranteedStackBytes;              /* f78/1748 */
	PVOID                        ReservedForPerf;                   /* f7c/1750 */
	PVOID                        ReservedForOle;                    /* f80/1758 */
	ULONG                        WaitingOnLoaderLock;               /* f84/1760 */
	PVOID                        Reserved5[3];                      /* f88/1768 */
	PVOID* TlsExpansionSlots;                 /* f94/1780 */
#ifdef _WIN64
	PVOID                        DeallocationBStore;                /*    /1788 */
	PVOID                        BStoreLimit;                       /*    /1790 */
#endif
	ULONG                        ImpersonationLocale;               /* f98/1798 */
	ULONG                        IsImpersonating;                   /* f9c/179c */
	PVOID                        NlsCache;                          /* fa0/17a0 */
	PVOID                        ShimData;                          /* fa4/17a8 */
	ULONG                        HeapVirtualAffinity;               /* fa8/17b0 */
	PVOID                        CurrentTransactionHandle;          /* fac/17b8 */
	TEB_ACTIVE_FRAME* ActiveFrame;                       /* fb0/17c0 */
	TEB_FLS_DATA* FlsSlots;                          /* fb4/17c8 */
	PVOID                        PreferredLanguages;                /* fb8/17d0 */
	PVOID                        UserPrefLanguages;                 /* fbc/17d8 */
	PVOID                        MergedPrefLanguages;               /* fc0/17e0 */
	ULONG                        MuiImpersonation;                  /* fc4/17e8 */
	USHORT                       CrossTebFlags;                     /* fc8/17ec */
	USHORT                       SameTebFlags;                      /* fca/17ee */
	PVOID                        TxnScopeEnterCallback;             /* fcc/17f0 */
	PVOID                        TxnScopeExitCallback;              /* fd0/17f8 */
	PVOID                        TxnScopeContext;                   /* fd4/1800 */
	ULONG                        LockCount;                         /* fd8/1808 */
	LONG                         WowTebOffset;                      /* fdc/180c */
	PVOID                        ResourceRetValue;                  /* fe0/1810 */
	PVOID                        ReservedForWdf;                    /* fe4/1818 */
	ULONGLONG                    ReservedForCrt;                    /* fe8/1820 */
	GUID                         EffectiveContainerId;              /* ff0/1828 */
} __TEB, * __PTEB;

typedef struct _WOW64INFO
{
	ULONG   NativeSystemPageSize;
	ULONG   CpuFlags;
	ULONG   Wow64ExecuteFlags;
	ULONG   unknown[5];
	USHORT  NativeMachineType;
	USHORT  EmulatedMachineType;
} WOW64INFO;

UINT32 memaccessfunc(UINT32 prm_0, UINT32 prm_1, UINT32 prm_2) {
	switch (prm_2 & 0xF) {
	case 0:
		switch ((prm_2 >> 4) & 0xF) {
		case 0:
			*(UINT8*)(prm_0) = (UINT8)(prm_1 & 0xFF);
			return 0;
			break;
		case 1:
			*(UINT16*)(prm_0) = (UINT16)(prm_1 & 0xFFFF);
			return 0;
			break;
		case 2:
			*(UINT32*)(prm_0) = (UINT32)(prm_1 & 0xFFFFFFFF);
			return 0;
			break;
		}
		return 0;
		break;
	case 1:
		switch ((prm_2 >> 4) & 0xF) {
		case 0:
			return *(UINT8*)(prm_0);
			break;
		case 1:
			return *(UINT16*)(prm_0);
			break;
		case 2:
			return *(UINT32*)(prm_0);
			break;
		}
		return 0;
		break;
	}
	return 0;
}

#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllexport) void* WINAPI BTCpuGetBopCode(void) { return (UINT32*)&bopcode; }
    __declspec(dllexport) NTSTATUS WINAPI BTCpuGetContext(HANDLE thread, HANDLE process, void* unknown, ARM_CONTEXT* ctx) { return NtQueryInformationThread_alternative(thread, ThreadWow64Context, ctx, sizeof(*ctx), NULL); }
    __declspec(dllexport) NTSTATUS WINAPI BTCpuProcessInit(void) { if ((ULONG_PTR)BTCpuProcessInit >> 32) { return STATUS_INVALID_ADDRESS; } return STATUS_SUCCESS; }
    __declspec(dllexport) NTSTATUS WINAPI BTCpuThreadInit(void) { return STATUS_SUCCESS; }
    __declspec(dllexport) NTSTATUS WINAPI BTCpuResetToConsistentState(EXCEPTION_POINTERS* ptrs) { return STATUS_SUCCESS; }
    __declspec(dllexport) NTSTATUS WINAPI BTCpuSetContext(HANDLE thread, HANDLE process, void* unknown, ARM_CONTEXT* ctx) { return NtSetInformationThread_alternative(thread, ThreadWow64Context, ctx, sizeof(*ctx)); }
    __declspec(dllexport) void WINAPI BTCpuSimulate(void) {
		ARM_CONTEXT* wow_context;
		NTSTATUS ret;
		RtlWow64GetCurrentCpuArea(NULL, (void**)&wow_context, NULL);
		IRQ* emuirq = new IRQ(0xF8000000);
		DTimer* emudtimer = new DTimer(0xFC000000, 0, emuirq);
		Mem* emumem = new Mem(memaccessfunc, 2);
		ARMV7_CPU* armv7cpu = new ARMV7_CPU(emumem,emudtimer);
		armv7cpu->regs[0] = wow_context->R0;
		armv7cpu->regs[1] = wow_context->R1;
		armv7cpu->regs[2] = wow_context->R2;
		armv7cpu->regs[3] = wow_context->R3;
		armv7cpu->regs[4] = wow_context->R4;
		armv7cpu->regs[5] = wow_context->R5;
		armv7cpu->regs[6] = wow_context->R6;
		armv7cpu->regs[7] = wow_context->R7;
		armv7cpu->regs[8] = wow_context->R8;
		armv7cpu->regs[9] = wow_context->R9;
		armv7cpu->regs[10] = wow_context->R10;
		armv7cpu->regs[11] = wow_context->R11;
		armv7cpu->regs[12] = wow_context->R12;
		armv7cpu->regs[13] = wow_context->Sp;
		armv7cpu->regs[14] = wow_context->Lr;
		armv7cpu->regs[15] = wow_context->Pc;
		armv7cpu->cpsr = armv7cpu->parse_psr(wow_context->Cpsr);
		while (((armv7cpu->regs[15]) & 0xFFFFFFFE) != (((UINT32)&bopcode) & 0xFFFFFFFE) && ((armv7cpu->regs[15]) & 0xFFFFFFFE) != (((UINT32)&unixbopcode) & 0xFFFFFFFE)) {
			{
				armv7cpu->regs[15] &= 0xFFFFFFFE;
				armv7cpu->regs[15] |= ((armv7cpu->cpsr.t) & 1);
				if (armv7cpu->cpsr.t) {// Thumb-2
					UINT64 opcodeaddr4arm = armv7cpu->regs[15] & 0xFFFFFFFE;
					UINT64 opcode4arm = armv7cpu->fetch_instruction(opcodeaddr4arm);
					armv7cpu->exec(armv7cpu->decode_thumb(opcode4arm, opcodeaddr4arm), opcode4arm, opcodeaddr4arm);
					armv7cpu->regs[15] += 2;
				}
				else 				  {// AArch32
					UINT64 opcodeaddr4arm = armv7cpu->regs[15] & 0xFFFFFFFC;
					UINT64 opcode4arm = armv7cpu->fetch_instruction(opcodeaddr4arm);
					armv7cpu->exec(armv7cpu->decode(opcode4arm, opcodeaddr4arm), opcode4arm, opcodeaddr4arm);
					armv7cpu->regs[15] += 4;
				}
				armv7cpu->cpsr.t = (armv7cpu->regs[15] & 1);
			}
		}
		wow_context->R0  = armv7cpu->regs[0];
		wow_context->R1  = armv7cpu->regs[1];
		wow_context->R2  = armv7cpu->regs[2];
		wow_context->R3  = armv7cpu->regs[3];
		wow_context->R4  = armv7cpu->regs[4];
		wow_context->R5  = armv7cpu->regs[5];
		wow_context->R6  = armv7cpu->regs[6];
		wow_context->R7  = armv7cpu->regs[7];
		wow_context->R8  = armv7cpu->regs[8];
		wow_context->R9  = armv7cpu->regs[9];
		wow_context->R10 = armv7cpu->regs[10];
		wow_context->R11 = armv7cpu->regs[11];
		wow_context->R12 = armv7cpu->regs[12];
		wow_context->Sp  = armv7cpu->regs[13];
		wow_context->Lr  = armv7cpu->regs[14];
		wow_context->Pc  = armv7cpu->regs[15];
		wow_context->Cpsr = armv7cpu->psr_to_value(&armv7cpu->cpsr);
		UINT64 arm32pcaddr = armv7cpu->regs[15];
		delete(armv7cpu);
		delete(emumem);
		delete(emudtimer);
		delete(emuirq);
		if (((arm32pcaddr) & 0xFFFFFFFE) == (((UINT32)&bopcode) & 0xFFFFFFFE)) {
			wow_context->R0 = Wow64SystemServiceEx(wow_context->R12, (UINT*)ULongToPtr(wow_context->Sp));
			wow_context->Pc = wow_context->Lr;
			wow_context->Lr = wow_context->R3;
			wow_context->Sp += (4 * 4);
		}
		else if (((arm32pcaddr) & 0xFFFFFFFE) == (((UINT32)&unixbopcode) & 0xFFFFFFFE)) {
			wow_context->R0  = p__wine_unix_call(((UINT64)((((UINT64)wow_context->R0) << (32 * 0)) | (((UINT64)wow_context->R1) << (32 * 1)))), wow_context->R2, (UINT*)ULongToPtr(wow_context->R3));
			wow_context->Pc = wow_context->Lr;
		}
    }
	__declspec(dllexport) void* WINAPI __wine_get_unix_opcode(void) { return (UINT32*)&unixbopcode; }
	__declspec(dllexport) BOOLEAN WINAPI BTCpuIsProcessorFeaturePresent(UINT feature) { if (feature == 2 || feature == 3 || feature == 6 || feature == 7 || feature == 8 || feature == 10 || feature == 13 || feature == 17 || feature == 36 || feature == 37 || feature == 38) { return true; } return false; }
	__declspec(dllexport) NTSTATUS WINAPI BTCpuTurboThunkControl(ULONG enable) { if (enable) { return STATUS_NOT_SUPPORTED; } return STATUS_SUCCESS; }

#ifdef __cplusplus
}
#endif
