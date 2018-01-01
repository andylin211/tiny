#pragma once
#include "FL\Fl_RGB_Image.H"
#include <Windows.h>

class bmp_t : public Fl_RGB_Image
{
public:
	bmp_t(int res_id) :Fl_RGB_Image(0, 0, 0)
	{
		char* buf = read_resource(res_id);
		int width = 0;
		int height = 0;
		Fl_RGB_Image::array = (uchar*)parse_bmp(buf, width, height);
		d(4);
		w(width);
		h(height);
		free(buf);
	}

	static char* parse_bmp(char* buf, int& w, int& h)
	{
		char* ret = 0;
		w = h = 0;

		do
		{
			if (!buf || strncmp("BM", buf, 2))
				break;

			int offset = *(int*)(buf + 10);
			w = *(int*)(buf + 18);
			h = *(int*)(buf + 22);
			int depth = *(int*)(buf + 28);

			if (depth != 24 || h <= 0 || !w)
				break;

			ret = new char[w * h * 4];

			/* (feed + w * 3) % 4 = 0 */
			int feed = w * 3 % 4;
			feed = feed ? 4 - feed : feed;

			int row_bytes = w * 3 + feed;

			char* p = ret;
			for (int y = h - 1; y >= 0; y--)
			{
				for (int x = 0; x < w; x++)
				{
					int t = y * w * 4 + x * 4;
					int s = offset + (h - 1 - y) * row_bytes + x * 3;
					p[t + 2] = buf[s + 0];
					p[t + 1] = buf[s + 1];
					p[t + 0] = buf[s + 2];
					p[t + 3] = (char)0xff;
					if (p[t + 2] == (char)0xff && p[t + 1] == (char)0xff && p[t + 0] == (char)0xff)
						p[t + 3] = 0;
				}
			}

		} while (0);

		return ret;
	}

	static char* read_resource(int id)
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
};