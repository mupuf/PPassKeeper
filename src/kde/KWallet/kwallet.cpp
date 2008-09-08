#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include <string>

#include <kwallet.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

//local functions
std::string* last_error()
{
	static std::string last_err;
	return &last_err;
}

void setError(std::string error)
{
	*(last_error())="PPK_KWallet : " + error;
}

KWallet::Wallet* openWallet()
{
	if(KWallet::Wallet::isEnabled())
	{
		KWallet::Wallet* wallet=KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),0);

		//if the wallet is oppened
		if(wallet!=NULL)
		{
			return wallet;
		}
		else
		{
			setError("getPwd : openWallet failed");
			return NULL;
		}
	}
	else
	{
		setError("getPwd : KWallet is not available");
		return NULL;
	}
}

const char* _getPassword(const char* key)
{
	KWallet::Wallet* wallet=openWallet();
	if(wallet!=NULL)
	{
		//Get the password
		static QString pwd;	
		if(wallet->readPassword(key,pwd)==0)
			return pwd.toAscii().data();
		else
		{
			setError("getPwd : wallet->readPassword failed, key="+toString(key));
			return NULL;
		}
	}
	else
		return NULL;
}

bool _setPassword(const char* key, const char* pwd)
{
	KWallet::Wallet* wallet=openWallet();
	if(wallet!=NULL)
	{
		//Set the password
		if(wallet->writePassword(key,pwd)==0)
			return true;
		else
		{
			setError("getPwd : wallet->writePassword failed, key="+toString(key));
			return false;
		}
	}
	else
		return false;
}

const char* getPassword(const char* key)
{
	//if KApplication is already running, don't start another one
	if(KApplication::instance()==NULL)
	{
		//Init KDE Application
		KAboutData about(QByteArray("ppasskeeper-kwallet"),QByteArray("ppasskeeper-kwallet"),KLocalizedString(),QByteArray("1.0"));	
		KCmdLineArgs::init(&about);
		KApplication app(false);

		//Ask the password and return it
		return _getPassword(key);
	}
	else
		return _getPassword(key);
}

bool setPassword(const char* key, const char* pwd)
{
	//if KApplication is already running, don't start another one
	if(KApplication::instance()==NULL)
	{
		//Init KDE Application
		KAboutData about(QByteArray("ppasskeeper-kwallet"),QByteArray("ppasskeeper-kwallet"),KLocalizedString(),QByteArray("1.0"));	
		KCmdLineArgs::init(&about);
		KApplication app(false);

		//Ask the password and return it
		return _setPassword(key, pwd);
	}
	else
		return _setPassword(key, pwd);
}

std::string generateNetworkKey(std::string server, int port, std::string username)
{
	return "ppasskeeper_network://"+username+"@"+server+":"+toString(port);
}

std::string generateApplicationKey(std::string application_name, std::string username)
{
	return "ppasskeeper_app://"+username+"@"+application_name;
}

std::string generateItemKey(std::string key)
{
	return "ppasskeeper_item://"+key;
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
	return "KWallet";
}

extern "C" const char* getModuleName()
{
	return "KWallet - Store it into KDE's Wallet";
}

extern "C" const int getABIVersion()
{
	return 1;
}

//Non-Silent operations
extern "C" const char* getNetworkPassword(const char* server, int port, const char* username)
{
	return getPassword(generateNetworkKey(server, port, username).c_str());
}

extern "C" int setNetworkPassword(const char* server, int port, const char* username,  const char* pwd)
{
	return setPassword(generateNetworkKey(server, port, username).c_str(), pwd)?0:-1;
}

extern "C" const char* getApplicationPassword(const char* application_name, const char* username)
{
	return getPassword(generateApplicationKey(application_name, username).c_str());
}

extern "C" int setApplicationPassword(const char* application_name, const char* username,  const char* pwd)
{
	return setPassword(generateApplicationKey(application_name, username).c_str(), pwd)?0:-1;
}

extern "C" const char* getItem(const char* key)
{
	return getPassword(generateItemKey(key).c_str());
}

extern "C" int setItem(const char* key, const char* item)
{
	return setPassword(generateItemKey(key).c_str(), item)?0:-1;
}

//Silent operations
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
