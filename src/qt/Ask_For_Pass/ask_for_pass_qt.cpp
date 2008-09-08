#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>

//Local definitions
std::string last_error;
std::string last_pwd;

//local functions
bool QT_Get_Password(std::string title, std::string label, std::string& pwd);
void setError(std::string error)
{
	last_error="PPK_Ask_For_Pass_QT : " + error;
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

extern "C" const char* getNetworkPassword(const char* server, int port, const char* username)
{
	bool res=QT_Get_Password("Please key in the password ...", "Please key in the password corresponding to "+toString(username)+"@"+toString(server)+":"+toString(port)+" : ", last_pwd);

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
	bool res=QT_Get_Password("Please key in the password ...", "Please key in the password corresponding to "+toString(username)+"@"+toString(application_name)+" : ", last_pwd);

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
	bool res=QT_Get_Password("Please key in the item ...","Please key in the item corresponding to this key("+toString(key)+") : ",last_pwd);

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
	return last_error.c_str();
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

