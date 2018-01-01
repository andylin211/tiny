#pragma once

#include "FL\Fl_Image.H"

class Ioctl_Bmp : public Fl_RGB_Image 
{
public:
	Ioctl_Bmp(int id);

	char* read_resource(int id);
};
