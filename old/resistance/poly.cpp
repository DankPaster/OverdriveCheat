#include "poly.h"

struct nop_equ 
{
	char* opcode;
	int len;
};

nop_equ possible_opcodes[] =
{
	{ "\x90",1 },			//nop
	{ "\xD9\xD0",2 },		//fnop
	{ "\x8B\xC0",2 },		//mov     eax, eax
	{ "\x8B\xDB",2 },		//mov     ebx, ebx
	{ "\x8B\xC9",2 },		//mov     ecx, ecx
	{ "\x8B\xD2",2 },		//mov     edx, edx
	{ "\x8B\xE4",2 },		//mov     esp, esp
	{ "\x8B\xF6",2 },		//mov     esi, esi
	{ "\x8B\xFF",2 },		//mov     edi, edi
	{ "\x87\xDB",2 },		//xchg    ebx, ebx
	{ "\x87\xC9",2 },		//xchg    ecx, ecx
	{ "\x87\xD2",2 },		//xchg    edx, edx
	{ "\x87\xE4",2 },		//xchg    esp, esp
	{ "\x87\xED",2 },		//xchg    ebp, ebp
	{ "\x87\xF6",2 },		//xchg    esi, esi
	{ "\x87\xFF",2 },		//xchg    edi, edi
	{ "\x50\x58",2 },		//push,pop eax
	{ "\x53\x5B",2 },		//push,pop ebx
	{ "\x51\x59",2 },		//push,pop ecx
	{ "\x52\x5A",2 },		//push,pop edx
	{ "\x56\x5E",2 },		//push,pop esi
	{ "\x57\x5F",2 },		//push,pop edi
	{ "\x83\xC8\x00",3 },	//or eax, 0
	{ "\x83\xC9\x00",3 },	//or ecx, 0
	{ "\x83\xCA\x00",3 },	//or edx, 0
	{ "\x83\xCB\x00",3 },	//or ebx, 0
	{ "\x83\xCC\x00",3 },	//or esp, 0
	{ "\x83\xCD\x00",3 },	//or ebp, 0
	{ "\x83\xCE\x00",3 },	//or esi, 0
	{ "\x83\xCF\x00",3 },	//or edi, 0
	{ "\x83\xE0\xFF",3 },	//and eax, FFFFFFFF
	{ "\x83\xE1\xFF",3 },	//and ecx, FFFFFFFF
	{ "\x83\xE2\xFF",3 },	//and edx, FFFFFFFF
	{ "\x83\xE3\xFF",3 },	//and ebx, FFFFFFFF
	{ "\x83\xE4\xFF",3 },	//and esp, FFFFFFFF
	{ "\x83\xE5\xFF",3 },	//and ebp, FFFFFFFF
	{ "\x83\xE6\xFF",3 },	//and esi, FFFFFFFF
	{ "\x83\xE7\xFF",3 },	//and edi, FFFFFFFF
	{ "\x83\xF0\x00",3 },	//xor eax, 0
	{ "\x83\xF1\x00",3 },	//xor ecx, 0
	{ "\x83\xF2\x00",3 },	//xor edx, 0
	{ "\x83\xF3\x00",3 },	//xor ebx, 0
	{ "\x83\xF4\x00",3 },	//xor esp, 0
	{ "\x83\xF5\x00",3 },	//xor ebp, 0
	{ "\x83\xF6\x00",3 },	//xor esi, 0
	{ "\x83\xF7\x00",3 },	//xor edi, 0
	{ 0,0 }
};

void __stdcall FreeJumpgate(PBYTE target)
{
	VirtualFreeEx(INVALID_HANDLE_VALUE, target, 0, MEM_RELEASE);
}

PBYTE __stdcall AllocateJumpgate(PBYTE target, int minlen, int maxlen)
{
	PBYTE buf;
	DWORD dwOld;
	int iLen = GenRand(minlen, maxlen);
	buf = (PBYTE)VirtualAllocEx(INVALID_HANDLE_VALUE, NULL, iLen + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	real_AddNops(buf, buf + iLen);
	WriteJmp(buf + iLen, target);
	VirtualProtectEx(INVALID_HANDLE_VALUE, buf, iLen, PAGE_EXECUTE_READ, &dwOld);
	return buf;
}

void __stdcall _AddNops(PBYTE start, PBYTE end)
{
	DWORD dwOld, dwOld2;
	VirtualProtectEx(INVALID_HANDLE_VALUE, start, end - start, PAGE_EXECUTE_READWRITE, &dwOld);
	real_AddNops(start, end);
	VirtualProtectEx(INVALID_HANDLE_VALUE, start, end - start, dwOld, &dwOld2);
}


void WriteJmp(const PBYTE from, const PBYTE to)
{
	*from = 0xE9;
	PDWORD dwJmp = (PDWORD)(from + 1);
	*dwJmp = (int)to - (int)from - 5;
}

int GenRand(int min, int max)
{
	UINT number = 0;
	__asm push edx
	__asm rdtsc
	__asm pop edx
	__asm mov number, eax
	number %= (max - min);
	return number + min;
}

//perform the actual nopping, called from macro's trough _AddNops, or from AllocateJumpgate
void real_AddNops(PBYTE start, PBYTE end)
{
	int iLeft = end - start;
	while (iLeft) {
		if (GenRand(0, 7) < 5) {
			nop_equ* p = possible_opcodes + GenRand(0, ARRAYSIZE(possible_opcodes));
			if (!p->len) continue;
			if (p->len > iLeft) continue;
			for (int i = 0; i < p->len; i++)
				end[-iLeft + i] = p->opcode[i];
			iLeft -= p->len;
		}
		else {
			end[-(iLeft--)] = 0xEB;
			int iSize = GenRand(0, iLeft);
			end[-(iLeft--)] = (BYTE)(iSize & 0xFF);
			while (iSize) {
				end[-iLeft] = (BYTE)(GenRand(0, 256) & 0xFF);
				iSize--;
				iLeft--;
			}
		}
		if (iLeft == 1) {
			end[-1] = 0x90;
			break;
		}
	}
}

void OBFUSCATE()
{
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP10);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP10);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP10);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP10);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP10);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP10);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP10);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP10);
	OBFUSCATENOPS(NOP20);
	OBFUSCATENOPS(NOP5);
	OBFUSCATENOPS(NOP5);
}