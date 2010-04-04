#ifndef SAFELOCK_H
#define SAFELOCK_H

#include <ppasskeeper.h>
#include <QMap>
#include <QFtp>
#include <QFile>
#include "sfentry.h"

class SafeLock : public QObject
{
Q_OBJECT
	QString safelockPath;
	QString ppk_module_passphrase;
	bool _isOpen;
	bool _hasBeenModified;

	int revision;
	QMap<QString, SFEntry> entries;
	QString key;

	QString ppkEntryToString(const ppk_entry* entry) const;
	
	QString createFile();
	bool isDBAvailable();
	
	ppk_error getKey(QString passphrase, QString& key);

	//FTP
	QFtp ftp;
	QFile Uploadfile;
	int idConnect, idLogin, idCd, idPut; //Commands'id
	QString FTP_Host, FTP_Login, FTP_PWD, FTP_RemoteDir;
	qint16 FTP_Port;
	
	void sendFile(QString host, quint16 port, QString login, QString pwd, QString filepath, QString remoteDir=QString());

public:
	SafeLock(QString safelockPath);
	~SafeLock();

	ppk_error open(const char* passphrase=NULL);
	bool flushChanges();
	bool close();
	
	void setPPKModuleForPassphrase(QString ppk_passphrase_module);
	QString PPKModuleForPassphrase();
	
	bool isOpen() const;

	bool add(SFEntry e);
	bool reset(SFEntry e);
	bool remove(QString entry);
	bool remove(const ppk_entry* entry);
	const SFEntry get(QString entry) const;
	const SFEntry get(const ppk_entry* entry) const;
	QList<QString> list() const;
	
private slots:
	void commandStarted(int id);
	void dataTransferProgress(qint64 done, qint64 total);
	void commandFinished(int id, bool error);
};

#endif // SAFELOCK_H
