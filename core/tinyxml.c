#include "tinyxml.h" 
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <assert.h>

/*
* space, tab, new line, return
*/
static char* skip_white_space(char* buffer)
{
	if (!buffer)
		return 0;

	while (is_white_space(*buffer))
		buffer++;

	return buffer;
}

static void xml_write_indent(FILE* file, int indent)
{
	if (indent <= 0 || !file)
		return;

	while (indent--)
		fprintf(file, "  ");
}

/* attribute */

xml_attribute* xml_new_attribute(char* key, wchar_t* value)
{
	xml_attribute* attr = (xml_attribute*)safe_malloc(sizeof(xml_attribute));

	attr->key = (char*)safe_malloc(strlen(key) + 1);
	memcpy(attr->key, key, strlen(key));

	attr->value = (wchar_t*)safe_malloc(sizeof(wchar_t) * (wcslen(value) + 1));
	memcpy(attr->value, value, sizeof(wchar_t) * wcslen(value));
	return attr;
}

void xml_free_attribute(xml_attribute* attr)
{
	if (!attr)
		return;

	if (attr->key)
		free(attr->key);

	if (attr->value)
		free(attr->value);

	free(attr);
}

/*
* 1. name="andy" ....
*/
char* xml_load_attribute(char* buffer, str_encoding encoding, xml_attribute** attribute)
{
	char* key = 0;
	int key_len = 0;
	char* value = 0;
	int value_len = 0;

	if (!buffer || !attribute)
		return 0;

	*attribute = 0;

	buffer = skip_white_space(buffer);

	if (!isalpha(*buffer))
		return buffer;

	*attribute = (xml_attribute*)safe_malloc(sizeof(xml_attribute));

	/* must all-alpha= */
	key = buffer;
	while (1)
	{
		assert(isalnum(*buffer) || *buffer == '=');
		if (!isalnum(*buffer))
			break;

		key_len++;
		buffer++;
	}

	(*attribute)->key = (char*)safe_malloc(key_len + 1);
	memcpy((*attribute)->key, key, key_len);

	/* skip = */
	buffer++;

	/* must "all-print" */
	assert(*buffer == '"');

	/* skip " */
	buffer++;

	value = buffer;
	while (1)
	{
		assert(*buffer != 0);
		if (*buffer == '"')
			break;

		value_len++;
		buffer++;
	}

	(*attribute)->value = str_to_wcs(value, value_len, encoding);

	/* skip " */
	buffer++;

	return buffer;
}

void xml_write_attribute(FILE* file, str_encoding encoding, xml_attribute* attribute)
{
	char* str = 0;
	if (!attribute || !attribute->key || !attribute->value || !file)
		return;

	str = wcs_to_str(attribute->value, -1, encoding);
	fprintf(file, " %s=\"%s\"", attribute->key, str);
	free(str);
}


/* element */

/* new or free */
xml_element* xml_new_element(char* name)
{
	xml_element* ele = (xml_element*)safe_malloc(sizeof(xml_element));

	ele->name = (char*)safe_malloc(strlen(name) + 1);
	memcpy(ele->name, name, strlen(name));

	initialize_list_head(&ele->attribute_list);
	initialize_list_head(&ele->element_list);
	ele->text = 0;
	return ele;
}

void xml_free_element(xml_element* element)
{
	xml_attribute* attr = 0;
	xml_element* ele = 0;
	list_entry_t* list_entry = 0;

	if (!element)
		return;

	if (element->name)
		free(element->name);

	while (!is_list_empty(&element->attribute_list))
	{
		attr = container_of(element->attribute_list.flink, xml_attribute, list_entry);
		remove_from_list(element->attribute_list.flink);
		xml_free_attribute(attr);
	}

	while (!is_list_empty(&element->element_list))
	{
		ele = container_of(element->element_list.flink, xml_element, list_entry);
		remove_from_list(element->element_list.flink);
		xml_free_element(ele);
	}
}

