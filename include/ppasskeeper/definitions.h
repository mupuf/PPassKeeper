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

#ifndef PPK_DEFINITIONS_H
#define PPK_DEFINITIONS_H

/**
 * \file definitions.h
 * \author MùPùF - Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 27-07-2009
 */

///Boolean 
typedef enum
{
	///FALSE
	PPK_FALSE=0,
	///TRUE
	PPK_TRUE=1
} ppk_boolean;

///Error codes
typedef enum
{
	//General
	PPK_OK = 0,
	PPK_UNKNOWN_ERROR=1,
	PPK_INVALID_ARGUMENTS=2,

	//PPK
	PPK_LOCKED_NO_ACCESS=100,
	PPK_ALREADY_LOCKED=101,
	PPK_CANNOT_LOCK=102,
	PPK_CANNOT_UNLOCK=103,
	PPK_NOT_LOCKED=104,

	//Modules
	PPK_MODULE_UNAVAILABLE=200,
	PPK_UNSUPPORTED_METHOD=201,

	//Params
	PPK_CANNOT_SAVE_PARAM=300,
	PPK_CANNOT_GET_PARAM=301,
	PPK_CANNOT_REMOVE_PARAM=302,
	PPK_CANNOT_VALIDATE_PARAM=303,
	
	//Entry
	PPK_ENTRY_UNAVAILABLE=400,
	PPK_UNKNOWN_ENTRY_TYPE=401,

	//System errors
	PPK_FILE_CANNOT_BE_ACCESSED=500,
	PPK_INSUFFICIENT_MEMORY=501,

	//Password Managers
	PPK_CANNOT_OPEN_PASSWORD_MANAGER=600,
	PPK_INVALID_PASSWORD=601,

	//User
	PPK_USER_CANCELLED=700,

	//Flags
	PPK_INCOMPATIBLE_FLAGS=800
} ppk_error;

///Security Level
typedef enum
{
	///Lowest (eg Stored in a plain-text file)
	ppk_sec_lowest=0,
	///Scrambled (eg Stored in a poorly-encrypted file)
	ppk_sec_scrambled=1,
	///Safe (eg Keyrings/Wallets)	
	ppk_sec_safe=2,		
	///Perfect (eg Prompt users to key-in their password)	
	ppk_sec_perfect=3
} ppk_security_level;

///Reading flags
typedef enum {
	///Retrieve the password by the default way
	ppk_rf_none=0,
	///Will retrieve the password silently or abort, but won't prompt anything to users
	ppk_rf_silent=1
} ppk_readFlag;

///Writing flags
typedef enum {
	///Retrieve the password by the default way
	ppk_wf_none=0,
	///Will retrieve the password silently or abort, but won't prompt anything to users
	ppk_wf_silent=1
} ppk_writeFlag;

///Listing flags
typedef enum {
	///Retrieve the password by the default way
	ppk_lf_none=0,
	///Will retrieve the password silently or abort, but won't prompt anything to users
	ppk_lf_silent=1
} ppk_listingFlag;

#endif
