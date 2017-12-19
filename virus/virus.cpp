#include "virus.h"
#include <stdio.h>
#include <assert.h>

wchar_t* doc_file[] = {
	L"7ffe_callback.png",
	L"7ffe_file.png",
	L"7ffe_filter.png",
	L"7ffe_module.png",
	L"anyun_dpc.png",
	L"hqb_callback.png",
	L"hqb_file.png",
	L"hqb_filter.png",
	L"hqb_inline_hook.png",
	L"hqb_module.png",
	L"MpK_callback.png",
	L"MpK_filter.png",
	L"MpK_module.png",
	L"netgjo_callback.png",
	L"netgjo_file.png",
	L"netgjo_filter.png",
	L"netgjo_module.png",
	L"tqkq_callback.png",
	L"tqkq_file.png",
	L"tqkq_filter.png",
	L"tqkq_module.png",
	L"vbr_hook.png",
	L"vbr_module.png",
	L"markdown.css",
	L"virus_help_doc.html",
};


Virus_Window::Virus_Window()
	:Fl_Double_Window(540, 500)
{
	char* p = 0;

	icon((char *)LoadIcon(fl_display, MAKEINTRESOURCE(IDI_ICON1)));
		
	p = "病毒一键构造工具（仅用于测试）";
	fl_utf8from_mb(title_buf, 256, p, strlen(p));
	label(title_buf);

	p = "病毒样本(系统):";
	fl_utf8from_mb(choice_buf, 256, p, strlen(p));
		
	choice = new Fl_Choice(120, 20, 300, 25, choice_buf);
	choice->callback((Fl_Callback *)choice_cb, this);

	p = "一键构造";
	fl_utf8from_mb(btn_do_buf, 256, p, strlen(p));
	btn_do = new Fl_Return_Button(430, 20, 100, 25, btn_do_buf);
	btn_do->callback(btn_do_cb, this);

	p = "查看文档";
	fl_utf8from_mb(btn_doc_buf, 256, p, strlen(p));
	btn_doc = new Fl_Button(430, 50, 100, 25, btn_doc_buf);
	btn_doc->callback(btn_doc_cb, this);	

	p = "释放样本";
	fl_utf8from_mb(btn_sample_buf, 256, p, strlen(p));
	btn_sample = new Fl_Button(320, 50, 100, 25, btn_sample_buf);
	btn_sample->callback(btn_sample_cb, this);

	p = "打开pchunter";
	fl_utf8from_mb(btn_pchunter_buf, 256, p, strlen(p));
	btn_pchunter = new Fl_Button(210, 50, 100, 25, btn_pchunter_buf);
	btn_pchunter->callback(btn_pchunter_cb, this);

	p = "重启系统";
	fl_utf8from_mb(btn_reboot_buf, 256, p, strlen(p));
	btn_reboot = new Fl_Button(110, 50, 90, 25, btn_reboot_buf);
	btn_reboot->callback(btn_reboot_cb, this);

	if (!is_xp())
	{
		p = "关闭UAC";
		fl_utf8from_mb(btn_uac_buf, 256, p, strlen(p));
		btn_uac = new Fl_Button(10, 50, 90, 25, btn_uac_buf);
		btn_uac->callback(btn_uac_cb, this);
	}
	

	log_buf = new Fl_Text_Buffer();
	//log_buf->text(">>\n");

	log = new Fl_Text_Display(10, 80, 520, 410);
	log->buffer(log_buf);                 // attach text buffer to display widget
	log->wrap_mode(Fl_Text_Display::WRAP_AT_COLUMN, log->textfont());
	resizable(log);

	labelfont(FL_COURIER);
	//resizable(this);
	end();

	//Fl::scheme("gtk+");
	//init_choice();
}

