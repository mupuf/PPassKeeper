#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>
#include <stdio.h>

//local functions
std::string* last_error()
{
	static std::string last_err;
	return &last_err;
}
bool Qt_Get_Password(std::string title, std::string label, std::string& pwd);
void setError(std::string error)
{
	*(last_error())="PPK_AskForPass_Qt : " + error;
}

//functions
extern "C" const char* getModuleID()
{
	return "AskForPass_Qt";
}

extern "C" const char* getModuleName()
{
	return "Ask for the password through a Qt window";
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
	return ppk_wf_none|ppk_wf_none;
}

extern "C" unsigned int listingFlagsAvailable()
{
	return ppk_lf_none|ppk_lf_none;
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

		bool res=Qt_Get_Password("Please key in the item ...","Please key in the item corresponding to " + text + " : ",pwd);

		//if everything went fine
		if(res)
		{
			setError("");
			edata->type=ppk_string;
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
			return (unsigned int)-1;
		case ppk_blob:
			return 0;
	}

	return 0;
}

ppk_settings_group ppk_settings_basic = { "Basic", "Basic settings" };
ppk_settings_group ppk_settings_network = { "Network", "Network-related parameters" };
ppk_settings_group ppk_settings_display = { "Display", "Display-related parameters" };
ppk_settings_group ppk_settings_security = { "Security", "Security-related parameters" };

extern "C" ppk_proto_param* availableParameters()
{
	static ppk_proto_param tst_param;
	tst_param.expected_type=cvariant_string;
	tst_param.name="test_param";
	tst_param.help_text="Supposed to be the help string. Sorry ...";
	tst_param.default_value=cvariant_from_string("default_value for 'test_param'");
	tst_param.group=&ppk_settings_basic;

	static ppk_proto_param tst_param2;
	tst_param2.expected_type=cvariant_int;
	tst_param2.name="test_param2";
	tst_param2.help_text="Supposed to be the second help string. Sorry ...";
	tst_param2.default_value=cvariant_from_int(123456789);
	tst_param2.group=&ppk_settings_basic;

	static ppk_proto_param proto_null;
	proto_null.expected_type=cvariant_none;
	proto_null.group=&ppk_settings_basic;

	static ppk_proto_param params[]={tst_param, tst_param2, proto_null};

	return params;
}

extern "C" void setParam(const char* paramName, const cvariant value)
{
	std::string key(paramName);

	if(key == "test_param")
	{
		if(cvariant_get_type(value)==cvariant_string)
			printf("Set Param : paramName(%s), value(%s)\n", paramName, cvariant_get_string(value));
		else
			printf("Wrong data type for the parameter '%s' !\n", paramName);
	}
	else if(key == "test_param2")
	{
		if(cvariant_get_type(value)==cvariant_int)
			printf("Set Param : paramName(%s), value(%i)\n", paramName, cvariant_get_int(value));
		else
			printf("Wrong data type for the parameter '%s' !\n", paramName);
	}
	else
		printf("	Unknown param !!\n");
}

extern "C" const char* getLastError()
{
	return last_error()->c_str();
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

#include <qstring.h>

#include <qdialog.h>
#include <qboxlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdialogbuttonbox.h>
#include <qstyle.h>

class PasswordDialog : public QDialog
{
public:
	static bool getPassword(const std::string &title, const std::string &label, std::string &pwd)
	{
		PasswordDialog dialog(title, label);
		bool ok = dialog.exec() == QDialog::Accepted;
		pwd=dialog.pwdEdit->text().toStdString();
		return ok;
	}

private:
	PasswordDialog(const std::string &title, const std::string &label) : QDialog()
	{
		setWindowTitle(QString::fromStdString(title));

		QVBoxLayout *layout = new QVBoxLayout;

		QString qlabel = QString::fromStdString(label);
		qlabel.prepend("<b>");
		qlabel.append("</b>");

		QHBoxLayout *hlayout = new QHBoxLayout;
		QLabel *iconLabel = new QLabel;
		iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		iconLabel->setPixmap(qApp->style()->standardIcon(QStyle::SP_MessageBoxQuestion).pixmap(64, 64));
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

bool Qt_Get_Password(std::string title, std::string label, std::string& pwd)
{
	bool ok;

	//If there is a custom
	if(*(customPrompt())!=std::string())
		label=customPrompt()->c_str();

	//Init Qt if it has not already been done
	if(QApplication::instance()==0)
	{
		//create the instance
		int argc;
		QApplication app(argc,NULL);

		//Retrieve the password
		ok = PasswordDialog::getPassword(title, label, pwd);
	}
	else
		ok = PasswordDialog::getPassword(title, label, pwd);

	//if user pressed cancel
	if(!ok)
		setError("User pressed <Cancel>");

	//Tell everything went fine
	return ok;
}

