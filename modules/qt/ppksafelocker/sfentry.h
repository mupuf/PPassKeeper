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
	
	//Tags
	unsigned int _tags;
	enum Tags
	{
		entry_normal=0,
		entry_deleted=1
	};

public:
	SFEntry();

	///Create a new entry. By doing so, you'll reset the timestamp to Now()
	SFEntry(int revision, const ppk_entry* entry, const ppk_data* data);

	///Read an existing entry from a line
	SFEntry(const QString line);

	///Serialization
	QString toString() const;
	
	///get the entry
	QString entry() const;
	
	///Get the data
	QString data() const;
	
	///Get the revision number
	int revision() const;
	
	///Get the timestamp
	QDateTime timestamp() const;

	///Get a ppk_entry from the entry. Must be freed
	ppk_entry* ppkEntry_new() const;
	
	///Get a ppk_data from the data. Must be freed
	ppk_data* ppkData_new() const;

	///Is the entry a string ?
	bool isString() const;
	
	///Is the entry a blob ?
	bool isBlob() const;
	
	///Marked as deleted ?
	bool isMarkedDeleted();
	
	///Mark this entry as deleted
	void markAsDeleted(bool value);

	///Merge b into this
	bool merge(const SFEntry& b);

	///overload the operators ==
	bool operator==(const SFEntry& a) const;
	
	///overload the operators !=
	bool operator!=(const SFEntry& a) const;
};

#endif // ENTRY_H
