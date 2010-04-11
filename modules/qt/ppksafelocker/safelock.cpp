/*
PPassKeeper
Copyright (C) 2008-2010 MùPùF.org

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "safelock.h"
#include "crypt.h"

#include "modulecreation.h"

#include <stdio.h>
#include <iostream>
#include <QStringList>
#include <QMapIterator>
#include <QApplication>

extern "C" const char* getModuleID();

void SafeLock::getQApp()
{
	if(qApp==NULL && app.data()==NULL)
		app=QSharedPointer<QApplication>(new QApplication(0, NULL));
}

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

void SafeLock::SetHasBeenModified()
{
	if(!_hasBeenModified)
		this->revision++;

	_hasBeenModified=true;;
}

void SafeLock::resetClosingTimer()
{
	if(_closingDelay>0)
		timer.start(_closingDelay*60*1000);
}

QString SafeLock::createFile()
{
	QString file;

	file=QString::fromUtf8("#PPK_NETWORK\n");
	file+=QString::fromUtf8("Version: 1.0\n");
	file+=QString::fromUtf8("Revision: %1 %2\n").arg(this->revision).arg(this->timestamp.toTime_t());
	file+=QString::fromUtf8("Data:\n");

	QMapIterator<QString, SFEntry> i(entries);
	while (i.hasNext()) {
		i.next();
		if(i.value()!=SFEntry())
			file+=i.value().toString();
	}

	file+=QString::fromUtf8("\n");

	qDebug("File='%s'", qPrintable(file));

	return file;
}

bool SafeLock::isDBAvailable()
{
	//Decrypt the file
	char* c_data;
	crypt_error ret=decryptFromFile(qPrintable(safeLockPath()), &c_data, ""/*empty key*/);
	return ret!=crypt_file_cannot_be_oppenned;
}

ppk_error SafeLock::getKey(QString passphrase, QString& key)
{
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
	key=QString::fromUtf8(key_c);
	freeKeyFromPassphrase(key_c);
	
	return PPK_OK;
}

void SafeLock::sendFile(QString host, quint16 port, QString login, QString pwd, QString filepath, QString remoteDir)
{
	Uploadfile.setFileName(filepath);
	if(Uploadfile.open(QIODevice::ReadOnly))
	{
		QString name;
		
		//Get the file's name from its path
		int index = filepath.lastIndexOf(QString::fromUtf8("/"));
		if(index==-1)
			index = filepath.lastIndexOf(QString::fromUtf8("\\"));
		if(index!=-1)
			name=filepath.mid(index+1);
		else
			name=filepath;
		
		//connect to the FTP Server
		idConnect=ftp.connectToHost(host,port);

		//Login
		idLogin=ftp.login(login,pwd);

		//open the wanted directory
		if(remoteDir!=QString())
			idCd=ftp.cd(remoteDir);

		//Send the file
		idPut=ftp.put(&Uploadfile,name);
	}
}

SafeLock::SafeLock(QString safelockPath, int closingDelay) : _safelockPath(safelockPath), _closingDelay(closingDelay), _isOpen(false), _hasBeenModified(false), revision(0), timestamp(QDateTime::currentDateTime())
{
	//Connect FTP signals
	connect(&ftp, SIGNAL(commandFinished(int, bool)), this, SLOT(commandFinished(int, bool)));
	connect(&ftp, SIGNAL(dataTransferProgress(qint64, qint64)), this, SLOT(dataTransferProgress(qint64, qint64)));
	connect(&ftp, SIGNAL(commandStarted(int)), this, SLOT(commandStarted(int)));

	//Set the closing timer to the oneshot mode
	timer.setSingleShot(true);

	//connect closing timer signals
	connect(&timer, SIGNAL(timeout()), this, SLOT(close()));
}

SafeLock::~SafeLock()
{
	close();
	terminate();
	this->wait(100);
}

ppk_error SafeLock::open(const char* passphrase_c)
{
	QWriteLocker lock(&rwlock);

	QString passphrase=(passphrase_c==NULL?QString():QString::fromUtf8(passphrase_c));
	
	if(_isOpen)
		return PPK_OK; //it is already open
	
	//Check if the DB is available. if it is not, create the db
	if(!isDBAvailable())
	{
		getQApp();

		ModuleCreation m;
		m.exec();
		if(m.result()==QDialog::Accepted)
		{
			passphrase=m.passphrase();
			SetHasBeenModified();
		}
		else
			return PPK_USER_CANCELLED;
	}
	
	//get the blowfish key and store it
	ppk_error retGetKey=getKey(passphrase, key);
	if(retGetKey!=PPK_OK)
		return retGetKey;

	//Decrypt the file
	char* c_data=NULL;
	crypt_error ret=decryptFromFile(qPrintable(_safelockPath), &c_data, qPrintable(key));
	if(ret==crypt_ok)
	{
		//set attributes
		this->_isOpen=true;

		//remove all entries
		entries.clear();

		//Get data
		QString data=(c_data==NULL?QString():QString::fromUtf8(c_data));
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
			QRegExp getRevision(QString::fromUtf8("Revision: (\\d+) (\\d+)"));
			if(getRevision.exactMatch(lines[2]))
			{
				QStringList capTexts=getRevision.capturedTexts();
				this->revision=capTexts[1].toInt();
				this->timestamp.fromTime_t(capTexts[2].toUInt());
			}

			//Add all the data
			for(int i=4; i<lines.size(); i++)
			{
				SFEntry e(lines[i]);
				if(e!=SFEntry())
					entries[e.entry()]=e;
			}

			//start the event loop
			getQApp();
			start();

			//Start the closing timer
			resetClosingTimer();
		}
		else
		{
			fprintf(stderr, "SafeLock: Invalid format, cannot open the lock file '%s'.\n", qPrintable(_safelockPath));
			close();
			return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
		}
	}
	else if(ret==crypt_invalid_key)
	{
		fprintf(stderr, "SafeLock: Invalid key, cannot open the lock file '%s'.\n", qPrintable(_safelockPath));
		this->key=QString();
		return PPK_INVALID_PASSWORD;
	}
	else if(ret==crypt_file_cannot_be_oppenned || ret==crypt_unknown_error)
	{
		//Create a new safelock
		fprintf(stderr, "SafeLock: Create a new SafeLock at '%s'.\n", qPrintable(_safelockPath));

		//Set the parameters of the new SafeLock
		this->_isOpen=true;
		this->revision=0;
		this->entries.clear();
	}

	return PPK_OK;
}

