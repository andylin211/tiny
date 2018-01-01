#ifndef _tinyopt_h_
#define _tinyopt_h_
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

	/*
	 * return option char or 0 if no more option
	 * parg store string address of the argument if any
	 */
	char tinyopt(char** parg, char* opt, int argc, char** argv);

#ifdef define_tiny_here

	typedef struct
	{
		int argc;
		char** argv;

		/* consists of alpha and ':' */
		char* option_string;

		/* for function get option */
		char option_char;
		int option_index;
		int has_argument;
		char* the_argument;
	}option_context;


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

		len = strnlen(str, 256);
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
			if (!strncmp(short_option, context->argv[i], 256))
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

	char tinyopt(char** parg, char* opt, int argc, char** argv)
	{
		static option_context ctx;
		static int inited = 0;
		static int has_more = 1;
		char ch = 0;
		int i = 0;

		if (!has_more)
			return 0;
		if (!inited)
		{
			init_option_context(&ctx, argc, argv, opt);
			inited = 1;
		}

		has_more = get_option(&ctx);
		*parg = ctx.has_argument ? ctx.the_argument : 0;
		ch = ctx.option_char;
		if (has_more)
			return ch;

		return 0;
	}

#endif // define_tiny_here

#ifdef __cplusplus
}
#endif

#endif // _tinyopt_h_