/*
* 1. <person name="andy" id="123"/>
* 2. <person name="andy" id="123"> <friend name="bob"/> </person>
*/
char* xml_load_element(char* buffer, str_encoding encoding, xml_element** element)
{
	char* name = 0;
	char* text = 0;
	int name_len = 0;
	int text_len = 0;
	xml_attribute* attr = 0;
	xml_element* ele = 0;

	if (!buffer || !element)
		return 0;

	*element = 0;

	buffer = skip_white_space(buffer);

	/* must start with < */
	assert(*buffer == '<');

	/* not element start, but element ends </end> */
	if (*(buffer + 1) == '/')
		return buffer;

	/* must element */
	*element = (xml_element*)safe_malloc(sizeof(xml_element));

	buffer++;
	buffer = skip_white_space(buffer);

	/* name must all-alpha, ends with ' ' */
	name = buffer;
	while (1)
	{
		assert(*buffer != 0);
		if (!isalpha(*buffer))
			break;

		buffer++;
		name_len++;
	}
	(*element)->name = (char*)safe_malloc(name_len + 1);
	memcpy((*element)->name, name, name_len);

	/* deal with all attributes */
	initialize_list_head(&(*element)->attribute_list);

	do
	{
		buffer = xml_load_attribute(buffer, encoding, &attr);
		if (!attr)
			break;

		insert_into_list(&(*element)->attribute_list, &attr->list_entry);

	} while (1);


	buffer = skip_white_space(buffer);

	/* deal with all child elements */
	initialize_list_head(&(*element)->element_list);
	assert(*buffer == '>' || (*buffer == '/' && *(buffer + 1) == '>'));

	if (*buffer == '>')
	{
		buffer++;

		buffer = skip_white_space(buffer);

		/* text or children element */
		if (*buffer == '<')
		{
			do
			{
				buffer = xml_load_element(buffer, encoding, &ele);
				if (!ele)
					break;

				insert_into_list(&(*element)->element_list, &ele->list_entry);

			} while (1);
		}
		else
		{
			text = buffer;
			while (1)
			{
				assert(*buffer != 0);
				if (*buffer == '<')
					break;

				text_len++;
				buffer++;
			}

			(*element)->text = str_to_wcs(text, text_len, encoding);
		}


		/* deal with match <person /> */
		buffer = skip_white_space(buffer);

		if ((*buffer == '<' && *(buffer + 1) == '/'));
		buffer += 2;

		buffer = skip_white_space(buffer);

		assert(0 == strncmp(name, buffer, name_len));

		buffer += name_len;

		buffer = skip_white_space(buffer);

		assert(*buffer == '>');

		buffer++;
	}
	else
		buffer += 2;

	return buffer;
}

void xml_write_element(FILE* file, str_encoding encoding, xml_element* element, int indent)
{
	xml_attribute* attr = 0;
	xml_element* ele = 0;
	list_entry_t* list_entry = 0;
	char* text = 0;

	if (!element || !element->name || !file)
		return;

	xml_write_indent(file, indent);
	fprintf(file, "<%s", element->name);

	/* all attribute */
	for (list_entry = element->attribute_list.flink; list_entry != &element->attribute_list; list_entry = list_entry->flink)
	{
		attr = container_of(list_entry, xml_attribute, list_entry);
		xml_write_attribute(file, encoding, attr);
	}

	/* text or children element */
	if (element->text)
	{
		text = str_trim_end(wcs_to_str(element->text, -1, encoding));
		fprintf(file, "> %s </%s>\r\n", text, element->name);
		free(text);
	}
	else
	{
		if (!is_list_empty(&element->element_list))
		{
			fprintf(file, ">\r\n");

			/* all elements */
			for (list_entry = element->element_list.flink; list_entry != &element->element_list; list_entry = list_entry->flink)
			{
				ele = container_of(list_entry, xml_element, list_entry);
				xml_write_element(file, encoding, ele, indent + 1);
			}

			xml_write_indent(file, indent);
			fprintf(file, "</%s>\r\n", element->name);
		}
		else
			fprintf(file, "/>\r\n");
	}
}

/* set or insert */
void xml_set_text(xml_element* element, wchar_t* text)
{
	if (!element)
		return;

	if (element->text)
		free(element->text);

	element->text = (wchar_t*)safe_malloc(sizeof(wchar_t) * (wcslen(text) + 1));
	memcpy(element->text, text, sizeof(wchar_t) * wcslen(text));
}

void xml_set_attribute(xml_element* element, char* key, wchar_t* value)
{
	list_entry_t* list_entry = 0;
	xml_attribute* attr = 0;

	if (!element || !key || !value)
		return;

	for (list_entry = element->attribute_list.flink; list_entry != &element->attribute_list; list_entry = list_entry->flink)
	{
		attr = container_of(list_entry, xml_attribute, list_entry);
		if (attr)
		{
			if (!strcmp(attr->key, key))
			{
				free(attr->value);
				attr->value = (wchar_t*)malloc(sizeof(wchar_t) * (wcslen(value) + 1));
				memcpy(attr->value, value, sizeof(wchar_t) * wcslen(value));
				return;
			}
		}
	}

	/* new attribute */
	attr = xml_new_attribute(key, value);
	if (attr)
		insert_into_list(&element->attribute_list, &attr->list_entry);
}

