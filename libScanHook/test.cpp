// libScanHook.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<iomanip>
#include "libScanHook.h"
#include "HookTest\HookTest.h"
using namespace std;
using namespace libScanHook;

int _tmain(int argc, _TCHAR* argv[])
{
	ScanHook Scan;
	PROCESS_HOOK_INFO HookInfo;
	//TestHook();
	if (Scan.InitScan(1176))
	{
		while (Scan.GetProcessHookInfo(&HookInfo))
		{
			cout << "��������: ";
			switch (HookInfo.HookType)
			{
			case EatHook:
				cout << "EatHook" << endl;
				break;
			case IatHook:
				cout << "IatHook" << endl;
				break;
			case InlineHook:
				cout << "InlineHook" << endl;
				break;
			default:
				break;
			}
			cout << "ԭ�����ĵ�ַ: 0x" << setw(8) << setfill('0') << hex << HookInfo.OriginalAddress << endl;
			cout << "���ӵĵ�ַ: 0x" << setw(8) << setfill('0') << hex << HookInfo.HookAddress << endl;
			cout << "���ҹ��ĺ�����: ";
			wcout << HookInfo.HookedApiName << endl;
			cout << "���ҹ���ģ����: ";
			wcout << HookInfo.HookedModule << endl;
			cout << "�������ڵ�ģ��: ";
			wcout << HookInfo.HookLocation << endl;
			cout << endl;
		}
	}
	Scan.CloseScan();
	system("pause");
	return 0;
}

