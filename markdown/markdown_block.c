#include "markdown_internal.h"


wchar_t* md_load_block_code(wchar_t* buffer, md_object_t** object)
{
	wchar_t* backup = buffer;
	wchar_t* start = 0;
	int len = 0;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	/* check start symbol */
	if (!(buffer[0] == L'`' && buffer[1] == L'`' && buffer[2] == L'`' && buffer[3] != L'`'))
		return buffer;

	/* point to start */
	buffer = skip_line(buffer);
	start = buffer;

	/* search until not match or match */
	while (1)
	{
		if (*buffer == 0)
			return backup;

		if (*buffer == L'\r' && 0 == wcsncmp(buffer, L"\r\n```", 5))
			break;

		buffer++;
		len++;
	}

	/* get text and skip line */
	*object = md_new_object();
	(*object)->text = (wchar_t*)safe_malloc(sizeof(wchar_t) * (len + 1));
	memcpy((*object)->text, start, sizeof(wchar_t) * len);
	buffer += 5;
	buffer = skip_line(buffer);

	while (is_empty_line(buffer) && *buffer != 0)
		buffer = skip_line(buffer);

	return buffer;
}

wchar_t* md_load_block_header(wchar_t* buffer, md_object_t** object)
{
	md_object_t* p = 0;
	wchar_t* backup = buffer;
	int level = 0;
	md_inline_element_t* ele = 0;
	md_inline_type type = 0;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	/* check start symbol */
	while (1)
	{
		if (*buffer != L'#')
			break;
		level++;
		buffer++;
	}

	if (level == 0 || level > 6)
		return backup;

	/* skip any white space and point to text */
	buffer = skip_white_space(buffer);

	/* get every inline element */
	*object = md_new_object();
	(*object)->number = level;

	while (1)
	{
		buffer = md_load_inline(buffer, &p, &type);

		if (!p || type == block_unknown)
			break;

		ele = (md_inline_element_t*)safe_malloc(sizeof(md_inline_element_t));
		ele->type = type;
		ele->object = p;
		insert_into_list(&((*object)->inline_element_list), &ele->list_entry);
	}

	/* skip line */
	if (is_list_empty(&((*object)->inline_element_list)))
	{
		md_free_object(*object);
		*object = 0;
	}
	buffer = skip_line(buffer);
	
	while (is_empty_line(buffer) && *buffer != 0)
		buffer = skip_line(buffer);

	return buffer;
}

wchar_t* md_load_block_horizontal_rule(wchar_t* buffer, md_object_t** object)
{
	wchar_t* backup = buffer;
	int count = 0;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	/* check start symbol */
	while (*buffer == L'-')
	{
		count++;
		buffer++;
	}
		

	if (count < 3)
		return backup;

	/* skip line */
	buffer = skip_line(buffer);

	*object = md_new_object();

	return buffer;
}

wchar_t* md_load_block_line_break(wchar_t* buffer, md_object_t** object)
{
	wchar_t* backup = buffer;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	/* check start symbol */
	if (!is_empty_line(buffer) || *buffer == 0)
		return backup;

	while (is_empty_line(buffer) && *buffer != 0)
		buffer = skip_line(buffer);

	*object = md_new_object();

	return buffer;
}

int md_check_ordered_list_count(wchar_t* buffer)
{
	int count = 0;

	if (!buffer)
		return 0;

	for (;;)
	{
		if (*buffer >= L'0' && *buffer <= L'9' && L'.' == buffer[1] && is_white_spacew(buffer[2]))
		{
			buffer = skip_line(buffer);
			count++;
		}
		else
			break;

		while (*buffer != 0 && is_empty_line(buffer))
			buffer = skip_line(buffer);
	}

	return count;
}

wchar_t* md_load_block_ordered_list(wchar_t* buffer, md_object_t** object)
{
	md_object_t* p = 0;
	wchar_t* backup = buffer;
	md_inline_element_t* ele = 0;
	md_inline_type type = 0;
	int count = 0;
	int i = 0;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	count = md_check_ordered_list_count(buffer);
	if (!count)
		return buffer;

	/* get every inline element */
	*object = md_new_object();
	(*object)->number = count;
	(*object)->data_list_head = (list_entry_t*)safe_malloc(sizeof(list_entry_t) * count);

	for (i = 0; i < count; i++)
	{
		initialize_list_head(&(*object)->data_list_head[i]);

		buffer += 2;

		while (1)
		{
			buffer = md_load_inline(buffer, &p, &type);

			if (!p || type == block_unknown)
				break;

			ele = (md_inline_element_t*)safe_malloc(sizeof(md_inline_element_t));
			ele->type = type;
			ele->object = p;
			insert_into_list(&(*object)->data_list_head[i], &ele->list_entry);
		}

		while (*buffer != 0 && is_empty_line(buffer))
			buffer = skip_line(buffer);
	}

	return buffer;
}

