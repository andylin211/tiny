#include "common.h"

type_def_t type_def[] =
{
	{ L"FILE_DEVICE_UNKNOWN", "unknown" },// almost this
	{ L"FILE_DEVICE_BEEP", "beep" },
	{ L"FILE_DEVICE_CD_ROM", "cd rom" },
	{ L"FILE_DEVICE_CD_ROM_FILE_SYSTEM", "cd rom file system" },
	{ L"FILE_DEVICE_CONTROLLER", "controller" },
	{ L"FILE_DEVICE_DATALINK", "datalink" },
	{ L"FILE_DEVICE_DFS", "dfs" },
	{ L"FILE_DEVICE_DISK", "disk" },
	{ L"FILE_DEVICE_DISK_FILE_SYSTEM", "disk file system" },
	{ L"FILE_DEVICE_FILE_SYSTEM", "file system" },
	{ L"FILE_DEVICE_INPORT_PORT", "inport port" },
	{ L"FILE_DEVICE_KEYBOARD", "keyboard" },
	{ L"FILE_DEVICE_MAILSLOT", "mailslot" },
	{ L"FILE_DEVICE_MIDI_IN", "midi in" },
	{ L"FILE_DEVICE_MIDI_OUT", "midi out" },
	{ L"FILE_DEVICE_MOUSE", "mouse" },
	{ L"FILE_DEVICE_MULTI_UNC_PROVIDER", "multi unc provider" },
	{ L"FILE_DEVICE_NAMED_PIPE", "named pipe" },
	{ L"FILE_DEVICE_NETWORK", "network" },
	{ L"FILE_DEVICE_NETWORK_BROWSER", "network browser" },
	{ L"FILE_DEVICE_NETWORK_FILE_SYSTEM", "network file system" },
	{ L"FILE_DEVICE_NULL", "null" },
	{ L"FILE_DEVICE_PARALLEL_PORT", "parallel port" },
	{ L"FILE_DEVICE_PHYSICAL_NETCARD", "physical netcard" },
	{ L"FILE_DEVICE_PRINTER", "printer" },
	{ L"FILE_DEVICE_SCANNER", "scanner" },
	{ L"FILE_DEVICE_SERIAL_MOUSE_PORT", "serial mouse port" },
	{ L"FILE_DEVICE_SERIAL_PORT", "serial port" },
	{ L"FILE_DEVICE_SCREEN", "screen" },
	{ L"FILE_DEVICE_SOUND", "sound" },
	{ L"FILE_DEVICE_STREAMS", "streams" },
	{ L"FILE_DEVICE_TAPE", "tape" },
	{ L"FILE_DEVICE_TAPE_FILE_SYSTEM", "tape file system" },
	{ L"FILE_DEVICE_TRANSPORT", "transport" },
	{ L"FILE_DEVICE_UNKNOWN", "unknown" },
	{ L"FILE_DEVICE_VIDEO", "video" },
	{ L"FILE_DEVICE_VIRTUAL_DISK", "virtual disk" },
	{ L"FILE_DEVICE_WAVE_IN", "wave in" },
	{ L"FILE_DEVICE_WAVE_OUT", "wave out" },
	{ L"FILE_DEVICE_8042_PORT", "8042 port" },
	{ L"FILE_DEVICE_NETWORK_REDIRECTOR", "network redirector" },
	{ L"FILE_DEVICE_BATTERY", "battery" },
	{ L"FILE_DEVICE_BUS_EXTENDER", "bus extender" },
	{ L"FILE_DEVICE_MODEM", "modem" },
	{ L"FILE_DEVICE_VDM", "vdm" },
	{ L"FILE_DEVICE_MASS_STORAGE", "mass storage" },
	{ L"FILE_DEVICE_SMB", "smb" },
	{ L"FILE_DEVICE_KS", "ks" },
	{ L"FILE_DEVICE_CHANGER", "changer" },
	{ L"FILE_DEVICE_SMARTCARD", "smartcard" },
	{ L"FILE_DEVICE_ACPI", "acpi" },
	{ L"FILE_DEVICE_DVD", "dvd" },
	{ L"FILE_DEVICE_FULLSCREEN_VIDEO", "fullscreen video" },
	{ L"FILE_DEVICE_DFS_FILE_SYSTEM", "dfs file system" },
	{ L"FILE_DEVICE_DFS_VOLUME", "dfs volume" },
	{ L"FILE_DEVICE_SERENUM", "serenum" },
	{ L"FILE_DEVICE_TERMSRV", "termsrv" },
	{ L"FILE_DEVICE_KSEC", "ksec" },
	{ L"FILE_DEVICE_FIPS", "fips" },
	{ L"FILE_DEVICE_INFINIBAND", "infiniband" },
	{ L"FILE_DEVICE_VMBUS", "vmbus" },
	{ L"FILE_DEVICE_CRYPT_PROVIDER", "crypt provider" },
	{ L"FILE_DEVICE_WPD", "wpd" },
	{ L"FILE_DEVICE_BLUETOOTH", "bluetooth" },
	{ L"FILE_DEVICE_MT_COMPOSITE", "mt composite" },
	{ L"FILE_DEVICE_MT_TRANSPORT", "mt transport" },
	{ L"FILE_DEVICE_BIOMETRIC", "biometric" },
	{ L"FILE_DEVICE_PMI", "pmi" },
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


char* type_str(wchar_t* str)
{
	if (str)
		for (int i = 0; i < sizeof(type_def) / sizeof(type_def[0]); i++)
		{
			if (0 == wcscmp(type_def[i].v1, str))
				return type_def[i].v2;
		}

	return 0;
}

static void buf_gbk_to_utf8(char* buf, char* src)
{
	fl_utf8from_mb(buf, buf_len, src, strlen(src));
}

char* access_str(wchar_t* str)
{
	static char read[buf_len];
	static char write[buf_len];
	static char any[buf_len];
	static char both[buf_len];
	static int i = 0;
	if (!i)
	{
		i++;
		buf_gbk_to_utf8(read, "只读");
		buf_gbk_to_utf8(write, "只写");
		buf_gbk_to_utf8(any, "任意");
		buf_gbk_to_utf8(both, "读写");
	}

	if (!str)
		return 0;

	if (!wcscmp(L"FILE_READ_ACCESS", str))
		return read;

	if (!wcscmp(L"FILE_WRITE_ACCESS", str))
		return write;

	if (!wcscmp(L"FILE_ANY_ACCESS", str))
		return any;

	wchar_t buf[buf_len];
	wcsncpy(buf, str, buf_len);
	wcs_remove(buf, L' ');
	if (!wcscmp(L"FILE_READ_ACCESS|FILE_WRITE_ACCESS", buf))
		return both;

	return 0;
}

char* method_str(wchar_t* str)
{
	if (!str)
		return 0;

	if (!wcscmp(L"METHOD_BUFFERED", str))
		return "buffered";

	if (!wcscmp(L"METHOD_IN_DIRECT", str))
		return "in direct";

	if (!wcscmp(L"METHOD_OUT_DIRECT", str))
		return "out direct";

	if (!wcscmp(L"METHOD_NEITHER", str))
		return "neither";

	return 0;
}