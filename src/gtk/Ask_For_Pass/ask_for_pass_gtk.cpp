#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>


//local functions
bool GTK_Get_Password(std::string title, std::string label, std::string& pwd);
std::string* last_error()
{
	static std::string last_err;
	return &last_err;
}

void setError(std::string error)
{
	*(last_error())="PPK_Ask_For_Pass_GTK : " + error;
}

//functions
extern "C" const char* getModuleID()
{
	return "Ask_For_Pass_GTK";
}

extern "C" const char* getModuleName()
{
	return "Ask for the password through a GTK window";
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
extern "C" ppk_readFlag readFlagsAvailable()
{
	return ppk_rf_silent;
}

extern "C" ppk_writeFlag writeFlagsAvailable()
{
	return ppk_wf_none;
}


extern "C" unsigned int getPasswordListCount(ppk_password_type type, unsigned int flags)
{	
	return 0;
}

extern "C"  unsigned int getPasswordList(ppk_password_type type, void* pwdList, unsigned int maxModuleCount, unsigned int flags)
{
	return 0;
}

#include <iostream>
extern "C" const char* getNetworkPassword(const char* server, int port, const char* username, unsigned int flags)
{
	static std::string pwd;	
	if((int)(flags&ppk_rf_silent)==0)
	{
		bool res=GTK_Get_Password("Please key in the password ...", "Please key in the password corresponding to "+toString(username)+"@"+toString(server)+":"+toString(port)+" : ", pwd);

		//if everything went fine
		if(res)
		{
			setError("");
			return pwd.c_str();
		}
		else
		{
			setError("User pressed cancel");
			return NULL;
		}
	}
	else
		return NULL;
}

extern "C" int setNetworkPassword(const char* server, int port, const char* username,  const char* pwd, unsigned int flags)
{
	return 0;
}


extern "C" const char* getApplicationPassword(const char* application_name, const char* username, unsigned int flags)
{
	static std::string pwd; 

	if((int)(flags&ppk_rf_silent)==0)
	{
		bool res=GTK_Get_Password("Please key in the password ...", "Please key in the password corresponding to "+toString(username)+"@"+toString(application_name)+" : ", pwd);

		//if everything went fine
		if(res)
		{
			setError("");
			return pwd.c_str();
		}
		else
		{
			setError("User pressed cancel");
			return NULL;
		}
	}
	else
		return NULL;
}

extern "C" int setApplicationPassword(const char* application_name, const char* username,  const char* pwd, unsigned int flags)
{
	return 0;
}

extern "C" const char* getItem(const char* key, unsigned int flags)
{
	static std::string pwd;
	if((int)(flags&ppk_rf_silent)==0)
	{
		bool res=GTK_Get_Password("Please key in the item ...","Please key in the item corresponding to this key("+toString(key)+") : ",pwd);

		//if everything went fine
		if(res)
		{
			setError("");
			return pwd.c_str();
		}
		else
		{
			setError("User pressed cancel");
			return NULL;
		}
	}
	else
		return NULL;
}

extern "C" int setItem(const char* key,  const char* pwd, unsigned int flags)
{
	return 0;
}

extern "C" const char* getLastError()
{
	return last_error()->c_str();
}

//optionnal
std::string* customPrompt()
{
	static std::string msg;
	return &msg;
}

extern "C" enum ppk_boolean setCustomPromptMessage(const char* customMessage)
{
	if(customMessage!=NULL)
		*(customPrompt())=customMessage;
	else
		*(customPrompt())=std::string();

	return PPK_TRUE;
}

/*************************************************************************************************
**************************************************************************************************
*******************************										******************************
*******************************				  GTK PART				******************************
*******************************										******************************
**************************************************************************************************
**************************************************************************************************/

#include <gtk/gtk.h>

bool GTK_Get_Password(std::string title, std::string default_label, std::string& pwd)
{
	bool succeded=false;
	GtkWidget* pBoite;
	GtkWidget* pEntry;
	GtkWidget *pLabel;
	const gchar* sNom;

	//Init GTK
	gtk_init(0, NULL);
	
	//Create the dialog box
	pBoite = gtk_dialog_new_with_buttons(title.c_str(),
		GTK_WINDOW(NULL),
		GTK_DIALOG_MODAL,
		GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
		GTK_STOCK_OK,GTK_RESPONSE_OK,
		NULL);

	//Create the label
	if(*(customPrompt())!=std::string())
		pLabel = gtk_label_new(customPrompt()->c_str());
	else
		pLabel = gtk_label_new(default_label.c_str());
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pBoite)->vbox), pLabel, TRUE, FALSE, 0);

	//Create the editbox
	pEntry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(pEntry), FALSE);
	gtk_entry_set_text(GTK_ENTRY(pEntry), "");
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(pBoite)->vbox), pEntry, TRUE, FALSE, 0);

	//Show the widgets
	gtk_widget_show_all(GTK_DIALOG(pBoite)->vbox);

	//Launch the dialog box and wait for events
	switch (gtk_dialog_run(GTK_DIALOG(pBoite)))
	{
		//User clicked on OK
		case GTK_RESPONSE_OK:
			pwd = gtk_entry_get_text(GTK_ENTRY(pEntry));
			gtk_label_set_text(GTK_LABEL(pLabel), sNom);
			succeded=true;
			break;
		 //User clicked on cancel
		case GTK_RESPONSE_CANCEL:
		case GTK_RESPONSE_NONE:
		default:
			succeded=false;
			break;
	}

	//Destroy the dialog box
	gtk_widget_destroy(pBoite);

	return succeded;
}
