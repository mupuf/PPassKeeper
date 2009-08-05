#include "ppasskeeper.h"
#include "tokenizer.h"
#include "list_pwd.h"
#include <string>
#include <iostream>

#include <kwallet.h>
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

//plugin data
KWallet::Wallet* _wallet;
KApplication *_app;

//local functions
std::string* last_error()
{
	static std::string last_err;
	return &last_err;
}

extern "C" const char* getLastError()
{
	return last_error()->c_str();
}

void setError(std::string error)
{
	*(last_error())="PPK_KWallet : " + error;
	std::cerr << getLastError() << std::endl;
}

bool init_kde_lazy()
{
	static bool initialized = false;
	
	if (! initialized)
	{
		if (! KApplication::instance())
		{
			static char kdeAppName[] = "ppasskeeper-kwallet";
			int argc = 1;
			char *argv[2] = { kdeAppName, NULL };
			
			QByteArray qbApp(kdeAppName);
			KAboutData about(qbApp, qbApp, KLocalizedString(),QByteArray("1.0"));
			KCmdLineArgs::init(argc, argv, &about);
			if (! qApp)
				_app = new KApplication(true);
		}

		initialized = true;
	}
}

extern "C" void constructor()
{
	//lazy initialization
	_wallet=NULL;
	_app=NULL;
}

extern "C" void destructor()
{
	delete _wallet;
	delete _app;
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
extern "C" unsigned int readFlagsAvailable()
{
	return ppk_rf_none|ppk_rf_silent;
}

extern "C" unsigned int writeFlagsAvailable()
{
	return ppk_wf_none|ppk_wf_silent;
}

extern "C" unsigned int listingFlagsAvailable()
{
	return ppk_lf_none|ppk_lf_silent;
}

KWallet::Wallet* openWallet(unsigned int flags)
{
	init_kde_lazy();
	
	bool initialised=KWallet::Wallet::isOpen(KWallet::Wallet::NetworkWallet());
	
	if (!initialised && (flags & ppk_lf_silent!=0 || flags & ppk_rf_silent!=0 || flags & ppk_wf_silent!=0))
	{
		//continue only if it won't annoy people who don't want to be prompted
		setError("openWallet : openWallet was not performed because doing so would have conflicted with the silent flag.");
		return NULL;
	}

	if (_wallet == NULL)
	{
		//lazy init
		_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(), NULL);
		if (_wallet == NULL)
		{
			setError("openWallet : openWallet failed");
			return NULL;
		}
	}
	
	if (! KWallet::Wallet::isOpen(KWallet::Wallet::NetworkWallet()))
	{
		//is the wallet still open? if not, try to reinitialize it
		delete _wallet;
		_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(),0);
		if (_wallet == NULL)
		{
			std::cout << "openWallet : openWallet failed" << std::endl;
			return NULL;
		}
	}

	return _wallet;
}

ppk_error getPassword(const char* key, ppk_data** edata, unsigned int flags)
{
	QString pwd;

	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		//Get the password
		if(wallet->readPassword(QString::fromUtf8(key), pwd)==0)
		{
			*edata=ppk_string_data_new(qPrintable(pwd));
			return PPK_OK;
		}
		else
			return PPK_ENTRY_UNAVAILABLE;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

ppk_error getBlob(const char *key, ppk_data** edata, unsigned int flags)
{
	static QByteArray blob;

	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		if(wallet->readEntry(QString::fromAscii(key), blob)==0)
		{
			*edata=ppk_blob_data_new(blob, blob.size());
			return PPK_OK;
		}
		else
			return PPK_ENTRY_UNAVAILABLE;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

bool setPassword(const char* key, const char* pwd, unsigned int flags)
{
	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		//Set the password
		if(wallet->writePassword(QString::fromUtf8(key), QString::fromUtf8(pwd))==0)
			return true;
		else
		{
			setError("Set Entry: wallet->writePassword failed, key="+toString(key));
			return false;
		}
	}

	return false;
}

bool setBlob(const char *key, const void *data, unsigned long size, unsigned int flags)
{
	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		//Set the password
		QByteArray blobData((const char *) data, size);
		if(wallet->writeEntry(QString::fromAscii(key), blobData)==0)
			return true;
		else
		{
			setError("Set Entry: wallet->writeEntry failed, key="+toString(key));
			return false;
		}
	}
}

