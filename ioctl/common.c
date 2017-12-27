#include "common.h"

type_def_t type_def[] =
{
	{ L"FILE_DEVICE_UNKNOWN",L"未知","FILE_DEVICE_UNKNOWN"},
	{ L"FILE_DEVICE_8042_PORT", L"8042 Port","FILE_DEVICE_8042_PORT"},
	{ L"FILE_DEVICE_ACPI", L"Acpi","FILE_DEVICE_ACPI" },
	{ L"FILE_DEVICE_BATTERY", L"Battery","FILE_DEVICE_BATTERY" },
	{ L"FILE_DEVICE_BEEP", L"Beep","FILE_DEVICE_BEEP" },
	{ L"FILE_DEVICE_BUS_EXTENDER",L"Bus Extender","FILE_DEVICE_BUS_EXTENDER" },
	{ L"FILE_DEVICE_CD_ROM",L"CD Rom","FILE_DEVICE_CD_ROM" },
	{ L"FILE_DEVICE_CD_ROM_FILE_SYSTEM",L"CD Rom File System","FILE_DEVICE_CD_ROM_FILE_SYSTEM" },
	{ L"FILE_DEVICE_CHANGER",L"Changer","FILE_DEVICE_CHANGER" },
	{ L"FILE_DEVICE_CONTROLLER",L"Controller","FILE_DEVICE_CONTROLLER" },
	{ L"FILE_DEVICE_DATALINK",L"Data Link","FILE_DEVICE_DATALINK" },
	{ L"FILE_DEVICE_DFS",L"DFS","FILE_DEVICE_DFS" },
	{ L"FILE_DEVICE_DFS_FILE_SYSTEM",L"DFS File System","FILE_DEVICE_DFS_FILE_SYSTEM" },
	{ L"FILE_DEVICE_DFS_VOLUME",L"DFS Volume","FILE_DEVICE_DFS_VOLUME" },
	{ L"FILE_DEVICE_DISK",L"Disk","FILE_DEVICE_DISK" },
	{ L"FILE_DEVICE_DISK_FILE_SYSTEM",L"Disk File System","FILE_DEVICE_DISK_FILE_SYSTEM" },
	{ L"FILE_DEVICE_DVD",L"DVD","FILE_DEVICE_DVD" },
	{ L"FILE_DEVICE_FILE_SYSTEM",L"File System","FILE_DEVICE_FILE_SYSTEM" },
	{ L"FILE_DEVICE_FIPS",L"FIPS","FILE_DEVICE_FIPS" },
	{ L"FILE_DEVICE_FULLSCREEN_VIDEO",L"Fullscreen Video","FILE_DEVICE_FULLSCREEN_VIDEO" },
	{ L"FILE_DEVICE_INPORT_PORT",L"Inport Port","FILE_DEVICE_INPORT_PORT" },
	{ L"FILE_DEVICE_KEYBOARD",L"Keyboard","FILE_DEVICE_KEYBOARD" },
	{ L"FILE_DEVICE_KS",L"KS","FILE_DEVICE_KS" },
	{ L"FILE_DEVICE_KSEC",L"KSEC","FILE_DEVICE_KSEC" },
	{ L"FILE_DEVICE_MAILSLOT",L"Mailslot","FILE_DEVICE_MAILSLOT" },
	{ L"FILE_DEVICE_MASS_STORAGE",L"Mass Storage","FILE_DEVICE_MASS_STORAGE" },
	{ L"FILE_DEVICE_MIDI_IN",L"MIDI In","FILE_DEVICE_MIDI_IN" },
	{ L"FILE_DEVICE_MIDI_OUT",L"MIDI Out","FILE_DEVICE_MIDI_OUT" },
	{ L"FILE_DEVICE_MODEM",L"Modem","FILE_DEVICE_MODEM" },
	{ L"FILE_DEVICE_MOUSE",L"Mouse","FILE_DEVICE_MOUSE" },
	{ L"FILE_DEVICE_MULTI_UNC_PROVIDER ",L"Multi Unc Provider","FILE_DEVICE_MULTI_UNC_PROVIDER " },
	{ L"FILE_DEVICE_NAMED_PIPE",L"Named Pipe","FILE_DEVICE_NAMED_PIPE" },
	{ L"FILE_DEVICE_NETWORK",L"Network","FILE_DEVICE_NETWORK" },
	{ L"FILE_DEVICE_NETWORK_BROWSER",L"Network Browser","FILE_DEVICE_NETWORK_BROWSER" },
	{ L"FILE_DEVICE_NETWORK_FILE_SYSTEM",L"Network File System","FILE_DEVICE_NETWORK_FILE_SYSTEM" },
	{ L"FILE_DEVICE_NETWORK_REDIRECTOR ",L"Network Redirector","FILE_DEVICE_NETWORK_REDIRECTOR " },
	{ L"FILE_DEVICE_NULL",L"Null","FILE_DEVICE_NULL" },
	{ L"FILE_DEVICE_PARALLEL_PORT",L"Parallel Port","FILE_DEVICE_PARALLEL_PORT" },
	{ L"FILE_DEVICE_PHYSICAL_NETCARD",L"Physical Netcard","FILE_DEVICE_PHYSICAL_NETCARD" },
	{ L"FILE_DEVICE_PRINTER",L"Printer","FILE_DEVICE_PRINTER" },
	{ L"FILE_DEVICE_SCANNER",L"Scanner","FILE_DEVICE_SCANNER" },
	{ L"FILE_DEVICE_SCREEN",L"Screen","FILE_DEVICE_SCREEN" },
	{ L"FILE_DEVICE_SERENUM",L"Serenum","FILE_DEVICE_SERENUM" },
	{ L"FILE_DEVICE_SERIAL_MOUSE_PORT",L"Serial Mouse Port","FILE_DEVICE_SERIAL_MOUSE_PORT" },
	{ L"FILE_DEVICE_SERIAL_PORT",L"Serial Port","FILE_DEVICE_SERIAL_PORT" },
	{ L"FILE_DEVICE_SMARTCARD",L"Smartcard","FILE_DEVICE_SMARTCARD" },
	{ L"FILE_DEVICE_SMB",L"SMB","FILE_DEVICE_SMB" },
	{ L"FILE_DEVICE_SOUND",L"Sound","FILE_DEVICE_SOUND" },
	{ L"FILE_DEVICE_STREAMS",L"Streams","FILE_DEVICE_STREAMS" },
	{ L"FILE_DEVICE_TAPE",L"Tape","FILE_DEVICE_TAPE" },
	{ L"FILE_DEVICE_TAPE_FILE_SYSTEM",L"Tape File System","FILE_DEVICE_TAPE_FILE_SYSTEM" },
	{ L"FILE_DEVICE_TERMSRV",L"Termsrv","FILE_DEVICE_TERMSRV" },
	{ L"FILE_DEVICE_TRANSPORT",L"Transport","FILE_DEVICE_TRANSPORT" },
	{ L"FILE_DEVICE_VDM",L"VDM","FILE_DEVICE_VDM" },
	{ L"FILE_DEVICE_VIDEO",L"Video","FILE_DEVICE_VIDEO" },
	{ L"FILE_DEVICE_VIRTUAL_DISK",L"Virtual Disk","FILE_DEVICE_VIRTUAL_DISK" },
	{ L"FILE_DEVICE_WAVE_IN",L"Wave In","FILE_DEVICE_WAVE_IN" },
	{ L"FILE_DEVICE_WAVE_OUT",L"Wave Out","FILE_DEVICE_WAVE_OUT" },
};

