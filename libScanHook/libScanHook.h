#pragma once

#include<Windows.h>
#include<VersionHelpers.h>
#include<TlHelp32.h>
#include<vector>
#include "ldasm.h"
#include "libdasm.h"
#include "libPe.h"
#include "PeLoader.h"
#include "apisetschema.h"
using std::vector;
using namespace libpe;
using namespace peloader;
using namespace apisetschema;

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

namespace libscanhook
{
	class SCANHOOK
	{
	public:
		SCANHOOK();
		~SCANHOOK();
		bool InitScan(DWORD Pid);
		bool GetProcessHookInfo(PPROCESS_HOOK_INFO Entry);

	private:
		typedef struct _MODULE_INFO
		{
			DWORD  DllBase;
			DWORD SizeOfImage;
			WCHAR FullName[260];
			WCHAR BaseName[64];
			void *MemoryImage;
			void *DiskImage;
		} MODULE_INFO;

	private:
		bool ScanInlineHook(char *ApiName, DWORD Address);
		bool ScanEatHook();
		bool ScanIatHook();
		void AddHookInfoToList(DWORD HookType, DWORD OriginalAddress, DWORD HookAddress, char *HookedApiName, WCHAR *HookedModule);
		void AddHookInfoToList(DWORD HookType, DWORD OriginalAddress, DWORD HookAddress, char *HookedApiName, char *HookedModule);

	private:
		bool m_IsFromIat, m_IsFromEat;
		HANDLE m_hProcess;
		vector<MODULE_INFO> ModuleInfo;
		vector<MODULE_INFO>::iterator ModuleInfoiter;
		vector<PROCESS_HOOK_INFO> HookInfo;
		vector<PROCESS_HOOK_INFO>::iterator HookInfoiter;
		ApiSet Api;

	private:
		bool ElevatedPriv();
		bool QueryModuleInfo();
		bool ReadMemoryImage();
		void FreeMemoryImage();
		DWORD GetExportByOrdinal(DWORD ImageBase, WORD Ordinal);
		DWORD GetExportByName(DWORD ImageBase, char *ProcName);
		DWORD FileNameRedirection(char *RedirectionName);
		DWORD MyGetProcAddress(char *DllName, char *ApiName, bool *IsApiSet, WCHAR *RealDllName);
		bool GetModuleInfomation(WCHAR *DllName, vector<MODULE_INFO>::iterator &iter);
		bool GetModuleInfomation(DWORD Address, vector<MODULE_INFO>::iterator &iter);
		void GetModulePath(DWORD Address, WCHAR *ModulePath);
		void GetModulePathByAddress(DWORD Address, WCHAR *ModulePath);
		DWORD FindDosHeadInMemory(DWORD Address);
	};
}