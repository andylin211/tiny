#include "ioctl_bmp.h"
#include <windows.h>

Ioctl_Bmp::Ioctl_Bmp(int res)
	:Fl_RGB_Image(0, 0, 0)
{
	char* buf = read_resource(res);
	do
	{
		if (!buf)
			break;

		if (strncmp("BM", buf, 2))
			break;

		int off = *(int*)(buf + 10);
		int width = *(int*)(buf + 18);
		int height = *(int*)(buf + 22);
		int bits = *(int*)(buf + 28);

		if (bits != 24 || height <= 0 || !width)
			break;

		d(4);
		w(width);
		h(height);
		array = new uchar[width * height * 4];

		char* p = (char*)array;
		int feed = width * 3 % 4;
		if (feed)
			feed = 4 - feed;
		int row_bits = width * 3 + feed;
		for (int y = height - 1; y >= 0; y--)
		{
			for (int x = 0; x < width; x++)
			{
				int t = y * width * 4 + x * 4;
				int s = off + (height - 1 - y) * row_bits + x * 3;
				p[t + 2] = buf[s + 0];
				p[t + 1] = buf[s + 1];
				p[t + 0] = buf[s + 2];
				p[t + 3] = (char)0xff;
				if (p[t + 2] == (char)0xff && p[t + 1] == (char)0xff && p[t + 0] == (char)0xff)
					p[t + 3] = 0;
			}
		}
	} while (0);

	free(buf);
}

char* Ioctl_Bmp::read_resource(int id)
{
	HGLOBAL hglobal = NULL;
	HINSTANCE hinst;
	HRSRC hrsrc;
	unsigned int size = 0;
	char* buffer = 0;
	char* res = 0;

	hinst = NULL;
	hrsrc = FindResource(hinst, MAKEINTRESOURCE(id), L"RAW");
	GetLastError();
	if (!hrsrc)
		return 0;

	size = SizeofResource(hinst, hrsrc);
	if (!size)
		return 0;

	hglobal = LoadResource(hinst, hrsrc);
	if (!hglobal)
		return 0;

	res = (char*)LockResource(hglobal);
	if (!res)
		return 0;

	buffer = (char*)malloc(size);
	memcpy(buffer, res, size);

	if (hglobal)
		FreeResource(hglobal);

	return buffer;
}