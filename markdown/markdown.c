#include "markdown.h"
#include "markdown_internal.h"
#include <Windows.h>

wchar_t* skip_line(wchar_t* buffer)
{
	if (!buffer)
		return 0;

	while (1)
	{
		if (*buffer == 0)
			break;
		else if (*buffer == L'\r' && buffer[1] == L'\n')
		{
			buffer += 2;
			break;
		}
		buffer++;
	}

	return buffer;
}

int is_empty_line(wchar_t* buffer)
{
	if (!buffer)
		return 0;

	while (1)
	{
		if (*buffer == 0 || (*buffer == L'\r' && buffer[1] == L'\n'))
			break;

		if (!is_white_spacew(*buffer))
		{
			return 0;
		}

		buffer++;
	}

	return 1;
}

wchar_t* skip_white_space(wchar_t* buffer)
{
	if (!buffer)
		return 0;

	while (is_white_spacew(*buffer))
		buffer++;

	return buffer;
}

md_object_t* md_new_object()
{
	md_object_t* obj = (md_object_t*)safe_malloc(sizeof(md_object_t));
	initialize_list_head(&obj->inline_element_list);
	return obj;
}

void md_free_object(md_object_t* object)
{
	md_inline_element_t* ele = 0;
	int i = 0;

	if (!object)
		return;

	free(object->text);
	free(object->href);
	free(object->src);

	while (!is_list_empty(&object->inline_element_list))
	{
		ele = container_of(object->inline_element_list.flink, md_inline_element_t, list_entry);
		md_free_object(ele->object);
		remove_from_list(&ele->list_entry);
		free(ele);
	}

	for (i = 0; i < object->col_count * object->row_count; i++)
	{
		while (!is_list_empty(&object->data_list_head[i]))
		{
			ele = container_of(object->data_list_head[i].flink, md_inline_element_t, list_entry);
			md_free_object(ele->object);
			remove_from_list(&ele->list_entry);
			free(ele);
		}
	}

	free(object->data_list_head);
}


md_document_t* md_load_document(wchar_t* buffer)
{
	md_block_type type;
	md_document_t* doc = 0;
	md_object_t* object = 0;
	md_block_element_t* ele = 0;

	if (!buffer)
		return 0;

	doc = (md_document_t*)safe_malloc(sizeof(md_document_t));
	initialize_list_head(&doc->block_element_list);

	while (1)
	{
		buffer = md_load_block(buffer, &object, &type);

		if (!object || type == block_unknown)
			break;

		ele = (md_block_element_t*)safe_malloc(sizeof(md_block_element_t));
		ele->type = type;
		ele->object = object;
		insert_into_list(&doc->block_element_list, &ele->list_entry);
	}

	return doc;
}

void md_free_document(md_document_t* document)
{
	md_block_element_t* ele = 0;

	if (!document)
		return;

	while (!is_list_empty(&document->block_element_list))
	{
		ele = container_of(document->block_element_list.flink, md_block_element_t, list_entry);
		md_free_object(ele->object);
		remove_from_list(&ele->list_entry);
		free(ele);
	}
}

//void md_write_block(md_block_element_t* block_element)
//{
//
//}
//
//void md_write_document(md_document_t* document, str_encoding encoding, FILE* file)
//{
//	list_entry_t* list_entry = 0;
//	md_block_element_t* ele = 0;
//
//	if (!document || !file)
//		return;
//
//	for (list_entry = document->block_element_list.flink; list_entry != &document->block_element_list; list_entry = list_entry->flink)
//	{
//		ele = container_of(list_entry, md_block_element_t, list_entry);
//		md_write_block(ele);
//	}
//}

/*https://github.com/sindresorhus/github-markdown-css/blob/gh-pages/github-markdown.css*/

