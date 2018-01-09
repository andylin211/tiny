#include "tiny.h"
#include "tinylog.h"
#include "tinyopt.h"
#include <string>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi")

void usage()
{
	char* usage_str = ""
		"                                                                           \r\n"
		"Usage: longpath.exe -p parent_dir -n name -d depth [-r dir] [-h]           \r\n"
		"                                                                           \r\n"
		"    -p    specify parent dir, which should have not been exited.           \r\n"
		"    -n    specify name of sub directory.                                   \r\n"
		"    -d    specify depth of long path.                                      \r\n"
		"          then result is like:(depth=8)                                    \r\n"
		"          \"c:\\parent\\dir\\name1\\name2\\name3\\...\\name8\"             \r\n"
		"    -r    remove directories and its sub directories                       \r\n"
		"    -h    show this usage                                                  \r\n"
		"                                                                           \r\n"
		"Example:                                                                   \r\n"
		"     longpath.exe -p c:\test -n some-long-name -d 6                        \r\n"
		"          (this will generate some junctions in c:\test)                   \r\n"
		"     longpath.exe -r c:\test                                               \r\n"
		"          (this will remove all sub directories of c:\test!!)              \r\n" 
		"                                                                           \r\n";
	log_printa(usage_str);
}

void create_long_path_r(char* path)
{

}

void create_long_path(char* parent, char* name, int depth)
{
	std::string path = "\\\\?\\";
	path += parent;
	path += '\\';

	if (PathFileExistsA(parent))
	{
		log_errora("%s exists!", parent);
		return;
	}

	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOALIGNMENTFAULTEXCEPT | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);
	while (depth)
	{
		if (!CreateDirectoryA(path.c_str(), NULL))
		{
			log_errora("fail to create directory: %s", path.c_str());
			return;
		}
		path += name;
		path += '\\';
		depth--;
	}
	
	log_printa("%s", path.c_str());
}

void remove_long_path(char* path)
{

}

int main(int argc, char* argv[])
{
	char opt = 0;
	char* arg = 0;
	char* parent = 0;
	char* name = 0;
	int depth = 0;
	char* remove = 0;

	for (;;)
	{
		opt = tinyopt(&arg, "d:n:p:r:h", argc, argv);
		if (!opt)
			break;
		switch (opt)
		{
		case 'd':
			depth = strtol(arg, 0, 10);
			break;
		case 'h':
			usage();
			return 0;
		case 'n':
			name = arg;
			break;
		case 'p':
			parent = arg;
			break;
		case 'r':
			remove = arg;
			break;
		default:
			break;
		}
	}

	if (parent && name && depth)
	{
		create_long_path(parent, name, depth);
	}
	else if (remove)
	{
		remove_long_path(remove);
	}
	else
		usage();

	return 0;
}
