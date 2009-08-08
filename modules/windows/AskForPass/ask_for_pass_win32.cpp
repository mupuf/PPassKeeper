#include "ppasskeeper.h"
#include "tokenizer.h"
#include <string>
#include <map>

//Parameters
std::map<std::string, cvariant> parameters;
#define PARAM_WINDOW_CAPTION "Window's caption"
#define PARAM_MAIN_TEXT "Main text"

#define PARAM_WINDOW_CAPTION_DEFAULT "Please key in the item ..."
#define PARAM_MAIN_TEXT_DEFAULT "Please key in the item corresponding to %1 :"

//local functions
bool WIN32_Get_Password(std::string title, std::string label, std::string& pwd);

//functions
extern "C" void constructor()
{
	//Set parameters's default value
	parameters[PARAM_WINDOW_CAPTION]=cvariant_from_string(PARAM_WINDOW_CAPTION_DEFAULT);
	parameters[PARAM_MAIN_TEXT_DEFAULT]=cvariant_from_string(PARAM_MAIN_TEXT_DEFAULT);
}

extern "C" void destructor()
{
	//Nothing to free
}

extern "C" const char* getModuleID()
{
	return "AskForPass_WIN32";
}

extern "C" const char* getModuleName()
{
	return "Ask for the password through a WIN32 window";
}

extern "C" const int getABIVersion()
{
	return 1;
}

extern "C" ppk_boolean isWritable()
{
	return PPK_FALSE;
}

extern "C" ppk_security_level securityLevel(const char* module_id)
{
	return ppk_sec_perfect;
}

//Get available flags
extern "C" unsigned int readFlagsAvailable()
{
	return ppk_rf_none|ppk_rf_silent;
}

extern "C" unsigned int writeFlagsAvailable()
{
	return ppk_wf_none;
}

extern "C" unsigned int listingFlagsAvailable()
{
	return ppk_lf_none;
}


extern "C" unsigned int getEntryListCount(unsigned int entry_types, unsigned int flags)
{
	return 0;
}

extern "C" unsigned int getEntryList(unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags)
{	
	return 0;
}

std::string str_replace(std::string pattern, std::string toReplace, std::string newText)
{
	int len = toReplace.size(),pos;
	while((pos=pattern.find(toReplace)) != std::string::npos)
	{
		pattern.replace(pos, len, newText);
	}
	return pattern;
}

extern "C" ppk_error getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
{
	if((int)(flags&ppk_rf_silent)==0)
	{
		char key[101];
		if(ppk_get_key(entry, key, sizeof(key))==PPK_FALSE)
			return PPK_UNKNOWN_ENTRY_TYPE;

		const char* title=cvariant_get_string(parameters[PARAM_WINDOW_CAPTION]);
		const char* label_pattern=cvariant_get_string(parameters[PARAM_MAIN_TEXT]);
		std::string label=str_replace(label_pattern, "%1", key);

		std::string pwd;
		bool res=WIN32_Get_Password(title, label.c_str(), pwd);

		//if everything went fine
		if(res)
		{
			*edata=ppk_string_data_new(pwd.c_str());
			return PPK_OK;
		}
		else
			return PPK_USER_CANCELLED;
	}
	else
		return PPK_INCOMPATIBLE_FLAGS;
}

extern "C" ppk_error setEntry(const ppk_entry* entry, const ppk_data* edata, unsigned int flags)
{
	return PPK_UNSUPPORTED_METHOD;
}

extern "C" ppk_error removeEntry(const ppk_entry* entry, unsigned int flags)
{
	return PPK_UNSUPPORTED_METHOD;
}

extern "C" ppk_boolean entryExists(const ppk_entry* entry, unsigned int flags)
{
	return PPK_TRUE;
}

extern "C" unsigned int maxDataSize(ppk_data_type type)
{
	switch(type)
	{
		case ppk_string:
			return (unsigned int)-1;
		case ppk_blob:
			return 0;
	}
	
	return 0;
}

const ppk_settings_group ppk_settings_custom_texts = { "Custom Texts", "Customize the texts shown by AskForPass_win32" };
extern "C" const ppk_proto_param** availableParameters()
{
	static ppk_proto_param win_cap;
	win_cap.expected_type=cvariant_string;
	win_cap.name=PARAM_WINDOW_CAPTION;
	win_cap.help_text="The caption you would like to see";
	win_cap.default_value=cvariant_from_string(PARAM_WINDOW_CAPTION_DEFAULT);
	win_cap.group=&ppk_settings_custom_texts;

	static ppk_proto_param win_text;
	win_text.expected_type=cvariant_string;
	win_text.name=PARAM_MAIN_TEXT;
	win_text.help_text="The main text you would like to shown.\nUse %1 were you actually want the key to be shown.";
	win_text.default_value=cvariant_from_string(PARAM_MAIN_TEXT_DEFAULT);
	win_text.group=&ppk_settings_custom_texts;

	static const ppk_proto_param* params[]={&win_cap, &win_text, NULL};

	return params;
}

#include <stdio.h>
extern "C" void setParam(const char* paramName, const cvariant value)
{
	std::string key(paramName);

	if(key == PARAM_WINDOW_CAPTION)
	{
		if(cvariant_get_type(value)==cvariant_string)
			parameters[PARAM_WINDOW_CAPTION]=value;
		else
			printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
	}
	else if(key == PARAM_MAIN_TEXT)
	{
		if(cvariant_get_type(value)==cvariant_string)
			parameters[PARAM_MAIN_TEXT]=value;
		else
			printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
	}
	else
		printf("%s: Unknown param(%s) !!\n", getModuleID(), paramName);
}

