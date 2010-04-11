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

#ifndef PPK_UTILS_H
#define PPK_UTILS_H

/**
 * \file utils.h
 * \author MùPùF - Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 27-07-2009
 */

#ifdef __cplusplus 
extern "C"
{
#endif
	/*! \brief Returns whether the library is locked, waiting for the right password, or not
	* \return PPK_TRUE is the library is locked, PPK_FALSE else */
	ppk_boolean ppk_is_locked();
	
	/*! \brief Lock the library with the given password.
	 * \param password The password that will lock the library
	* \return Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_set_password(const char* password);
	
	/*! \brief Try to unlock the library using the right password
	 * \param password The password to try to unlock the library
	* \return Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_unlock(const char* password);

	/*! \brief Get a string from an error code explaining the error code
	* \param error_code The error code you would like to be translated into human-readable text
	* \return  Return the string corresponding to the error code (should not be freed)*/
	const char *ppk_error_get_string(ppk_error error_code);

	/*! \brief Get ppk's settings directory
	* \return  Return the settings directory of ppk*/
	const char* ppk_settings_directory();

#ifdef __cplusplus 
}
#endif

#endif