wchar_t* md_load_block_paragraph(wchar_t* buffer, md_object_t** object)
{
	md_object_t* p = 0;
	wchar_t* backup = buffer;
	md_inline_element_t* ele = 0;
	md_inline_type type = 0;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	/* skip any white space and point to text */
	buffer = skip_white_space(buffer);

	/* get every inline element */
	*object = md_new_object();

	while (1)
	{
		buffer = md_load_inline(buffer, &p, &type);

		if (!p || type == block_unknown)
		{
			buffer = skip_line(buffer);
			if (is_empty_line(buffer))
				break;
		}

		ele = (md_inline_element_t*)safe_malloc(sizeof(md_inline_element_t));
		ele->type = type;
		ele->object = p;
		insert_into_list(&(*object)->inline_element_list, &ele->list_entry);
	}

	/* skip line */
	if (is_list_empty(&(*object)->inline_element_list))
	{
		md_free_object(*object);
		*object = 0;
	}
	buffer = skip_line(buffer);

	return buffer;
}

wchar_t* md_load_block_quote(wchar_t* buffer, md_object_t** object)
{
	md_object_t* p = 0;
	wchar_t* backup = buffer;
	md_inline_element_t* ele = 0;
	md_inline_type type = 0;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	/* check start symbol */
	if (!(L'>' == *buffer && is_white_spacew(buffer[1])))
		return backup;

	/* skip any white space and point to text */
	buffer += 2;
	buffer = skip_white_space(buffer);

	/* get every inline element */
	*object = md_new_object();

	while (1)
	{
		buffer = md_load_inline(buffer, &p, &type);

		if (!p || type == block_unknown)
		{
			buffer = skip_line(buffer);
			if (is_empty_line(buffer) || !(L'>' == *buffer && is_white_spacew(buffer[1])))
				break;
			buffer += 2;
		}

		ele = (md_inline_element_t*)safe_malloc(sizeof(md_inline_element_t));
		ele->type = type;
		ele->object = p;
		insert_into_list(&(*object)->inline_element_list, &ele->list_entry);
	}

	/* skip line */
	if (is_list_empty(&(*object)->inline_element_list))
	{
		md_free_object(*object);
		*object = 0;
	}
	buffer = skip_line(buffer);

	return buffer;
}

int md_check_block_table_col(wchar_t* buffer)
{
	int count = 0;

	if (!buffer)
		return 0;

	/* skip current line, first char must be '|' */
	buffer = skip_line(buffer);
	if (*buffer != L'|')
		return 0;

	for (;;)
	{	
		/* now is '|', so skip it */
		buffer++;

		/* skip any ':'/'-'/'white space' */
		while (*buffer == L':' || *buffer == L'-' || (is_white_spacew(*buffer) && *buffer != L'\r'))
			buffer++;

		/* if reach '|' then count++; if reach end of line, finish; if any other char, not a table */
		if (*buffer == L'|')
			count++;
		else if (*buffer == L'\r')
			break;
		else
			return 0;
	}
	
	return count;
}

int md_check_block_table_row(wchar_t* buffer)
{
	int count = 0;

	if (!buffer)
		return 0;

	/* skip head */
	buffer = skip_line(buffer);

	while (*buffer == L'|')
	{
		count++;
		buffer = skip_line(buffer);
	}

	return count;
}

wchar_t* md_load_block_table(wchar_t* buffer, md_object_t** object)
{
	md_object_t* p = 0;
	wchar_t* backup = buffer;
	wchar_t* start = 0;
	md_inline_element_t* ele = 0;
	md_inline_type type = 0;
	int col = 0;
	int row = 0;
	int i = 0;
	int j = 0;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	/* check start symbol */
	if (L'|' != *buffer)
		return buffer;

	/* check |:---:|----|-----:| line*/
	col = md_check_block_table_col(buffer);
	if (!col)
		return buffer;

	/* check row count */
	row = md_check_block_table_row(buffer);
	if (!row)
		return buffer;

	/* take it as a table any way */
	*object = md_new_object();
	(*object)->col_count = col;
	(*object)->row_count = row;
	(*object)->data_list_head = (list_entry_t*)safe_malloc(sizeof(list_entry_t) * col * row);
	
	do
	{
		for (i = 0; i < row; i++)
		{
			/* skip second line */
			if (i == 1)
				buffer = skip_line(buffer);

			/* each column */
			for (j = 0; j < col; j++)
			{
				initialize_list_head(&(*object)->data_list_head[col * i + j]);

				if (*buffer != L'|')
					continue;

				buffer++;
				while (L'\r' != *buffer && is_white_spacew(*buffer))
					buffer++;

				while (1)
				{
					if (L'|' == *buffer)
						break;

					buffer = md_load_inline(buffer, &p, &type);
					if (!p || type == inline_unknown)
						break;

					ele = (md_inline_element_t*)safe_malloc(sizeof(md_inline_element_t));
					ele->type = type;
					ele->object = p;
					insert_into_list(&((*object)->data_list_head[col * i + j]), &ele->list_entry);
				}

				/* skip white space but not new line */
				while (L'\r' != *buffer && is_white_spacew(*buffer))
					buffer++;
			}

			buffer = skip_line(buffer);
		}
	} while (0);
	
	return buffer;
}

