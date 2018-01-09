#include "tinyui.h"
#include <windows.h>
#include <Shlobj.h>
#include "tinyxml.h"
#include <assert.h>
#include "tinyargs.h"

#define max_col_span 20

#define max_row_span 20

#define default_col_width 100

#define default_row_height 25

#define default_border_size 3

#define max_buf_len 512

#define default_res_name MAKEINTRESOURCE(101)

#define default_res_type RT_HTML

#define wm_post_task (WM_USER + 1)

typedef void(*handler_func)(void* ui_context);

typedef void(*task_func)(void* data);

typedef enum
{
	type_none = 0,
	type_input,
	type_button,
	type_label,
	type_textarea,
	type_radio,
	type_progress,
	type_select,
}ui_type_enum;

typedef enum
{
	align_none = 0,
	align_left,
	align_right,
	align_center,
}ui_align_enum;

typedef struct
{
	ui_align_enum align;
	int colspan;
	int rowspan;
	int row;
	int col;
	ui_type_enum type;
	wchar_t* name;
	wchar_t* value;
	list_entry_t list_entry;
	int item_count;
	wchar_t** select_item_value;
}ui_data;

typedef struct
{
	int row_height;
	int col_width;
	int row_count;
	int col_count;
	wchar_t* title;
	list_entry_t data_list;
}ui_table;

static char* unicode_to_utf8(wchar_t* text)
{
	char* str = 0;
	int len = 0;

	assert(text);

	len = WideCharToMultiByte(CP_UTF8, 0, text, -1, NULL, 0, NULL, NULL);

	str = (char*)safe_malloc(len + 1);

	WideCharToMultiByte(CP_UTF8, 0, text, -1, str, len, NULL, NULL);

	return str;
}

char* new_char_string(char* string)
{
	char* str = 0;

	if (!string)
		return 0;

	str = (char*)safe_malloc(strlen(string) + 1);
	memcpy(str, string, strlen(string));

	return str;
}

wchar_t* new_wchar_string(wchar_t* string)
{
	wchar_t* str = 0;

	if (!string)
		return 0;

	str = (wchar_t*)safe_malloc(sizeof(wchar_t) * (wcslen(string) + 1));
	memcpy(str, string, wcslen(string) * sizeof(wchar_t));

	return str;
}

ui_data* ui_new_data(xml_element* element, int row, int col, ui_type_enum type, wchar_t* name, wchar_t* value)
{
	ui_data* data = 0;
	wchar_t* align = 0;
	wchar_t* colspan = 0;
	wchar_t* rowspan = 0;
	int colspan_i = 0;
	int rowspan_i = 0;

	data = (ui_data*)safe_malloc(sizeof(ui_data));
	data->align = align_left;
	data->rowspan = 1;
	data->colspan = 1;
	data->row = row;
	data->col = col;
	data->type = type;
	data->name = new_wchar_string(name);
	data->value = new_wchar_string(value);
	
	align = xml_query_attribute(element, "align");
	if (align)
	{
		if (0 == wcscmp(align, L"center"))
			data->align = align_center;
		else if (0 == wcscmp(align, L"right"))
			data->align = align_right;
	}

	colspan = xml_query_attribute(element, "colspan");
	if (colspan)
	{
		colspan_i = _wtoi(colspan);
		if (0 < colspan_i && colspan_i < max_col_span)
			data->colspan = colspan_i;
	}

	rowspan = xml_query_attribute(element, "rowspan");
	if (rowspan)
	{
		rowspan_i = _wtoi(rowspan);
		if (0 < rowspan_i && rowspan_i < max_row_span)
			data->rowspan = rowspan_i;
	}

	return data;
}

void ui_free_data(ui_data* data)
{
	int i = 0;

	if (!data)
		return;

	free(data->name);
	free(data->value);

	for (i = 0; i < data->item_count; i++)
	{
		free(data->select_item_value[i]);
	}
	free(data->select_item_value);

	free(data);
}

ui_table* ui_new_table(wchar_t* title)
{
	ui_table* table = 0;
	
	table = (ui_table*)malloc(sizeof(ui_table));
	initialize_list_head(&table->data_list);
	table->col_width = default_col_width;
	table->row_height = default_row_height;
	table->title = new_wchar_string(title);

	return table;
}