//optionnal
std::string* customPrompt()
{
	static std::string msg;
	return &msg;
}

extern "C" ppk_boolean setCustomPromptMessage(const char* customMessage)
{
	if(customMessage!=NULL)
		*(customPrompt())=customMessage;
	else
		*(customPrompt())=std::string();

	return PPK_TRUE;
}


/*************************************************************************************************
**************************************************************************************************
*******************************                                     ******************************
*******************************               WIN32 PART            ******************************
*******************************                                     ******************************
**************************************************************************************************
**************************************************************************************************/

#include <windows.h>
#include "resource.h"

bool succeded;

BOOL CenterWindow(HWND hwnd)
{
	RECT rc, rcDlg, rcOwner;
	HWND hwndOwner = GetDesktopWindow();

    GetWindowRect(hwndOwner, &rcOwner); 
    GetWindowRect(hwnd, &rcDlg); 
    CopyRect(&rc, &rcOwner); 

    OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top); 
    OffsetRect(&rc, -rc.left, -rc.top); 
    OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom); 
	
    SetWindowPos(hwnd, 
                 HWND_TOP, 
                 rcOwner.left + (rc.right / 2), 
                 rcOwner.top + (rc.bottom / 2), 
                 0, 0,          // Ignores size arguments. 
                 SWP_NOSIZE); 
				 
	return TRUE;
}


LRESULT CALLBACK WndProcedure(HWND hWnd, UINT Msg,
			   WPARAM wParam, LPARAM lParam)
{
    switch(Msg)
    {
	case WM_CREATE:
	    CenterWindow(hWnd);
		break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			succeded=true;
			PostQuitMessage(WM_QUIT);
		}
		else if (LOWORD(wParam) == IDC_CANCEL)
		{
			succeded=false;
			PostQuitMessage(WM_QUIT);
		}
		break;

    case WM_DESTROY:
        PostQuitMessage(WM_QUIT);
        break;
    default:
        return DefWindowProc(hWnd, Msg, wParam, lParam);
    }

    return 0;
}

bool WIN32_Get_Password(std::string title, std::string label, std::string& pwd)
{
	succeded=false;
	
	MSG msg;
	HINSTANCE hInst = GetModuleHandle(0);
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof wcex);
	wcex.cbSize         = sizeof wcex;
	wcex.hbrBackground  = GetSysColorBrush(COLOR_3DFACE);
	wcex.lpszMenuName   = 0;


	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProcedure;
	wcex.hInstance      = hInst;
	wcex.hIcon          = LoadIcon(0, (LPCTSTR)IDI_QUESTION);
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName  = "WIN32_Get_Password";

	if(!RegisterClassEx(&wcex))
		return false;
	
	HFONT hfont0 = CreateFont(-11, 0, 0, 0, 400, FALSE, FALSE, FALSE, 1, 400, 0, 0, 0, TEXT("Ms Shell Dlg 2"));
	HWND hwnd = CreateWindowEx(0, TEXT("WIN32_Get_Password"), TEXT(title.c_str()), WS_VISIBLE | WS_BORDER | WS_CAPTION | WS_DLGFRAME | WS_POPUP | WS_SYSMENU, 0, 0, 426, 135, 0, 0, hInst, 0);
	if(hwnd!=NULL)
	{
		HWND hCtrl0_1 = CreateWindowEx(0, "Static", TEXT(label.c_str()), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 8, 8, 413, 26, hwnd, (HMENU)IDC_STATIC, hInst, 0);
		SendMessage(hCtrl0_1, WM_SETFONT, (WPARAM)hfont0, FALSE);
		HWND edit_pwd = CreateWindowEx(0, "Edit", 0, WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | ES_AUTOHSCROLL | WS_TABSTOP | ES_PASSWORD , 8, 42, 405, 21, hwnd, (HMENU)IDE_PASS, hInst, 0);
		SendMessage(edit_pwd, WM_SETFONT, (WPARAM)hfont0, FALSE);
		HWND hCtrl0_0 = CreateWindowEx(0, "Button", TEXT("OK"), WS_VISIBLE | WS_CHILD | WS_TABSTOP | BS_DEFPUSHBUTTON, 338, 75, 75, 23, hwnd, (HMENU)IDOK, hInst, 0);
		SendMessage(hCtrl0_0, WM_SETFONT, (WPARAM)hfont0, FALSE);
		HWND hCtrl0_2 = CreateWindowEx(0, "Button", TEXT("Cancel"), WS_VISIBLE | WS_CHILD | WS_TABSTOP, 252, 75, 78, 24, hwnd, (HMENU)IDC_CANCEL, hInst, 0);
		SendMessage(hCtrl0_2, WM_SETFONT, (WPARAM)hfont0, FALSE);
		SetFocus(edit_pwd);
		
		ShowWindow(hwnd, SW_SHOWNORMAL);
		UpdateWindow(hwnd);

		while(GetMessage(&msg, NULL, 0, 0)) 
		{
			if(!IsDialogMessage(hwnd,&msg))
			{
				DispatchMessage(&msg);
				TranslateMessage(&msg);
			} 
		}
		
		//get the password if user pressed OK
		if(succeded)
		{
			char bpwd[101];
			succeded=(GetWindowText(edit_pwd,bpwd,sizeof(bpwd))>0);
			pwd=bpwd;
		}
		
		return succeded;
	}
	else
		return false;
}

