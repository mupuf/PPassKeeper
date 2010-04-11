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

#ifndef PPASSKEEPER_KEY_H
#define PPASSKEEPER_KEY_H

/**
 * \file key.h
 * \author MùPùF - Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 27-07-2009
 */

#include <ppasskeeper.h>


#ifdef __cplusplus
extern "C"
{
#endif

	/*! \brief Get the space that would be needed if we were to store the entry in a string.
	* \param entry The concerned entry.
	* \return  Return the space that would be needed if we were to store the entry in a string.*/
	size_t ppk_key_length(const ppk_entry* entry);

	/*! \brief Convert an entry to a string
	* \param entry The entry to be converted.
	* \param[out] returned_key The string will be copied in this buffer.
	* \param max_key_size How much character are we allowed to copy to returned_key ?
	* \return Returns PPK_TRUE if everything went fine and PPK_FALSE if there were not enough room in returned_key to fit the given entry.*/
	ppk_boolean ppk_get_key(const ppk_entry* entry, char* returned_key, size_t max_key_size);

	/*! \brief Convert a string to a ppk_entry
	* \param key The key to be converted to a ppk_entry.
	* \return Returns a ppk_entry if everything went fine, NULL otherwise.*/
	ppk_entry* ppk_entry_new_from_key(const char* key);

#ifdef __cplusplus
}
#endif

#endif //PPASSKEEPER_KEY_H
