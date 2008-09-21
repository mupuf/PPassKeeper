#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>

//local functions
std::string* last_error()
{
	static std::string last_err;
	return &last_err;
}
bool QT_Get_Password(std::string title, std::string label, std::string& pwd);
void setError(std::string error)
{
	*(last_error())="PPK_Ask_For_Pass_QT : " + error;
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
	return "Ask_For_Pass_QT";
}

extern "C" const char* getModuleName()
{
	return "Ask for the password through a QT window";
}

extern "C" const int getABIVersion()
{
	return 1;
}

extern "C" ppk::boolean isWritable()
{
	return ppk::BFALSE;
}

extern "C" ppk::security_level securityLevel(const char* module_id)
{
	return ppk::sec_perfect;
}

//Get available flags
extern "C" ppk::readFlag readFlagsAvailable()
{
	return ppk::rd_silent;
}

extern "C" ppk::writeFlag writeFlagsAvailable()
{
	return ppk::wt_none;
}


extern "C" unsigned int getPasswordListCount(ppk::password_type type)
{	
	return 0;
}

extern "C"  unsigned int getPasswordList(ppk::password_type type, void* pwdList, unsigned int maxModuleCount)
{
	return 0;
}

extern "C" const char* getNetworkPassword(const char* server, int port, const char* username, unsigned int flags)
{
	static std::string pwd;	
	
	if(flags&ppk::rd_silent==0)
	{
		bool res=QT_Get_Password("Please key in the password ...", "Please key in the password corresponding to "+toString(username)+"@"+toString(server)+":"+toString(port)+" : ", pwd);

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

	if(flags&ppk::rd_silent==0)
	{
		bool res=QT_Get_Password("Please key in the password ...", "Please key in the password corresponding to "+toString(username)+"@"+toString(application_name)+" : ", pwd);

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

	if(flags&ppk::rd_silent==0)
	{
		bool res=QT_Get_Password("Please key in the item ...","Please key in the item corresponding to this key("+toString(key)+") : ",pwd);

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

extern "C" int setItem(const char* key, const char* item, unsigned int flags)
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
*******************************				  QT PART				******************************
*******************************										******************************
**************************************************************************************************
**************************************************************************************************/

#include <qapplication.h>
#include <qinputdialog.h>

#include <qstring.h>

bool QT_Get_Password(std::string title, std::string label, std::string& pwd)
{
	bool ok;

	//Init Qt if it has not already been done
	if(QApplication::instance()==0)
	{
		//create the instance
		QApplication app(0,NULL);

		//Retrieve the password
		pwd=QInputDialog::getText(NULL,title.c_str(),label.c_str(),QLineEdit::Normal,"",&ok).toStdString();
	}
	else
		pwd=QInputDialog::getText(NULL,title.c_str(),label.c_str(),QLineEdit::Normal,"",&ok).toStdString(); //Retrieve the password

	//if user pressed cancel
	if(!ok)
		setError("User pressed <Cancel>");

	//Tell everything went fine
	return ok;
}

