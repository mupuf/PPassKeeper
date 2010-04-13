#include "ppasskeeper.h"
#include "tokenizer.h"
#include <string>
#include <map>
#include <stdio.h>

#include <qstring.h>

//Parameters
std::map<std::string, cvariant> parameters;
std::map<std::string, ppk_settings_group*> params_group;
std::map<std::string, ppk_proto_param*> proto_params;
ppk_proto_param** availParams;
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
ppk_error Qt_Get_Password(QString title, QString label, QString icon, std::string& pwd);

//functions
extern "C" void constructor()
{
	availParams=NULL;
	
	//Create the group categories
	ppk_settings_group *ppk_settings_display, *ppk_settings_custom_texts;
	
	ppk_settings_display=ppk_settings_group_create("Display", "Display-related parameters");
	ppk_settings_custom_texts=ppk_settings_group_create("Custom Texts", "Customize the texts shown by AskForPass_GTK");
	
	params_group["Display"]=ppk_settings_display;
	params_group["Custom Texts"]=ppk_settings_custom_texts;
	
	
	//Create the parameters' prototypes
	ppk_proto_param *img_app, *img_net, *img_item, *win_cap, *win_text;
	img_app=ppk_param_proto_create_file(PARAM_IMG_APP,
										"The image you would like to be displayed when you're ask to enter a application password",
										"", //Default value
										ppk_settings_display,
										"Images (*.bmp *.gif *.jpg *.jpeg *.mng *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm *.svg);;All files (*.*)");
	proto_params[PARAM_IMG_APP]=img_app;
	
	img_net=ppk_param_proto_create_file(PARAM_IMG_NET,
										"The image you would like to be displayed when you're ask to enter a network password",
										"", //Default value
										ppk_settings_display,
										"Images (*.bmp *.gif *.jpg *.jpeg *.mng *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm *.svg);;All files (*.*)");
	proto_params[PARAM_IMG_NET]=img_net;
	
	img_item=ppk_param_proto_create_file(PARAM_IMG_ITEM,
										"The image you would like to be displayed when you're ask to enter an item's value",
										"", //Default value
										ppk_settings_display,
										"Images (*.bmp *.gif *.jpg *.jpeg *.mng *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm *.svg);;All files (*.*)");
	proto_params[PARAM_IMG_ITEM]=img_item;
	
	win_cap=ppk_param_proto_create_string(PARAM_WINDOW_CAPTION,
										"The caption you would like to see",
										PARAM_WINDOW_CAPTION_DEFAULT,
										ppk_settings_custom_texts);
	proto_params[PARAM_WINDOW_CAPTION]=win_cap;
	
	win_text=ppk_param_proto_create_string(PARAM_MAIN_TEXT,
										"The main text you would like to shown.\nUse %1 were you actually want the key to be shown.",
										PARAM_MAIN_TEXT_DEFAULT,
										ppk_settings_custom_texts);
	proto_params[PARAM_MAIN_TEXT]=win_text;
	
	//Get a list of available parameters
	availParams=new ppk_proto_param*[proto_params.size()+1];
	int i=0;
	std::map<std::string, ppk_proto_param*>::const_iterator itr;
	for(itr = proto_params.begin(); itr != proto_params.end(); ++itr, ++i)
		availParams[i]=itr->second;
	availParams[proto_params.size()]=NULL;
	
	//Set parameters's default value
	parameters[PARAM_IMG_APP]=cvariant_from_string(PARAM_IMG_APP_DEFAULT);
	parameters[PARAM_IMG_NET]=cvariant_from_string(PARAM_IMG_NET_DEFAULT);
	parameters[PARAM_IMG_ITEM]=cvariant_from_string(PARAM_IMG_ITEM_DEFAULT);
	parameters[PARAM_WINDOW_CAPTION]=cvariant_from_string(PARAM_WINDOW_CAPTION_DEFAULT);
	parameters[PARAM_MAIN_TEXT]=cvariant_from_string(PARAM_MAIN_TEXT_DEFAULT);
}

extern "C" void destructor()
{
	//Free the param prototypes
	std::map<std::string, ppk_proto_param*>::const_iterator itr;
	for(itr = proto_params.begin(); itr != proto_params.end(); ++itr)
		ppk_param_proto_free(itr->second);
	
	//Free the setting groups
	std::map<std::string, ppk_settings_group*>::const_iterator itr2;
	for(itr2 = params_group.begin(); itr2 != params_group.end(); ++itr2)
		ppk_settings_group_free(itr2->second);

	//Free the list of parameter
	delete[] availParams;

	//Free the parameters
	std::map<std::string, cvariant>::const_iterator itr3;
	for(itr3 = parameters.begin(); itr3 != parameters.end(); ++itr3)
		cvariant_free(itr3->second);
}

extern "C" const char* getModuleID()
{
	return "AskForPass_Qt";
}

