#pragma once
#include "resource.h"
#include "tinyfltk.h"
#include <assert.h>

class toolbox_t
{
public:
	
	int w() { return _w; }
	int h() { return _h; }

	int _x, _y, _w, _h;

public:
	toolbox_t(int x = 0, int y = 0, int w = 350, int h = 250)
		:_x(x), _y(y), _w(w), _h(h)
	{

	}
};