void xml_insert_element(xml_element* element, xml_element* child_element)
{
	if (!element || !child_element)
		return;

	insert_into_list(&element->element_list, &child_element->list_entry);
}

/* remove */
void xml_remove_attribute(xml_element* element, char* key)
{
	list_entry_t* list_entry = 0;
	xml_attribute* attr = 0;

	if (!element || !key)
		return;

	for (list_entry = element->attribute_list.flink; list_entry != &element->attribute_list; list_entry = list_entry->flink)
	{
		attr = container_of(list_entry, xml_attribute, list_entry);
		if (attr)
		{
			if (!strcmp(attr->key, key))
			{
				remove_from_list(list_entry);
				return;
			}
		}
	}
}

void xml_remove_element(xml_element* element, xml_element* child_element)
{
	if (!element || !child_element)
		return;

	remove_from_list(&child_element->list_entry);
}

void xml_remove_text(xml_element* element)
{
	if (!element)
		return;

	free(element->text);
	element->text = 0;
}

/* query */
wchar_t* xml_query_attribute(xml_element* element, char* key)
{
	list_entry_t* list_entry = 0;
	xml_attribute* attr = 0;

	if (!element || !key)
		return 0;

	for (list_entry = element->attribute_list.flink; list_entry != &element->attribute_list; list_entry = list_entry->flink)
	{
		attr = container_of(list_entry, xml_attribute, list_entry);
		if (attr)
		{
			if (!strcmp(attr->key, key))
				return attr->value;
		}
	}
	return 0;
}

/* document */
xml_document* xml_new_document(char* root)
{
	xml_document* doc = 0;

	doc = (xml_document*)safe_malloc(sizeof(xml_document));
	doc->root_element = xml_new_element(root);

	return doc;
}

char* xml_load_document(char* buffer, str_encoding encoding, xml_document** document)
{
	xml_attribute* attr = 0;
	list_entry_t* list_entry;
	if (!buffer || !document)
		return 0;

	*document = (xml_document*)safe_malloc(sizeof(xml_document));

	buffer = xml_load_declaration(buffer, encoding, &(*document)->declaration);
	if ((*document)->declaration && !is_list_empty(&(*document)->declaration->attribute_list))
	{
		for (list_entry = (*document)->declaration->attribute_list.flink; list_entry != &(*document)->declaration->attribute_list; list_entry = list_entry->flink)
		{
			attr = container_of(list_entry, xml_attribute, list_entry);
			if (attr && 0 == strcmp(attr->key, "encoding"))
			{
				if (0 == wcscmp(attr->value, L"utf-8"))
					encoding = encoding_utf8;
				else if (0 == wcscmp(attr->value, L"gb2312"))
					encoding = encoding_ansi;
				break;
			}
		}
	}

	buffer = xml_load_element(buffer, encoding, &(*document)->root_element);

	return buffer;
}

void xml_write_document(FILE* file, str_encoding encoding, xml_document* document)
{
	if (!document || !file)
		return;

	xml_write_element(file, encoding, document->root_element, 0);
}

void xml_free_document(xml_document* document)
{
	if (!document)
		return;

	if (document->root_element)
		xml_free_element(document->root_element);
}


/* declaration */

char* xml_load_declaration(char* buffer, str_encoding encoding, xml_declaration** declaration)
{
	xml_attribute* attr = 0;
	xml_attribute* attr_next = 0;
	
	if (!buffer || !declaration)
		return 0;

	*declaration = 0;

	buffer = skip_white_space(buffer);

	/* if not <?xml , then not declaration */
	if (strncmp(buffer, "<?xml ", 6))
		return buffer;

	/* must declaration */
	*declaration = (xml_declaration*)safe_malloc(sizeof(xml_declaration));
	buffer += 6;
	buffer = skip_white_space(buffer);

	/* deal with all attributes */
	initialize_list_head(&(*declaration)->attribute_list);
	do
	{
		buffer = xml_load_attribute(buffer, encoding, &attr);
		if (!attr)
			break;

		insert_into_list(&(*declaration)->attribute_list, &attr->list_entry);

	} while (1);

	buffer = skip_white_space(buffer);
	assert(*buffer == '?' && *(buffer + 1) == '>');

	buffer += 2;
	return buffer;
}

