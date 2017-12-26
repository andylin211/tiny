#ifndef _ioctlfuzz_h_
#define _ioctlfuzz_h_

#include <wchar.h>
#include "tinyargs.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef void(*fuzz_notify_func)(int current, int total, args_t* notify_data);

	void scan_in_range(wchar_t* device, int from, int to, wchar_t* output, fuzz_notify_func notify, args_t* notify_data);

	void scan(wchar_t* device, wchar_t* output, fuzz_notify_func notify, args_t* notify_data);

	void fuzz_one_ioctl(wchar_t* device, unsigned long ioctl, fuzz_notify_func notify, args_t* notify_data);

	void fuzz(wchar_t* device, wchar_t* config, fuzz_notify_func notify, args_t* notify_data);

#ifdef __cplusplus
}
#endif
#endif