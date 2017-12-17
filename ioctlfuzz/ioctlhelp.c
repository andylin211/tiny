#include "tinyui.h"
#include <Windows.h>
#include "tinystr.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct
{
	int code;
	int type;
	int function;
	int buffer;
	int access;
	char* name;
}ioctl_code_t;

unsigned long make_ioctl_code(unsigned long device_type, unsigned long function, unsigned long method, unsigned long access)
{
	return (((device_type) << 16) | ((access) << 14) | ((function) << 2) | (method));
}

static char* skip_line(char* buffer)
{
	if (!buffer)
		return 0;

	while (1)
	{
		if (*buffer == 0)
			break;
		else if (*buffer == '\r' && buffer[1] == '\n')
		{
			buffer += 2;
			break;
		}
		buffer++;
	}

	return buffer;
}


static char* skip_white_space(char* buffer)
{
	if (!buffer)
		return 0;

	while (is_white_space(*buffer))
		buffer++;

	return buffer;
}


static char* get_element(char* buffer, char** value)
{
	int len = 0;
	char* start = 0;

	start = buffer;

	/* search until not match or match finally */
	while (1)
	{
		/* no match*/
		if (*buffer == 0)
			return 0;

		/* 这里实现有问题的， 再说吧 */
		/* match */
		if (*buffer == '\\' || *buffer == ')' || *buffer == ',')
			break;

		buffer++;
		len++;
	}

	/* get text and skip match symbol */
	*value = (char*)safe_malloc(len + 1);
	strncpy(*value, start, len);
	buffer++;
	buffer = skip_white_space(buffer);
	if ('\\' == *buffer)
		buffer++;
	buffer = skip_white_space(buffer);

	return buffer;
}

char* parse_ioctl_code_define(char* buffer, ioctl_code_t** code)
{
	char* backup = buffer;
	char* start = 0;
	int len = 0;
	char* define_str = "#define";
	char* ctl_code = "CTL_CODE";
	char* name = 0;
	char* device = 0;
	char* function = 0;
	char* buffered = 0;
	char* access = 0;

	if (!buffer || !code)
		return buffer;

	*code = 0;

	/* find symbol (skip if any white space) */
	buffer = skip_white_space(buffer);
	if (0 != strncmp(buffer, define_str, strlen(define_str)))
		goto not_ioctl_code_define;

	/* skip symbol and point to start text*/
	buffer += strlen(define_str);
	buffer = skip_white_space(buffer);
	start = buffer;

	/* search until not match or match finally */
	while (1)
	{
		/* no match*/
		if (*buffer == 0)
			goto not_ioctl_code_define;

		/* match */
		if (is_white_space(*buffer))
			break;

		buffer++;
		len++;
	}

	/* get text and skip match symbol */
	name = (char*)safe_malloc(len + 1);
	strncpy(name, start, len);
	buffer = skip_white_space(buffer);

	if (0 != strncmp(buffer, ctl_code, strlen(ctl_code)))
		goto not_ioctl_code_define;
	
	buffer += strlen(ctl_code);
	buffer = skip_white_space(buffer);
	if ('(' != *buffer)
		goto not_ioctl_code_define;

	buffer++;
	buffer = skip_white_space(buffer);
	if ('\\' == *buffer)
		buffer++;
	buffer = skip_white_space(buffer);

	buffer = get_element(buffer, &device);
	if (!buffer) goto not_ioctl_code_define;
	buffer = get_element(buffer, &function);
	if (!buffer) goto not_ioctl_code_define;
	buffer = get_element(buffer, &buffered);
	if (!buffer) goto not_ioctl_code_define;
	buffer = get_element(buffer, &access);
	if (!buffer) goto not_ioctl_code_define;

	if (')' == *buffer)
	{
		buffer++;
		buffer = skip_line(buffer);
	}

	*code = (ioctl_code_t*)safe_malloc(sizeof(ioctl_code_t));
	(*code)->name = name;
	assert(0 == strcmp(device, "FILE_DEVICE_UNKNOWN"));
	(*code)->type = FILE_DEVICE_UNKNOWN;	
	(*code)->function = strtol(&function[2], 0, 16);
	assert(0 == strcmp(buffered, "METHOD_BUFFERED"));
	(*code)->buffer = METHOD_BUFFERED;
	if (0 == strcmp("FILE_ANY_ACCESS", access))
		(*code)->access = FILE_ANY_ACCESS;
	else if (0 == strcmp("FILE_READ_ACCESS", access))
		(*code)->access = FILE_READ_ACCESS;
	else if (0 == strcmp("FILE_WRITE_ACCESS", access))
		(*code)->access = FILE_WRITE_ACCESS;
	else
		(*code)->access = FILE_READ_ACCESS | FILE_WRITE_ACCESS;
	
	(*code)->code = make_ioctl_code((*code)->type, (*code)->function, (*code)->buffer, (*code)->access);

	//free(name);
	free(device);
	free(function);
	free(buffered);
	free(access);

	return buffer;

not_ioctl_code_define:
	free(name);
	free(device);
	free(function);
	free(buffered);
	free(access);
	return backup;
}

#define file_max_access 4

char* access_str(int i)
{
	static char* str[] = { "FILE_ANY_ACCESS", "FILE_READ_ACCESS", "FILE_WRITE_ACCESS", "FILE_READ_ACCESS | FILE_WRITE_ACCESS" };
	return i < file_max_access ? str[i] : "";
}

void test(char* file)
{
	//char* file = "d:\\QQPCMGR_CODE\\blueray_trunk\\Blueray_trunk\\FtSafe\\Drivers\\tsksp\\share\\TSKSPIoctl.h";
	FILE* fp = 0;
	char* buffer = 0;
	char* start = 0;
	int len = 0;
	ioctl_code_t* code = 0;

	if (!file)
		return;

	fp = fopen(file, "rb");
	if (!fp)
		return;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	start = buffer = (char*)safe_malloc(len + 1);
	fread(buffer, len, 1, fp);
	fclose(fp);

	while (1)
	{
		code = 0;
		buffer = parse_ioctl_code_define(buffer, &code);
		if (*buffer == 0)
			break;
		if (!code)
			buffer = skip_line(buffer);
		else
		{
			printf("    <ioctl code=\"%x\" type=\"FILE_DEVICE_UNKNOWN\" function=\"%x\" method=\"METHOD_BUFFERED\" access=\"%s\"/>\n", code->code, code->function, access_str(code->access));
			free(code->name);
			free(code);
		}
	}


	free(start);
}

int main(int argc, char* argv[])
{
	if (argc == 2)
		test(argv[1]);

	return 1;
}