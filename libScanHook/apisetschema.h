#pragma once

#include<Windows.h>

namespace apisetschema
{
	typedef struct _API_SET_NAMESPACE_ENTRY_V2
	{
		DWORD NameOffset;
		DWORD NameLength;
		DWORD DataOffset;                     //ָ��API_SET_VALUE_ARRAY_V2�����API_SET_NAMESPACE_ARRAY_V2��ƫ��
	} API_SET_NAMESPACE_ENTRY_V2, *PAPI_SET_NAMESPACE_ENTRY_V2;

	typedef struct _API_SET_NAMESPACE_ARRAY_V2
	{
		DWORD Version;
		DWORD Count;       //ָ���ж��ٸ�API_SET_MAP_ENTRY
		_API_SET_NAMESPACE_ENTRY_V2 Entry[1];
	} API_SET_NAMESPACE_ARRAY_V2, *PAPI_SET_NAMESPACE_ARRAY_V2;

	typedef struct _API_SET_VALUE_ENTRY_V2
	{
		DWORD NameOffset;
		DWORD NameLength;
		DWORD ValueOffset;
		DWORD ValueLength;
	} API_SET_VALUE_ENTRY_V2, *PAPI_SET_VALUE_ENTRY_V2;

	typedef struct _API_SET_VALUE_ARRAY_V2
	{
		DWORD Count;                                  //API_SET_VALUE_ENTRY_V2������
		_API_SET_VALUE_ENTRY_V2 Entry[1];
	} API_SET_VALUE_ARRAY_V2, *PAPI_SET_VALUE_ARRAY_V2;

	typedef struct _API_SET_NAMESPACE_ENTRY_V4
	{
		DWORD Flags;
		DWORD NameOffset;
		DWORD NameLength;
		DWORD AliasOffset;
		DWORD AliasLength;
		DWORD DataOffset;                                 //API_SET_VALUE_ARRAY_V4�����API_SET_NAMESPACE_ARRAY_V4��ƫ��
	} API_SET_NAMESPACE_ENTRY_V4, *PAPI_SET_NAMESPACE_ENTRY_V4;

	typedef struct _API_SET_NAMESPACE_ARRAY_V4
	{
		DWORD Version;
		DWORD Size;
		DWORD Flags;
		DWORD Count;                                         //ָ���ж��ٸ�API_SET_NAMESPACE_ENTRY_V4
		_API_SET_NAMESPACE_ENTRY_V4 Entry[1];
	} API_SET_NAMESPACE_ARRAY_V4, *PAPI_SET_NAMESPACE_ARRAY_V4;

	typedef struct _API_SET_VALUE_ENTRY_V4
	{
		DWORD Flags;
		DWORD NameOffset;
		DWORD NameLength;
		DWORD ValueOffset;
		DWORD ValueLength;
	} API_SET_VALUE_ENTRY_V4, *PAPI_SET_VALUE_ENTRY_V4;

	typedef struct _API_SET_VALUE_ARRAY_V4
	{
		DWORD Flags;
		DWORD Count;
		_API_SET_VALUE_ENTRY_V4 Entry[1];
	} API_SET_VALUE_ARRAY_V4, *PAPI_SET_VALUE_ARRAY_V4;

	class ApiSet
	{
	public:
		ApiSet();
		~ApiSet();

	private:
		DWORD m_Version;
		BYTE *m_Buffer;
		void *m_ApiSetHead;

	public:
		bool GetRealDll(WCHAR *ApiSetName, WCHAR *DllBaseName, WCHAR *RealName, DWORD Size, bool IsEat);

	private:
		void Init();

	};
}

