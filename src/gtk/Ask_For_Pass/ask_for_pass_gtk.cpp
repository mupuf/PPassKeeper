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

extern "C" const char* getNetworkPassword(const char* server, int port, const char* username)
{
	static std::string pwd;	

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

extern "C" int setNetworkPassword(const char* server, int port, const char* username,  const char* pwd)
{
	return 0;
}

extern "C" const char* getApplicationPassword(const char* application_name, const char* username)
{
	static std::string pwd;

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

extern "C" int setApplicationPassword(const char* application_name, const char* username,  const char* pwd)
{
	return 0;
}

extern "C" const char* getItem(const char* key)
{
	static std::string pwd;

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

extern "C" int setItem(const char* key, const char* item)
{
	return 0;
}

extern "C" const char* getNetworkPassword_silent(const char* server, int port, const char* username)
{
	return NULL;
}

extern "C" int setNetworkPassword_silent(const char* server, int port, const char* username,  const char* pwd)
{
	return 0;
}

extern "C" const char* getApplicationPassword_silent(const char* application_name, const char* username)
{
	return NULL;
}

extern "C" int setApplicationPassword_silent(const char* application_name, const char* username,  const char* pwd)
{
	return 0;
}

extern "C" const char* getItem_silent(const char* key)
{
	return NULL;
}

extern "C" int setItem_silent(const char* key, const char* item)
{
	return 0;
}

extern "C" const char* getLastError()
{
	return last_error()->c_str();
}

/*************************************************************************************************
**************************************************************************************************
*******************************										******************************
*******************************				  GTK PART				******************************
*******************************										******************************
**************************************************************************************************
**************************************************************************************************/

#include <gtk/gtk.h>

bool GTK_Get_Password(std::string title, std::string label, std::string& pwd)
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
		GTK_STOCK_OK,GTK_RESPONSE_OK,
		GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
		NULL);

	//Create the label
	pLabel = gtk_label_new(label.c_str());
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
