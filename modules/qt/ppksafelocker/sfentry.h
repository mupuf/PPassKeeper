#ifndef ENTRY_H
#define ENTRY_H

#include <QString>
#include <QDateTime>

#include <ppasskeeper.h>

class SFEntry
{
private:
	QString _entry;
	QString _data;
	bool _isBlob;

	int _revision;
	QDateTime _timestamp;

public:
	SFEntry();

	///Create a new entry. By doing so, you'll reset the timestamp to Now()
	SFEntry(int revision, const ppk_entry* entry, const ppk_data* data);

	///Read an existing entry from a line
	SFEntry(const QString line);

	QString toString() const;
	QString entry() const;
	QString data() const;
	int revision() const;
	QDateTime timestamp() const;

	ppk_entry* ppkEntry_new() const;
	ppk_data* ppkData_new() const;

	bool isString() const;
	bool isBlob() const;

	bool merge(const SFEntry& b);

	bool operator==(const SFEntry& a) const;
	bool operator!=(const SFEntry& a) const;
};

#endif // ENTRY_H