void Virus_Window::init_choice()
{
	char* buffer = 0;

	add_log(L">> read config ...");

	do
	{
		int len = 0;

		buffer = copy_resource(IDR_XML1, L"xml", &len);

		if (!buffer || !len)
		{
			add_log(L"fail!");
			break;
		}

		xml_load_document(buffer, encoding_utf8, &doc);

		if (!doc || !doc->root_element)
		{
			add_log(L"format error!");
			break;
		}

		for (list_entry_t* list_entry = doc->root_element->element_list.flink; list_entry != &doc->root_element->element_list; list_entry = list_entry->flink)
		{
			wchar_t* v1 = 0;
			wchar_t* v2 = 0;
			xml_element* ele = container_of(list_entry, xml_element, list_entry);
			v1 = xml_query_attribute(ele, "name");
			v2 = xml_query_attribute(ele, "os");
			if (v1 && v2)
			{
				char buf[256];
				wchar_t wbuf[256];
				wcs_format(wbuf, 256, L"%s (%s)", v1, v2);
				fl_utf8fromwc(buf, 256, wbuf, wcslen(wbuf));
				choice->add(buf);
				add_log(wcs_format(wbuf, 256, L"name=\"%s\" os=\"%s\"", v1, v2));
			}
		}

	} while (0);

	add_log(L"finished.");

	add_log(L"click \"查看文档\" for necessary information");

	free(buffer);
}

void Virus_Window::add_log(wchar_t* wcs)
{
	if (!wcs) return;

	char* str = wcs_to_str(wcs, -1, encoding_utf8);

	Fl::lock();

	log_buf->insert(log_buf->length(), str);
	log_buf->insert(log_buf->length(), "\n");
	log->scroll(log_buf->count_lines(0, log_buf->length()), 0);
	
	Fl::unlock();

	free(str);
}

void Virus_Window::choice_cb(Fl_Choice* o, void* v)
{
	/*Virus_Window* window = (Virus_Window*)v;
	wchar_t buf[256];
	int i = o->value();
	if (i != -1)
	{
		window->add_log(wcs_format(buf, 256, L"select \"%s\"", o->text(i)));
	}
	*/
}

/* 可能挂住，这个要处理下！ */
void do_steps(Virus_Window* window, xml_element* ele)
{
	int any_fail = 0;
	void* old_value = 0;
	
	window->add_log(L"---------------start----------------");

	disable_fs_redirection(&old_value);

	args_t notify_data;
	notify_data.p0 = (void*)window;
	for (list_entry_t* list_entry = ele->element_list.flink; list_entry != &ele->element_list; list_entry = list_entry->flink)
	{
		xml_element* ele_step = container_of(list_entry, xml_element, list_entry);
		if (!do_step(ele_step, (step_notify_func)&Virus_Window::step_notify, &notify_data))
		{
			any_fail = 1;
			window->add_log(L"error! break!");
			break;
		}
	}

	revert_fs_redirection(&old_value);


	if (!any_fail)
	{
		window->add_log(L"重启后生效！");
	}
	window->add_log(L"---------------end----------------");
}

void Virus_Window::btn_do_thread(void* data)
{
	Virus_Window* window = (Virus_Window*)data;

	window->add_log(L">> 开始构造...");

	int i = window->choice->value();
	if (i == -1)
	{
		window->add_log(L"请选择病毒样本");
		return;
	}

	const char* str = window->choice->text();
	wchar_t* value = value = str_to_wcs((char*)str, -1, encoding_utf8);

	i = wcs_find(value, L' ');
	assert(i != -1);

	wchar_t name[256] = { 0 };
	wchar_t os[256] = { 0 };
	wcsncpy(name, value, i);
	wcscpy(os, &value[i + 1]);
	wcs_remove(os, L'(');
	wcs_remove(os, L')');

	for (list_entry_t* list_entry = window->doc->root_element->element_list.flink; list_entry != &window->doc->root_element->element_list; list_entry = list_entry->flink)
	{
		xml_element* ele = container_of(list_entry, xml_element, list_entry);
		if (0 == wcscmp(name, xml_query_attribute(ele, "name")) && 0 == wcscmp(os, xml_query_attribute(ele, "os")))
		{
			do_steps(window, ele);
			break;
		}
	}

	window->add_log(L"完成");

	free(value);
}

