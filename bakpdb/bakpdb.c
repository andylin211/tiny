#include "tinylog.h"
#include "tinystr.h"
#include <assert.h>
#include <stdio.h>
#include <Windows.h>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")

void get_ext(wchar_t* file, wchar_t* ext)
{
	assert(file && ext);

	wcsncpy(ext, file, MAX_PATH);
	wcs_erase(ext, 0, wcs_rfind(ext, L'.') - 1);
}

void get_dir(wchar_t* file, wchar_t* dir)
{
	assert(file && dir);

	wcsncpy(dir, file, MAX_PATH);
	wcs_erase(dir, wcs_rfind(dir, L'\\') + 1, -1);
}

void get_name(wchar_t* file, wchar_t* name)
{
	assert(file && name);

	wcsncpy(name, file, MAX_PATH);
	wcs_erase(name, wcs_rfind(name, L'.'), -1);
	wcs_erase(name, 0, wcs_rfind(name, L'\\'));
}

typedef struct _DEBUG_DIRECTORY
{
	DWORD unknown;
	DWORD timestamp;
	DWORD unknown2;
	DWORD type;
	DWORD size;
	DWORD rva;
	DWORD pointer;
}DEBUG_DIRECTORY;

typedef struct _DEBUG_DIRECTORY_EX
{
	DWORD rsds;
	GUID guid;
	DWORD magic;
	char pdbpath[MAX_PATH];
}DEBUG_DIRECTORY_EX;

static int read_pe_guid_and_magic(wchar_t* file, GUID* guid)
{
	long offset = 0;
	DWORD lfanew = 0;
	char pe[4] = { 0 };
	unsigned short nos = 0;
	FILE* fp = 0;
	int i = 0;
	IMAGE_DATA_DIRECTORY debug;
	IMAGE_SECTION_HEADER ish;
	DEBUG_DIRECTORY debugdir;
	DEBUG_DIRECTORY_EX debugdirex;

	if (!file)
		return -1;

	fp = _wfopen(file, L"rb");
	if (0 == fp)
		return -1;

	fseek(fp, 0x3c, SEEK_SET);
	fread(&lfanew, 4, 1, fp);

	fseek(fp, lfanew, SEEK_SET);
	fread(pe, 4, 1, fp);
	assert(0 == _stricmp(pe, "pe"));

	fseek(fp, lfanew + 6, SEEK_SET);
	fread(&nos, 2, 1, fp);

	offset = lfanew + 4 + sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_OPTIONAL_HEADER32) - (IMAGE_NUMBEROF_DIRECTORY_ENTRIES - IMAGE_DIRECTORY_ENTRY_DEBUG) * sizeof(debug);
	fseek(fp, offset, SEEK_SET);
	fread(&debug, sizeof(debug), 1, fp);

	for (i = 0; i < (int)nos; i++)
	{
		offset = lfanew + 4 + sizeof(IMAGE_FILE_HEADER) + sizeof(IMAGE_OPTIONAL_HEADER32) + sizeof(ish)*i;
		fseek(fp, offset, SEEK_SET);
		fread(&ish, sizeof(ish), 1, fp);
		if (0 == strcmp((char*)ish.Name, ".rdata"))
		{
			break;
		}
	}
	
	offset = debug.VirtualAddress - ish.VirtualAddress + ish.PointerToRawData;
	fseek(fp, offset, SEEK_SET);
	fread(&debugdir, sizeof(debugdir), 1, fp);
	
	memset(&debugdirex, 0, sizeof(debugdirex));
	offset = debugdir.pointer;
	fseek(fp, offset, SEEK_SET);
	fread(&debugdirex, debugdir.size, 1, fp);

	fclose(fp);

	memcpy(guid, &debugdirex.guid, sizeof(GUID));
	return debugdirex.magic;
}

/*
* bakpdb.exe C:\Symbols abc.pdb
*	-> C:\Symbols\abc.pdb\{guid}\abc.pdb
*/
void back_up(wchar_t* sympath, wchar_t* file)
{
	static wchar_t buffer[MAX_PATH];
	static wchar_t temp[MAX_PATH];
	static wchar_t name[MAX_PATH];
	static wchar_t dir[MAX_PATH];
	static wchar_t pdb[MAX_PATH];
	GUID guid;
	int magic = 0;
	wchar_t str_guid[MAX_PATH] = { 0 };
	wchar_t buf[MAX_PATH];

	if (!sympath || !file)
		return;

	wcsncpy(temp, sympath, MAX_PATH);

	if (!wcs_endwith(file, L".exe") && !wcs_endwith(file, L".dll") && !wcs_endwith(file, L".sys"))
	{
		MessageBox(NULL, wcs_format(buf, MAX_PATH, L"%s 不是PE文件？", file), L"bakpdb", MB_OK);
		return;
	}

	if (!PathCanonicalize(buffer, file))
	{
		MessageBox(NULL, L"PathCanonicalize", L"bakpdb", MB_OK);
		return;
	}

	get_dir(file, dir);
	get_name(file, name);

	/* check pdb dir */
	wcscat(temp, L"\\");

	if (!PathFileExists(temp) && !CreateDirectory(temp, NULL))
	{
		tinylog(L"创建目录失败 %s", temp);
		return;
	}

	/* sub pdb dir */
	wsprintf(temp, L"%s%s.pdb\\", temp, name);
	if (!PathFileExists(temp) && !CreateDirectory(temp, NULL))
	{
		tinylog(L"创建目录失败 %s", temp);
		return;
	}

	/* get guid */
	magic = read_pe_guid_and_magic(file, &guid);
	if (magic == -1)
	{
		tinylog(L"获取guid失败 %s", file);
		return;
	}

	/* mkdir */
	StringFromGUID2(&guid, str_guid, MAX_PATH);
	tinylog(L"guid: %s, magic: %d", str_guid, magic);
	
	wcs_remove(str_guid, L'{');
	wcs_remove(str_guid, L'}');
	wcs_remove(str_guid, L'-');
	wsprintf(temp, L"%s%s%x\\", temp, str_guid, magic);
	if (!PathFileExists(temp) && !CreateDirectory(temp, NULL))
	{
		tinylog(L"创建目录失败 %s", temp);
		return;
	}

	/* copy */
	wsprintf(pdb, L"%s%s.pdb", dir, name);
	wsprintf(temp, L"%s%s.pdb", temp, name);
	if (!CopyFileEx(pdb, temp, NULL, NULL, FALSE, COPY_FILE_RESTARTABLE))
	{
		tinylog(L"拷贝文件失败 %s -> %s", pdb, temp);
		return;
	}
	else
	{
		tinylog(L"拷贝文件 %s -> %s", pdb, temp);
	}

	tinylog(L"备份完成!");
}

int wmain(int argc, wchar_t** argv)
{
	if (argc == 3)
	{
		back_up(argv[1], argv[2]);
	}
	else if (argc == 2)
	{
		back_up(L"d:\\symbols", argv[1]);
	}
	else
	{
		tinylog(L"参数数量不对。用法： bakpdb.exe [C:\\Symbols] abc.sys");
	}

	return 0;
}