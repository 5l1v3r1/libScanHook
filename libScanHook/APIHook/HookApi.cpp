/*
//////////////////////////////////////////////////////////////////////////
HookApi 0.6

thanks to xIkUg ,sucsor

by ������Ӱ[RCT] , StrongOD@Safengine.com
2011.06.08

//////////////////////////////////////////////////////////////////////////
//��������
2011.06.08	0.6
1������cdecl��hook
2��ʹ��malloc�����ڴ棬��Լ�ռ�
3���½ӿ�InstallHookStub��֧��ֱ�Ӵ�������ַȥhook
4��hook��û��ɵ�ʱ�򣬲��ᷢ������hookproc�����(��Ҫ��VirtualProtect����)

2008.04.15  0.5

1������д��Stub������һ��ģʽ��ʹhook�������ɣ���hookbefore��hookafter�ϲ�
HookProc�Ķ��巽ʽ����ǰ������ͬ��

HookProc�ĺ������ͺ�ԭ����apiһ����ֻ�ǲ�����ԭAPI��2��
DWORD WINAPI HookProc(DWORD RetAddr ,__pfnXXXX pfnXXXX, ...);

//������ԭʼ��API��2������
RetAddr	//����api�ķ��ص�ַ
pfnXXX 	//����Ϊ__pfnXXXX����hook��api���������ͣ����ڵ���δ��hook��api

���My_LoadLibraryA
ԭʼ��LoadLibraryA�������ǣ�

HMODULE WINAPI LoadLibraryA( LPCSTR lpLibFileName );

��ô���ȶ���һ��hook��WINAPI������
typedef HMODULE (WINAPI __pfnLoadLibraryA)(LPCTSTR lpFileName);

Ȼ��hookproc�ĺ����������£�
HMODULE WINAPI My_LoadLibraryA(DWORD RetAddr,
							   __pfnLoadLibraryA pfnLoadLibraryA,
							   LPCTSTR lpFileName
							   );

��ԭ���Ķ���2������������λ�ò��ܵߵ�����My_LoadLibraryA�п������ɵĵ���δ��hook��pfnLoadLibraryA
Ҳ���Ե���ϵͳ��LoadLibraryA������Ҫ�Լ���hookproc�д������������

���⣬Ҳ������My_LoadLibraryA��ʹ��UnInstallHookApi()������ж��hook���÷����£�
���ڶ�������__pfnLoadLibraryA pfnLoadLibraryAǿ��ת����PHOOKENVIRONMENT���ͣ�ʹ��UnInstallHookApi��ж��

���磺
UnInstallHookApi((PHOOKENVIRONMENT)pfnLoadLibraryA);


������ǰ�汾��HookBefore��HookAfter����ȫ�������Լ���HookProc�������ʹ����


2��֧��ж��hook
InstallHookApi()���ú�᷵��һ��PHOOKENVIRONMENT���͵�ָ��
��Ҫж�ص�ʱ�����ʹ��UnInstallHookApi(PHOOKENVIRONMENT pHookEnv)��ж��

��HookProc��Ҳ����ʹ��UnInstallHookApi��ж�أ���������HookProc�еĵڶ�������

ע�⣺��HookProc��ʹ��UnInstallHookApiж�����Ͳ����õڶ�������������API��~~���мǣ�

2008.04.15  0.41
1��ǰ���deroko��LdeX86 ��BUG��678b803412 �����
	����һ��LDX32��������٣����������

2���޸���VirtualProtect��һ��СBUG


0.4��ǰ
�Ķ�̫���ˣ�ǰ��ľͲ�д��
*/


#include <windows.h>
#include <stdio.h>
#include "HookApi.h"


#pragma comment(linker, "/SECTION:HookStub,R")

#define ALLOCATE_HookStub ALLOCATE(HookStub)

#pragma code_seg("HookStub")
#pragma optimize("gsy",on)
ALLOCATE_HookStub HOOKENVIRONMENT pEnv={0};
NAKED void StubShell_stdcall()
{
	__asm
	{
		push [esp];
		push [esp];
		call _next;
_next:
		xchg dword ptr [esp], eax;
		lea eax, [eax - 0x20];
		mov dword ptr [esp + 0xC], eax;
		pop eax;
		_emit 0xE9;
		_emit 'g';
		_emit 'o';
		_emit 'o';
		_emit 'd';
	}
}

