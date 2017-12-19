#ifndef _md5_h_
#define _md5_h_

#include <wchar.h>

#ifdef __cplusplus
extern "C" {
#endif

	typedef void(*md5_notify_func)(int current, int total, void* notify_data);

	wchar_t* md5_compute(wchar_t* file, md5_notify_func notify, void* notify_data);

#ifdef __cplusplus
}
#endif

#endif
