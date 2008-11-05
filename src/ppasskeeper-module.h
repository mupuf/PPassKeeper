#ifndef PPASSWORDKEEPER_MODULE_H
#define PPASSWORDKEEPER_MODULE_H

#include "settings.h"
#include "ppasskeeper.h"
#include <string>

/**
 * \file ppasskeeper-module
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


	typedef ppk_boolean (*_getEntry)(const ppk_entry entry, ppk_data *pwd, unsigned int flags);
	typedef ppk_boolean (*_setEntry)(const ppk_entry entry, const ppk_data pwd, unsigned int flags);
	typedef ppk_boolean (*_removeEntry)(const ppk_entry entry, unsigned int flags);

	typedef ppk_boolean (*_entryExists)(const ppk_entry entry, unsigned int flags);

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

	
	/*
	This function returns the number of passwords of the type 'type' currently stored by the module.
	Flags can be passed to the module
	*/
	typedef unsigned int (*_getEntryListCount)(unsigned int entry_types, unsigned int flags);

	/*
	This function fills the structure of type 'type' and given into pwdList that has a size of maxModuleCount.
	Flags can be passed to the module
	*/
	typedef unsigned int (*_getEntryList)(unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags);

	/*
	This function should return a human-readable string describing what caused the last error
	*/
	typedef const char* (*_getLastError)();

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
		_isWritable isWritable;
		_securityLevel securityLevel;
		_getEntryListCount getEntryListCount;
		_getEntryList getEntryList;
		_getLastError getLastError;
		
		//Optionnal
		_setCustomPromptMessage setCustomPromptMessage;
	};
#ifdef __cplusplus 
}
#endif

#endif