int md_check_unordered_list_count(wchar_t* buffer)
{
	int count = 0;

	if (!buffer)
		return 0;

	for (;;)
	{
		if (buffer[0] == L'*' && is_white_spacew(buffer[1]))
		{
			buffer = skip_line(buffer);
			count++;
		}
		else
			break;

		while (*buffer != 0 && is_empty_line(buffer))
			buffer = skip_line(buffer);
	}

	return count;
}

wchar_t* md_load_block_unordered_list(wchar_t* buffer, md_object_t** object)
{
	md_object_t* p = 0;
	wchar_t* backup = buffer;
	md_inline_element_t* ele = 0;
	md_inline_type type = 0;
	int count = 0;
	int i = 0;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	count = md_check_unordered_list_count(buffer);
	if (!count)
		return buffer;

	/* get every inline element */
	*object = md_new_object();
	(*object)->number = count;
	(*object)->data_list_head = (list_entry_t*)safe_malloc(sizeof(list_entry_t) * count);

	for (i = 0; i < count; i++)
	{
		initialize_list_head(&(*object)->data_list_head[i]);

		assert(*buffer == L'*');
		buffer++;

		while (1)
		{
			buffer = md_load_inline(buffer, &p, &type);

			if (!p || type == block_unknown)
				break;

			ele = (md_inline_element_t*)safe_malloc(sizeof(md_inline_element_t));
			ele->type = type;
			ele->object = p;
			insert_into_list(&(*object)->data_list_head[i], &ele->list_entry);
		}

		while (*buffer != 0 && is_empty_line(buffer))
			buffer = skip_line(buffer);
	}

	return buffer;
}

wchar_t* md_load_block(wchar_t* buffer, md_object_t** object, md_block_type* type)
{
	if (!buffer || !object || !type)
		return buffer;

	*type = block_unknown;
	*object = 0;

	do
	{
		buffer = md_load_block_code(buffer, object);
		if (*object) {
			*type = block_code;
			break;
		}

		buffer = md_load_block_header(buffer, object);
		if (*object) {
			*type = block_header;
			break;
		}

		buffer = md_load_block_horizontal_rule(buffer, object);
		if (*object) {
			*type = block_horizontal_rule;
			break;
		}

		buffer = md_load_block_line_break(buffer, object);
		if (*object) {
			*type = block_line_break;
			break;
		}

		buffer = md_load_block_ordered_list(buffer, object);
		if (*object) {
			*type = block_ordered_list;
			break;
		}

		buffer = md_load_block_quote(buffer, object);
		if (*object) {
			*type = block_quote;
			break;
		}

		buffer = md_load_block_table(buffer, object);
		if (*object) {
			*type = block_table;
			break;
		}

		buffer = md_load_block_unordered_list(buffer, object);
		if (*object) {
			*type = block_unordered_list;
			break;
		}

		buffer = md_load_block_paragraph(buffer, object);
		if (*object) {
			*type = block_paragraph;
			break;
		}

	} while (0);

	return buffer;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void md_block_code_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	wchar_t* wcs1 = 0;
	wchar_t* wcs2 = 0;
	char* str = 0;
	
	wcs1 = wcs_replace(object->text, L"<", L"&lt;");
	if (wcs1)
		wcs2 = wcs_replace(wcs1, L">", L"&gt;");
	else
		wcs2 = wcs_replace(object->text, L">", L"&gt;");
	
	if (wcs2)
		str = wcs_to_str(wcs2, -1, encoding);
	else
		str = wcs_to_str(object->text, -1, encoding);

	fprintf(file, "<pre><code>");
	//fwrite(str, strlen(str), 1, file);
	md_code_string_to_syntax_html(str, file);

	fprintf(file, "</code></pre>");
	
	free(str);
	free(wcs2);
	free(wcs1);
}