NAKED void StubShell_cdecl()
{
	__asm
	{
		push [esp];
		call _next;
_next:
		xchg dword ptr [esp], eax;
		lea eax, [eax - 0x1D];
		mov dword ptr [esp + 0x8], eax;
		pop eax;
		_emit 0x68;
		_emit 'b';
		_emit 'a';
		_emit 'd';
		_emit 'd';
		_emit 0xE9;
		_emit 'g';
		_emit 'o';
		_emit 'o';
		_emit 'd';
	}
}
#pragma optimize("",off)
#pragma code_seg()

NAKED void cdeclret_stub()
{
	__asm retn 4;
}
LONG MyInterlockedExchange32(PLONG Target, LONG Value)
{
	LONG retvalue;
	__asm
	{
		mov ecx, Target;
		mov eax, Value;
		xchg dword ptr[ecx], eax;
		mov retvalue, eax;
	}
	return retvalue;
}
SHORT MyInterlockedExchange16(PSHORT Target, SHORT Value)
{
	SHORT retvalue;
	__asm
	{
		mov ecx, Target;
		mov ax, Value;
		xchg word ptr[ecx], ax;
		mov retvalue, ax;
	}
	return retvalue;
}

DWORD __stdcall GetOpCodeSize(BYTE* iptr0)
{
	BYTE* iptr = iptr0;

	DWORD f = 0;

prefix:
	BYTE b = *iptr++;

	f |= table_1[b];

	if (f&C_FUCKINGTEST)
		if (((*iptr)&0x38)==0x00)   // ttt
			f=C_MODRM+C_DATAW0;       // TEST
		else
			f=C_MODRM;                // NOT,NEG,MUL,IMUL,DIV,IDIV

	if (f&C_TABLE_0F)
	{
		b = *iptr++;
		f = table_0F[b];
	}

	if (f==C_ERROR)
	{
		//printf("error in %02X\n",b);
		return C_ERROR;
	}

	if (f&C_PREFIX)
	{
		f&=~C_PREFIX;
		goto prefix;
	}

	if (f&C_DATAW0) if (b&0x01) f|=C_DATA66; else f|=C_DATA1;

	if (f&C_MODRM)
	{
		b = *iptr++;
		BYTE mod = b & 0xC0;
		BYTE rm  = b & 0x07;
		if (mod!=0xC0)
		{
			if (f&C_67)         // modrm16
			{
				if ((mod==0x00)&&(rm==0x06)) f|=C_MEM2;
				if (mod==0x40) f|=C_MEM1;
				if (mod==0x80) f|=C_MEM2;
			}
			else                // modrm32
			{
				if (mod==0x40) f|=C_MEM1;
				if (mod==0x80) f|=C_MEM4;
				if (rm==0x04) rm = (*iptr++) & 0x07;    // rm<-sib.base
				if ((rm==0x05)&&(mod==0x00)) f|=C_MEM4;
			}
		}
	} // C_MODRM

	if (f&C_MEM67)  if (f&C_67) f|=C_MEM2;  else f|=C_MEM4;
	if (f&C_DATA66) if (f&C_66) f|=C_DATA2; else f|=C_DATA4;

	if (f&C_MEM1)  iptr++;
	if (f&C_MEM2)  iptr+=2;
	if (f&C_MEM4)  iptr+=4;

	if (f&C_DATA1) iptr++;
	if (f&C_DATA2) iptr+=2;
	if (f&C_DATA4) iptr+=4;

	return iptr - iptr0;
}