void ui_free_table(ui_table* table)
{
	ui_data* data = 0;

	if (!table)
		return;
	
	while (!is_list_empty(&table->data_list))
	{
		data = container_of(table->data_list.flink, ui_data, list_entry);
		remove_from_list(table->data_list.flink);
		ui_free_data(data);
	}
		
	free(table);
}

ui_table* ui_create_table(char* buffer)
{
	ui_table* table = 0;
	ui_data* data = 0;
	xml_document* doc = 0;
	xml_element* ele_table = 0;
	xml_element* ele_tr = 0;
	xml_element* ele_td = 0;
	xml_element* ele = 0;
	xml_element* ele_select = 0;
	xml_element* ele_item = 0;
	list_entry_t* list_entrya = 0;
	list_entry_t* list_entryb = 0;
	list_entry_t* list_entry_item = 0;
	int has_control = 0;
	ui_type_enum type = 0;
	wchar_t* name = 0;
	wchar_t* value = 0;
	int row = 0;
	int col = 0;
	int max_col = 0;
	int row_skip = 0;
	int row_skip_max = 0;
	int i = 0;

	if (!buffer)
		return 0;	

	do
	{
		xml_load_document(buffer, encoding_ansi, &doc);

		if (!doc)
			break;

		//xml_write_document(stdout, encoding_ansi, doc);
		ele_table = doc->root_element;

		table = ui_new_table(xml_query_attribute(ele_table, "name"));

		for (list_entrya = ele_table->element_list.flink; list_entrya != &ele_table->element_list; list_entrya = list_entrya->flink)
		{
			/* max skip in this row, set to 0 */
			row_skip_max = 0;
			
			/* if current row is to be skipped */
			if (row_skip > 0)
			{
				row_skip--;
				continue;
			}
			
			ele_tr = container_of(list_entrya, xml_element, list_entry);
			for (list_entryb = ele_tr->element_list.flink; list_entryb != &ele_tr->element_list; list_entryb = list_entryb->flink)
			{
				ele_td = container_of(list_entryb, xml_element, list_entry);
				
				has_control = 0;
				if (!is_list_empty(&ele_td->element_list))
				{
					ele = container_of(ele_td->element_list.flink, xml_element, list_entry);
					if (0 == strcmp(ele->name, "input"))
					{
						has_control = 1;
						type = type_input;
						name = xml_query_attribute(ele, "name");
						value = xml_query_attribute(ele, "value");
					}
					else if (0 == strcmp(ele->name, "button"))
					{
						has_control = 1;
						type = type_button;
						name = xml_query_attribute(ele, "name");
						value = ele->text;
					}
					else if (0 == strcmp(ele->name, "label"))
					{
						has_control = 1;
						type = type_label;
						name = xml_query_attribute(ele, "name");
						value = ele->text;
					}
					else if (0 == strcmp(ele->name, "textarea"))
					{
						has_control = 1;
						type = type_textarea;
						name = xml_query_attribute(ele, "name");
						value = ele->text;
					}
					else if (0 == strcmp(ele->name, "progress"))
					{
						has_control = 1;
						type = type_progress;
						name = xml_query_attribute(ele, "name");
						value = ele->text;
					}
					else if (0 == strcmp(ele->name, "select"))
					{
						has_control = 1;
						type = type_select;
						name = xml_query_attribute(ele, "name");
						value = ele->text;			
						ele_select = ele;
					}
				}
				else if (ele_td->text)
				{
					has_control = 1;
					type = type_label;
					name = 0;
					value = ele_td->text;
				}

				if (has_control)
				{
					data = ui_new_data(ele_td, row, col, type, name, value);

					insert_into_list(&table->data_list, &data->list_entry);

					col += data->colspan;

					/* find max skip */
					if (row_skip_max < data->rowspan - 1)
						row_skip_max = data->rowspan - 1;

					if (type == type_select)
					{
						data->item_count = 0;
						for (list_entry_item = ele_select->element_list.flink; list_entry_item != &ele_select->element_list; list_entry_item = list_entry_item->flink)
							data->item_count++;

						if (data->item_count > 0)
						{
							i = 0;
							data->select_item_value = (wchar_t**)safe_malloc(sizeof(wchar_t*) * data->item_count);
							for (list_entry_item = ele_select->element_list.flink; list_entry_item != &ele_select->element_list; list_entry_item = list_entry_item->flink)
							{
								ele_item = container_of(list_entry_item, xml_element, list_entry);
								data->select_item_value[i] = (wchar_t*)safe_malloc(sizeof(wchar_t) * (wcslen(ele_item->text) + 1));
								wcscpy(data->select_item_value[i], ele_item->text);
								i++;
							}
						}				
					}
				}
				else
					col++;

				if (col > max_col)
					max_col = col;
			}
			
			col = 0;
			/* if find skip, then set */
			if (row_skip_max)
				row_skip = row_skip_max;
			
			row += 1 + row_skip_max;
		}

	} while (0);

	table->col_count = max_col;
	table->row_count = row;
	
	xml_free_document(doc);
	return table;
}