int type_def_size = sizeof(type_def) / sizeof(type_def[0]);

char* label_conv(char* gbk) {
	static char buf[buf_len];
	fl_utf8from_mb(buf, buf_len, gbk, strlen(gbk));
	return buf;
}

void RunAs(LPCSTR _param)
{
	SHELLEXECUTEINFOA sei = { 0 };
	CHAR path[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, path, MAX_PATH);


	sei.cbSize = sizeof(sei);
	sei.lpVerb = "runas";
	sei.lpFile = path;
	sei.nShow = SW_SHOWNORMAL;
	sei.lpParameters = _param;
	ShellExecuteExA(&sei);
}

int is_evevated()
{
	HANDLE hToken = NULL;
	BOOL bElevated = FALSE;
	TOKEN_ELEVATION tokenEle;
	DWORD dwRetLen = 0;

	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		return 0;

	if (GetTokenInformation(hToken, TokenElevation, &tokenEle, sizeof(tokenEle), &dwRetLen))
		bElevated = tokenEle.TokenIsElevated;

	CloseHandle(hToken);
	hToken = NULL;

	return bElevated;
}

int has_been_running()
{
	DWORD error = 0;
	HANDLE mutex = CreateMutex(NULL, FALSE, L"8130DAD1-4E8D-43C4-8A49-70C242533BBD");
	error = GetLastError();
	if (ERROR_ALREADY_EXISTS == error || ERROR_ACCESS_DENIED == error)
		return 1;
	return 0;
}