ppk_error removePassword(const char* key, unsigned int flags)
{
	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		//Set the password
		if(wallet->removeEntry(QString::fromAscii(key))==0)
			return PPK_OK;
		else
			return PPK_ENTRY_UNAVAILABLE;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

bool passwordExists(const char* key, unsigned int flags)
{
	static QString pwd;

	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		//Get the password
		if(wallet->hasEntry(QString::fromAscii(key))==0)
			return true;
		else
		{
			setError("Entry Exists : wallet->hasEntry failed, key="+toString(key));
			return false;
		}
	}
	else
		return false;
}

std::string prefix(ppk_entry_type type)
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
		default:
			setError("prefix : Invalid entry type.");
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

extern "C" unsigned int getEntryListCount(unsigned int entry_types, unsigned int flags)
{
	//Open the wallet
	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		static ListPwd pwdl;		
		return pwdl.getEntryListCount(wallet, entry_types, flags);
	}
	else
		return 0;
}

extern "C" unsigned int getEntryList(unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags)
{
	//Open the wallet
	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		static ListPwd pwdl;
		return pwdl.getEntryList(wallet, entry_types, entryList, nbEntries, flags);
	}
	else
		return 0;
}

static bool generateKey(const ppk_entry* entry, std::string &generatedKey)
{
	switch (entry->type)
	{
	case ppk_network:
		generatedKey = generateNetworkKey(entry->net.host, entry->net.port, entry->net.login);
		break;
	case ppk_application:
		generatedKey = generateApplicationKey(entry->app.app_name, entry->app.username);
		break;
	case ppk_item:
		generatedKey = generateItemKey(entry->item);
		break;
	default:
		return PPK_FALSE;
	}

	return PPK_TRUE;
}

extern "C" ppk_error getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
{
	std::string generatedKey;
	if (generateKey(entry, generatedKey) == PPK_FALSE)
		return PPK_UNKNOWN_ENTRY_TYPE;

	KWallet::Wallet *wallet = openWallet(flags);
	if (wallet == NULL)
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
	
	KWallet::Wallet::EntryType entryType = wallet->entryType(QString::fromStdString(generatedKey));

	if (entryType == KWallet::Wallet::Password)
		return getPassword(generatedKey.c_str(), edata, flags);
	else if (entryType == KWallet::Wallet::Stream)
		return getBlob(generatedKey.c_str(), edata, flags);
	else
		return PPK_UNKNOWN_ENTRY_TYPE;
}

extern "C" ppk_error setEntry(const ppk_entry* entry, const ppk_data* edata, unsigned int flags)
{
	std::string generatedKey;
	if (generateKey(entry, generatedKey) == PPK_FALSE)
		return PPK_UNKNOWN_ENTRY_TYPE;
	
	if (edata->type == ppk_string)
	{
		return setPassword(generatedKey.c_str(), edata->string, flags)?PPK_OK:PPK_UNKNOWN_ERROR;
	}
	else if (edata->type == ppk_blob)
	{
		return setBlob(generatedKey.c_str(), edata->blob.data, edata->blob.size, flags)?PPK_OK:PPK_UNKNOWN_ERROR;
	}
	else
		return PPK_UNKNOWN_ENTRY_TYPE;
}

extern "C" ppk_error removeEntry(const ppk_entry* entry, unsigned int flags)
{
	if(entry->type == ppk_network)
		return removePassword(generateNetworkKey(entry->net.host, entry->net.port, entry->net.login).c_str(), flags);
	else if(entry->type == ppk_application)
		return removePassword(generateApplicationKey(entry->app.app_name, entry->app.username).c_str(), flags);
	else if(entry->type == ppk_item)
		return removePassword(generateItemKey(entry->item).c_str(), flags);
	else
		return PPK_UNKNOWN_ENTRY_TYPE;
}

extern "C" ppk_boolean entryExists(const ppk_entry entry, unsigned int flags)
{
	if(entry.type == ppk_network)
		return passwordExists(generateNetworkKey(entry.net.host, entry.net.port, entry.net.login).c_str(), flags)?PPK_TRUE:PPK_FALSE;
	else if(entry.type == ppk_application)
		return passwordExists(generateApplicationKey(entry.app.app_name, entry.app.username).c_str(), flags)?PPK_TRUE:PPK_FALSE;
	else if(entry.type == ppk_item)
		return passwordExists(generateItemKey(entry.item).c_str(), flags)?PPK_TRUE:PPK_FALSE;
	else
		return PPK_FALSE;
}

extern "C" unsigned int maxDataSize(ppk_data_type type)
{
	switch(type)
	{
		case ppk_string:
			return -1;
		case ppk_blob:
			return -1;
	}
	
	return 0;
}