void md_document_to_html(md_document_t* document, str_encoding encoding, FILE* file)
{
	list_entry_t* list_entry = 0;
	md_block_element_t* ele = 0;
	char* buf =
		"<html>\r\n"
		"<head>\r\n"
		"\t<link href=\"markdown.css\" rel=\"stylesheet\" type=\"text/css\"/>\r\n"
		"</head>\r\n"
		"<body>\r\n"
		"<article>\r\n";

	char* buf2 =
		"</article>\r\n";
		"</body>\r\n"
		"</html>\r\n";

	if (!document || !file)
		return;

	fprintf(file, buf);

	for (list_entry = document->block_element_list.flink; list_entry != &document->block_element_list; list_entry = list_entry->flink)
	{
		ele = container_of(list_entry, md_block_element_t, list_entry);
		md_block_to_html(ele, encoding, file);
	}

	fprintf(file, buf2);
}
/*
void get_dir(char* file, char* dir)
{
	assert(file && dir);

	strncpy(dir, file, 256);
	str_erase(dir, str_rfind(dir, L'\\') + 1, -1);
}

void get_name(char* file, char* name)
{
	assert(file && name);

	strncpy(name, file, 256);
	str_erase(name, str_rfind(name, L'.'), -1);
	str_erase(name, 0, str_rfind(name, L'\\'));
}
*/
#include "tinylog.h"

void RunAs(LPCSTR _param)
{
	CHAR path[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, path, MAX_PATH);

	SHELLEXECUTEINFOA sei = { 0 };
	sei.cbSize = sizeof(sei);
	sei.lpVerb = "runas";
	sei.lpFile = path;
	sei.nShow = SW_SHOWNORMAL;
	sei.lpParameters = _param;
	ShellExecuteExA(&sei);
}

/* 失败认为是未提升好了，懒得处理 */
BOOL IsElevated()
{
	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		return FALSE;
	}

	BOOL bElevated = FALSE;
	TOKEN_ELEVATION tokenEle;
	DWORD dwRetLen = 0;
	if (GetTokenInformation(hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen))
	{
		bElevated = tokenEle.TokenIsElevated;
	}

	CloseHandle(hToken);
	hToken = NULL;
	return bElevated;
}

int main(int argc, char* argv[])
{
	FILE* fp = 0;
	FILE* fp2 = 0;
	char* buf = 0;
	wchar_t* wbuf = 0;
	int len = 0;
	md_document_t* doc = 0;
	/*char name[256], dir[256];
	char file[256];
	char cmd[256];
	STARTUPINFOA startupInfo = { 0 };
	PROCESS_INFORMATION processInfo = { 0 };*/

	free(0);

	if (argc != 2)
		return 0;

	if (!IsElevated())
	{
		RunAs(argv[1]);
		return 0;
	}

	do
	{
		//get_dir(argv[1], dir);
		//get_name(argv[1], name);

		fp = fopen(argv[1], "rb");
		if (!fp)
			break;
		fseek(fp, 0, SEEK_END);
		len = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		buf = (char*)safe_malloc(len + 1);
		fread(buf, len, 1, fp);

		wbuf = str_to_wcs(buf, len, encoding_utf8);
		if (!wbuf)
			break;

		doc = md_load_document(wbuf);
		if (!doc)
			break;

		fp2 = fopen("c:\\test.html", "wb");
		if (!fp2)
			break;

		md_document_to_html(doc, encoding_utf8, fp2);

		/*ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);
		startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
		startupInfo.wShowWindow = SW_SHOW;

		ZeroMemory(&processInfo, sizeof(processInfo));

		CreateProcessA(
			NULL,
			//str_format(cmd, 256, "\"C:\\Program Files\\internet explorer\\iexplore.exe\" %s", file),
			str_format(cmd, 256, "\"C:\\Program Files (x86)\\Google\\Chrome\\Application\\chrome.exe\" %s", file),
			NULL,
			NULL,
			TRUE,
			0,	
			NULL,
			NULL,
			&startupInfo,
			&processInfo);

		CloseHandle(processInfo.hProcess);
		CloseHandle(processInfo.hThread);
		*/
	} while (0);

	if (fp)
	{
		fclose(fp);
		fp = 0;
	}

	if (fp2)
	{
		fclose(fp2);
		fp2 = 0;
	}

	free(buf);
	free(wbuf);
	md_free_document(doc);
	return 0;
}