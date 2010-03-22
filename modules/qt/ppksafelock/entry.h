#ifndef ENTRY_H
#define ENTRY_H

#include <QString>

#include <ppasskeeper.h>

class Entry
{
private:
	QString _entry;
	QString _data;
	bool _isBlob;

public:
	Entry();
	Entry(const ppk_entry* entry, const ppk_data* data);
	Entry(const QString line);

	QString toString() const;
	QString entry() const;
	QString data() const;

	ppk_entry* ppkEntry() const;
	ppk_data* ppkData() const;

	bool isString() const;
	bool isBlob() const;

	bool operator==(const Entry& a) const;
	bool operator!=(const Entry& a) const;
};

#endif // ENTRY_H
