#include "markdown_internal.h"


wchar_t* md_load_inline_code(wchar_t* buffer, md_object_t** object)
{
	wchar_t* backup = buffer;
	wchar_t* start = 0;
	int len = 0;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	/* find symbol (skip if any white space) */
	buffer = skip_white_space(buffer);
	if (!(buffer[0] == '`' && buffer[1] != '`'))
		return backup;

	/* skip symbol and point to start text*/
	buffer++;
	//no need: buffer = skip_white_space(buffer);
	start = buffer;

	/* search until not match or match finally */
	while (1)
	{
		/* no match*/
		if (*buffer == 0 || *buffer == L'\r')
			return backup;

		/* match */
		if (*buffer == L'`')
			break;

		buffer++;
		len++;
	}

	/* get text and skip match symbol */
	*object = md_new_object();
	(*object)->text = (wchar_t*)safe_malloc((len + 1) * sizeof(wchar_t));
	memcpy((*object)->text, start, sizeof(wchar_t) * len);
	buffer++;

	return buffer;
}

wchar_t* md_load_inline_emphasis(wchar_t* buffer, md_object_t** object)
{
	wchar_t* backup = buffer;
	wchar_t* start = 0;
	int len = 0;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	/* find symbol (skip if any white space) */
	buffer = skip_white_space(buffer);
	if (!(buffer[0] == L'*' && buffer[1] != L'*'))
		return backup;

	/* skip symbol and point to start text*/
	buffer++;
	buffer = skip_white_space(buffer);
	start = buffer;

	/* search until not match or match finally */
	while (1)
	{
		/* no match*/
		if (*buffer == 0 || *buffer == L'\r')
			return backup;

		/* match */
		if (*buffer == L'*')
			break;

		buffer++;
		len++;
	}

	/* get text and skip match symbol */
	*object = md_new_object();
	(*object)->text = (wchar_t*)safe_malloc((len + 1) * sizeof(wchar_t));
	memcpy((*object)->text, start, sizeof(wchar_t) * len);
	buffer++;

	return buffer;
}

wchar_t* md_load_inline_image(wchar_t* buffer, md_object_t** object)
{
	wchar_t* backup = buffer;
	wchar_t* start = 0;
	int len = 0;
	wchar_t* text;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	/* find symbol (skip if any white space) */
	buffer = skip_white_space(buffer);
	if (buffer[0] != L'!' || buffer[1] != L'[')
		return backup;

	/* skip symbol and point to start text*/
	buffer += 2;
	buffer = skip_white_space(buffer);
	start = buffer;

	/* search until not match or match finally */
	while (1)
	{
		/* no match*/
		if (*buffer == 0 || *buffer == L'\r')
			return backup;

		/* match */
		if (*buffer == L']')
			break;

		buffer++;
		len++;
	}

	/* get text and skip match symbol */
	text = (wchar_t*)safe_malloc(sizeof(wchar_t) * (len + 1));
	memcpy(text, start, sizeof(wchar_t) * len);
	text = wcs_trim_end(text);
	buffer++;

	/* find symbol (skip if any white space) */
	buffer = skip_white_space(buffer);
	if (buffer[0] != L'(')
	{
		free(text);
		return backup;
	}

	/* skip symbol and point to start text*/
	buffer++;
	buffer = skip_white_space(buffer);
	start = buffer;

	/* search until not match or match finally */
	len = 0;
	while (1)
	{
		/* no match*/
		if (*buffer == 0 || *buffer == L'\r')
		{
			free(text);
			return backup;
		}

		/* match */
		if (*buffer == L')')
			break;

		buffer++;
		len++;
	}

	/* get text and skip match symbol */
	*object = md_new_object();
	(*object)->text = text;
	(*object)->src = (wchar_t*)safe_malloc(sizeof(wchar_t) * (len + 1));
	memcpy((*object)->src, start, sizeof(wchar_t) * len);
	(*object)->src = wcs_trim_end((*object)->src);
	buffer++;

	return buffer;
}

