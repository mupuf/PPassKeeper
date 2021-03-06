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

#ifndef PPASSWORDKEEPER_MODULE_H
#define PPASSWORDKEEPER_MODULE_H

#include "ppasskeeper/settings.h"
#include "ppasskeeper.h"
#include <string>

/**
 * \file ppasskeeper-module.h
 * \author MùPùF
 * \date 09-14-2008
 */


#ifdef __cplusplus 
extern "C"
{
#endif
	/*
	This function should return the module's ID to be used by ppasskeeper. Thus, it has to be unique.
	The ID should not be more than PPassKeeperModuleIDLength characters long.
	*/
	typedef const char* (*_getModuleID)(void);


	/*
	This function should return the module's name to be used by ppasskeeper. 
	It is meant to be displayed so try to find a name that would be easy to read.
	The name should not be more than PPassKeeperModuleDisplayNameLength characters long.
	*/
	typedef const char* (*_getModuleName)(void);


	/*
	This function should return the version of ppasskeeper interface's version that has been used when the module was created.
	It should be an integer higher or equal to 1.
	*/
	typedef const int (*_getABIVersion)(void);

	//Allow users to know which flags are available
	typedef ppk_readFlag (*_readFlagsAvailable)(void);
	typedef ppk_writeFlag (*_writeFlagsAvailable)(void);
	typedef ppk_listingFlag (*_listingFlagsAvailable)(void);


	typedef ppk_error (*_getEntry)(const ppk_entry* entry, ppk_data** pwd, unsigned int flags);
	typedef ppk_error (*_setEntry)(const ppk_entry* entry, const ppk_data* pwd, unsigned int flags);
	typedef ppk_error (*_removeEntry)(const ppk_entry* entry, unsigned int flags);

	typedef ppk_boolean (*_entryExists)(const ppk_entry* entry, unsigned int flags);
	
	typedef unsigned int (*_maxDataSize)(ppk_data_type type);

	/*
	This function returns wether the module can store password or not. 
	You may think it is stupid as there is Keeper in PPassKeeper, 
	but remind that user may just want a window to popup in order to key-in the already-in-mind password.
	parameters : none
	
	return : Return TRUE if the module is Writable.
	*/
	typedef ppk_boolean (*_isWritable)();

	/*
	This function returns the level of security. 
	parameters : none
	
	return : Return sec_null=0, sec_hidden=1, sec_safe=2 or sec_perfect=3.
	*/
	typedef ppk_security_level (*_securityLevel)();

	//LISTING
		/*
		Flags can be passed to the module
		*/
		typedef size_t (*_getEntryListCount)(unsigned int entry_types, unsigned int flags);

		/*
		
		Flags can be passed to the module
		*/
		typedef ppk_error (*_getEntryList)(unsigned int entry_types, ppk_entry*** entryList, size_t* nbEntries, unsigned int flags);
	//OR
		typedef ppk_error (*_getSimpleEntryList)(char*** list, unsigned int flags);

	/****************************************************************************************************/
	/****************************************************************************************************/
	/*                                                                                                  */
	/*                                              OPTIONAL                                            */
	/*                                                                                                  */
	/****************************************************************************************************/
	/****************************************************************************************************/
	
	/*
	Set a message that will be prompted to user when prompted to enter the password.
	parameters : 
		customMessage : The message to show up, NULL to go back to default message.
	
	return : Return sec_null=0, sec_hidden=1, sec_safe=2 or sec_perfect=3.
	*/
	typedef ppk_boolean (*_setCustomPromptMessage)(const char* customMessage);

	/*
	Function that will be called when the module is loaded
	return : void
	*/
	typedef void (*_constructor)(void);
	
	/*
	Function that will be called when the module is unloaded
	return : void
	*/
	typedef void (*_destructor)(void);
	
	typedef const ppk_proto_param** (*_availableParameters)();
	typedef void (*_setParam)(const char* paramName, const cvariant value);

	struct _module
	{
		void* dlhandle;
		const char* id;
		const char* display_name;
		_getModuleID getModuleID;
		_getModuleName getModuleName;
		_getABIVersion getABIVersion;
		_readFlagsAvailable readFlagsAvailable;
		_writeFlagsAvailable writeFlagsAvailable;
		_listingFlagsAvailable listingFlagsAvailable;
		_getEntry getEntry;
		_setEntry setEntry;
		_removeEntry removeEntry;
		_entryExists entryExists;
		_maxDataSize maxDataSize;
		_isWritable isWritable;
		_securityLevel securityLevel;
		_getEntryListCount getEntryListCount;
		_getEntryList getEntryList;
		_getSimpleEntryList getSimpleEntryList;
		
		//Optionnal
		_setCustomPromptMessage setCustomPromptMessage;
		_constructor constructor;
		_destructor destructor;
		_availableParameters availableParameters;
		_setParam setParam;
	};
#ifdef __cplusplus 
}
#endif

#endif
