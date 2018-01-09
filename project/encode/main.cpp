#include "tiny.h"
#include "tinylog.h"
#include "tinyopt.h"
#include "tinysys.h"

int count_gbk(char* buf, int len)
{
	int counter = 0;
	unsigned char first, second;

	for (int i = 0; i < len - 1; ++i)
	{
		first = buf[i];
		if (!(first & 0x80))
			continue;

		second = buf[++i];
		if (first >= 161 && first <= 247 && second >= 161 && second <= 254)
			counter += 2;
	}
	return counter;
}

int count_utf8(char* buf, int len)
{
	int counter = 0;
	char first;

	for (int i = 0; i < len; ++i)
	{
		first= buf[i];
		if (!(first & 0x80))
			continue;

		unsigned char tmp = (unsigned char)0x80;
		int word_len = 0;
		while (first & tmp)
		{
			++word_len;
			tmp = tmp >> 1;
		}

		if (word_len <= 1)
			continue;

		word_len--;

		if (word_len + i >= len)
			break;

		int j = 0;
		for (j = 1; j <= word_len; ++j)
		{
			char next = (unsigned char)buf[i + j];
			if (!(next & 0x80))
				break;
		}
		if (j > word_len)
		{
			counter += word_len + 1;
			i += word_len;
		}
	}
	return counter;
}

int is_utf8(char* buf, int len)
{
	int ngbk = count_gbk(buf, len);
	int nutf8 = count_utf8(buf, len);
	if (nutf8 > ngbk)
		return 1;
	return 0;
}

void convert(char* file, char* encoding, char* output)
{
	char* str = 0;
	char* buf = 0;
	int len = read_raw_file(file, &buf);

	if (!buf || !len)
	{
		log_errora("cannot read.");
		return;
	}

	if (is_utf8(buf, len))
	{
		if (!strcmp(encoding, "utf8"))
		{
			write_raw_file(output, buf, len);
		}
		else
		{
			str = utf82gbk(buf);
			write_raw_file(output, str, strlen(str));
		}
	}
	else
	{
		if (!strcmp(encoding, "ansi"))
		{
			write_raw_file(output, buf, len);
		}
		else
		{
			str = gbk2utf8(buf);
			write_raw_file(output, str, strlen(str));
		}
	}
	

	free(buf);
	free(str);
}

void what_encoding(char* file)
{
	char* buf = 0;
	int len = read_raw_file(file, &buf);
	if (!buf || !len)
	{
		log_errora("cannot read.");
		return;
	}
	if (is_utf8(buf, len))
		log_printa("utf8");
	else
		log_printa("ansi");

	free(buf);
}

void usage()
{
	char* usage_str = ""
		"                                                                           \r\n"
		"Usage: encode.exe -f fullpath [-c utf8|ansi -o fullpath] [-h]              \r\n"
		"                                                                           \r\n"
		"    -f    specify ini format file full path                                \r\n"
		"    -c    if set then convert; if not then just give what encoding it is.  \r\n"
		"    -o    output file.                                                     \r\n"
		"    -h    show this usage                                                  \r\n"
		"                                                                           \r\n";
	log_printa(usage_str);
}


int main(int argc, char* argv[])
{
	char opt = 0;
	char* arg = 0;
	char* file = 0;
	char* output = 0;
	char* encoding = 0;

	for (;;)
	{
		opt = tinyopt(&arg, "c:f:o:h", argc, argv);
		if (!opt)
			break;
		switch (opt)
		{
		case 'f':
			assert(arg);
			file = arg;
			break;
		case 'h':
			usage();
			return 0;
		case 'o':
			assert(arg);
			output = arg;
			break;
		case 'c':
			assert(arg);
			encoding = arg;
			break;
		default:
			break;
		}
	}

	if (file)
	{
		if (encoding)
		{
			if (output)
				convert(file, encoding, output);
			else
				usage();
		}
		else
			what_encoding(file);	
	}
	else
		usage();

	return 0;
}