/**************************************************************************************************************/

typedef struct
{
	ui_data* data;
	HWND hwnd;
	int x, y, w, h;
	int id;
	handler_func handler;
	list_entry_t list_entry;
}ui_control;

/**************************************************************************************************************/

typedef struct
{
	task_func task_foo;
	void* args;
	int id;
	list_entry_t list_entry;
}ui_delay_task;

typedef struct
{
	HANDLE hwnd;
	int x, y, w, h;
	int id_max;
	list_entry_t control_list;
	HBRUSH hbr_bk;
	list_entry_t delay_task_list;
	int delay_task_start_id;
}ui_window;

LRESULT CALLBACK ui_window_proc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	ui_window* window = 0;
	list_entry_t* list_entry = 0;
	ui_control* control = 0;
	task_func task_foo = 0;
	HDC hdc;
	int task_id = (int)wparam;
	ui_delay_task* task = 0;
	
	window = (ui_window*)(LONG_PTR)GetWindowLongPtr(hwnd, GWLP_USERDATA);

	switch (umsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_TIMER:
		for (list_entry = window->delay_task_list.flink; list_entry != &window->delay_task_list; list_entry = list_entry->flink)
		{
			task = container_of(list_entry, ui_delay_task, list_entry);
			if (task->id == task_id)
			{
				if (task->task_foo)
					task->task_foo(task->args);

				remove_from_list(&task->list_entry);

				KillTimer(hwnd, task->id);
				
				free(task);

				break;
			}
		}
		return 0;
	case WM_COMMAND:
		if (HIWORD(wparam) == BN_CLICKED)
		{
			for (list_entry = window->control_list.flink; list_entry != &window->control_list; list_entry = list_entry->flink)
			{
				control = container_of(list_entry, ui_control, list_entry);
				if (control->id == LOWORD(wparam) && control->handler)
				{
					control->handler((void*)window);
					break;
				}
			}
		}
		break; 
	case WM_CTLCOLORSTATIC:
	{
		hdc = (HDC)wparam;
		SetBkColor(hdc, RGB(0xee, 0xee, 0xee));
		return (INT_PTR)window->hbr_bk;
	}
	case wm_post_task:
		task_foo = (task_func)wparam;
		assert(task_foo);
		task_foo((void*)lparam);
		return 1;
	default:
		break;
	}
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

