#ifndef ENTRY_H
#define ENTRY_H

#include <QString>

#include <ppasskeeper.h>

class SFEntry
{
private:
	QString _entry;
	QString _data;
	bool _isBlob;

public:
	SFEntry();
	SFEntry(const ppk_entry* entry, const ppk_data* data);
	SFEntry(const QString line);

	QString toString() const;
	QString entry() const;
	QString data() const;

	ppk_entry* ppkEntry_new() const;
	ppk_data* ppkData_new() const;

	bool isString() const;
	bool isBlob() const;

	bool operator==(const SFEntry& a) const;
	bool operator!=(const SFEntry& a) const;
};

#endif // ENTRY_H