extern "C" const char* getModuleName()
{
	return "Ask for the password (Qt window)";
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

extern "C" ppk_error getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
{
	if((int)(flags&ppk_rf_silent)==0)
	{
		char key[101];
		if(ppk_get_key(entry, key, sizeof(key))==PPK_FALSE)
			return PPK_UNKNOWN_ENTRY_TYPE;
		std::string text=key;

		QString icon;
		QString title=QString::fromUtf8(cvariant_get_string(parameters[PARAM_WINDOW_CAPTION]));
		QString label=QString::fromUtf8(cvariant_get_string(parameters[PARAM_MAIN_TEXT]));
		
		if(!label.contains(QString::fromUtf8("%1")))
			label=QString::fromUtf8(PARAM_MAIN_TEXT_DEFAULT);
		
		label=label.arg(QString::fromUtf8(text.c_str()));

		if(entry->type==ppk_application)
			icon=QString::fromUtf8(cvariant_get_string(parameters[PARAM_IMG_APP]));
		else if(entry->type==ppk_network)
			icon=QString::fromUtf8(cvariant_get_string(parameters[PARAM_IMG_NET]));
		else if(entry->type==ppk_item)
			icon=QString::fromUtf8(cvariant_get_string(parameters[PARAM_IMG_ITEM]));

		std::string pwd;
		ppk_error res=Qt_Get_Password(title, label, icon, pwd);

		//if everything went fine
		if(res==PPK_OK)
		{
			*edata=ppk_string_data_new(pwd.c_str());
			return res;
		}
		else
			return res;
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

extern "C" const ppk_proto_param** availableParameters()
{
	return const_cast<const ppk_proto_param**>(availParams);
}

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

//Optionnal
std::string& customPrompt()
{
	static std::string msg;
	return msg;
}

extern "C" ppk_boolean setCustomPromptMessage(const char* customMessage)
{
	if(customMessage!=NULL)
		customPrompt()=customMessage;
	else
		customPrompt()=std::string();

	return PPK_TRUE;
}

/*************************************************************************************************
**************************************************************************************************
*******************************                                     ******************************
*******************************               QT PART               ******************************
*******************************                                     ******************************
**************************************************************************************************
**************************************************************************************************/

#include <qapplication.h>
#include <qinputdialog.h>

#include <qdialog.h>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdialogbuttonbox.h>
#include <qstyle.h>

class PasswordDialog : public QDialog
{
	static QApplication* qapp;

public:
	static bool getPassword(const QString &title, const QString &label, const QString& icon, std::string &pwd)
	{
		/*                                  *****************
									 * !! Warning !! *
									 *****************

		If qApp!=NULL, the QApplication instance already existing will be reused.
		This will "work" as long as the main application doesn't exit while the user is being prompted.
		I haven't found any other solution.
		*/
		if(qApp==NULL)
			qapp=new QApplication(0, NULL);

		//Fetch the password
		PasswordDialog dialog(title, label, icon);
		bool ok = dialog.exec() == QDialog::Accepted;

		//Get the password
		pwd=dialog.pwdEdit->text().toStdString();

		return ok;
	}

private:
	PasswordDialog(const QString &title, const QString &label, const QString& icon=QString()) : QDialog()
	{
		setWindowTitle(title);

		QVBoxLayout *layout = new QVBoxLayout;

		QString qlabel = label;
		qlabel.prepend(QString::fromAscii("<b>"));
		qlabel.append(QString::fromAscii("</b>"));

		QPixmap pixmap;
		if(icon==QString() || !pixmap.load(icon))
			pixmap=qApp->style()->standardIcon(QStyle::SP_MessageBoxQuestion).pixmap(32, 32);

		QHBoxLayout *hlayout = new QHBoxLayout;
		QLabel *iconLabel = new QLabel;
		iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		iconLabel->setPixmap(pixmap);
		hlayout->addWidget(iconLabel);
		hlayout->addSpacing(12);
		QLabel *textLabel = new QLabel(qlabel);
		textLabel->setMinimumWidth(200);
		textLabel->setWordWrap(true);
		hlayout->addWidget(textLabel);
		layout->addLayout(hlayout);

		layout->addStretch();

		pwdEdit = new QLineEdit();
		pwdEdit->setEchoMode(QLineEdit::Password);
		layout->addWidget(pwdEdit);

		layout->addStretch();

		QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal);
		layout->addWidget(box);

		connect(box, SIGNAL(accepted()), this, SLOT(accept()));
		connect(box, SIGNAL(rejected()), this, SLOT(reject()));

		setLayout(layout);
	}

	QLineEdit *pwdEdit;
};

QApplication* PasswordDialog::qapp=NULL;

ppk_error Qt_Get_Password(QString title, QString label, QString icon, std::string& pwd)
{
	bool ok;

	//If there is a custom text
	if(customPrompt()!=std::string())
		label=QString::fromUtf8(customPrompt().c_str());

	//Get the password
	ok = PasswordDialog::getPassword(title, label, icon, pwd);

	//if user pressed cancel
	if(ok)
		return PPK_OK;
	else
		return PPK_USER_CANCELLED;
}

