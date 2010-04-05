#include "sfentry.h"

#include <QRegExp>
#include <QStringList>

#include <iostream>

#define STR_STRING "str :"
#define BLOB_STRING "blob:"

SFEntry::SFEntry() : _isBlob(false), _revision(0), _timestamp()
{}

SFEntry::SFEntry(int revision, const ppk_entry* entry, const ppk_data* data) : _isBlob(false), _revision(revision), _timestamp(QDateTime::currentDateTime())
{
	//Copy the entry
	size_t lenKey=ppk_key_length(entry);
	char* key=new char[lenKey+1];
	if(ppk_get_key(entry, key, lenKey)==PPK_FALSE)
	{
		std::cerr << "Entry: Invalid key" << std::endl;
		return;
	}

	//Copy the data
	QString qdata;
	if(ppk_get_data_type(data)==ppk_string)
	{
		this->_isBlob=false;
		qdata=QString::fromUtf8(ppk_get_data_string(data));
	}
	else
	{
		this->_isBlob=true;
		qdata=QString::fromUtf8(ppk_blob_data_to_base64(data));
	}

	this->_entry=QString::fromUtf8(key);
	this->_data=qdata;
}

SFEntry::SFEntry(const QString line)
{
	QRegExp regexp(QString::fromUtf8("(\\d+) (\\d+) (s|b)\"(.+)\":\"(.+)\"\n?")); //match 's"entry":"data"'
	if(regexp.exactMatch(line))
	{
		QStringList values=regexp.capturedTexts();

		this->_revision=values[1].toInt();

		this->_timestamp=QDateTime::fromTime_t(values[2].toUInt());

		this->_isBlob=(values[3]==QString::fromUtf8("b"));
		this->_entry=values[4];
		this->_data=values[5];

		//std::cout << "Create a " << std::string(isBlob()?"blob":"string") << " entry; entry='" << qPrintable(entry()) << "''; data='" << qPrintable(data()) << std::endl;
	}
	/*else
		std::cerr << "Cannot create an entry from line '"<< qPrintable(line) << "'" << std::endl;*/
}


QString SFEntry::toString() const
{
	return QString::fromUtf8("%1 %2 %3\"%4\":\"%5\"\n").arg(revision()).arg(timestamp().toTime_t()).arg(QString::fromUtf8(isString()?"s":"b"), entry(), data());
}

QString SFEntry::entry() const
{
	return _entry;
}

QString SFEntry::data() const
{
	return _data;
}

int SFEntry::revision() const
{
	return _revision;
}

QDateTime SFEntry::timestamp() const
{
	return _timestamp;
}

ppk_entry* SFEntry::ppkEntry_new() const
{
	return ppk_entry_new_from_key(qPrintable(entry()));
}

ppk_data* SFEntry::ppkData_new() const
{
	if(isBlob())
		return ppk_blob_data_new_from_base64(qPrintable(data()));
	else
		return ppk_string_data_new(qPrintable(data()));
}

bool SFEntry::isString() const
{
	return !_isBlob;
}

bool SFEntry::isBlob() const
{
	return _isBlob;
}

bool SFEntry::merge(const SFEntry& b)
{
	return true;
}

bool SFEntry::operator==(const SFEntry& a) const
{
	return _data==a._data && _entry==a._entry && _isBlob==a._isBlob;
}

bool SFEntry::operator!=(const SFEntry& a) const
{
	return !operator==(a);
}
