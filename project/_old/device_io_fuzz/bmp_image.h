#pragma once
#include "FL\Fl_Image.H"

class bmp_image : public Fl_RGB_Image
{
public:
	bmp_image(int res);
	
	char* read_resource(int id);
};