int is_xp()
{
	OSVERSIONINFO osVer = { 0 };

	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx((OSVERSIONINFO*)&osVer))
	{
		if (osVer.dwMajorVersion == 5 && osVer.dwMinorVersion == 1)
			return 1;
	}

	return 0;
}

wchar_t* ask_open_file(HWND hwnd)
{
	static wchar_t file[buf_len] = { 0 };
	OPENFILENAME ofn = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = file;
	ofn.nMaxFile = buf_len;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	return GetOpenFileName(&ofn) ? file : 0;
}

wchar_t* ask_save_file(HWND hwnd)
{
	static wchar_t file[buf_len] = { 0 };
	OPENFILENAME ofn = { 0 };

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = file;
	ofn.nMaxFile = buf_len;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrTitle = L"保存为";
	return GetSaveFileName(&ofn) ? file : 0;
}

wchar_t* ask_open_path(HWND hwnd)
{
	static wchar_t path[buf_len] = { 0 };
	BROWSEINFO bi = { 0 };

	bi.hwndOwner = hwnd;
	bi.pszDisplayName = path;
	bi.ulFlags = BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	return SHGetPathFromIDList(idl, path) ? path : 0;
};

char* read_file_raw(wchar_t* file)
{
	FILE* fp = 0;
	char* buf = 0;
	int len = 0;

	if (!file)
		return 0;

	fp = _wfopen(file, L"rb");
	if (!fp)
		return 0;

	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buf = (char*)malloc(len + 1);
	assert(buf);

	fread(buf, len, 1, fp);
	fclose(fp);

	return buf;
}

void set_menu_label(char* utf8, char* gbk)
{
	fl_utf8from_mb(utf8, buf_len, gbk, strlen(gbk));
}


wchar_t* type_str(wchar_t* str)
{
	if (str)
		for (int i = 0; i < sizeof(type_def) / sizeof(type_def[0]); i++)
		{
			if (0 == wcscmp(type_def[i].v1, str))
				return type_def[i].v2;
		}

	return 0;
}

wchar_t* access_str(wchar_t* str)
{
	if (!str)
		return 0;

	if (!wcscmp(L"FILE_READ_ACCESS", str))
		return L"只读";

	if (!wcscmp(L"FILE_WRITE_ACCESS", str))
		return L"只写";

	if (!wcscmp(L"FILE_ANY_ACCESS", str))
		return L"任意";

	wchar_t buf[buf_len];
	wcsncpy(buf, str, buf_len);
	wcs_remove(buf, L' ');
	if (!wcscmp(L"FILE_READ_ACCESS|FILE_WRITE_ACCESS", buf))
		return L"读写";

	return 0;
}

wchar_t* method_str(wchar_t* str)
{
	if (!str)
		return 0;

	if (!wcscmp(L"METHOD_BUFFERED", str))
		return L"Buffered";

	if (!wcscmp(L"METHOD_IN_DIRECT", str))
		return L"In Direct";

	if (!wcscmp(L"METHOD_OUT_DIRECT", str))
		return L"Out Direct";

	if (!wcscmp(L"METHOD_NEITHER", str))
		return L"Neither";

	return 0;
}