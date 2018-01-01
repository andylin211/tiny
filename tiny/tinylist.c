#include "tinylist.h"

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