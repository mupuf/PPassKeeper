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
		static const char* prefix(ppk_entry_type type);

		static ppk_entry* parseNetworkPassword(const std::string& stripped_name);
		static ppk_entry* parseAppPassword(const std::string& stripped_name);
		static ppk_entry* parseItemPassword(const std::string& stripped_name);
		static ppk_entry* parseFileName(const std::string& filename, unsigned int entry_types, unsigned int flags);
	public:
		static unsigned int getEntryListCount(KWallet::Wallet* wallet, unsigned int entry_types, unsigned int flags);
		static ppk_entry** getEntryList(KWallet::Wallet* wallet, unsigned int entry_types, unsigned int flags, unsigned int* count);
};
