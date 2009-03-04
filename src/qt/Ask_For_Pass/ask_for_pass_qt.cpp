#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>

//local functions
std::string* last_error()
{
	static std::string last_err;
	return &last_err;
}
bool Qt_Get_Password(std::string title, std::string label, std::string& pwd);
void setError(std::string error)
{
	*(last_error())="PPK_Ask_For_Pass_Qt : " + error;
}

//functions
extern "C" const char* getModuleID()
{
	return "Ask_For_Pass_Qt";
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
*******************************										******************************
*******************************				  QT PART				******************************
*******************************										******************************
**************************************************************************************************
**************************************************************************************************/

#include <qapplication.h>
#include <qinputdialog.h>

#include <qstring.h>

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
		QApplication app(0,NULL);

		//Retrieve the password
		pwd=QInputDialog::getText(NULL,title.c_str(),label.c_str(),QLineEdit::Password,"",&ok).toStdString();
	}
	else
		pwd=QInputDialog::getText(NULL,title.c_str(),label.c_str(),QLineEdit::Password,"",&ok).toStdString(); //Retrieve the password

	//if user pressed cancel
	if(!ok)
		setError("User pressed <Cancel>");

	//Tell everything went fine
	return ok;
}

