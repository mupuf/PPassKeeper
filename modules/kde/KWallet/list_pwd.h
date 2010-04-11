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
		static ppk_entry** getEntryList(KWallet::Wallet* wallet, unsigned int entry_types, unsigned int flags, size_t* count);
};
