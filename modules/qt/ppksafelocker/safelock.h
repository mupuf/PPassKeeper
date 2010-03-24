#ifndef SAFELOCK_H
#define SAFELOCK_H

#include <ppasskeeper.h>
#include <QMap>
#include "sfentry.h"

class SafeLock
{
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
	QString createDataBase();

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
};

#endif // SAFELOCK_H