void md_block_header_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	list_entry_t* list_entry = 0;
	md_inline_element_t* ele = 0;

	fprintf(file, "<h%d>", object->number);
	for (list_entry = object->inline_element_list.flink; list_entry != &object->inline_element_list; list_entry = list_entry->flink)
	{
		ele = container_of(list_entry, md_inline_element_t, list_entry);
		md_inline_to_html(ele, encoding, file);
	}
	fprintf(file, "</h%d>", object->number);
}

void md_block_horizontal_rule_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	fprintf(file, "<hr/>\r\n");
}

void md_block_line_break_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	fprintf(file, "<br/>\r\n");
}

void md_block_ordered_list_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	list_entry_t* list_entry = 0;
	md_inline_element_t* ele = 0;
	int i = 0;

	fprintf(file, "<ol>");

	for (i = 0; i < object->number; i++)
	{
		fprintf(file, "<li>");

		for (list_entry = object->data_list_head[i].flink; list_entry != &object->data_list_head[i]; list_entry = list_entry->flink)
		{
			ele = container_of(list_entry, md_inline_element_t, list_entry);
			md_inline_to_html(ele, encoding, file);
		}
		fprintf(file, "</li>\r\n");
	}

	fprintf(file, "</ol>\r\n");
}

void md_block_paragraph_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	list_entry_t* list_entry = 0;
	md_inline_element_t* ele = 0;

	fprintf(file, "<p>");
	for (list_entry = object->inline_element_list.flink; list_entry != &object->inline_element_list; list_entry = list_entry->flink)
	{
		ele = container_of(list_entry, md_inline_element_t, list_entry);
		md_inline_to_html(ele, encoding, file);
	}
	fprintf(file, "</p>\r\n");
}

void md_block_quote_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	list_entry_t* list_entry = 0;
	md_inline_element_t* ele = 0;

	fprintf(file, "<blockquote>");
	for (list_entry = object->inline_element_list.flink; list_entry != &object->inline_element_list; list_entry = list_entry->flink)
	{
		ele = container_of(list_entry, md_inline_element_t, list_entry);
		md_inline_to_html(ele, encoding, file);
	}
	fprintf(file, "</blockquote>\r\n");
}

void md_block_table_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	list_entry_t* list_entry = 0;
	md_inline_element_t* ele = 0;
	int i = 0;
	int j = 0;

	fprintf(file, "<table>");

	for (i = 0; i < object->row_count; i++)
	{
		fprintf(file, "<tr>");

		for (j = 0; j < object->col_count; j++)
		{
			fprintf(file, i == 0 ? "<th>" : "<td>");
			for (list_entry = object->data_list_head[i * object->col_count + j].flink; list_entry != &object->data_list_head[i * object->col_count + j]; list_entry = list_entry->flink)
			{
				ele = container_of(list_entry, md_inline_element_t, list_entry);
				md_inline_to_html(ele, encoding, file);				
			}
			fprintf(file, i == 0 ? "</th>" : "</td>");
		}

		fprintf(file, "</tr>\r\n");
	}

	fprintf(file, "</table>\r\n");
}

void md_block_unordered_list_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	list_entry_t* list_entry = 0;
	md_inline_element_t* ele = 0;
	int i = 0;

	fprintf(file, "<ul>");

	for (i = 0; i < object->number; i++)
	{
		fprintf(file, "<li>");

		for (list_entry = object->data_list_head[i].flink; list_entry != &object->data_list_head[i]; list_entry = list_entry->flink)
		{
			ele = container_of(list_entry, md_inline_element_t, list_entry);
			md_inline_to_html(ele, encoding, file);
		}
		fprintf(file, "</li>\r\n");
	}

	fprintf(file, "</ul>\r\n");
}

void md_block_to_html(md_block_element_t* block_element, str_encoding encoding, FILE* file)
{
	if (!block_element || !file)
		return;

	switch (block_element->type)
	{
	case block_code:
		md_block_code_to_html(block_element->object, encoding, file);
		break;
	case block_header:
		md_block_header_to_html(block_element->object, encoding, file);
		break;
	case block_horizontal_rule:
		md_block_horizontal_rule_to_html(block_element->object, encoding, file);
		break;
	case block_line_break:
		md_block_line_break_to_html(block_element->object, encoding, file);
		break;
	case block_ordered_list:
		md_block_ordered_list_to_html(block_element->object, encoding, file);
		break;
	case block_paragraph:
		md_block_paragraph_to_html(block_element->object, encoding, file);
		break;
	case block_quote:
		md_block_quote_to_html(block_element->object, encoding, file);
		break;
	case block_table:
		md_block_table_to_html(block_element->object, encoding, file);
		break;
	case block_unordered_list:
		md_block_unordered_list_to_html(block_element->object, encoding, file);
		break;
	default:
		break;
	}
}