#ifndef PPASSWORDKEEPER_MODULE_H
#define PPASSWORDKEEPER_MODULE_H

#include "settings.h"
#include <string>

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

	/*
	This function should get the previously stored password using setNetworkPassword.
	You should consider the association of server, port and username as a primary key.
	parameters : 
	server = hostname/IP of the network service
	port = port of the service
	username = username of the service
	
	return : The function should return the password or NULL if something went wrong.
	*/
	typedef const char* (*_getNetworkPassword)(const char* server, int port, const char* username);

	/*
	This function should store the password that will be read using getNetworkPassword.
	You should consider the association of server, port and username as a primary key.
	parameters : 
	server = hostname/IP of the network service
	port = port of the service
	username = username of the service
	pwd = the password to be stored
	
	return : The function should return 0 if it succeded, any other value else.
	*/
	typedef int (*_setNetworkPassword)(const char* server, int port, const char* username,  const char* pwd);

	
	/*
	This function should get the previously stored password using setApplicationPassword.
	You should consider the association of server, port and username as a primary key.
	parameters : 
	application_name = The name of your application
	username = username of the service
	
	return : The function should return the password or NULL if something went wrong.
	*/
	typedef const char* (*_getApplicationPassword)(const char* application_name, const char* username);
	
	/*
	This function should store the password that will be read using getNetworkPassword.
	You should consider the association of server, port and username as a primary key.
	parameters : 
	application_name = The name of your application
	username = username of the service
	pwd = the password to be stored
	
	return : The function should return 0 if it succeded, any other value else.
	*/
	typedef int (*_setApplicationPassword)(const char* application_name, const char* username,  const char* pwd);


	/*
	This function should get the previously stored password using setItem.
	parameters : 
	key = something unique
	
	return : The function should return the password or NULL if something went wrong.
	*/
	typedef const char* (*_getitem)(const char* key);

	/*
	This function should get the previously stored password using setItem.
	parameters : 
	key = something unique
	item = the secret to be stored
	
	return : The function should return 0 if it succeded, any other value else.
	*/
	typedef int (*_setItem)(const char* key, const char* item);
	
	/*
	This function should return a human-readable string describing what caused the last error
	*/
	typedef const char* (*_getLastError)();

	
	struct _module
	{
		void* dlhandle;
		const char* id;
		const char* display_name;
		_getModuleID getModuleID;
		_getModuleName getModuleName;
		_getABIVersion getABIVersion;
		_getNetworkPassword getNetworkPassword;
		_setNetworkPassword setNetworkPassword;
		_getApplicationPassword getApplicationPassword;
		_setApplicationPassword setApplicationPassword;
		_getitem getItem;
		_setItem setItem;
		_getLastError getLastError;
	};
#ifdef __cplusplus 
}
#endif

#endif