ui_window* ui_create_window(ui_table* table)
{
	WNDCLASS wc = { 0 };
	RECT rect = { 0 };
	ui_window* window = 0;
	list_entry_t* list_entry = 0;
	list_entry_t* list_entry_select = 0;
	ui_control* control = 0;
	HFONT hfont = { 0 };
	LOGFONT logfont = { 0 };
	unsigned long style = 0;
	int i = 0;
	int j = 0;
	char buf[max_buf_len];

	SystemParametersInfoW(SPI_GETWORKAREA, 0, &rect, 0);

	window = (ui_window*)safe_malloc(sizeof(ui_window));
	window->w = table->col_count * table->col_width;
	window->h = table->row_count * table->row_height;
	window->x = (rect.left + rect.right - window->w) / 2;
	window->y = (rect.top + rect.bottom - window->h) / 2;
	window->id_max = 500;
	window->delay_task_start_id = 1;

	window->hbr_bk = CreateSolidBrush(RGB(0xee, 0xee, 0xee));

	wc.lpszClassName = L"tinyui_window_class";
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpfnWndProc = ui_window_proc;
	wc.hbrBackground = window->hbr_bk;
	RegisterClassW(&wc);

	// create
	window->hwnd = CreateWindowW(
		wc.lpszClassName,
		table->title ? table->title : L"",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		window->x, window->y, window->w, window->h,
		NULL,
		NULL,
		NULL,
		NULL
		);

	SetWindowLongPtr(window->hwnd, GWLP_USERDATA, (LONG)(LONG_PTR)window);

	GetClientRect(window->hwnd, &rect);
	window->w += window->w - (rect.right - rect.left + 1);
	window->h += window->h - (rect.bottom - rect.top + 1);
	SetWindowPos(window->hwnd, 0, 0, 0, window->w, window->h, SWP_NOMOVE | SWP_NOZORDER);

	initialize_list_head(&window->delay_task_list);

	initialize_list_head(&window->control_list);
	for (list_entry = table->data_list.flink; list_entry != &table->data_list; list_entry = list_entry->flink)
	{
		control = (ui_control*)safe_malloc(sizeof(ui_control));
		control->data = container_of(list_entry, ui_data, list_entry);
		insert_into_list(&window->control_list, &control->list_entry);

		control->x = control->data->col * table->col_width + default_border_size;
		control->y = control->data->row * table->row_height + default_border_size;
		control->w = control->data->colspan * table->col_width - default_border_size * 2;
		control->h = table->row_height * control->data->rowspan - 2 * default_border_size;
		control->id = window->id_max;
		window->id_max++;
		if (control->data->type == type_button)
		{
			control->hwnd = CreateWindowW(
				L"BUTTON",
				control->data->value ? control->data->value : L"",
				WS_VISIBLE | WS_CHILD,
				control->x, control->y, control->w, control->h,
				window->hwnd,
				(HMENU)control->id,
				NULL,
				NULL);
		}
		else if (control->data->type == type_input)
		{
			control->hwnd = CreateWindowW(
				L"EDIT",
				control->data->value ? control->data->value : L"",
				ES_AUTOHSCROLL | WS_CHILD | WS_VISIBLE | WS_BORDER,
				control->x, control->y, control->w, control->h,
				window->hwnd,
				(HMENU)control->id,
				NULL,
				NULL);
		}
		else if (control->data->type == type_label)
		{
			style = WS_CHILD | WS_VISIBLE;
			if (control->data->align == align_center)
				style |= SS_CENTER;
			else if (control->data->align == align_left)
				style |= SS_LEFT;
			else
				style |= SS_RIGHT;
			control->hwnd = CreateWindowW(
				L"STATIC",
				control->data->value ? control->data->value : L"",
				style,
				control->x, control->y, control->w, control->h,
				window->hwnd,
				(HMENU)control->id,
				NULL,
				NULL);
		}
		else if (control->data->type == type_textarea)
		{
			style = WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_BORDER;
			control->hwnd = CreateWindowW(
				L"EDIT",
				control->data->value ? control->data->value : L"",
				style,
				control->x, control->y, control->w, control->h,
				window->hwnd,
				(HMENU)control->id,
				NULL,
				NULL);
		}
		else if (control->data->type == type_progress)
		{
			style = WS_CHILD | WS_VISIBLE;
			control->hwnd = CreateWindowW(
				PROGRESS_CLASS,
				NULL,
				style,
				control->x, control->y, control->w, control->h,
				window->hwnd,
				(HMENU)control->id,
				NULL,
				NULL);
			SendMessage(control->hwnd, PBM_SETRANGE32, (WPARAM)0, (LPARAM)(100));
		}
		else if (control->data->type == type_select)
		{
			style = CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | WS_VSCROLL;
			control->hwnd = CreateWindowW(
				WC_COMBOBOX,
				NULL,
				style,
				control->x, control->y, control->w, 200,
				window->hwnd,
				(HMENU)control->id,
				NULL,
				NULL);
			for (j = 0; j < control->data->item_count; j++)
			{
				SendMessage(control->hwnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)(control->data->select_item_value[j]));
			}
			SendMessage(control->hwnd, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
		}
		
		hfont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
		SendMessage(control->hwnd, WM_SETFONT, (WPARAM)hfont, 1);

		if (control->data->name && control->data->type == type_button)
		{
			control->handler = (handler_func)GetProcAddress(NULL, str_format(buf, max_buf_len, "on_%S_click", control->data->name));
		}		
	}

	return window;
}

