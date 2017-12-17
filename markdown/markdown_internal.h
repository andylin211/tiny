#ifndef _markdown_type_h_
#define _markdown_type_h_

#include "tinylist.h"
#include "tinystr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef enum
{
	inline_unknown = 0,
	inline_code,
	inline_emphasis,
	inline_image,
	inline_link,
	inline_plaintext,
	inline_strong,
}md_inline_type;

typedef enum
{
	block_unknown = 0,
	block_code,
	block_header,
	block_horizontal_rule,
	block_line_break,
	block_ordered_list,
	block_paragraph,
	block_quote,
	block_table,
	block_unordered_list,
}md_block_type;

struct _md_object_t;

typedef struct
{
	md_inline_type type;
	struct _md_object_t* object;
	list_entry_t list_entry;
}md_inline_element_t;

typedef struct
{
	md_block_type type;
	struct _md_object_t* object;
	list_entry_t list_entry;
}md_block_element_t;

typedef struct _md_object_t
{
	wchar_t* text;
	wchar_t* href;
	wchar_t* src;
	int number;
	list_entry_t inline_element_list;
	int row_count;
	int col_count;
	list_entry_t* data_list_head;
}md_object_t;

wchar_t* md_load_block(wchar_t* buffer, md_object_t** object, md_block_type* type);

wchar_t* md_load_inline(wchar_t* buffer, md_object_t** object, md_inline_type* type);

void md_block_to_html(md_block_element_t* block_element, str_encoding encoding, FILE* file);

void md_inline_to_html(md_inline_element_t* inline_element, str_encoding encoding, FILE* file);

void md_code_string_to_syntax_html(char* string, FILE* file);

md_object_t* md_new_object();

void md_free_object(md_object_t* object);

wchar_t* skip_line(wchar_t* buffer);

int is_empty_line(wchar_t* buffer);

wchar_t* skip_white_space(wchar_t* buffer);



#endif
