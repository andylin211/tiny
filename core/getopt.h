#ifndef _getopt_h_
#define _getopt_h_

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

void init_option_context(option_context* context, int argc, char** argv, char* option_string);

int get_option(option_context* context);

#endif // !_getopt_h_
