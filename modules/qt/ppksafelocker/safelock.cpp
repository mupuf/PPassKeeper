#include "safelock.h"
#include "crypt.h"

#include <stdio.h>
#include <iostream>
#include <QStringList>
#include <QMapIterator>

extern "C" const char* getModuleID();

QString SafeLock::ppkEntryToString(const ppk_entry* entry) const
{
	//Get the key
	size_t lenKey=ppk_key_length(entry);
	char* key=new char[lenKey+1];
	if(ppk_get_key(entry, key, lenKey)==PPK_FALSE)
	{
		std::cerr << "Entry: Invalid key" << std::endl;
		return QString();
	}
	
	QString key2=QString::fromUtf8(key);
	
	delete[] key;
	
	return key2;
}

QString SafeLock::createFile()
{
	QString file;

	file=QString::fromUtf8("#PPK_NETWORK\n");
	file+=QString::fromUtf8("Version: 1.0\n");
	file+=QString::fromUtf8("Revision: %1\n").arg(this->revision+1);
	file+=QString::fromUtf8("Data:\n");

	QMapIterator<QString, SFEntry> i(entries);
	while (i.hasNext()) {
		i.next();
		if(i.value()!=SFEntry())
			file+=i.value().toString();
	}

	file+=QString::fromUtf8("\n");

	//qDebug("File = '%s'", qPrintable(file));

	return file;
}

bool SafeLock::isDBAvailable()
{
	//Decrypt the file
	char* c_data;
	crypt_error ret=decryptFromFile(qPrintable(safelockPath), &c_data, ""/*empty key*/);
	return ret!=crypt_file_cannot_be_oppenned;
}

ppk_error SafeLock::getKey(QString passphrase, QString& key)
{
	QString ret;
	char* key_c;
	
	//Get the passphrase
	if(passphrase==QString())
	{
		ppk_entry* entry=ppk_application_entry_new("passphrase", getModuleID());
		
		ppk_data* edata;
		ppk_error res=ppk_module_get_entry(qPrintable(ppk_module_passphrase), entry, &edata, ppk_rf_none);
		ppk_entry_free(entry);
		
		if(res==PPK_OK)
			passphrase=QString::fromUtf8(ppk_get_data_string(edata));
		else
			return res;
	}
	
	//Get the key
	key_c=getKeyFromPassphrase(qPrintable(passphrase), "ppk");
	ret=QString::fromUtf8(key_c);
	freeKeyFromPassphrase(key_c);
	
	return PPK_OK;
}

SafeLock::SafeLock(QString safelockPath) : safelockPath(safelockPath), _isOpen(false), _hasBeenModified(false), revision(0)
{

}

SafeLock::~SafeLock()
{
	close();
}

#include "modulecreation.h"
ppk_error SafeLock::open(const char* passphrase_c)
{
	QString passphrase=(passphrase_c==NULL?QString():QString::fromUtf8(passphrase_c));
	
	if(isOpen())
		return PPK_OK; //it is already open
	
	//Check if the DB is available. if it is not, create the db
	if(!isDBAvailable())
	{
		ModuleCreation m;
		m.exec();
		if(m.result()==QDialog::Accepted)
			passphrase=m.passphrase();
		else
			return PPK_USER_CANCELLED;
	}
	
	//get the blowfish key and store it
	ppk_error retGetKey=getKey(QString::fromUtf8(passphrase_c), key);
	if(retGetKey!=PPK_OK)
		return retGetKey;

	//Decrypt the file
	char* c_data;
	crypt_error ret=decryptFromFile(qPrintable(safelockPath), &c_data, qPrintable(key));
	if(ret==crypt_ok || ret==crypt_file_cannot_be_oppenned)
	{
		//set attributes
		this->_isOpen=true;

		//Get data
		QString data=QString::fromUtf8(c_data);
		QStringList lines=data.split(QString::fromUtf8("\n"));

		if(lines[0]==QString::fromUtf8("#PPK_NETWORK"))
		{
			//The passphrase was right, get the information

			/*QRegExp getVersion(QString::fromUtf8("Version: (\\d+\\.\\d+)")); //match 1.0
			if(getVersion.exactMatch(lines[1]))
			{
				QStringList matchedVars=getVersion.capturedTexts();
				QString version=matchedVars[1];
			}*/

			//get the revision number
			QRegExp getRevision(QString::fromUtf8("Revision: (\\d+)"));
			if(getRevision.exactMatch(lines[2]))
			{
				QStringList capTexts=getRevision.capturedTexts();
				this->revision=capTexts[1].toInt();
			}

			//Add all the data
			for(int i=4; i<lines.size(); i++)
			{
				SFEntry e(lines[i]);
				if(e!=SFEntry())
					entries[e.entry()]=e;
			}
		}
		else
		{
			fprintf(stderr, "SafeLock: Invalid format, cannot open the lock file '%s'.\n", qPrintable(safelockPath));
			close();
			return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
		}
	}
	else if(ret==crypt_invalid_key)
	{
		fprintf(stderr, "SafeLock: Invalid key, cannot open the lock file '%s'.\n", qPrintable(safelockPath));
		this->key=QString();
		return PPK_INVALID_PASSWORD;
	}
	else if(ret==crypt_unknown_error) //TODO SafeLocker creation module
		fprintf(stderr, "SafeLock: Unknown error. Create a new SafeLock at '%s'.\n", qPrintable(safelockPath));

	return PPK_OK;
}

bool SafeLock::flushChanges()
{
	if(_hasBeenModified)
	{
		QString data=createFile();
		return cryptToFile(qPrintable(safelockPath), qPrintable(data), qPrintable(this->key))==crypt_ok;
	}
	else
		return true;
}

bool SafeLock::close()
{
	if(isOpen())
	{
		if(!flushChanges())
			fprintf(stderr, "SafeLock: Flushing data failed.");

		//Clear secure data --> we should zero the memory space
		entries.clear();
		key=QString();

		this->_isOpen=false;

		return true;
	}

	return false;
}

void SafeLock::setPPKModuleForPassphrase(QString ppk_passphrase_module)
{
	this->ppk_module_passphrase=ppk_passphrase_module;
}

QString SafeLock::PPKModuleForPassphrase()
{
	return this->ppk_module_passphrase;
}

bool SafeLock::isOpen() const
{
	return _isOpen;
}

bool SafeLock::add(SFEntry e)
{
	if(entries.contains(e.entry()))
		return false;

	entries[e.entry()]=e;
	_hasBeenModified=true;

	return true;
}

bool SafeLock::reset(SFEntry e)
{
	entries[e.entry()]=e;
	_hasBeenModified=true;

	return true;
}

bool SafeLock::remove(QString entry)
{
	bool ret=entries.remove(entry)>0;
	if(ret)
		_hasBeenModified=true;
	return ret;
}

bool SafeLock::remove(const ppk_entry* entry)
{
	return remove(ppkEntryToString(entry));
}

const SFEntry SafeLock::get(QString entry) const
{
	return entries[entry];
}

const SFEntry SafeLock::get(const ppk_entry* entry) const
{
	//Get the entry
	return get(ppkEntryToString(entry));
}

QList<QString> SafeLock::list() const
{
	return entries.keys();
}
