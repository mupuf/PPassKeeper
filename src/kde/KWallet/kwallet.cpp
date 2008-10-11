#include "../../ppasskeeper-module.h"
#include "../../tokenizer.h"
#include "list_pwd.h"
#include <string>
#include <iostream>

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

extern "C" ppk_boolean isWritable()
{
	return PPK_TRUE;
}

extern "C" ppk_security_level securityLevel(const char* module_id)
{
	return ppk_sec_safe;
}

//Get available flags
extern "C" ppk_readFlag readFlagsAvailable()
{
	return ppk_rf_silent;
}

extern "C" ppk_writeFlag writeFlagsAvailable()
{
	return ppk_wf_silent;
}

KWallet::Wallet* openWallet(unsigned int flags)
{
	if(KWallet::Wallet::isEnabled())
	{
		KWallet::Wallet* wallet=NULL;
	
		//OPen the wallet only if it won't annoy people who don't want to be prompted
		if(KWallet::Wallet::isOpen(KWallet::Wallet::NetworkWallet()) || (int)(flags&ppk_wf_silent)==0)
		{
			wallet=KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),0);

			//if the wallet is oppened
			if(wallet!=NULL)
			{
				return wallet;
			}
			else
			{
				setError("openWallet : openWallet failed");
				return NULL;
			}
		}
		else
		{
			setError("openWallet : openWallet was not performed because it wasn't silent to do so.");
			return NULL;
		}
	}
	else
	{
		setError("openWallet : KWallet is not available");
		return NULL;
	}
}

const char* _getPassword(const char* key, unsigned int flags)
{
	static QString pwd;

	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		//Get the password
		if(wallet->readPassword(key,pwd)==0)
			return pwd.toAscii().data();
		else
		{
			setError("getPwd : wallet->readPassword failed, key="+toString(key));
			return NULL;
		}
	}
}

bool _setPassword(const char* key, const char* pwd, unsigned int flags)
{
	if((int)(flags&ppk_wf_silent)==0)
	{
		KWallet::Wallet* wallet=openWallet(flags);
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
	}
	else
		return NULL;
}

bool init_kde(unsigned int flags)
{
	//Init KDE
	KAboutData about(QByteArray("ppasskeeper-kwallet"),QByteArray("ppasskeeper-kwallet"),KLocalizedString(),QByteArray("1.0"));	
	KComponentData kcd(about);

	return true;
}
const char* getPassword(const char* key, unsigned int flags)
{
	//Init KDE Application
	if(init_kde(flags))
		return _getPassword(key, flags);
	else
		return NULL;
}

bool setPassword(const char* key, const char* pwd, unsigned int flags)
{
	//Init KDE Application
	if(init_kde(flags))
		return _setPassword(key, pwd, flags);
	else
		return PPK_FALSE;
}

std::string prefix(ppk_password_type type)
{
	static const char* ppk_network_string = "ppasskeeper_network://";
	static const char* ppk_app_string = "ppasskeeper_app://";
	static const char* ppk_item_string = "ppasskeeper_item://";

	switch(type)
	{
		case ppk_network:
			return ppk_network_string;
			break;
		case ppk_application:
			return ppk_app_string;
			break;
		case ppk_item:
			return ppk_item_string;
			break;
	}
	return std::string();
}

std::string generateNetworkKey(std::string server, int port, std::string username)
{
	return prefix(ppk_network)+username+"@"+server+":"+toString(port);
}

std::string generateApplicationKey(std::string application_name, std::string username)
{
	return prefix(ppk_application)+username+"@"+application_name;
}

std::string generateItemKey(std::string key)
{
	return prefix(ppk_item)+key;
}


//functions
extern "C" const char* getModuleID()
{
	return "KWallet4";
}

extern "C" const char* getModuleName()
{
	return "KWallet - Store it into KDE's Wallet";
}

extern "C" const int getABIVersion()
{
	return 1;
}

extern "C" unsigned int getPasswordListCount(ppk_password_type type, unsigned int flags)
{
	//Init KDE Application
	if(init_kde(flags))
	{
		//Open the wallet
		KWallet::Wallet* wallet=openWallet(ppk_rf_silent);
		if(wallet!=NULL)
		{
			ListPwd pwdl;		
			return pwdl.getPasswordListCount(wallet, prefix(type).c_str(), type);
		}
	}
	
	return 0;
}

extern "C" unsigned int getPasswordList(ppk_password_type type, void* pwdList, unsigned int maxModuleCount, unsigned int flags)
{
	//Init KDE Application
	if(init_kde(flags))
	{
		//Open the wallet
		KWallet::Wallet* wallet=openWallet(ppk_rf_silent);
		if(wallet!=NULL)
		{
			ListPwd pwdl;
			return pwdl.getPasswordList(wallet, prefix(type).c_str(), type, pwdList, maxModuleCount);
		}
	}
	else
		return 0;
}

extern "C" const char* getNetworkPassword(const char* server, int port, const char* username, unsigned int flags)
{
	return getPassword(generateNetworkKey(server, port, username).c_str(), flags);
}

extern "C" int setNetworkPassword(const char* server, int port, const char* username,  const char* pwd, unsigned int flags)
{
	return setPassword(generateNetworkKey(server, port, username).c_str(), pwd, flags)?PPK_TRUE:PPK_FALSE;
}

extern "C" const char* getApplicationPassword(const char* application_name, const char* username, unsigned int flags)
{
	return getPassword(generateApplicationKey(application_name, username).c_str(), flags);
}

extern "C" int setApplicationPassword(const char* application_name, const char* username,  const char* pwd, unsigned int flags)
{
	return setPassword(generateApplicationKey(application_name, username).c_str(), pwd, flags)?PPK_TRUE:PPK_FALSE;
}

extern "C" const char* getItem(const char* key, unsigned int flags)
{
	return getPassword(generateItemKey(key).c_str(), flags);
}

extern "C" int setItem(const char* key, const char* item, unsigned int flags)
{
	return setPassword(generateItemKey(key).c_str(), item, flags)?PPK_TRUE:PPK_FALSE;
}

extern "C" const char* getLastError()
{
	return last_error()->c_str();
}
