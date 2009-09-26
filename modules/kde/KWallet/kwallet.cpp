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
	//It crashes the application to uncomment the following :s
	/*delete _wallet;
	delete _app;*/
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
		return NULL;
	}

	if (_wallet == NULL)
	{
		//lazy init
		_wallet = KWallet::Wallet::openWallet(KWallet::Wallet::NetworkWallet(), NULL);
		if (_wallet == NULL)
		{
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
			return NULL;
		}
	}
	
	if(! _wallet->setFolder(QString::fromAscii("PPassKeeper")))
	{
			_wallet->createFolder(QString::fromAscii("PPassKeeper"));
			if(! _wallet->setFolder(QString::fromAscii("PPassKeeper")))
				return NULL;
	}

	return _wallet;
}

ppk_error getPassword(const QString key, ppk_data** edata, unsigned int flags)
{
	QString pwd;

	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		//Get the password
		if(wallet->readPassword(key, pwd)==0)
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

ppk_error getBlob(const QString key, ppk_data** edata, unsigned int flags)
{
	static QByteArray blob;

	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		if(wallet->readEntry(key, blob)==0)
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

ppk_error setPassword(const QString key, const char* pwd, unsigned int flags)
{
	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		//Set the password
		if(wallet->writePassword(key, QString::fromUtf8(pwd))==0)
			return PPK_OK;
		else
			return PPK_ENTRY_UNAVAILABLE;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

ppk_error setBlob(const QString key, const void *data, unsigned long size, unsigned int flags)
{
	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		//Set the password
		QByteArray blobData((const char *) data, size);
		if(wallet->writeEntry(key, blobData)==0)
			return PPK_OK;
		else
			return PPK_ENTRY_UNAVAILABLE;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

ppk_error removePassword(const QString key, unsigned int flags)
{
	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		//Set the password
		if(wallet->removeEntry(key)==0)
			return PPK_OK;
		else
			return PPK_ENTRY_UNAVAILABLE;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

bool passwordExists(const QString key, unsigned int flags)
{
	static QString pwd;

	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		//Get the password
		if(wallet->hasEntry(key)==0)
			return true;
		else
			return false;
	}
	else
		return false;
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
		return ListPwd::getEntryListCount(wallet, entry_types, flags);
	}
	else
		return 0;
}

extern "C" ppk_error getEntryList(unsigned int entry_types, ppk_entry*** entryList, size_t* nbEntries, unsigned int flags)
{
	//Open the wallet
	KWallet::Wallet* wallet=openWallet(flags);
	if(wallet!=NULL)
	{
		*entryList = ListPwd::getEntryList(wallet, entry_types, flags, nbEntries);
		return PPK_OK;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

static bool generateKey(const ppk_entry* entry, QString &generatedKey)
{
	size_t size=ppk_key_length(entry);
	if(size>0)
	{
		char* buf=new char[size+1];
		
		ppk_boolean ret=ppk_get_key(entry, buf, size);
		if(ret==PPK_TRUE)
			generatedKey=QString::fromUtf8(buf);
		else
			std::cerr << "generateKey: invalid ppk_get_key" << std::endl;
		
		delete[] buf;
		
		return ret;
	}
	else
	{
		std::cerr << "generateKey: invalid ppk_key_length" << std::endl;
		return PPK_FALSE;
	}
}

extern "C" ppk_error getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
{
	QString generatedKey;
	if (generateKey(entry, generatedKey) == PPK_FALSE)
		return PPK_UNKNOWN_ENTRY_TYPE;

	KWallet::Wallet *wallet = openWallet(flags);
	if (wallet == NULL)
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
	
	KWallet::Wallet::EntryType entryType = wallet->entryType(generatedKey);

	if (entryType == KWallet::Wallet::Password)
		return getPassword(generatedKey, edata, flags);
	else if (entryType == KWallet::Wallet::Stream)
		return getBlob(generatedKey, edata, flags);
	else
		return PPK_UNKNOWN_ENTRY_TYPE;
}

extern "C" ppk_error setEntry(const ppk_entry* entry, const ppk_data* edata, unsigned int flags)
{
	QString generatedKey;
	if (generateKey(entry, generatedKey) == PPK_FALSE)
		return PPK_UNKNOWN_ENTRY_TYPE;
	
	if (edata->type == ppk_string)
		return setPassword(generatedKey, edata->string, flags);
	else if (edata->type == ppk_blob)
		return setBlob(generatedKey, edata->blob.data, edata->blob.size, flags);
	else
		return PPK_UNKNOWN_ENTRY_TYPE;
}

extern "C" ppk_error removeEntry(const ppk_entry* entry, unsigned int flags)
{
	QString generatedKey;
	if (generateKey(entry, generatedKey) == PPK_FALSE)
		return PPK_UNKNOWN_ENTRY_TYPE;
	
	return removePassword(generatedKey, flags);
}

extern "C" ppk_boolean entryExists(const ppk_entry* entry, unsigned int flags)
{
	QString generatedKey;
	if (generateKey(entry, generatedKey) == PPK_FALSE)
		return PPK_FALSE;
	
	return passwordExists(generatedKey, flags)==PPK_OK?PPK_TRUE:PPK_FALSE;
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
