#ifndef _tinyxml_h_
#define _tinyxml_h_

#include <stdio.h>
#include <wchar.h>
#include "tinylist.h"
#include "tinystr.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct
	{
		char* key;
		wchar_t* value;
		list_entry_t list_entry;
	}xml_attribute;

	typedef struct
	{
		char* name;
		list_entry_t attribute_list;
		list_entry_t element_list;
		wchar_t* text;
		list_entry_t list_entry;
	}xml_element;

	typedef struct
	{
		list_entry_t attribute_list;
	}xml_declaration;

	typedef struct
	{	
		xml_declaration* declaration;
		xml_element* root_element;
	}xml_document;



	/* attribute */
	/* new or free */
	xml_attribute* xml_new_attribute(char* key, wchar_t* value);

	void xml_free_attribute(xml_attribute* attribute);

	/* load or write */
	char* xml_load_attribute(char* buffer, str_encoding encoding, xml_attribute** attribute);

	void xml_write_attribute(FILE* file, str_encoding encoding, xml_attribute* attribute);


	/* element */
	/* new or free */
	xml_element* xml_new_element(char* name);

	void xml_free_element(xml_element* element);

	/* load or write */
	char* xml_load_element(char* buffer, str_encoding encoding, xml_element** element);

	void xml_write_element(FILE* file, str_encoding encoding, xml_element* element, int indent);

	/* set or insert */
	void xml_insert_element(xml_element* element, xml_element* child_element);

	void xml_set_text(xml_element* element, wchar_t* text);

	void xml_set_attribute(xml_element* element, char* key, wchar_t* value);

	/* remove */
	void xml_remove_element(xml_element* element, xml_element* child_element);

	void xml_remove_attribute(xml_element* element, char* key);

	void xml_remove_text(xml_element* element);

	/* query */
	wchar_t* xml_query_attribute(xml_element* element, char* key);


	/* declaration */
	/* new or free */
	void xml_free_declaration(xml_declaration* declaration);

	/* load or write */
	char* xml_load_declaration(char* buffer, str_encoding encoding, xml_declaration** declaration);

	void xml_write_declaration(FILE* file, str_encoding encoding, xml_declaration* declaration);



	/* document */
	/* new or free */
	xml_document* xml_new_document(char* root);

	void xml_free_document(xml_document* document);

	/* load or write */
	char* xml_load_document(char* buffer, str_encoding encoding, xml_document** document);

	void xml_write_document(FILE* file, str_encoding encoding, xml_document* document);

#ifdef __cplusplus
}
#endif

#endif
