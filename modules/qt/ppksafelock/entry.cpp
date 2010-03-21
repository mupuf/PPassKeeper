#include "entry.h"

#include <QRegExp>
#include <QStringList>

#include <iostream>

#define STR_STRING "str :"
#define BLOB_STRING "blob:"

Entry::Entry(){}

Entry::Entry(const ppk_entry* entry, const ppk_data* data)
{
	//Copy the entry
	size_t lenKey=ppk_key_length(entry);
	char* key=new char[lenKey+1];
	if(ppk_get_key(entry, key, lenKey-1)==PPK_FALSE)
	{
		std::cerr << "Entry: Invalid key" << std::endl;
		return;
	}

	//Copy the data
	QString qdata;
	if(ppk_get_data_type(data)==ppk_string)
	{
		this->_isBlob=false;
		qdata=ppk_get_data_string(data);
	}
	else
	{
		this->_isBlob=true;
		qdata=ppk_blob_data_to_base64(data);
	}

	this->_entry=QString::fromUtf8(key);
	this->_data=qdata;
}

Entry::Entry(const QString line)
{
	QRegExp regexp(QString::fromUtf8("(s|b)\"(.*)\":\"(.*)\"\n")); //match "entry":"data"
	if(regexp.exactMatch(line))
	{
		QStringList values=regexp.capturedTexts();

		this->_isBlob=(values[1]=="b");
		this->_entry=values[2];
		this->_data=values[3];
	}
}

QString Entry::toString() const
{
	if(isString())
		return QString::fromUtf8("s\"%1\":\"%2\"\n").arg(entry(), data());
	else if(isBlob())
		return QString::fromUtf8("b\"%1\":\"%2\"\n").arg(entry(), data());
	else
		return QString();
}

QString Entry::entry() const
{
	return _entry;
}

QString Entry::data() const
{
	return _data;
}

ppk_entry* Entry::ppkEntry() const
{
	return ppk_entry_new_from_key(qPrintable(entry()));
}

ppk_data* Entry::ppkData() const
{
	if(isBlob())
		return ppk_blob_data_new_from_base64(qPrintable(data()));
	else
		return ppk_string_data_new(qPrintable(data()));
}

bool Entry::isString() const
{
	return !_isBlob;
}

bool Entry::isBlob() const
{
	return _isBlob;
}

bool Entry::operator==(const Entry& a)
{
	return _data==a._data && _entry==a._entry && _isBlob==a._isBlob;
}

bool Entry::operator!=(const Entry& a)
{
	return !operator==(a);
}
