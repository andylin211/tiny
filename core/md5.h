#ifndef _md5_h_
#define _md5_h_

#include <wchar.h>

typedef void(*md5_notify_func)(int current, int total, void* notify_data);

wchar_t* md5_compute(wchar_t* file, md5_notify_func notify, void* notify_data);

#endif
