#include "ppasskeeper.h"
#include "tokenizer.h"
#include <string>
#include <map>
#include <stdio.h>

#include <qstring.h>

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
ppk_error Qt_Get_Password(QString title, QString label, QString icon, std::string& pwd);

//functions
extern "C" void constructor()
{
	//Set parameters's default value
	parameters[PARAM_IMG_APP]=cvariant_from_string(PARAM_IMG_APP_DEFAULT);
	parameters[PARAM_IMG_NET]=cvariant_from_string(PARAM_IMG_NET_DEFAULT);
	parameters[PARAM_IMG_ITEM]=cvariant_from_string(PARAM_IMG_ITEM_DEFAULT);
	parameters[PARAM_WINDOW_CAPTION]=cvariant_from_string(PARAM_WINDOW_CAPTION_DEFAULT);
	parameters[PARAM_MAIN_TEXT_DEFAULT]=cvariant_from_string(PARAM_MAIN_TEXT_DEFAULT);
}

extern "C" void destructor()
{
	//Nothing to free
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
		QString label=QString::fromUtf8(cvariant_get_string(parameters[PARAM_MAIN_TEXT])).arg(QString::fromUtf8(text.c_str()));

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

const ppk_settings_group ppk_settings_display = { "Display", "Display-related parameters" };
const ppk_settings_group ppk_settings_custom_texts = { "Custom Texts", "Customize the texts shown by AskForPass_Qt" };

extern "C" const ppk_proto_param** availableParameters()
{
	static ppk_proto_param img_app;
	img_app.expected_type=cvariant_string;
	img_app.name=PARAM_IMG_APP;
	img_app.help_text="The image you would like to be displayed when you're ask to enter a application password";
	img_app.default_value=cvariant_from_string("");
	img_app.group=&ppk_settings_display;

	static ppk_proto_param img_net;
	img_net.expected_type=cvariant_string;
	img_net.name=PARAM_IMG_NET;
	img_net.help_text="The image you would like to be displayed when you're ask to enter a network password";
	img_net.default_value=cvariant_from_string("");
	img_net.group=&ppk_settings_display;

	static ppk_proto_param img_item;
	img_item.expected_type=cvariant_string;
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
public:
	static bool getPassword(const QString &title, const QString &label, const QString& icon, std::string &pwd)
	{
		PasswordDialog dialog(title, label, icon);
		bool ok = dialog.exec() == QDialog::Accepted;
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
		if(icon!=QString() && pixmap.load(icon))
		{}
		else
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

ppk_error Qt_Get_Password(QString title, QString label, QString icon, std::string& pwd)
{
	bool ok;

	//If there is a custom text
	if(*(customPrompt())!=std::string())
		label=QString::fromUtf8(customPrompt()->c_str());

	//Init Qt if it has not already been done
	if(QApplication::instance()==0)
	{
		//create the instance
		int argc;
		QApplication app(argc,NULL);

		//Retrieve the password
		ok = PasswordDialog::getPassword(title, label, icon, pwd);
	}
	else
		ok = PasswordDialog::getPassword(title, label, icon, pwd);

	//if user pressed cancel
	if(ok)
		return PPK_OK;
	else
		return PPK_USER_CANCELLED;
}

