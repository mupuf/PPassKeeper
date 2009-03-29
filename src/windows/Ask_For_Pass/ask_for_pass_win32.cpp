#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>

//local functions
std::string* last_error()
{
	static std::string lerror;
	return &lerror;
}
extern "C" const char* getLastError()
{
	return last_error()->c_str();
}
bool WIN32_Get_Password(std::string title, std::string label, std::string& pwd);
void setError(std::string error)
{
	*(last_error())="PPK_Ask_For_Pass_WIN32 : " + error;
}

//functions
extern "C" const char* getModuleID()
{
	return "Ask_For_Pass_WIN32";
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

extern "C" ppk_boolean getEntry(const ppk_entry entry, ppk_data *edata, unsigned int flags)
{
	static std::string pwd;
	if((int)(flags&ppk_rf_silent)==0)
	{
		std::string text;
		if(entry.type==ppk_network)
			text=toString(entry.net.login)+"@"+toString(entry.net.host)+":"+toString(entry.net.port);
		else if(entry.type==ppk_application)
			text=toString(entry.app.username)+"@"+toString(entry.app.app_name);
		else if(entry.type==ppk_item)
			text="this key("+toString(entry.item)+")";
		
		bool res=WIN32_Get_Password("Please key in the item ...","Please key in the item corresponding to " + text + " : ",pwd);

		//if everything went fine
		if(res)
		{
			setError("");
			edata->string=pwd.c_str();
			return PPK_TRUE;
		}
		else
		{
			setError("User pressed cancel");
			return PPK_FALSE;
		}
	}
	else
		return PPK_FALSE;
}

extern "C" ppk_boolean setEntry(const ppk_entry entry, const ppk_data edata, unsigned int flags)
{
	return PPK_FALSE;
}

extern "C" ppk_boolean removeEntry(const ppk_entry entry, unsigned int flags)
{
	return PPK_FALSE;
}

extern "C" ppk_boolean entryExists(const ppk_entry entry, unsigned int flags)
{
	return PPK_TRUE;
}

extern "C" unsigned int maxDataSize(ppk_data_type type)
{
	switch(type)
	{
		case ppk_string:
			return -1;
		case ppk_blob:
			return 0;
	}
	
	return 0;
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