wchar_t* md_load_inline_link(wchar_t* buffer, md_object_t** object)
{
	wchar_t* backup = buffer;
	wchar_t* start = 0;
	int len = 0;
	wchar_t* text;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	/* find symbol (skip if any white space) */
	buffer = skip_white_space(buffer);
	if (buffer[0] != L'[')
		return backup;

	/* skip symbol and point to start text*/
	buffer++;
	buffer = skip_white_space(buffer);
	start = buffer;

	/* search until not match or match finally */
	while (1)
	{
		/* no match*/
		if (*buffer == 0 || *buffer == L'\r')
			return backup;

		/* match */
		if (*buffer == L']')
			break;

		buffer++;
		len++;
	}

	/* get text and skip match symbol */
	text = (wchar_t*)safe_malloc(sizeof(wchar_t) * (len + 1));
	memcpy(text, start, sizeof(wchar_t) * len);
	text = wcs_trim_end(text);
	buffer++;

	/* find symbol (skip if any white space) */
	buffer = skip_white_space(buffer);
	if (buffer[0] != L'(')
	{
		free(text);
		return backup;
	}

	/* skip symbol and point to start text*/
	buffer++;
	buffer = skip_white_space(buffer);
	start = buffer;

	/* search until not match or match finally */
	len = 0;
	while (1)
	{
		/* no match*/
		if (*buffer == 0 || *buffer == L'\r')
		{
			free(text);
			return backup;
		}

		/* match */
		if (*buffer == L')')
			break;

		buffer++;
		len++;
	}

	/* get text and skip match symbol */
	*object = md_new_object();
	(*object)->text = text;
	(*object)->href = (wchar_t*)safe_malloc(sizeof(wchar_t) * (len + 1));
	memcpy((*object)->href, start, sizeof(wchar_t) * len);
	(*object)->href = wcs_trim_end((*object)->href);
	buffer++;

	return buffer;
}

wchar_t* md_load_inline_plaintext(wchar_t* buffer, md_object_t** object)
{
	wchar_t* backup = buffer;
	wchar_t* start = 0;
	int len = 0;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	/* skip white space and point to start */
	buffer = skip_white_space(buffer);
	start = buffer;

	/* start with symbol, but fail to match */
	if (L'`' == *buffer || (L'*' == *buffer && L'*' != buffer[1]) || (L'[' == *buffer) || L'|' == *buffer)
	{
		buffer++;
		len++;
	}
	else if ((L'*' == buffer[0] && L'*' == buffer[1] && L'*' != buffer[2]) ||
		(L'!' == buffer[0] && L'[' == buffer[1]))
	{
		buffer += 2;
		len += 2;
	}
	/* or start with plain text*/
	/* do nothing*/

	/* search until not match or match finally */
	while (1)
	{
		/* no match*/
		if (*buffer == 0 || *buffer == L'\r' || *buffer == L'`'
			|| *buffer == L'*' || *buffer == L'['
			|| (L'*' == buffer[0] && L'*' == buffer[1] && L'*' != buffer[2])
			|| (L'!' == buffer[0] && L'[' == buffer[1])
			|| L'|' == *buffer)
			break;

		buffer++;
		len++;
	}

	/* get text and skip match symbol */
	*object = md_new_object();
	(*object)->text = (wchar_t*)safe_malloc(sizeof(wchar_t) * (len + 1));
	memcpy((*object)->text, start, sizeof(wchar_t) * len);
	(*object)->text = wcs_trim_end((*object)->text);
	// skip nothing

	return buffer;
}