void xml_write_declaration(FILE* file, str_encoding encoding, xml_declaration* declaration)
{
	xml_attribute* attr = 0;
	list_entry_t* list_entry = 0;

	if (!declaration || !file)
		return;

	/* all attribute */
	if (!is_list_empty(&declaration->attribute_list))
	{
		fprintf(file, "<?xml");

		for (list_entry = declaration->attribute_list.flink; list_entry != &declaration->attribute_list; list_entry = list_entry->flink)
		{
			attr = container_of(list_entry, xml_attribute, list_entry);
			xml_write_attribute(file, encoding, attr);
		}

		fprintf(file, "?>");
	}
}

void xml_free_declaration(xml_declaration* declaration)
{
	xml_attribute* attr = 0;
	xml_attribute* attr_tmp = 0;

	if (!declaration)
		return;

	while (!is_list_empty(&declaration->attribute_list))
	{
		attr = container_of(declaration->attribute_list.flink, xml_attribute, list_entry);
		remove_from_list(declaration->attribute_list.flink);
		xml_free_attribute(attr);
	}
}

#ifdef test_xml
static void test_declaration()
{
	char* buffer = " <?xml version=\"1.0\" ?>";
	xml_declaration* decl = 0;

	buffer = xml_load_declaration(buffer, encoding_utf8, &decl);
	xml_write_declaration(stdout, encoding_ansi, decl);
	xml_free_declaration(decl);
	decl = 0;
}

static void test_attribute()
{
	char* buffer = "	name=\"andy\" id=\"123\"";
	xml_attribute* attr = 0;

	buffer = xml_load_attribute(buffer, encoding_utf8, &attr);
	xml_write_attribute(stdout, encoding_ansi, attr);
	xml_free_attribute(attr);
	attr = 0;

	buffer = xml_load_attribute(buffer, encoding_utf8, &attr);
	xml_write_attribute(stdout, encoding_ansi, attr);
	xml_free_attribute(attr);
	attr = 0;

	printf("\r\n");
}

static void test_element()
{
	char* buffer =
		"<books>\r\n"
		"    <book name=\"C++ Programming\" id=\"12345-12\" />\r\n"
		"    <book name=\"wuwuwu\" id=\"432432-12\" >\r\n"
		"        <borrower name=\"andy\" id=\"31234\" />\r\n"
		"    </book>\r\n"
		"</books>\r\n";
	xml_element* ele = 0;
	FILE* file = 0;

	file = fopen("C:\\test.xml", "wb");
	if (!file)
		file = stdout;

	buffer = xml_load_element(buffer, encoding_utf8, &ele);
	xml_write_element(file, encoding_ansi, ele, 0);
	xml_free_element(ele);
	ele = 0;

	fclose(file);
}

static void test_document(char* file_path)
{
	xml_document* doc = 0;
	long len = 0;
	char* buffer = 0;
	FILE* file = fopen(file_path, "rb");
	if (!file)
		return;

	fseek(file, 0, SEEK_END);
	len = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer = (char*)safe_malloc(len + 1);
	fread(buffer, len, 1, file);
	fclose(file);

	buffer = xml_load_document(buffer, encoding_utf8, &doc);
	xml_write_document(stdout, encoding_ansi, doc);
	xml_free_document(doc);
}

static void test()
{
	xml_element *ele = 0, *team = 0;
	list_entry_t* list_entry = 0;
	xml_attribute* attr = 0;
	xml_document* doc = 0;
	long len = 0;
	char* buffer = 0;
	FILE* file = fopen("nba.xml", "rb");
	if (!file)
		return;

	fseek(file, 0, SEEK_END);
	len = ftell(file);
	fseek(file, 0, SEEK_SET);

	buffer = (char*)safe_malloc(len + 1);
	fread(buffer, len, 1, file);
	fclose(file);

	buffer = xml_load_document(buffer, encoding_utf8, &doc);
	xml_write_document(stdout, encoding_ansi, doc);

	ele = doc->root_element;
	if (ele)
	{
		team = container_of(ele->element_list.flink, xml_element, list_entry);
		if (team) 
		{
			for (list_entry = team->element_list.flink; list_entry != &team->element_list; list_entry = list_entry->flink)
			{
				ele = container_of(list_entry, xml_element, list_entry);
				xml_write_element(stdout, encoding_ansi, ele, 0);

				if (0 == wcscmp(L"垂戦", xml_query_attribute(ele, "name")))
				{
					xml_set_text(ele, L"込込込込込込 ");
				}
			}
		}
	}
	xml_write_document(stdout, encoding_ansi, doc);

	xml_free_document(doc);
}

void xml_run_tests()
{
	test_document("config.xml");
	test_document("nba.xml");
	test_declaration();
	test_attribute();
	test_element();
	test();
}

int main()
{
	xml_run_tests();
	return 0;
}
#endif