bool SafeLock::flushChanges()
{
	QWriteLocker lock(&rwlock);

	if(_hasBeenModified)
	{
		QString data=createFile();
		return cryptToFile(qPrintable(_safelockPath), qPrintable(data), qPrintable(this->key))==crypt_ok;
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
		
		//Send onto the ftp
		sendFile(FTP_Host, FTP_Port, FTP_Login, FTP_PWD, _safelockPath, FTP_RemoteDir);

		//Set the file as being closed
		this->_isOpen=false;

		return true;
	}

	return false;
}

void SafeLock::setPPKModuleForPassphrase(QString ppk_passphrase_module)
{
	QWriteLocker lock(&rwlock);

	this->ppk_module_passphrase=ppk_passphrase_module;
}

QString SafeLock::PPKModuleForPassphrase()
{
	QReadLocker lock(&const_cast<SafeLock*>(this)->rwlock);
	return this->ppk_module_passphrase;
}

void SafeLock::setSafeLockPath(QString safelocker)
{
	this->_safelockPath=safelocker;
}

QString SafeLock::safeLockPath()
{
	return _safelockPath;
}

void SafeLock::setClosingDelay(int closingDelay)
{
	this->_closingDelay=closingDelay;
}

int SafeLock::closingDelay()
{
	return this->_closingDelay;
}

bool SafeLock::isOpen() const
{
	QReadLocker lock(&const_cast<SafeLock*>(this)->rwlock);
	return _isOpen;
}

bool SafeLock::add(const ppk_entry* entry, const ppk_data* data)
{
	QWriteLocker lock(&rwlock);

	SFEntry e(this->revision, entry, data);

	if(entries.contains(e.entry()))
		return false;

	SetHasBeenModified();
	entries[e.entry()]=e;

	return true;
}

bool SafeLock::reset(const ppk_entry* entry, const ppk_data* data)
{
	QWriteLocker lock(&rwlock);

	SetHasBeenModified();

	SFEntry e(this->revision, entry, data);
	entries[e.entry()]=e;

	return true;
}

bool SafeLock::remove(QString entry)
{
	QWriteLocker lock(&rwlock);

	bool ret=entries.remove(entry)>0;
	if(ret)
		SetHasBeenModified();
	return ret;
}

bool SafeLock::remove(const ppk_entry* entry)
{
	return remove(ppkEntryToString(entry));
}

const SFEntry SafeLock::get(QString entry) const
{
	QReadLocker lock(&const_cast<SafeLock*>(this)->rwlock);
	return entries[entry];
}

const SFEntry SafeLock::get(const ppk_entry* entry) const
{
	QReadLocker lock(&const_cast<SafeLock*>(this)->rwlock);

	//Get the entry
	return get(ppkEntryToString(entry));
}

QList<QString> SafeLock::list() const
{
	QReadLocker lock(&const_cast<SafeLock*>(this)->rwlock);

	return entries.keys();
}

bool SafeLock::merge(const SafeLock& b)
{
	QList<SFEntry> o_entries=b.entries.values();
	
	for(int i=0; i<o_entries.size(); i++)
	{
		const SFEntry e=o_entries[i].entry();
		if(entries.contains(e.entry()))
		{
			if(!entries[e.entry()].merge(e))
			{
				//TODO: Merge error !
			}
		}
		else
		{
			//This entry has been deleted or didn't already exist
		}
	}
	
	return true;
}

//Slots
void SafeLock::commandStarted(int id)
{
	/*if(id==idConnect)
		lbl_state->setText("State : Connection to " + host +":" + QString::number(port));
	else if(id==idLogin)
		lbl_state->setText("State : Authentification of the user '"+login+"'");
	else if(id==idCd)
		lbl_state->setText("State : Setting the right upload directory");
	else if(id==idPut)
	{
		prevPos=0;
		lbl_state->setText(QString(UPLOAD_STRING).arg(fileName).arg(getStringFromSizeInBytes(0)));
		t_speed.start();
		t_beginning.start();
		t_refresh_title.start();
	}*/
}

void SafeLock::dataTransferProgress(qint64 done, qint64 total)
{
	
}

void SafeLock::commandFinished(int id, bool error)
{
	if(error)
	{
		//Display the correct Error Message
		/*if(id==idConnect)
			notify(true,"Connection Error : The server is unreachable on this IP/port.\n\nCheck the host and the port, but maybe the server is just offline.\n\n"+ftp.errorString());
		else if(id==idLogin)
			notify(true,"Authentification Error : The login/Pwd is not valid on this server.\n\n"+ftp.errorString());
		else if(id==idCd)
			notify(true,"Error : The remote directory you would like to host uploaded file does not exist. Please create it or change the path in the settings.\n\n"+ftp.errorString());
		else if(id==idPut)
			notify(true,"Upload Error : The file's upload has failed.\n\n"+ftp.errorString());
		*/
	}
	else if(id==idPut)
	{
		//OK
		Uploadfile.close();
	}
}
