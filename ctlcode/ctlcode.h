#pragma once
#include "tinylog.h"

class ctlcode_t
{
public:
	static void print(int c, int t, int a, int f, int m)
	{
		log_printa("%08X %8X %8d %8X %8d", c, t, a, f, m);
	}

	static void code(int c)
	{
		int t = c >> 16;
		int a = c >> 14 & 0x3;
		int f = c >> 2 & 0xfff;
		int m = c & 0x3;
		print(c, t, a, f, m);
	}

	static void component(int t, int a, int f, int m)
	{
		int c = make_ioctl_code(t, a, f, m);
		print(c, t, a, f, m);
	}

	static int make_ioctl_code(int device_type, int access, int function, int method)
	{
		return (((device_type) << 16) | ((access) << 14) | ((function) << 2) | (method));
	}

};



