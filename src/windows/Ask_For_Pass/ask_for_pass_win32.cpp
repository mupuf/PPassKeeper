#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>

//Local definitions
std::string last_error;
std::string last_pwd;

//local functions
bool WIN32_Get_Password(std::string title, std::string label, std::string& pwd);
void setError(std::string error)
{
	last_error="PPK_Ask_For_Pass_WIN32 : " + error;
}

//constructors & destructors
/*extern "C" void _init(void)
{

}

extern "C" void _fini(void)
{

}*/


//functions
extern "C" const char* getModuleID()
{
	return "Ask_For_Pass_WIN32";
}

extern "C" const char* getModuleName()
{
	return "Ask for the password through a WIN32 window";
}

extern "C" const char* getNetworkPassword(const char* server, int port, const char* username)
{
	bool res=WIN32_Get_Password("Please key in the password ...", "Please key in the password corresponding to "+toString(username)+"@"+toString(server)+":"+toString(port)+" : ", last_pwd);

	//if everything went fine
	if(res)
	{
		setError("");
		return last_pwd.c_str();
	}
	else
	{
		setError("User pressed cancel");
		return NULL;
	}
}

extern "C" int setNetworkPassword(const char* server, int port, const char* username,  const char* pwd)
{
	return 0;
}

extern "C" const char* getApplicationPassword(const char* application_name, const char* username)
{
	bool res=WIN32_Get_Password("Please key in the password ...", "Please key in the password corresponding to "+toString(username)+"@"+toString(application_name)+" : ", last_pwd);

	//if everything went fine
	if(res)
	{
		setError("");
		return last_pwd.c_str();
	}
	else
	{
		setError("User pressed cancel");
		return NULL;
	}
}

extern "C" int setApplicationPassword(const char* application_name, const char* username,  const char* pwd)
{
	return 0;
}

extern "C" const char* getItem(const char* key)
{
	bool res=WIN32_Get_Password("Please key in the item ...","Please key in the item corresponding to this key("+toString(key)+") : ",last_pwd);

	//if everything went fine
	if(res)
	{
		setError("");
		return last_pwd.c_str();
	}
	else
	{
		setError("User pressed cancel");
		return NULL;
	}
}

extern "C" int setItem(const char* key, const char* item)
{
	return 0;
}

extern "C" const char* getLastError()
{
	return last_error.c_str();
}

/*************************************************************************************************
**************************************************************************************************
*******************************										******************************
*******************************				  WIN32 PART				******************************
*******************************										******************************
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

/*int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	bool res=WIN32_Get_Password("Please key in the password ...", "Please key in the password corresponding to "+toString("mupuf")+"@"+toString("mupuf.fr.nf")+":"+toString(21)+" : ", last_pwd);
	if(res)
		printf("Tout est OK, le pwd est '%s'\n",last_pwd.c_str());
	else
		printf("NOK\n");
	return 0;
}*/

