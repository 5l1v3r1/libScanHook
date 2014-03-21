#pragma once

#include<Windows.h>
#include<vector>
#include "ntdll.h"
#include "libdasm.h"
using std::vector;

typedef enum _HOOK_TYPE
{
	EatHook,
	IatHook,
	InlineHook
} HOOK_TYPE;

typedef struct _PROCESS_HOOK_INFO
{
	DWORD HookType;
	DWORD OriginalAddress;                   //ԭ������ַ
	DWORD HookAddress;                       //���ӵĵ�ַ
	WCHAR HookedApiName[128];         //���ҹ��ĺ�����
	WCHAR HookedModule[64];            //���ҹ���ģ����
	WCHAR HookLocation[260];            //�������ڵ�ģ��
} PROCESS_HOOK_INFO, *PPROCESS_HOOK_INFO;

namespace libScanHook
{
	class ScanHook
	{
	public:
		ScanHook();
		bool InitScan(DWORD Pid);
		void CloseScan();
		bool GetProcessHookInfo(PPROCESS_HOOK_INFO Entry);

	private:
		typedef struct _MODULE_INFO
		{
			DWORD  DllBase;
			DWORD SizeOfImage;
			WCHAR FullName[260];
			WCHAR BaseName[64];
			void *ScanBuffer;
			void *OrigBuffer;
		} MODULE_INFO;

		typedef struct _PE_INFO
		{
			PIMAGE_NT_HEADERS PeHead;
			DWORD ExportTableRva;
			DWORD ExportSize;
			DWORD ImportTableRva;
			DWORD ImportSize;
		} PE_INFO, *PPE_INFO;

	private:
		bool ScanInlineHook(char *ApiName, DWORD Address);
		bool ScanEatHook();
		bool ScanIatHook();

	private:
		bool IsRedirction, IsFromRedirction;
		HANDLE hProcess;
		DWORD MajorVersion, MinorVersion;
		DWORD *ApiSetMapHead;
		vector<MODULE_INFO> ModuleInfo;
		vector<MODULE_INFO>::iterator ModuleInfoiter;
		vector<PROCESS_HOOK_INFO> HookInfo;
		vector<PROCESS_HOOK_INFO>::iterator HookInfoiter;
		bool ElevatedPriv();
		bool QuerySystemInfo();
		bool QueryModuleInfo();
		bool PeLoader(WCHAR *FilePath, void *BaseAddress, DWORD BufferSize, DWORD DllBase);
		bool FixBaseRelocTable(DWORD NewImageBase, DWORD ExistImageBase);
		PIMAGE_BASE_RELOCATION ProcessRelocationBlock(ULONG_PTR VA, ULONG SizeOfBlock, PUSHORT NextOffset, LONGLONG Diff);
		bool IsGlobalVar(PIMAGE_NT_HEADERS PeHead, DWORD Rva);
		bool ParsePe(DWORD ImageBase, PPE_INFO PeInfo);
		DWORD GetExportByOrdinal(DWORD ImageBase, WORD Ordinal);
		DWORD GetExportByName(DWORD ImageBase, char *ProcName);
		DWORD FileNameRedirection(DWORD ImageBase, char *RedirectionName);
		bool ResolveApiSet(WCHAR *ApiSetName, WCHAR *HostName, DWORD Size);
		DWORD MyGetProcAddress(char *DllName, char *ApiName, bool *IsApiSet, WCHAR *RealDllName);
		bool GetModuleInfomation(WCHAR *DllName, vector<MODULE_INFO>::iterator &iter);
		bool GetModuleInfomation(DWORD Address, vector<MODULE_INFO>::iterator &iter);
		void GetModulePath(DWORD Address, WCHAR *ModulePath);
		void GetModulePathByAddress(DWORD Address, WCHAR *ModulePath);
	};
}