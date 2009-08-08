#include "ppasskeeper.h"
#include <vector>
#include <string>

namespace KWallet
{
	class Wallet;
}

class ListPwd
{
	private:
		struct networkList{std::string host; std::string user; unsigned short int port;};
		struct appList{std::string app; std::string user;};
		struct itemList{std::string key;};

		std::vector<networkList> listNet;
		std::vector<appList> listApp;
		std::vector<itemList> listItem;

		std::string prefix(ppk_entry_type type);		

		bool addNetworkPassword(std::string stripped_name);
		bool addAppPassword(std::string stripped_name);
		bool addItemPassword(std::string stripped_name);
		bool parseFileName(std::string filename, unsigned int entry_types, unsigned int flags);
		unsigned int updateDataBase(const char* baseKey, unsigned int entry_types, unsigned int flags);

		unsigned int copyDBToPwdList(unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries);
		unsigned int copyNetworkToPwdList(ppk_entry *entryList, unsigned int nbEntries);
		unsigned int copyApplicationToPwdList(ppk_entry *entryList, unsigned int nbEntries);
		unsigned int copyItemToPwdList(ppk_entry *entryList, unsigned int nbEntries);
	public:
		unsigned int getEntryListCount(const char* baseKey, unsigned int entry_types, unsigned int flags);
		unsigned int getEntryList(const char* baseKey, unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags);
};