PHOOKENVIRONMENT __stdcall InstallHookStub(PVOID StubAddress, PVOID HookProc, int type)
{
	int ReplaceCodeSize;
	DWORD oldpro;
	DWORD SizeOfStub;

	DWORD SizeOfStubShell = 0;
	DWORD AddrOfStubShell = 0;

	DWORD dwHookStubAddress;
	DWORD RetSize =0;

	PHOOKENVIRONMENT pHookEnv;

	if (HookProc == NULL)
	{
		return NULL;
	}

	if (StubAddress == NULL) return NULL;

	if(type == e_stdcall)
	{
		SizeOfStubShell = 0x1B;
		AddrOfStubShell = (DWORD)StubShell_stdcall;
	}
	else if (type == e_cdecl)
	{
		SizeOfStubShell = 0x1D;
		AddrOfStubShell = (DWORD)StubShell_cdecl;
	}

#ifdef _DEBUG
	AddrOfStubShell = AddrOfStubShell + 5 + *(DWORD*)(AddrOfStubShell + 1);
#endif

	ReplaceCodeSize = GetOpCodeSize((BYTE*)StubAddress);

	while (ReplaceCodeSize < 5)
	{
		ReplaceCodeSize += GetOpCodeSize((BYTE*)((DWORD)StubAddress + (DWORD)ReplaceCodeSize));
	}

	if (ReplaceCodeSize > 16) return NULL;

	SizeOfStub = SizeOfStubShell + sizeof(HOOKENVIRONMENT);

	//pHookEnv = (PHOOKENVIRONMENT)VirtualAlloc(NULL, SizeOfStub, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	pHookEnv = (PHOOKENVIRONMENT)malloc(sizeof(HOOKENVIRONMENT));
	if(!VirtualProtect((LPVOID)pHookEnv, sizeof(HOOKENVIRONMENT), PAGE_EXECUTE_READWRITE, &pHookEnv->oldpro))
	{
		free((void*)pHookEnv);
		return NULL;
	}

	memcpy(pHookEnv, (PVOID)&pEnv, sizeof(HOOKENVIRONMENT));
	memset((void*)pHookEnv->savebytes, 0x90, sizeof(pHookEnv->savebytes));
	memcpy((void*)pHookEnv->hookstub, (PVOID)AddrOfStubShell, SizeOfStubShell);
	memcpy(pHookEnv->savebytes, StubAddress, ReplaceCodeSize);

	pHookEnv->OrgApiAddr = StubAddress;
	pHookEnv->SizeOfReplaceCode = ReplaceCodeSize;

	pHookEnv->jmptoapi[0] = 0xE9;
	*(DWORD*)(&pHookEnv->jmptoapi[1]) = (DWORD)StubAddress + ReplaceCodeSize - ((DWORD)pHookEnv->jmptoapi + 5);

	dwHookStubAddress = (DWORD)pHookEnv->hookstub;

	pHookEnv->jmptostub[0] = 0xE9;
	*(DWORD*)(&pHookEnv->jmptostub[1]) = (DWORD)pHookEnv->savebytes - ((DWORD)pHookEnv->jmptostub + 5);
	//*(DWORD*)(&pHookEnv->jmptostub[1]) = (DWORD)(dwHookStubAddress) - ((DWORD)pHookEnv->jmptostub + 5);

	*(DWORD*)(&JMPGate[1]) = ((DWORD)pHookEnv->jmptostub) - ((DWORD)StubAddress + 5);

	//д�����������Ҫ��д�������������hook VirtualProtect�������api���û������
	if(type == e_stdcall)
	{
		*(DWORD*)(dwHookStubAddress + SizeOfStubShell - 4) = (DWORD)HookProc - (dwHookStubAddress + SizeOfStubShell);
	}
	else if(type == e_cdecl)
	{
		*(DWORD*)(dwHookStubAddress + SizeOfStubShell - 4) = (DWORD)HookProc - (dwHookStubAddress + SizeOfStubShell);
		*(DWORD*)(dwHookStubAddress + SizeOfStubShell - 9) = (DWORD)cdeclret_stub;
	}

	//patch api
	if (VirtualProtect(StubAddress, ReplaceCodeSize, PAGE_EXECUTE_READWRITE, &oldpro))
	{
		WriteProcessMemory(GetCurrentProcess(), StubAddress, JMPGate, sizeof(JMPGate), &RetSize);
		VirtualProtect(StubAddress, ReplaceCodeSize, oldpro, &oldpro);
		MyInterlockedExchange32((PLONG)(&pHookEnv->jmptostub[1]), (DWORD)(dwHookStubAddress) - ((DWORD)pHookEnv->jmptostub + 5));
	}
	else
	{
		//ʧ���ˣ��޷�hook
		free((void*)pHookEnv);
		return NULL;
	}
	return pHookEnv;
}

PHOOKENVIRONMENT __stdcall InstallHookApi(PCHAR DllName, PCHAR ApiName, PVOID HookProc, int type)
{
	PVOID addr = (PVOID)GetProcAddress(LoadLibraryA(DllName), ApiName);
	return InstallHookStub(addr, HookProc, type);
}

