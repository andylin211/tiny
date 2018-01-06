#include <tchar.h>
#include <Windows.h>  
#include <stdio.h>
#pragma comment(lib, "version")

typedef struct _tagLanguage
{
	WORD wLanguage;
	WORD wCodePage;
} tagLanguage, *LPLanguage;



int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3)
	{
		return 0;
	}

	TCHAR* FileName = argv[1];
	TCHAR* Version = argv[2];

	DWORD dwVerHnd = 0;
	//先获取整个文件版本信息的大小  
	DWORD dwVerInfoSize = GetFileVersionInfoSize(FileName, &dwVerHnd);
	//根据大小来申请内存  
	TCHAR *VerInfo = new TCHAR[dwVerInfoSize];
	//获取文件版本信息，这些信息存在刚刚申请的内存中，修改版本信息通过直接修改内存后一次性将版本信息更新  
	BOOL res = GetFileVersionInfo(FileName, 0, dwVerInfoSize, VerInfo);
	if (!res)
	{
		delete[]VerInfo;
		return 0;
	}

	//首先在获取的版本信息中读取语言信息，因为修改版本信息需要用到  
	LPLanguage language = NULL;
	UINT size = 0;
	VerQueryValue(VerInfo, _T("\\VarFileInfo\\Translation"), (LPVOID*)&language, &size);

	//读取文件版本信息  
	VS_FIXEDFILEINFO*  FixedFileInfo = NULL;
	VerQueryValue(VerInfo, _T("\\"), (LPVOID*)&FixedFileInfo, &size);
	TCHAR TempBuf[MAX_PATH] = { 0 };
	if (FixedFileInfo)
	{
		//修改文件版本信息的版本号，这里通过CVersion 类的分别获取数字“1234”，“456”，“789”，“1110”  
		//这里需要将高位与低位组合   12.1.123.32
		FixedFileInfo->dwFileVersionMS = MAKELONG(8, 2017);
		FixedFileInfo->dwFileVersionLS = MAKELONG(121, 3);


		//FixedFileInfo->dwProductVersionMS = MAKELONG(1, 12);
		//FixedFileInfo->dwProductVersionMS = MAKELONG(32, 123);2017.8.3.0121

	}

	////读取StringFileInfo中的信息信息  
	//TCHAR *ProductVer = NULL;
	//TCHAR *FileVer = NULL;
	//_stprintf_s(TempBuf, _T("\\StringFileInfo\\%04x%04x\\FileVersion"), language->wLanguage, language->wCodePage);
	//VerQueryValue(VerInfo, TempBuf, (LPVOID*)&FileVer, &size);
	//_stprintf_s(TempBuf, _T("\\StringFileInfo\\%04x%04x\\ProductVersion"), language->wLanguage, language->wCodePage);
	//VerQueryValue(VerInfo, TempBuf, (LPVOID*)&ProductVer, &size);


	//size_t productlength = _tcslen(ProductVer);
	//size_t fileLength = _tcslen(FileVer);
	///*if (_tcslen(Version)>productlength)
	//{
	//	printf("t1\n");
	//	return 0;
	//}

	//if (_tcslen(Version)>fileLength)
	//{
	//	printf("t2\n");
	//	return 0;
	//}*/

	////修改内存  
	//_tcscpy_s(ProductVer, _tcslen(Version) + 1, Version);
	//_tcscpy_s(FileVer, _tcslen(Version) + 1, Version);

	//上面都是读取与修改，这里才是更新资源  
	HANDLE hResource = BeginUpdateResource(FileName, FALSE);
	if (NULL != hResource)
	{
		//这里参数没错，就是最原始读取的资源，通过修改原有的资源内存，达到更新的目的  
		res = UpdateResource(hResource, RT_VERSION, MAKEINTRESOURCE(VS_VERSION_INFO), language->wLanguage, VerInfo, dwVerInfoSize);
		if (!res)
		{
			printf("t3\n");
			return 0;
		}
		res = EndUpdateResource(hResource, FALSE);
		if (!res)
		{
			printf("t5\n");
			return 0;
		}
	}
	else
	{
		printf("%d\n", GetLastError());
	}

	return 0;
}
