#ifndef _tinylist_h_
#define _tinylist_h_

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct _list_entry_t
	{
		struct _list_entry_t* flink;
		struct _list_entry_t* blink;
	}list_entry_t;

	#define container_of(ptr, type, member) \
		((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

	int is_list_empty(list_entry_t* head);

	void initialize_list_head(list_entry_t* head);

	void insert_into_list(list_entry_t* head, list_entry_t* entry);

	void remove_from_list(list_entry_t* entry);

#ifdef __cplusplus
}
#endif

#endif