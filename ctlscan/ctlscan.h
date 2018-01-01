#pragma once
#include <Windows.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "tinylog.h"

const int buf_len = 256;

const int buf_len_ext = 1024;

/*
 * 1. 如果同一个功能号，不同缓冲方式都可以成功，认为无效（针对无论都返回ok的处理，只好去掉）
 */
class ctlscan_t
{
public:
	char device_name[buf_len];
	HANDLE handle;
	int code;
	int inlen;
	char inbuf[buf_len_ext];
	int oulen;
	char oubuf[buf_len_ext];
	DWORD retlen;
	BOOL status;
	BOOL status2;

	ctlscan_t(char* _name)
		:handle(INVALID_HANDLE_VALUE),
		code(0), inlen(0), oulen(0), status(FALSE)
	{
		if (!_name)
		{
			log_errora("device name is null!");
			return;
		}
		
		log_debuga("specify %s as device name.", _name);
		strncpy_s(device_name, buf_len, _name, buf_len);
		open_device();
	}

	~ctlscan_t()
	{
		if (INVALID_HANDLE_VALUE != handle)
			CloseHandle(handle);
	}
	
	/* 
	 * scan with specifying code 
	 * 
	 * 1. inlen=256,oulen=256
	 * 2. inlen=2^{10,9,7,6,5,4,3,2,1,0},oulen=256
	 * 3. inlen=2^{10,9,8,7,6,5,4,3,2,1,0},oulen=2^{10,9,7,6,5,4,3,2,1,0}
	 *
	 */
	bool scan(int code)
	{
		this->code = code;
		status = FALSE;
		do
		{
			if (INVALID_HANDLE_VALUE == handle)
			{
				log_errora("device handle is invalid.");
				break;
			}

			if (do_io_control(256, 256))
				break;

			int i;
			for (i = 10; i >= 0; i--)
			{
				if (do_io_control(1 << i, 256))
					break;
			}

			if (i != -1)
				break;

			int j;
			for (i = 10; i >= 0; i--)
			{
				for (j = 10; j >= 0; j--)
				{
					if (do_io_control(1 << i, 1 << j))
						break;
				}
				if (j != -1)
					break;
			}
			
		} while (0);
		
		if (status && !status2)
		{
			log_printa("%08X %8d %8d", code, inlen, oulen);
			return true;
		}
		return false;
	}

	/* 
	 * specify component 
	 *   equals to specifying code
	 */
	bool scan(int function, int type, int access, int method)
	{
		return scan(ioctl_code(type, access, function, method));
	}

	/* specify range */
	bool scan(int func_from, int func_to)
	{
		bool ret = false;

		if (INVALID_HANDLE_VALUE == handle)
		{
			log_errora("device handle is invalid.");
			return ret;
		}

		if (func_from < 0 || func_to > 0xfff || func_to < func_from)
		{
			log_errora("invalid parameter. from=%d, to=%d", func_from, func_to);
			return ret;
		}
			

		for (int i = func_from; i < func_to; i++)
		{
			for (int a = 0; a < 4; a++)
			{
				if (scan(i, FILE_DEVICE_UNKNOWN, a, METHOD_BUFFERED))
				{
					ret = true;
					break;
				}
			}
		}

		return ret;
	}

	void open_device()
	{
		handle = INVALID_HANDLE_VALUE;

		if (!device_name)
		{
			log_errora("device name is null!");
			return;
		}
			

		char symlnk_name[buf_len];
		sprintf_s(symlnk_name, buf_len, "\\\\.\\%s", device_name);
		log_debuga("symbolic link name is \"%s\"", symlnk_name);

		handle = CreateFileA(
			symlnk_name,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_SYSTEM,
			NULL);

		if (handle == INVALID_HANDLE_VALUE)
			log_errora("fail to open device. %d.", GetLastError());
	}
	
	BOOL do_io_control(int il, int ol)
	{
		inlen = il;
		oulen = ol;
		status2 = 0;
		status = DeviceIoControl(handle,
			code,
			inbuf,
			inlen,
			oubuf,
			oulen,
			&retlen,
			0);
		
		log_debuga("do_io_control: code=0x%x, inlen=%d, oulen=%d, retlen=%d, status=0x%x", code, inlen, oulen, retlen, status);
		
		if (status)
		{
			status2 = DeviceIoControl(handle,
				code ^ 3,// modify method
				inbuf,
				inlen,
				oubuf,
				oulen,
				&retlen,
				0);
			log_debuga("try again: code=0x%x, inlen=%d, oulen=%d, retlen=%d, status=0x%x", code ^ 3, inlen, oulen, retlen, status2);
		}
		
		return status;
	}

	static int ioctl_code(int type, int access, int function, int method)
	{
		return (((type) << 16) | ((access) << 14) | ((function) << 2) | (method));
	}

};