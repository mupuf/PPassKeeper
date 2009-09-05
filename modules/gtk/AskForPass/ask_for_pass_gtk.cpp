#include <ppasskeeper.h>
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
bool GTK_Get_Password(std::string title, std::string label, std::string& pwd);

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
	return "AskForPass_GTK";
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

extern "C" ppk_error getEntryList(unsigned int entry_types, ppk_entry*** entryList, size_t* nbEntries, unsigned int flags)
{
	return PPK_UNSUPPORTED_METHOD;
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

#include "ppasskeeper/data.h"
extern "C" ppk_error getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
{
	if((int)(flags&ppk_rf_silent)==0)
	{
		char key[101];
		if(ppk_get_key(entry, key, sizeof(key)-1)==PPK_FALSE)
			return PPK_UNKNOWN_ENTRY_TYPE;

		const char* title=cvariant_get_string(parameters[PARAM_WINDOW_CAPTION]);
		const char* label_pattern=cvariant_get_string(parameters[PARAM_MAIN_TEXT]);
		std::string label=str_replace(label_pattern, "%1", key);

		std::string pwd;
		bool res=GTK_Get_Password(title, label.c_str(), pwd);

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
	return PPK_FALSE;
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

const ppk_settings_group ppk_settings_custom_texts = { "Custom Texts", "Customize the texts shown by AskForPass_GTK" };
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