void Virus_Window::btn_doc_thread(void* data)
{
	Virus_Window* window = (Virus_Window*)data;
	int i = 0;
	wchar_t temp[256];
	wchar_t buf[256];
	wchar_t* select = 0;
	wchar_t name[256];

	if (!GetTempPath(256, temp))
	{
		window->add_log(L"获取%temp%目录失败");
		return;
	}

	window->add_log(temp);

	for (i = IDR_DOC1; i <= IDR_DOC25; i++)
	{
		if (!release(L"", wcs_format(buf, 256, L"%s%s", temp, doc_file[i - IDR_DOC1]), i, 0, 0))
		{
			window->add_log(buf);
			window->add_log(L"释放出错! 忽略.");
		}
	}

	i = window->choice->value();

	window->add_log(L"正在打开ie查看文档.");

	if (i != -1)
	{
		const char* str = window->choice->text(i);
		select = str_to_wcs((char*)str, -1, encoding_utf8);
		i = wcs_find(select, L' ');
		if (i != -1)
		{
			ZeroMemory(name, 256);
			wcsncpy(name, select, i);
			runexe(L"C:\\Program Files\\Internet Explorer\\iexplore.exe", wcs_format(buf, 256, L"%svirus_help_doc.html#病毒样本%s", temp, name), 1, 0, 0);
			free(select);
			return;
		}
		free(select);
	}

	runexe(L"C:\\Program Files\\Internet Explorer\\iexplore.exe", wcs_format(buf, 256, L"%svirus_help_doc.html", temp), 1, 0, 0);
}

void Virus_Window::btn_sample_thread(void* data)
{
	Virus_Window* window = (Virus_Window*)data;

	wchar_t* to = L"c:\\virus_sample(密码123).zip";

	window->add_log(to);

	if (release(L"", to, IDR_VIRUS_ZIP, 0, 0))
	{
		window->add_log(L"成功！");
		runexe(L"explorer.exe", to, 1, 0, 0);
	}
	else
	{
		window->add_log(L"释放失败！");
	}
}


void Virus_Window::btn_pchunter_thread(void* data)
{
	Virus_Window* window = (Virus_Window*)data;
	wchar_t temp[256];

	if (!GetTempPath(256, temp))
	{
		window->add_log(L"获取%temp%目录失败");
		return;
	}

	wchar_t file[256];
	wcs_format(file, 256, L"%spchunter.exe", temp);

	if (is_x64_system())
		release(L"", file, IDR_VIRUS_PH64, 0, 0);
	else
		release(L"", file, IDR_VIRUS_PH32, 0, 0);
	
	window->add_log(L"正在打开pchunter.");
	runexe(file, 0, 1, 0, 0);
}

/* https://msdn.microsoft.com/zh-cn/library/windows/desktop/aa376871(v=vs.85).aspx */
void Virus_Window::btn_reboot_thread(void* data)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for this process. 
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		return;

	// Get the LUID for the shutdown privilege. 
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set    
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process. 
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if (GetLastError() != ERROR_SUCCESS)
		return;

	ExitWindowsEx(/*EWX_SHUTDOWN*/ EWX_REBOOT | EWX_FORCE,
		SHTDN_REASON_MAJOR_OPERATINGSYSTEM |
		SHTDN_REASON_MINOR_UPGRADE |
		SHTDN_REASON_FLAG_PLANNED);
}

void Virus_Window::btn_uac_thread(void* data)
{
	Virus_Window* window = (Virus_Window*)data;
	window->add_log(L"正在打开UserAccountControlSettings.exe");
	wchar_t* file = L"C:\\Windows\\system32\\UserAccountControlSettings.exe";
	runexe(file, 0, 1, 0, 0);
}

void Virus_Window::step_notify(args_t* notify_data)
{
	Virus_Window* window = (Virus_Window*)notify_data->p0;
	wchar_t* msg = (wchar_t*)notify_data->p1;

	window->add_log(msg);
}

Virus_Window::~Virus_Window()
{
	xml_free_document(doc);
}

int main(int argc, char* argv[])
{
	if (!is_xp() && !is_evevated())
	{
		RunAs("");
		return 0;
	}

	/* 必须在提权后面，否则没权限打开句柄 */
	if (has_been_running())
	{
		MessageBox(NULL, L"实例已运行！", L"", MB_OK);
		return 0;
	}

	Virus_Window window;

	window.show(1, argv);

	/* 先显示了再初始化 */
	window.init_choice();

	Fl::lock();

	return Fl::run();
}

