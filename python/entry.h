#ifndef PPK_PYTHON_ENTRY_H
#define PPK_PYTHON_ENTRY_H

#include <boost/python.hpp>
using namespace boost::python;

#include <iostream>

#include "../include/ppasskeeper.h"

struct Entry
{
	Entry() : m_entry(NULL) {}
	~Entry() { if (m_entry != NULL) ppk_entry_free(m_entry); }
	static Entry* create_network_entry(const char* protocol, const char* login, const char* host, unsigned short port)
	{
		Entry* entry = new Entry;
		entry->m_entry = ppk_network_entry_new(protocol, login, host, port);
		return entry;
	}
	static Entry* create_application_entry(const char* username, const char* app_name)
	{
		Entry* entry = new Entry;
		entry->m_entry = ppk_application_entry_new(username, app_name);
		return entry;
	}
	static Entry* create_item_entry(const char* item)
	{
		Entry* entry = new Entry;
		entry->m_entry = ppk_item_entry_new(item);
		return entry;
	}
	static Entry* from_key(const char* key)
	{
		Entry* entry = new Entry;
		entry->m_entry = ppk_entry_new_from_key(key);
		return entry;
	}
	std::string to_key() const
	{
		size_t len = ppk_key_length(m_entry);
		char buf[len];
		return (ppk_get_key(m_entry, buf, len) == PPK_TRUE) ? buf : std::string();
	}
	ppk_entry* m_entry;
};

std::ostream& operator<<(std::ostream& s, const Entry& e) { return s << e.to_key(); }

#endif
