#include "markdown_internal.h"
#include <ctype.h>

char* keyword[] = {
	"char",
	"double",
	"enum",
	"float",
	"int",
	"inline",
	"long",
	"short",
	"signed",
	"struct",
	"union",
	"unsigned",
	"void",
	"for",
	"do",
	"while",
	"break",
	"continue",
	"if",
	"else",
	"goto",
	"switch",
	"case",
	"default",
	"return",
	"auto",
	"extern",
	"register",
	"static",
	"const",
	"sizeof",
	"typedef",
	"volatile",
	"define",
	"include",
	"ifdef",
	"ifndef",
	"endif",
	"elif",
};

static int is_delimeter(char ch)
{
	return !((unsigned)ch >= 0x80 || isalpha(ch) || ch == '_');
}

static int is_keyword(char* string)
{
	int i = 0;

	assert(string);

	if ((unsigned)*string >= 0x80 || !isalpha(*string))
		return -1;

	for (i = 0; i < sizeof(keyword) / sizeof(keyword[0]); i++)
	{
		if (strlen(string) > strlen(keyword[i]) && is_delimeter(string[strlen(keyword[i])])
			&& 0 == strncmp(string, keyword[i], strlen(keyword[i])))
			return i;
	}

	return -1;
}

static char* is_function(char* string, int* len)
{
	char* start = string;

	assert(len);
	assert(string);

	*len = 0;

	if ((*string) & 0x80 || !(isalpha(*string) || *string == '_'))
		return 0;

	while ((!(*string & 0x80)) && (isalnum(*string) || *string == '_'))
	{
		string++;
		(*len)++;
	}

	if (*string != '(' || !(*len))
		return 0;

	return start;
}

static char* is_number(char* string, int* len)
{
	char* start = string;

	assert(len);
	assert(string);

	*len = 0;

	if (!(*string >= '0' && *string <= '9'))
		return 0;

	if (string[0] == '0' && string[1] == 'x')
	{
		*len = 2;
		string += 2;

		while ((*string >= '0' && *string <= '9') || (*string >= 'A' && *string <= 'F') || (*string >= 'a' && *string <= 'f'))
		{
			string++;
			(*len)++;
		}
	}
	else
	{
		while (*string >= '0' && *string <= '9')
		{
			string++;
			(*len)++;
		}
	}
	
	if (!is_delimeter(*string) || !(*len))
		return 0;

	return start;
}

static char* is_string(char* string, int* len)
{
	char* start = string;

	assert(len);
	assert(string);

	*len = 0;

	if (!(*string == '\'' || *string == '\"'))
		return 0;

	if (string[0] == '\'')
	{
		*len = 1;
		string++;

		while (*string != '\r' && *string)
		{
			if (*string == '\'')
				if (*(string - 1) != '\\')
					break;
			string++;
			(*len)++;
		}

		(*len)++;

		if (*string == '\'')
			return start;
	}
	else
	{
		*len = 1;
		string++;

		while (*string != '\r' && *string)
		{
			if (*string == '\"')
				if (*(string - 1) != '\\')
					break;
			string++;
			(*len)++;
		}

		(*len)++;

		if (*string == '\"')
			return start;
	}

	return 0;
}

void md_code_string_to_syntax_html(char* string, FILE* file)
{
	int i = 1;
	int chance_keyword_next = 1;
	int index = 0;
	int in_comment_star = 0;
	char* function = 0;
	char* number = 0;
	char* str = 0;
	int function_len = 0;
	int number_len = 0;
	int str_len = 0;
	
	if (!string || !file)
		return;

	fputs("<table>\r\n", file);

	for (;;i++)
	{
		fprintf(file, "<tr><td class=\"code-line-number\">%d</td>\r\n<td>", i);

		chance_keyword_next = 1;

		while (*string != '\r' && *string)
		{
			if (*string == '/' && string[1] == '*')
				in_comment_star = 1;

			if (in_comment_star)
			{
				fputs("<span class=\"code-comment\">", file);
				while (*string != '\r' && *string && !(string[0] == '*' && string[1] == '/'))
				{
					fputc(*string, file);
					string++;
				}

				if (string[0] == '*' && string[1] == '/')
				{
					string += 2;
					fputs("*/</span>", file);
					in_comment_star = 0;

					if (*string == '\r')
						continue;
				}
				else
				{
					fputs("</span>", file);
					continue;
				}
			}

			/* line comment */
			if (*string == '/' && string[1] == '/')
			{
				fputs("<span class=\"code-comment\">", file);
				while (*string != '\r' && *string)
				{
					fputc(*string, file);
					string++;
				}

				fputs("</span>", file);
				continue;
			}

			if (chance_keyword_next)
			{
				index = is_keyword(string);
				function = is_function(string, &function_len);
				number = is_number(string, &number_len);
				str = is_string(string, &str_len);
			}

			if (index >= 0)
			{
				fprintf(file, "<span class=\"code-key-word\">%s</span>", keyword[index]);

				string += strlen(keyword[index]);

				index = -1;
			}
			else if (function)
			{
				fputs("<span class=\"code-function\">", file);
				fwrite(function, function_len, 1, file);
				fputs("</span>", file);

				string += function_len;

				function = 0;
			}
			else if (number)
			{
				fputs("<span class=\"code-number\">", file);
				fwrite(number, number_len, 1, file);
				fputs("</span>", file);

				string += number_len;

				number = 0;
			}
			else if (str)
			{
				fputs("<span class=\"code-string\">", file);
				fwrite(str, str_len, 1, file);
				fputs("</span>", file);

				string += str_len;

				str = 0;
			}
			else
			{
				fputc(*string, file);

				chance_keyword_next = is_delimeter(*string);

				string++;
			}
		}

		if (!*string)
			break;

		string += 2;

		fputs("</td></tr>", file);
	}

	fputs("</table>\r\n", file);
}