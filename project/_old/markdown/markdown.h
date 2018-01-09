#ifndef _markdown_h_
#define _markdown_h_

#include "tinylist.h"
#include "tinystr.h"
//#include "tinyhtml.h"

#ifdef __cplusplus
extern "C" {
#endif

	/* https://github.com/adam-p/markdown-here/wiki/Markdown-Cheatsheet */

	typedef struct
	{
		list_entry_t block_element_list;
	}md_document_t;

	md_document_t* md_load_document(wchar_t* buffer);

	void md_free_document(md_document_t* document);

	void md_write_document(md_document_t* document, str_encoding encoding, FILE* file);

	void md_document_to_html(md_document_t* document, str_encoding encoding, FILE* file);

#ifdef __cplusplus
}
#endif

#endif _markdown_h_
