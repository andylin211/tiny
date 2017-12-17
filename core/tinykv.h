#ifndef _tinykv_h_
#define _tinykv_h_

#include <wchar.h>
#include "tinylist.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct
	{
		char* key;
		wchar_t* value;
		list_entry_t list_entry;
	}kv_t;

	typedef struct
	{
		list_entry_t kv_list;
	}kv_list_t;

#ifdef __cplusplus
}
#endif

#endif
