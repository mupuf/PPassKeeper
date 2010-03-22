#ifndef SAFELOCK_H
#define SAFELOCK_H

#include <ppasskeeper.h>
#include <QMap>
#include "sfentry.h"

class SafeLock
{
	QString safelockPath;
	bool _isOpen;
	bool _hasBeenModified;

	int revision;
	QMap<QString, SFEntry> entries;
	QString passphrase;

	QString createFile();

public:
	SafeLock(QString safelockPath);
	~SafeLock();

	bool open(const char* passphrase);
	bool flushChanges();
	bool close();

	bool isOpen() const;

	bool add(SFEntry e);
	bool remove(QString entry);
	const SFEntry get(QString entry) const;
	const SFEntry get(const ppk_entry* entry) const;
	QList<QString> list() const;
};

#endif // SAFELOCK_H