void ui_show_window(ui_window* window)
{
	MSG msg = { 0 };

	ShowWindow(window->hwnd, SW_SHOW);
	
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void ui_free_window(ui_window* window)
{
	ui_control* control = 0;

	if (!window)
		return;

	DeleteObject(window->hbr_bk);

	while (!is_list_empty(&window->control_list))
	{
		control = container_of(window->control_list.flink, ui_control, list_entry);
		remove_from_list(window->control_list.flink);
		free(control);
	}

	free(window);
}

char* ui_copy_resource()
{
	HGLOBAL hglobal = NULL;
	HINSTANCE hinst;
	HRSRC hrsrc;
	unsigned int size = 0;
	char* buffer = 0;
	char* res = 0;

	hinst = NULL;
	hrsrc = FindResource(hinst, default_res_name, default_res_type);
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

	return buffer;
}

void ui()
{
	list_entry_t* list_entry = 0;
	ui_table* table = 0;
	ui_window* window = 0;
	char* buffer = 0;

	buffer = ui_copy_resource();
	if (!buffer)
		return;

	table = ui_create_table(buffer);
	if (!table)
		return;

	window = ui_create_window(table);

	ui_show_window(window);

	ui_free_window(window);

	ui_free_table(table);

	free(buffer);
}

void ui2(void (*foo)(void*))
{
	list_entry_t* list_entry = 0;
	ui_table* table = 0;
	ui_window* window = 0;
	char* buffer = 0;

	buffer = ui_copy_resource();
	if (!buffer)
		return;

	table = ui_create_table(buffer);
	if (!table)
		return;

	window = ui_create_window(table);

	foo(window);

	ui_show_window(window);

	ui_free_window(window);

	ui_free_table(table);

	free(buffer);
}

void* ui_get_hwnd(void* ui_context)
{
	ui_window* window = (ui_window*)ui_context;
	
	if (!window)
		return 0;

	return (void*)window->hwnd;
}

void ui_post_task(void* ui_context, char* name, void* arg_list)
{
	ui_window* window = (ui_window*)ui_context;
	char buf[max_buf_len];

	task_func task = (task_func)GetProcAddress(NULL, str_format(buf, max_buf_len, "on_%s_task", name));

	if (!window || !task)
		return;

	PostMessage(window->hwnd, wm_post_task, (WPARAM)task, (LPARAM)arg_list);
}

void ui_post_delay_task(void* ui_context, int ms, char* name, void* arg_list)
{
	ui_window* window = (ui_window*)ui_context;
	char buf[max_buf_len];

	task_func task_foo = (task_func)GetProcAddress(NULL, str_format(buf, max_buf_len, "on_%s_task", name));

	if (!window || !task_foo)
		return;

	ui_delay_task* task = (ui_delay_task*)safe_malloc(sizeof(ui_delay_task));
	task->id = window->delay_task_start_id++;
	task->task_foo = task_foo;
	task->args = arg_list;

	insert_into_list(&window->delay_task_list, &task->list_entry);

	SetTimer(window->hwnd, task->id, ms, (TIMERPROC)0);
}

wchar_t* ui_ask_open_file(void* ui_context)
{
	static wchar_t file[max_buf_len] = { 0 };
	OPENFILENAME ofn = { 0 };
	ui_window* window = (ui_window*)ui_context;

	if (!window)
		return 0;

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = window->hwnd;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = file;
	ofn.nMaxFile = max_buf_len;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
	return GetOpenFileName(&ofn) ? file : 0;
}

wchar_t* ui_ask_save_file(void* ui_context)
{
	static wchar_t file[max_buf_len] = { 0 };
	OPENFILENAME ofn = { 0 };
	ui_window* window = (ui_window*)ui_context;

	if (!window)
		return 0;

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = window->hwnd;
	ofn.nFilterIndex = 1;
	ofn.lpstrFile = file;
	ofn.nMaxFile = max_buf_len;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;
	ofn.lpstrTitle = L"±£´æÎª";
	return GetSaveFileName(&ofn) ? file : 0;
}

wchar_t* ui_ask_open_path(void* ui_context)
{
	static wchar_t path[max_buf_len] = { 0 };
	ui_window* window = (ui_window*)ui_context;
	BROWSEINFO bi = { 0 };

	if (!window)
		return 0;

	bi.hwndOwner = window->hwnd;
	bi.pszDisplayName = path;
	bi.ulFlags = BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	return SHGetPathFromIDList(idl, path) ? path : 0;
};

static ui_control* ui_find_control(void* ui_context, wchar_t* name)
{
	list_entry_t* list_entry = 0;
	ui_window* window = (ui_window*)ui_context;
	ui_control* control = 0;

	if (!window || !name)
		return 0;

	for (list_entry = window->control_list.flink; list_entry != &window->control_list; list_entry = list_entry->flink)
	{
		control = container_of(list_entry, ui_control, list_entry);
		if (control->data->name && 0 == wcscmp(control->data->name, name))
		{
			return control;
		}
	}

	return 0;
}

void ui_set_icon(void* ui_context, int id)
{
	HICON hicon = NULL;
	ui_window* window = (ui_window*)ui_context;

	if (!window)
		return;

	hicon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(id));
	SendMessage(window->hwnd, WM_SETICON, ICON_BIG, (LPARAM)hicon);
	SendMessage(window->hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hicon);
}


