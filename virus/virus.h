#ifndef _virus_h_
#define _virus_h_

#include <windows.h>
#include "tinystr.h"
#include "tinyxml.h"
#include "./../virus/resource.h"
#include "tinyargs.h"
#include <process.h>
#include "FL\Fl.H"
#include "FL\Fl_Double_Window.H"
#include "FL\Fl_Button.H"
#include "FL\Fl_Return_Button.H"
#include "Fl\Fl_Choice.H"
#include "FL\fl_utf8.h"
#include "FL\Fl_Text_Buffer.H"
#include "FL\Fl_Text_Editor.H"
#include "FL\Fl_Text_Display.H"
#include <Shellapi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define max_buf_len 512

#define max_log_size 10240

	inline void RunAs(LPCSTR _param)
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

	inline int is_evevated()
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
	
	typedef void(*step_notify_func)(void* notify_data);

	/* 1 done; 0 fail */
	int do_step(xml_element* element, step_notify_func notify, void* notify_data);

	int release(wchar_t* file, wchar_t* to, int id, int del, step_notify_func notify, void* notify_data);

	int runexe(wchar_t* file, wchar_t* arg_str, int show, step_notify_func notify, void* notify_data);


	typedef BOOL(__stdcall *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

	/*
	* true for 32 bits process running under 64 bits environment
	*/
	inline int is_x64_system()
	{
		BOOL bWow64Process = 0;
		LPFN_ISWOW64PROCESS pfnIsWow64 = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "IsWow64Process");

		if (!pfnIsWow64)
			return 0;

		if (!pfnIsWow64(GetCurrentProcess(), &bWow64Process))
			return 0;

		return bWow64Process;
	}

	inline int is_xp()
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

	typedef BOOL (WINAPI * pWow64DisableWow64FsRedirection)(void**);
	typedef BOOL (WINAPI * pWow64RevertWow64FsRedirection)(void**);

	inline void disable_fs_redirection(void** old_value)
	{
		pWow64DisableWow64FsRedirection func = (pWow64DisableWow64FsRedirection)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "Wow64DisableWow64FsRedirection");

		if (!func)
			return;

		func(old_value);
	}

	inline void revert_fs_redirection(void** old_value)
	{
		pWow64DisableWow64FsRedirection func = (pWow64DisableWow64FsRedirection)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "Wow64DisableWow64FsRedirection");
		
		if (!func)
			return;

		func(old_value);
	}

	inline int has_been_running()
	{
		DWORD error = 0;
		HANDLE mutex = CreateMutex(NULL, FALSE, L"91806317-FBAC-43BC-BDC0-EFE833A608AE");
		error = GetLastError();
		if (ERROR_ALREADY_EXISTS == error || ERROR_ACCESS_DENIED == error)
			return 1;
		return 0;
	}

	inline char* copy_resource(int id, wchar_t* type, int* length)
	{
		HGLOBAL hglobal = NULL;
		HINSTANCE hinst;
		HRSRC hrsrc;
		unsigned int size = 0;
		char* buffer = 0;
		char* res = 0;

		if (!type || !length)
			return 0;

		*length = 0;

		hinst = NULL;
		hrsrc = FindResource(hinst, MAKEINTRESOURCE(id), type);
		GetLastError();
		if (!hrsrc)
			return 0;

		size = SizeofResource(hinst, hrsrc);
		if (!size)
			return 0;

		hglobal = LoadResource(hinst, hrsrc);
		if (!hglobal)
			return 0;

		res = (char*)LockResource(hglobal);
		if (!res)
			return 0;

		buffer = (char*)safe_malloc(size);
		memcpy(buffer, res, size);

		if (hglobal)
			FreeResource(hglobal);

		*length = size;

		return buffer;
	}

#ifdef __cplusplus
}
#endif

class Virus_Window : public Fl_Double_Window
{
public:
	Virus_Window();

	void add_log(wchar_t* wcs);
	
	~Virus_Window();
	
	void init_choice();

	static void choice_cb(Fl_Choice* o, void*);
		
	static void btn_do_thread(void* data);
	static void btn_do_cb(Fl_Widget* o, void* v)
	{
		_beginthread(&Virus_Window::btn_do_thread, 0, v);
	}

	static void btn_doc_thread(void* data);
	static void btn_doc_cb(Fl_Widget* o, void* v)
	{
		_beginthread(&Virus_Window::btn_doc_thread, 0, v);
	}

	static void btn_sample_thread(void* data);
	static void btn_sample_cb(Fl_Widget* o, void* v)
	{
		_beginthread(&Virus_Window::btn_sample_thread, 0, v);
	}

	static void btn_pchunter_thread(void* data);
	static void btn_pchunter_cb(Fl_Widget* o, void* v)
	{
		_beginthread(&Virus_Window::btn_pchunter_thread, 0, v);
	}

	static void btn_reboot_thread(void* data);
	static void btn_reboot_cb(Fl_Widget* o, void* v)
	{
		_beginthread(&Virus_Window::btn_reboot_thread, 0, v);
	}	
	
	static void btn_uac_thread(void* data);
	static void btn_uac_cb(Fl_Widget* o, void* v)
	{
		_beginthread(&Virus_Window::btn_uac_thread, 0, v);
	}

	static void step_notify(args_t* notify_data);
	
private:
	

	Fl_Choice* choice;
	Fl_Return_Button* btn_do;
	Fl_Button* btn_doc;
	Fl_Button* btn_sample;
	Fl_Button* btn_pchunter;
	Fl_Button* btn_reboot;
	Fl_Button* btn_uac;
	Fl_Text_Buffer* log_buf;
	Fl_Text_Display* log;
	xml_document* doc;

	char title_buf[256];
	char choice_buf[256];
	char btn_do_buf[256];
	char btn_doc_buf[256];
	char btn_sample_buf[256];
	char btn_pchunter_buf[256];
	char btn_reboot_buf[256];
	char btn_uac_buf[256];
};


#endif
