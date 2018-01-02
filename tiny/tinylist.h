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

#ifdef define_tiny_here

	int is_list_empty(list_entry_t* head)
	{
		if (!head)
			return 0;

		return (head->flink == head->blink) && (head->flink == head);
	}

	void initialize_list_head(list_entry_t* head)
	{
		if (!head)
			return;

		(head)->flink = (head)->blink = head;
	}

	void insert_into_list(list_entry_t* head, list_entry_t* entry)
	{
		if (!head || !entry)
			return;

		head->blink->flink = entry;
		entry->flink = head;
		entry->blink = head->blink;
		head->blink = entry;
	}

	void remove_from_list(list_entry_t* entry)
	{
		if (!entry)
			return;

		entry->flink->blink = entry->blink;
		entry->blink->flink = entry->flink;
	}
	
#endif
	
#ifdef __cplusplus
}
#endif

#endif