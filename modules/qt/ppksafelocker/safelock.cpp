#include "safelock.h"
#include "crypt.h"

#include <stdio.h>
#include <iostream>
#include <QStringList>
#include <QMapIterator>

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

SafeLock::SafeLock(QString safelockPath) : safelockPath(safelockPath), _isOpen(false), _hasBeenModified(false), revision(0)
{

}

SafeLock::~SafeLock()
{
	close();
}

bool SafeLock::open(const char* passphrase_c)
{
	if(isOpen())
		return false; //it is already open

	char* c_data;
	crypt_error ret=decryptFromFile(qPrintable(safelockPath), &c_data, passphrase_c);
	if(ret==crypt_ok || ret==crypt_file_cannot_be_oppenned)
	{
		//set attributes
		this->passphrase=QString::fromUtf8(passphrase_c);
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
			return false;
		}
	}
	else if(ret==crypt_invalid_key)
	{
		fprintf(stderr, "SafeLock: Invalid key, cannot open the lock file '%s'.\n", qPrintable(safelockPath));
		return false;
	}
	else if(ret==crypt_unknown_error)
		fprintf(stderr, "SafeLock: Unknown error. Create a new SafeLock at '%s'.\n", qPrintable(safelockPath));

	return true;
}

bool SafeLock::flushChanges()
{
	if(_hasBeenModified)
	{
		QString data=createFile();
		return cryptToFile(qPrintable(safelockPath), qPrintable(data), qPrintable(this->passphrase))==crypt_ok;
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
		passphrase=QString();

		this->_isOpen=false;

		return true;
	}

	return false;
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
