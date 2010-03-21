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
	Entry(const ppk_entry* entry, const ppk_data* data);
	Entry(const QString line);

	QString toString();
	QString entry();
	QString data();

	ppk_entry* ppkEntry();
	ppk_data* ppkData();

	bool isString();
	bool isBlob();
};

#endif // ENTRY_H