void ui_set_value(void* ui_context, wchar_t* name, wchar_t* value)
{
	ui_control* control = ui_find_control(ui_context, name);
	int line_count = 0;

	if (!ui_context || !name || !value)
		return;

	if (control)
	{
		if (control->data->type == type_select)
		{
			SendMessage(control->hwnd, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)value);
		}
		else
		{
			free(control->data->value);
			control->data->value = new_wchar_string(value);
			if (control->data->type == type_progress)
				SendMessage(control->hwnd, PBM_SETPOS, (WPARAM)_wtoi(control->data->value), (LPARAM)0);
			else
				SendMessage(control->hwnd, WM_SETTEXT, (WPARAM)0, (LPARAM)control->data->value);

			if (control->data->type == type_textarea)
			{
				line_count = SendMessage(control->hwnd, EM_GETLINECOUNT, 0, 0);
				SendMessage(control->hwnd, EM_LINESCROLL, (WPARAM)0, (LPARAM)line_count);
			}
		}
	}
}

wchar_t* ui_get_value(void* ui_context, wchar_t* name)
{
	int len = 0;
	ui_control* control = ui_find_control(ui_context, name);

	if (!ui_context || !name)
		return L"";

	if (control)
	{
		len = SendMessage(control->hwnd, WM_GETTEXTLENGTH, 0, 0);
		if (!control->data->value || len > (int)wcslen(control->data->value))
		{
			free(control->data->value);
			control->data->value = (wchar_t*)safe_malloc(sizeof(wchar_t) * (len + 1));
		}

		SendMessage(control->hwnd, WM_GETTEXT, (WPARAM)(len + 1), (LPARAM)control->data->value);
		return control->data->value;
	}

	return L"";
}

static void ui_enable_internal(void* ui_context, wchar_t* name, int enable)
{
	ui_control* control = ui_find_control(ui_context, name);
	if (control)
		EnableWindow(control->hwnd, enable ? TRUE : FALSE);
}

void ui_disable(void* ui_context, wchar_t* name)
{
	ui_enable_internal(ui_context, name, FALSE);
}

void ui_enable(void* ui_context, wchar_t* name)
{
	ui_enable_internal(ui_context, name, TRUE);
}

static void ui_show_internal(void* ui_context, wchar_t* name, int show)
{
	ui_control* control = ui_find_control(ui_context, name);

	if (control)
		ShowWindow(control->hwnd, show ? SW_SHOW : SW_HIDE);
}

void ui_show(void* ui_context, wchar_t* name)
{
	ui_show_internal(ui_context, name, TRUE);
}

void ui_hide(void* ui_context, wchar_t* name)
{
	ui_show_internal(ui_context, name, FALSE);
}

#ifdef tinyui_test

ui_click(virus_button, ui_context)
{
	ui_set_value(ui_context, L"virus_select", L"Ëæ±ãË®µç·Ñ¸¶¸¶¸¶¸¶¸¶¸¶¸¶¸¶");
}

int main()
{	
	ui();
	
	return 0;
}

#endif