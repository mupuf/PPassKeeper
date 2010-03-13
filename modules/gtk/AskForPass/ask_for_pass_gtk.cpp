#include <ppasskeeper.h>
#include "tokenizer.h"
#include <string>
#include <map>

//Parameters
std::map<std::string, cvariant> parameters;
#define PARAM_IMG_APP "App image"
#define PARAM_IMG_NET "Net image"
#define PARAM_IMG_ITEM "Item image"
#define PARAM_WINDOW_CAPTION "Window's caption"
#define PARAM_MAIN_TEXT "Main text"

#define PARAM_IMG_APP_DEFAULT ""
#define PARAM_IMG_NET_DEFAULT ""
#define PARAM_IMG_ITEM_DEFAULT ""
#define PARAM_WINDOW_CAPTION_DEFAULT "Please key in the item ..."
#define PARAM_MAIN_TEXT_DEFAULT "Please key in the item corresponding to %1 :"


//local functions
bool GTK_Get_Password(std::string title, std::string label, std::string& pwd, const char* icon=NULL);

//functions
extern "C" void constructor()
{
	//Set parameters's default value
	parameters[PARAM_IMG_APP]=cvariant_from_string(PARAM_IMG_APP_DEFAULT);
	parameters[PARAM_IMG_NET]=cvariant_from_string(PARAM_IMG_NET_DEFAULT);
	parameters[PARAM_IMG_ITEM]=cvariant_from_string(PARAM_IMG_ITEM_DEFAULT);
	parameters[PARAM_WINDOW_CAPTION]=cvariant_from_string(PARAM_WINDOW_CAPTION_DEFAULT);
	parameters[PARAM_MAIN_TEXT]=cvariant_from_string(PARAM_MAIN_TEXT_DEFAULT);
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
	int len = toReplace.size();
	unsigned int pos;
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
		
		const char* icon=NULL;
		switch(ppk_get_entry_type(entry))
		{
			case ppk_application:
				icon=cvariant_get_string(parameters[PARAM_IMG_APP]);
				break;
				
			case ppk_network:
				icon=cvariant_get_string(parameters[PARAM_IMG_NET]);
				break;
			
			case ppk_item:
				icon=cvariant_get_string(parameters[PARAM_IMG_ITEM]);
				break;
		}

		std::string label;
		if(label_pattern)
			label=str_replace(label_pattern, "%1", key);
		else
			label=str_replace(PARAM_MAIN_TEXT_DEFAULT, "%1", key);

		std::string pwd;
		bool res=GTK_Get_Password(title, label.c_str(), pwd, icon);

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

const ppk_settings_group ppk_settings_display = { "Display", "Display-related parameters" };
const ppk_settings_group ppk_settings_custom_texts = { "Custom Texts", "Customize the texts shown by AskForPass_GTK" };
extern "C" const ppk_proto_param** availableParameters()
{
	static ppk_proto_param img_app;
	img_app.expected_type=cvariant_string;
	img_app.name=PARAM_IMG_APP;
	img_app.file_filter="Images (*.bmp *.gif *.jpg *.jpeg *.mng *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm *.svg);;All files (*.*)";
	img_app.help_text="The image you would like to be displayed when you're ask to enter a application password";
	img_app.default_value=cvariant_from_string("");
	img_app.group=&ppk_settings_display;

	static ppk_proto_param img_net;
	img_net.expected_type=cvariant_string;
	img_net.file_filter="Images (*.bmp *.gif *.jpg *.jpeg *.mng *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm *.svg);;All files (*.*)";
	img_net.name=PARAM_IMG_NET;
	img_net.help_text="The image you would like to be displayed when you're ask to enter a network password";
	img_net.default_value=cvariant_from_string("");
	img_net.group=&ppk_settings_display;

	static ppk_proto_param img_item;
	img_item.expected_type=cvariant_string;
	img_item.file_filter="Images (*.bmp *.gif *.jpg *.jpeg *.mng *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm *.svg);;All files (*.*)";
	img_item.name=PARAM_IMG_ITEM;
	img_item.help_text="The image you would like to be displayed when you're ask to enter an item's value";
	img_item.default_value=cvariant_from_string("");
	img_item.group=&ppk_settings_display;
	
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

	static const ppk_proto_param* params[]={&img_app, &img_net, &img_item, &win_cap, &win_text, NULL};

	return params;
}

#include <stdio.h>
extern "C" void setParam(const char* paramName, const cvariant value)
{
	std::string key(paramName);

	if(key == PARAM_IMG_APP)
	{
		if(cvariant_get_type(value)==cvariant_string)
			parameters[PARAM_IMG_APP]=value;
		else
			printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
	}
	else if(key == PARAM_IMG_NET)
	{
		if(cvariant_get_type(value)==cvariant_string)
			parameters[PARAM_IMG_NET]=value;
		else
			printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
	}
	else if(key == PARAM_IMG_ITEM)
	{
		if(cvariant_get_type(value)==cvariant_string)
			parameters[PARAM_IMG_ITEM]=value;
		else
			printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
	}
	else if(key == PARAM_WINDOW_CAPTION)
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
bool GTK_Get_Password(std::string title, std::string default_label, std::string& pwd, const char* icon)
{
	bool succeeded=false;
	GtkWidget* dialog;
	GtkWidget* entry;
	GtkWidget* image;
	GtkWidget* label;
	GtkWidget* hbox;
	GtkWidget* vbox;

	//Init GTK
	gtk_init(0, NULL);

	//Create the dialog box
	dialog = gtk_dialog_new_with_buttons(title.c_str(),
		GTK_WINDOW(NULL),
		GTK_DIALOG_MODAL,
		GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,
		GTK_STOCK_OK,GTK_RESPONSE_OK,
		NULL);

	//Create container widgets
	hbox = gtk_hbox_new(false, 0);
	vbox = gtk_vbox_new(false, 0);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox), hbox, TRUE, FALSE, 0);

	//Create the image
 	if(icon!=NULL)
		image = gtk_image_new_from_file(icon);
	else
		image = gtk_image_new_from_stock(GTK_STOCK_DIALOG_QUESTION, GTK_ICON_SIZE_DIALOG);
	gtk_box_pack_start(GTK_BOX(hbox), image, TRUE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, TRUE, FALSE, 0);

	//Create the label
	if(*(customPrompt())!=std::string())
		label = gtk_label_new(customPrompt()->c_str());
	else
		label = gtk_label_new(default_label.c_str());
	gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, FALSE, 0);

	//Create the editbox
	entry = gtk_entry_new();
	gtk_entry_set_visibility(GTK_ENTRY(entry), FALSE);
	gtk_entry_set_text(GTK_ENTRY(entry), "");
	gtk_box_pack_start(GTK_BOX(vbox), entry, TRUE, FALSE, 0);

	//Show the widgets
	gtk_widget_show_all(GTK_DIALOG(dialog)->vbox);

	//Make the dialog sensitive
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);

	//Launch the dialog box and wait for events
	switch (gtk_dialog_run(GTK_DIALOG(dialog)))
	{
		//User clicked on OK
		case GTK_RESPONSE_OK:
			pwd = gtk_entry_get_text(GTK_ENTRY(entry));
			succeeded=true;
			break;
		 //User clicked on cancel
		case GTK_RESPONSE_CANCEL:
		case GTK_RESPONSE_NONE:
		default:
			succeeded=false;
			break;
	}

	//Destroy the dialog box
	gtk_widget_destroy(dialog);

	return succeeded;
}

/*int main()
{
	std::string pwd;
	GTK_Get_Password("poulpe de titre", "valeur du titre !", pwd);
	return 0;
}*/
