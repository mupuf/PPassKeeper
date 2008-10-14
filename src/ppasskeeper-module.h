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

	/*
	This function should get the previously stored password using setNetworkPassword.
	You should consider the association of server, port and username as a primary key.
	parameters : 
	server = hostname/IP of the network service
	port = port of the service
	username = username of the service
	flags : Flags to use when trying to perform the function
	
	return : The function should return the password or NULL if something went wrong.
	*/
	typedef const char* (*_getNetworkPassword)(const char* server, int port, const char* username, unsigned int flags);


	/*
	This function should store the password that will be read using getNetworkPassword.
	You should consider the association of server, port and username as a primary key.
	parameters : 
	server = hostname/IP of the network service
	port = port of the service
	username = username of the service
	pwd = the password to be stored
	flags : Flags to use when trying to perform the function
	
	return : The function should return 0 if it succeded, any other value else.
	*/
	typedef ppk_boolean (*_setNetworkPassword)(const char* server, int port, const char* username,  const char* pwd, unsigned int flags);
	
	/*
	This function should delete the password identified by username@server:port
	parameters : 
	server = hostname/IP of the network service
	port = port of the service
	username = username of the service
	flags : Flags to use when trying to perform the function
	
	return : Return PPK_TRUE when it succeeds, PPK_FALSE else.
	*/
	typedef ppk_boolean (*_removeNetworkPassword)(const char* server, int port, const char* username, unsigned int flags);


	/*
	This function should get the previously stored password using setApplicationPassword.
	You should consider the association of server, port and username as a primary key.
	parameters : 
	application_name = The name of your application
	username = username of the service
	flags : Flags to use when trying to perform the function
	
	return : The function should return the password or NULL if something went wrong.
	*/
	typedef const char* (*_getApplicationPassword)(const char* application_name, const char* username, unsigned int flags);
	
	
	/*
	This function should store the password that will be read using getNetworkPassword.
	You should consider the association of server, port and username as a primary key.
	parameters : 
	application_name = The name of your application
	username = username of the service
	pwd = the password to be stored
	flags : Flags to use when trying to perform the function
	
	return : The function should return 0 if it succeded, any other value else.
	*/
	typedef ppk_boolean (*_setApplicationPassword)(const char* application_name, const char* username,  const char* pwd, unsigned int flags);
	
	/*
	This function should delete the password identified by username@application_name
	parameters : 
	server = hostname/IP of the network service
	port = port of the service
	username = username of the service
	flags : Flags to use when trying to perform the function
	
	return : Return PPK_TRUE when it succeeds, PPK_FALSE else.
	*/
	typedef ppk_boolean (*_removeApplicationPassword)(const char* application_name, const char* username, unsigned int flags);


	/*
	This function should get the previously stored password using setItem.
	parameters : 
	key = something unique
	flags : Flags to use when trying to perform the function
	
	return : The function should return the password or NULL if something went wrong.
	*/
	typedef const char* (*_getItem)(const char* key, unsigned int flags);


	/*
	This function should get the previously stored password using setItem.
	parameters : 
	key = something unique
	item = the secret to be stored
	flags : Flags to use when trying to perform the function
	
	return : The function should return 0 if it succeded, any other value else.
	*/
	typedef ppk_boolean (*_setItem)(const char* key, const char* item, unsigned int flags);
	
	/*
	This function should delete the password identified by the key
	parameters : 
	key : The primary key
	flags : Flags to use when trying to perform the function
	
	return : Return PPK_TRUE when it succeeds, PPK_FALSE else.
	*/
	typedef ppk_boolean (*_removeItem)(const char* key, unsigned int flags);

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
	typedef unsigned int (*_getPasswordListCount)(ppk_password_type type, unsigned int flags);

	/*
	This function fills the structure of type 'type' and given into pwdList that has a size of maxModuleCount.
	Flags can be passed to the module
	*/
	typedef unsigned int (*_getPasswordList)(ppk_password_type type, void* pwdList, unsigned int maxModuleCount, unsigned int flags);

	/*
	This function should return a human-readable string describing what caused the last error
	*/
	typedef const char* (*_getLastError)();

	/****************************************************************************************************/
	/****************************************************************************************************/
	/*																									*/
	/*											OPTIONNAL !												*/
	/*																									*/
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
		_getNetworkPassword getNetworkPassword;
		_setNetworkPassword setNetworkPassword;
		_removeNetworkPassword removeNetworkPassword;
		_getApplicationPassword getApplicationPassword;
		_setApplicationPassword setApplicationPassword;
		_removeApplicationPassword removeApplicationPassword;
		_getItem getItem;
		_setItem setItem;
		_removeItem removeItem;
		_isWritable isWritable;
		_securityLevel securityLevel;
		_getPasswordListCount getPasswordListCount;
		_getPasswordList getPasswordList;
		_getLastError getLastError;
		
		//Optionnal
		_setCustomPromptMessage setCustomPromptMessage;
	};
#ifdef __cplusplus 
}
#endif

#endif
