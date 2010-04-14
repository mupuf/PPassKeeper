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

#ifndef SAFELOCK_H
#define SAFELOCK_H

#include <ppasskeeper.h>
#include <QMap>
#include <QFtp>
#include <QFile>
#include <QThread>
#include <QTimer>
#include <QReadWriteLock>
#include <QSharedPointer>
#include "sfentry.h"

class QApplication;

class SafeLock : private QThread
{
Q_OBJECT
	//Keyring attributes
	QString _safelockPath;
	int _closingDelay;
	QString ppk_module_passphrase;
	bool _isOpen;
	bool _hasBeenModified;

	//versionning and data
	int revision;
	QDateTime timestamp;
	QMap<QString, SFEntry> entries;
	QString key;

	//Multi-Threading
	QSharedPointer<QApplication> app;
	QReadWriteLock rwlock;
	QTimer timer;

	//Local functions
	QString ppkEntryToString(const ppk_entry* entry) const;

	void SetHasBeenModified();

	QString createFile();
	bool isDBAvailable();
	
	ppk_error getKey(QString passphrase, QString& key);

	//FTP
	/*QFtp ftp;
	QFile Uploadfile;
	int idConnect, idLogin, idCd, idPut; //Commands'id
	QString FTP_Host, FTP_Login, FTP_PWD, FTP_RemoteDir;
	qint16 FTP_Port;
	
	void sendFile(QString host, quint16 port, QString login, QString pwd, QString filepath, QString remoteDir=QString());*/

protected:
	void run();

public:
	SafeLock(QString safelockPath=QString(), int closingDelay=0);
	~SafeLock();

	ppk_error open(const char* passphrase=NULL);
	bool flushChanges();
	
	void setPPKModuleForPassphrase(QString ppk_passphrase_module);
	QString PPKModuleForPassphrase();

	void setSafeLockPath(QString safelocker);
	QString safeLockPath();

	void setClosingDelay(int closingDelay);
	int closingDelay();
	
	bool isOpen() const;

	bool add(const ppk_entry* entry, const ppk_data* data);
	bool reset(const ppk_entry* entry, const ppk_data* data);
	bool remove(QString entry);
	bool remove(const ppk_entry* entry);
	const SFEntry get(QString entry) const;
	const SFEntry get(const ppk_entry* entry) const;
	QList<QString> list() const;
	
	bool merge(const SafeLock& b);

public slots:
	bool close();
	
private slots:
	void resetClosingTimer();
	/*void commandStarted(int id);
	void dataTransferProgress(qint64 done, qint64 total);
	void commandFinished(int id, bool error);*/

signals:
	void newActionPerformed();
};

#endif // SAFELOCK_H