BOOL __stdcall UnInstallHookApi(PHOOKENVIRONMENT pHookEnv)
{
	DWORD oldpro;
	DWORD RetSize;

	//����ڴ治�����ˣ����˳�
	if(IsBadReadPtr((const void*)pHookEnv, sizeof(HOOKENVIRONMENT)))
		return FALSE;

	if(!VirtualProtect(pHookEnv->OrgApiAddr, pHookEnv->SizeOfReplaceCode, PAGE_EXECUTE_READWRITE, &oldpro))
		return FALSE;
	WriteProcessMemory(GetCurrentProcess(), pHookEnv->OrgApiAddr, pHookEnv->savebytes, pHookEnv->SizeOfReplaceCode, &RetSize);

	if(!VirtualProtect(pHookEnv->OrgApiAddr, pHookEnv->SizeOfReplaceCode, oldpro, &oldpro))
		return FALSE;

	//VirtualProtect((void*)pHookEnv, sizeof(HOOKENVIRONMENT), pHookEnv->oldpro, &pHookEnv->oldpro);
	free((void*)pHookEnv);
	return TRUE;
}


//�����������п�����Ϊ��ʾʹ��
//#define TEST_MAIN


#ifdef TEST_MAIN

BOOL IsMe = FALSE;

//�ȶ���һ��Ҫhook��WINAPI
typedef HMODULE (WINAPI __pfnLoadLibraryA)(LPCSTR lpFileName);

/*
HookProc�Ĳ���������ʽ���͵Ⱥ�ԭ����apiһ����ֻ�ǲ�����ԭAPI��2��
DWORD WINAPI HookProc(DWORD RetAddr ,__pfnXXXX pfnXXXX, ...);

//������ԭʼ��API��2������
RetAddr	//����api�ķ��ص�ַ
pfnXXX 	//����Ϊ__pfnXXXX����hook��api���������ͣ����ڵ���δ��hook��api

���My_LoadLibraryA
ԭʼ��LoadLibraryA�������ǣ�

HMODULE WINAPI LoadLibraryA( LPCSTR lpLibFileName );

��ô���ȶ���һ��hook��WINAPI������
typedef HMODULE (WINAPI __pfnLoadLibraryA)(LPCTSTR lpFileName);

Ȼ��hookproc�ĺ����������£�
HMODULE WINAPI My_LoadLibraryA(DWORD RetAddr,
							   __pfnLoadLibraryA pfnLoadLibraryA,
							   LPCTSTR lpFileName
							   );

��ԭ���Ķ���2������������λ�ò��ܵߵ�����My_LoadLibraryA�п������ɵĵ���δ��hook��pfnLoadLibraryA
Ҳ���Ե���ϵͳ��LoadLibraryA������Ҫ�Լ���hookproc�д������������

���⣬Ҳ������My_LoadLibraryA��ʹ��UnInstallHookApi()������ж��hook���÷����£�
���ڶ�������__pfnLoadLibraryA pfnLoadLibraryAǿ��ת����PHOOKENVIRONMENT���ͣ�ʹ��UnInstallHookApi��ж��

���磺
UnInstallHookApi((PHOOKENVIRONMENT)pfnLoadLibraryA);


������ǰ�汾��HookBefore��HookAfter����ȫ�������Լ���HookProc�������ʹ����

*/

HMODULE WINAPI My_LoadLibraryA(DWORD RetAddr,
							   __pfnLoadLibraryA pfnLoadLibraryA,
							   LPCSTR lpFileName
							   )
{
	HMODULE hLib;
	
	//��Ҫ�Լ�����������̰߳�ȫ����
	if (!IsMe)
	{
		IsMe = TRUE;
		MessageBoxA(NULL, lpFileName, "test", MB_ICONINFORMATION);
		hLib = LoadLibraryA(lpFileName);//������õ���ϵͳ�ģ��Ѿ���hook����
		IsMe = FALSE;
		//������ж��Hook������ж����Ͳ�����pfnLoadLibraryA��������
		UnInstallHookApi((PHOOKENVIRONMENT)pfnLoadLibraryA);
		return hLib;
	}
	return pfnLoadLibraryA(lpFileName);//������÷�hook��
}


int main()
{
	PHOOKENVIRONMENT pHookEnv;

	pHookEnv = InstallHookApi("Kernel32.dll", "LoadLibraryA", My_LoadLibraryA);
	LoadLibraryA("InjectDll.dll");
	MessageBoxA(NULL,"Safe Here!!!","Very Good!!",MB_ICONINFORMATION);
	UnInstallHookApi(pHookEnv);//����HookProc��ж�ع��ˣ����������ж�ؾ���Ч��
	MessageBoxA(NULL,"UnInstall Success!!!","Good!!",MB_ICONINFORMATION);
	return 0;
}

#endif

 