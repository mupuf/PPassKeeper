#include "ppasskeeper.h"
#include <vector>
#include <string>

class ListPwd
{
	private:
		static std::string prefix(ppk_entry_type type);		

		static ppk_entry* parseNetworkPassword(const std::string& stripped_name);
		static ppk_entry* parseAppPassword(const std::string& stripped_name);
		static ppk_entry* parseItemPassword(const std::string& stripped_name);
		static ppk_entry* parseFileName(const std::string& filename, unsigned int entry_types, unsigned int flags);
		
		static void appendEntries(const char* baseKey, unsigned int entry_types, unsigned int flags, std::vector<ppk_entry*>& list);
	public:
		static unsigned int getEntryListCount(const char* baseKey, unsigned int entry_types, unsigned int flags);
		static ppk_entry** getEntryList(const char* baseKey, unsigned int entry_types, size_t* nbEntries, unsigned int flags);
};