wchar_t* md_load_inline_strong(wchar_t* buffer, md_object_t** object)
{
	wchar_t* backup = buffer;
	wchar_t* start = 0;
	int len = 0;

	if (!buffer || !object)
		return buffer;

	*object = 0;

	/* find symbol (skip if any white space) */
	buffer = skip_white_space(buffer);
	if (!(buffer[0] == '*' && buffer[1] == L'*') && buffer[2] != L'*')
		return buffer;

	/* skip symbol and point to start text*/
	buffer += 2;
	buffer = skip_white_space(buffer);
	start = buffer;

	/* search until not match or match finally */
	while (1)
	{
		/* no match*/
		if (*buffer == 0 || *buffer == L'\r')
			return backup;

		/* match */
		if (*buffer == L'*' && buffer[1] == L'*')
			break;

		buffer++;
		len++;
	}

	/* get text and skip match symbol */
	*object = md_new_object();
	(*object)->text = (wchar_t*)safe_malloc(sizeof(wchar_t) * (len + 1));
	memcpy((*object)->text, start, sizeof(wchar_t) * len);
	(*object)->text = wcs_trim_end((*object)->text);
	buffer += 2;

	return buffer;
}


wchar_t* md_load_inline(wchar_t* buffer, md_object_t** object, md_inline_type* type)
{
	if (!buffer || !object || !type)
		return buffer;

	*type = block_unknown;
	*object = 0;

	if (is_empty_line(buffer))
		return buffer;

	do
	{
		buffer = md_load_inline_code(buffer, object);
		if (*object) {
			*type = inline_code;
			break;
		}

		buffer = buffer = md_load_inline_emphasis(buffer, object);
		if (*object) {
			*type = inline_emphasis;
			break;
		}

		buffer = md_load_inline_image(buffer, object);
		if (*object) {
			*type = inline_image;
			break;
		}

		buffer = md_load_inline_link(buffer, object);
		if (*object) {
			*type = inline_link;
			break;
		}

		buffer = md_load_inline_strong(buffer, object);
		if (*object) {
			*type = inline_strong;
			break;
		}

		buffer = md_load_inline_plaintext(buffer, object);
		if (*object) {
			*type = inline_plaintext;
			break;
		}

	} while (0);

	return buffer;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void md_inline_code_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	char* str = wcs_to_str(object->text, -1, encoding);
	fprintf(file, "<code>");
	fwrite(str, strlen(str), 1, file);
	fprintf(file, "</code>");
	free(str);
}

void md_inline_emphasis_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	char* str = wcs_to_str(object->text, -1, encoding);
	fprintf(file, "<em>");
	fwrite(str, strlen(str), 1, file);
	fprintf(file, "</em>");
	free(str);
}

void md_inline_image_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	char* str = wcs_to_str(object->src, -1, encoding);
	fprintf(file, "<img src=\"");
	fwrite(str, strlen(str), 1, file);
	fprintf(file, "\"/>");
	free(str);
}

void md_inline_link_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	char* str = wcs_to_str(object->href, -1, encoding);
	fprintf(file, "<a href=\"");
	fwrite(str, strlen(str), 1, file);
	fprintf(file, "\"/>");
	free(str);
}

void md_inline_plaintext_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	char* str = wcs_to_str(object->text, -1, encoding);
	fwrite(str, strlen(str), 1, file);
	free(str);
}

void md_inline_strong_to_html(md_object_t* object, str_encoding encoding, FILE* file)
{
	char* str = wcs_to_str(object->text, -1, encoding);
	fprintf(file, "<b>");
	fwrite(str, strlen(str), 1, file);
	fprintf(file, "</b>");
	free(str);
}

void md_inline_to_html(md_inline_element_t* inline_element, str_encoding encoding, FILE* file)
{
	if (!inline_element || !inline_element->object || !file)
		return;

	switch (inline_element->type)
	{
	case inline_code:
		md_inline_code_to_html(inline_element->object, encoding, file);
		break;
	case inline_emphasis:
		md_inline_emphasis_to_html(inline_element->object, encoding, file);
		break;
	case inline_image:
		md_inline_image_to_html(inline_element->object, encoding, file);
		break;
	case inline_link:
		md_inline_link_to_html(inline_element->object, encoding, file);
		break;
	case inline_plaintext:
		md_inline_plaintext_to_html(inline_element->object, encoding, file);
		break;
	case inline_strong:
		md_inline_strong_to_html(inline_element->object, encoding, file);
		break; 
	default:
		break;
	}
}