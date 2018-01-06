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
	//�Ȼ�ȡ�����ļ��汾��Ϣ�Ĵ�С  
	DWORD dwVerInfoSize = GetFileVersionInfoSize(FileName, &dwVerHnd);
	//���ݴ�С�������ڴ�  
	TCHAR *VerInfo = new TCHAR[dwVerInfoSize];
	//��ȡ�ļ��汾��Ϣ����Щ��Ϣ���ڸո�������ڴ��У��޸İ汾��Ϣͨ��ֱ���޸��ڴ��һ���Խ��汾��Ϣ����  
	BOOL res = GetFileVersionInfo(FileName, 0, dwVerInfoSize, VerInfo);
	if (!res)
	{
		delete[]VerInfo;
		return 0;
	}

	//�����ڻ�ȡ�İ汾��Ϣ�ж�ȡ������Ϣ����Ϊ�޸İ汾��Ϣ��Ҫ�õ�  
	LPLanguage language = NULL;
	UINT size = 0;
	VerQueryValue(VerInfo, _T("\\VarFileInfo\\Translation"), (LPVOID*)&language, &size);

	//��ȡ�ļ��汾��Ϣ  
	VS_FIXEDFILEINFO*  FixedFileInfo = NULL;
	VerQueryValue(VerInfo, _T("\\"), (LPVOID*)&FixedFileInfo, &size);
	TCHAR TempBuf[MAX_PATH] = { 0 };
	if (FixedFileInfo)
	{
		//�޸��ļ��汾��Ϣ�İ汾�ţ�����ͨ��CVersion ��ķֱ��ȡ���֡�1234������456������789������1110��  
		//������Ҫ����λ���λ���   12.1.123.32
		FixedFileInfo->dwFileVersionMS = MAKELONG(8, 2017);
		FixedFileInfo->dwFileVersionLS = MAKELONG(121, 3);


		//FixedFileInfo->dwProductVersionMS = MAKELONG(1, 12);
		//FixedFileInfo->dwProductVersionMS = MAKELONG(32, 123);2017.8.3.0121

	}

	////��ȡStringFileInfo�е���Ϣ��Ϣ  
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

	////�޸��ڴ�  
	//_tcscpy_s(ProductVer, _tcslen(Version) + 1, Version);
	//_tcscpy_s(FileVer, _tcslen(Version) + 1, Version);

	//���涼�Ƕ�ȡ���޸ģ�������Ǹ�����Դ  
	HANDLE hResource = BeginUpdateResource(FileName, FALSE);
	if (NULL != hResource)
	{
		//�������û��������ԭʼ��ȡ����Դ��ͨ���޸�ԭ�е���Դ�ڴ棬�ﵽ���µ�Ŀ��  
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
