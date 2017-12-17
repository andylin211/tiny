#include "getopt.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#define max_str_len 256

/*
* 1. context not null
* 2. set or empty 
*/
void init_option_context(option_context* context, int argc, char** argv, char* option_string)
{
	if (!context)
		return;

	context->argc = argc;
	context->argv = argv;
	context->option_string = option_string;
	context->option_char = 0;
	context->option_index = 0;
	context->the_argument = 0;
	context->has_argument = 0;
}

/*
* 1. a~z, A~Z
* 2. ':'
*/
static int is_option_character(char ch)
{
	return ((isalpha(ch) || ch == ':'));
}

/*
* 1. not null
* 2. strlen > 0
* 3. each is optioin character
*/
static int is_option_string(char* str)
{
	int i = 0;
	int len = 0;

	if (!str)
		return 0;

	len = strnlen(str, max_str_len);
	if (!len)
		return 0;

	for (i = 0; i < len; i++)
	{
		if (!is_option_character(str[i]))
			return 0;
	}

	return 1;
}

/*
* 1. if reach end of string, CANNOT move any more
* 2. if next option, move to it
*/
static int can_and_move_to_next_option(option_context* context)
{
	char ch = 0;

	/* find a new option */
	while (1)
	{
		ch = context->option_string[context->option_index];

		/* reach end of string */
		if (ch == 0)
			return 0;

		/* skip any which is not alpha */
		if (isalpha(ch))
			break;

		context->option_index++;
	}

	/* move to next option */
	context->option_char = ch;
	context->option_index++;
	context->has_argument = 0;
	context->the_argument = 0;

	if (context->option_string[context->option_index] == ':')
	{
		context->has_argument = 1;
		context->option_index++;
	}

	return 1;
}

/*
* 1. option not found
* 2. option found without argument when required
* 3. found
*/
static int scan_argv_to_find_option(option_context* context)
{
	int i = 0;
	char short_option[3] = { '-', context->option_char, 0 };

	/* find the option within argv */
	for (i = 1; i < context->argc; i++)
	{
		if (!strncmp(short_option, context->argv[i], max_str_len))
			break;
	}

	/* find nothing */
	if (i == context->argc)
		return 0;

	/* has no argument when required */
	if (i == (context->argc - 1) && context->has_argument)
		return 0;

	/* set argument */
	if (context->has_argument)
		context->the_argument = context->argv[i + 1];

	return 1;
}

/*
* 1. context is null
* 2. context is invalid
* 3. move to next option
*	3.1 if fail, no more option
*	3.2 if succeed, try to find option in argv
*		3.2.1 if fail, try next option, turn to condition 3
*		3.2.2 if succeed, BINGO!
*/
int get_option(option_context* context)
{
	if (!context)
		return 0;

	/* context is not acceptable */
	if (context->argc <= 1 || !context->argv || !is_option_string(context->option_string))
		return 0;

	do
	{
		/* find next option (or first option) */
		if (!can_and_move_to_next_option(context))
			return 0;

		/* find option */
		if (scan_argv_to_find_option(context))
			return 1;

		/* if find nothing, continue */
	} while (1);
}

#ifdef _get_option_test_


/*
* test.exe -d -f c:\cidox -i -u
* abcdef:ghi
*/
static void test_normal()
{
	option_context context;
	char* argv[] = { "test.exe", "-d", "-f", "c:\\cidox", "-i", "-u"};
	int argc = 6;
	char* option_string = "abcdef:ghi";
	int ret = 0;

	init_option_context(&context, argc, argv, option_string);
	ret = get_option(&context);
	if (ret && !context.has_argument && context.option_char == 'd' && context.option_index == 4)
		printf("ok -d\n");
	else
		printf("fail -d\n");

	ret = get_option(&context);
	if (ret && context.has_argument && context.option_char == 'f' && !strncmp("c:\\cidox", context.the_argument, max_str_len))
		printf("ok -f %s\n", context.the_argument);
	else
		printf("fail -f\n");

	ret = get_option(&context);
	if (ret && !context.has_argument && context.option_char == 'i')
		printf("ok -i\n");
	else
		printf("fail -i\n");

	ret = get_option(&context);
	if (!ret)
		printf("ok end\n");
	else
		printf("fail end\n");
}

/*
* 1. argc <= 1
* 2. argv is null
* 3. option string is null
* 4. option string format is wrong
*/
static void test_argc_invalid()
{
	option_context context;
	int argc = 1;
	char* argv[] = { "test.exe" };
	char* option_string = "abcdef:ghi";
	int ret = 0;

	init_option_context(&context, argc, argv, option_string);
	ret = get_option(&context);
	if (!ret)
		printf("ok argc = 1\n");
	else
		printf("fail argc = 1\n");
}

static void test_argv_invalid()
{
	option_context context;
	int argc = 3;
	char** argv = 0;
	char* option_string = "abcdef:ghi";
	int ret = 0;

	init_option_context(&context, argc, argv, option_string);
	ret = get_option(&context);
	if (!ret)
		printf("ok argv is null\n");
	else
		printf("fail argc is null\n");
}

static void test_optstr_invalid()
{
	option_context context;
	int argc = 3;
	char* argv[] = { "test.exe", "-a", "-b" };
	char* option_string = 0;
	int ret = 0;

	init_option_context(&context, argc, argv, option_string);
	ret = get_option(&context);
	if (!ret)
		printf("ok option string is null\n");
	else
		printf("fail option string is null\n");
}

static void test_optstr_format_error()
{
	option_context context;
	int argc = 3;
	char* argv[] = { "test.exe", "-a", "-b" };
	char* option_string = "abcdef:ghi*";
	int ret = 0;

	init_option_context(&context, argc, argv, option_string);
	ret = get_option(&context);
	if (!ret)
		printf("ok option string format error\n");
	else
		printf("fail option string format error\n");
}

void run_tests()
{
	test_normal();
	test_argc_invalid();
	test_argv_invalid();
	test_optstr_invalid();
	test_optstr_format_error();
}

int main()
{
	run_tests();
}

#endif // _get_option_test_