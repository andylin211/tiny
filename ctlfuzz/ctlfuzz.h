#pragma once
#include <Windows.h>
#include <wchar.h>
#include <stdio.h>
#include "tinyrand.h"
#include "tinylog.h"
#include "tinysys.h"

const int buf_len = 256;

const int buf_len_ext = 1024;

char ascii_string10[0x10];
char ascii_string100[0x100];
char ascii_string1000[0x1000];

wchar_t unicode_string10[0x10];
wchar_t unicode_string100[0x100];
wchar_t unicode_string1000[0x1000];

int table_dwords[0x100];

/* void* */
unsigned fuzz_constants[] = {
	0x00000000, 0x00000001, 0x00000004, 0xFFFFFFFF,
	0x00001000, 0xFFFF0000, 0xFFFFFFFE, 0xFFFFFFF0,
	0xFFFFFFFC, 0x70000000, 0x7FFEFFFF, 0x7FFFFFFF,
	0x80000000,
	(unsigned)ascii_string10,
	(unsigned)ascii_string100,
	(unsigned)ascii_string1000,
	(unsigned)unicode_string10,
	(unsigned)unicode_string100,
	(unsigned)unicode_string1000,
	(unsigned)table_dwords
};

class ctlfuzz_t
{
public:
	char device_name[buf_len];
	HANDLE handle;
	int code;
	int inlen;
	char inbuf[buf_len_ext * 2];
	int oulen;
	char oubuf[buf_len_ext * 2];
	DWORD retlen;
	BOOL status;
	COORD cursor;
	char buf[256];

	ctlfuzz_t(char* _name)
		:handle(INVALID_HANDLE_VALUE),
		code(0), inlen(0), oulen(0), status(FALSE)
	{
		if (!_name)
		{
			log_errora("device name is null.");
			return;
		}

		log_debuga("specify %s as device name", _name);
		strncpy_s(device_name, buf_len, _name, buf_len);
		open_device();
		initialize_predefine_data();
	}

	~ctlfuzz_t()
	{
		if (INVALID_HANDLE_VALUE != handle)
			CloseHandle(handle);
	}

	void fuzz(int code, int il, int ol)
	{
		if (INVALID_HANDLE_VALUE == handle)
		{
			log_errora("handle value is invalid.");
			return;
		}
		log_debuga("fuzz %X, inlen=%d, oulen=%d", code, il, ol);

		inlen = il;
		oulen = ol;
		fuzz_overflow();
		fuzz_predefine();
		fuzz_random();
	}

	void fuzz(int* code, int* il, int* ol, int size)
	{
		if (!code || !il || !ol || size <= 0)
		{
			log_errora("fuzz: invalid parameters.");
			return;
		}

		for (int i = 0; i < size; i++)
		{
			fuzz(code[i], il[i], ol[i]);
		}
	}

	/* 
	 * 1. 没有处理inlen过长的情况
     * 2. 
	 */
	void fuzz_overflow()
	{
		log_debuga("fuzz overflow.");
		cursor = cursor_position();
		memset(inbuf, 'A', buf_len_ext * 2);
		for (int i = buf_len_ext - 1; i >= 0; i--)
		{
			do_io_control(i);
			print_at(str_format(buf, 256, "overflow: %3d %%%%", (buf_len_ext - i) * 100 / buf_len_ext), cursor);
		}
		fprintf(stdout, " ok\r\n");
	}

	void fuzz_predefine()
	{
		log_debuga("fuzz predefine.");
		cursor = cursor_position();
		for (int i = 0; i < buf_len_ext * 2; i++)
		{
			inbuf[i] = (char)getrand(0x00, 0xff);
			print_at(str_format(buf, 256, "predefine 1: %3d %%%%", (i + 1) * 50 / buf_len_ext), cursor);
		}
		fprintf(stdout, " ok\r\n");

		memcpy(inbuf, fuzz_constants, sizeof(fuzz_constants));

		cursor = cursor_position();
		for (int i = 0; i < sizeof(fuzz_constants); i += 4)
		{
			do_io_control(inlen, &inbuf[i]);
			print_at(str_format(buf, 256, "predefine 2: %3d %%%%", (i / 4 + 1) * 400 / sizeof(fuzz_constants)), cursor);
		}
		fprintf(stdout, " ok\r\n");

		/* to do */
	}

	void fuzz_random()
	{
		log_debuga("fuzz random.");
		cursor = cursor_position();
		for (int i = 0; i < buf_len_ext; i++)
		{
			memset(inbuf, 0x00, inlen);
			for (int j = 0; j < inlen; j++)
				inbuf[j] = (char)getrand(0x00, 0xff);

			do_io_control();

			print_at(str_format(buf, 256, "random: %3d %%%%", (i + 1) * 100 / buf_len_ext), cursor);
		}
		fprintf(stdout, " ok\r\n");
	}

	void initialize_predefine_data()
	{
		int i;
		memset(ascii_string10, 0x41, 0x10);
		memset(ascii_string100, 0x41, 0x100);
		memset(ascii_string1000, 0x41, 0x1000);

		wmemset(unicode_string10, 0x0041, 0x10);
		wmemset(unicode_string100, 0x0041, 0x100);
		wmemset(unicode_string1000, 0x0041, 0x1000);

		for (i = 0; i<(sizeof(table_dwords) / 4); i++)
			table_dwords[i] = 0xFFFF0000;
	}


	void open_device()
	{
		handle = INVALID_HANDLE_VALUE;

		if (!device_name)
			return;

		char symlnk_name[buf_len];
		sprintf_s(symlnk_name, buf_len, "\\\\.\\%s", device_name);

		handle = CreateFileA(
			symlnk_name,
			GENERIC_READ | GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_SYSTEM,
			NULL);
	}

	BOOL do_io_control(int il = 0, char* pinbuf = 0)
	{
		status = DeviceIoControl(handle,
			code,
			pinbuf ? pinbuf : inbuf,
			il ? il : inlen,
			oubuf,
			oulen,
			&retlen,
			0);

		return status;
	}
};

