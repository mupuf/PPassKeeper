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
#include "ppasskeeper/data.h"
#include "libsafelocker.h"

#include <string>
#include <iostream>
#include <fstream>
#include <map>

#include <stdlib.h>
#include <string.h>

//Parameters
std::map<std::string, cvariant> parameters;
std::map<std::string, ppk_settings_group*> params_group;
std::map<std::string, ppk_proto_param*> proto_params;
ppk_proto_param** availParams;
#define PARAM_MOD_PASSPHRASE "module passphrase"
#define PARAM_CLOSING_DELAY "closing delay"
#define PARAM_SAFELOCKER_PATH "SafeLocker's filepath"
#define PARAM_FTP_USE "Use FTP"
#define PARAM_FTP_HOST "FTP host"
#define PARAM_FTP_PORT "FTP port"
#define PARAM_FTP_LOGIN "FTP login"
#define PARAM_FTP_PWD "FTP password"
#define PARAM_FTP_PATH "FTP dir path"

#define PARAM_MOD_PASSPHRASE_DEFAULT "AskForPass_Qt"
#define PARAM_CLOSING_DELAY_DEFAULT 10
#define PARAM_SAFELOCKER_PATH_DEFAULT (qPrintable(safelockDefaultPath()))
#define PARAM_FTP_USE_DEFAULT cvariant_true
#define PARAM_FTP_HOST_DEFAULT ""
#define PARAM_FTP_PORT_DEFAULT 21
#define PARAM_FTP_LOGIN_DEFAULT ""
#define PARAM_FTP_PWD_DEFAULT ""
#define PARAM_FTP_PATH_DEFAULT ""

//functions
extern "C"
{
	void constructor()
	{
		availParams=NULL;
		
		//Create the group categories
		ppk_settings_group *ppk_settings_security, *ppk_settings_network;
		
		ppk_settings_security=ppk_settings_group_create("Security", "Security related parameters");
		ppk_settings_network=ppk_settings_group_create("Network", "Network related parameters");
		
		params_group["Security"]=ppk_settings_security;
		params_group["Network"]=ppk_settings_network;
		
		//Create the parameters' prototypes
		ppk_proto_param *mod_passphrase, *close_dly, *safelocker_path, *ftp_use, *ftp_host, *ftp_port, *ftp_login, *ftp_pwd, *ftp_path;
		
		mod_passphrase=ppk_param_proto_create_module(PARAM_MOD_PASSPHRASE,
											"The ppk module you would like the passphrase to be got from.",
											PARAM_MOD_PASSPHRASE_DEFAULT,
											ppk_settings_security,
											PPK_FALSE,	//Allow self
											PPK_FALSE, //writable only
											ppk_sec_lowest,
											ppk_rf_none,
											ppk_wf_none,
											ppk_lf_none);
		proto_params[PARAM_MOD_PASSPHRASE]=mod_passphrase;
		
		close_dly=ppk_param_proto_create_ranged_int(PARAM_CLOSING_DELAY,
											"Closing delay when the module is not used. Set it to 0 if you would like it not to be closed.",
											PARAM_CLOSING_DELAY_DEFAULT,
											ppk_settings_security,
											0,
											99);
		proto_params[PARAM_CLOSING_DELAY]=close_dly;
		
		safelocker_path=ppk_param_proto_create_file(PARAM_SAFELOCKER_PATH,
											"Where would you like to save the safelock.",
											qPrintable(_safelockDefaultPath()),
											ppk_settings_security,
											"PPassKeeper's safeLocker files(*.ppksl)");
		proto_params[PARAM_SAFELOCKER_PATH]=safelocker_path;
		
		ftp_use=ppk_param_proto_create_boolean(PARAM_FTP_USE,
											"Would you like to centralize your passwords on an FTP server ?",
											PARAM_FTP_USE_DEFAULT,
											ppk_settings_network);
		proto_params[PARAM_FTP_USE]=ftp_use;
		
		ftp_host=ppk_param_proto_create_string(PARAM_FTP_HOST,
											"The hostname of your ftp server",
											PARAM_FTP_HOST_DEFAULT,
											ppk_settings_network);
		proto_params[PARAM_FTP_HOST]=ftp_host;
		
		ftp_port=ppk_param_proto_create_ranged_int(PARAM_FTP_PORT,
											"The port of your ftp server",
											PARAM_FTP_PORT_DEFAULT,
											ppk_settings_network,
											0,
											65535);
		proto_params[PARAM_FTP_PORT]=ftp_port;
		
		ftp_login=ppk_param_proto_create_string(PARAM_FTP_LOGIN,
											"The login on your ftp server",
											PARAM_FTP_LOGIN_DEFAULT,
											ppk_settings_network);
		proto_params[PARAM_FTP_LOGIN]=ftp_login;
		
		ftp_pwd=ppk_param_proto_create_string(PARAM_FTP_PWD,
											"The password on your ftp server",
											PARAM_FTP_PWD_DEFAULT,
											ppk_settings_network);
		proto_params[PARAM_FTP_PWD]=ftp_pwd;
		
		ftp_path=ppk_param_proto_create_string(PARAM_FTP_PATH,
											"The path on your ftp server where you would like to store the safelocker (must already exist)",
											PARAM_FTP_PATH_DEFAULT,
											ppk_settings_network);
		proto_params[PARAM_FTP_PATH]=ftp_path;
		
		//Get a list of available parameters
		availParams=new ppk_proto_param*[proto_params.size()+1];
		availParams[0]=mod_passphrase;
		availParams[1]=close_dly;
		availParams[2]=safelocker_path;
		availParams[3]=ftp_use;
		availParams[4]=ftp_host;
		availParams[5]=ftp_port;
		availParams[6]=ftp_login;
		availParams[7]=ftp_pwd;
		availParams[8]=ftp_path;
		availParams[9]=NULL;
		
		//Set parameters's default value
		parameters[PARAM_MOD_PASSPHRASE]=cvariant_from_string(PARAM_MOD_PASSPHRASE_DEFAULT);
 		parameters[PARAM_CLOSING_DELAY]=cvariant_from_int(PARAM_CLOSING_DELAY_DEFAULT);
		parameters[PARAM_SAFELOCKER_PATH]=cvariant_from_string(qPrintable(_safelockDefaultPath()));
		parameters[PARAM_FTP_USE]=cvariant_from_bool(PARAM_FTP_USE_DEFAULT);
		parameters[PARAM_FTP_HOST]=cvariant_from_string(PARAM_FTP_HOST_DEFAULT);
 		parameters[PARAM_FTP_PORT]=cvariant_from_int(PARAM_FTP_PORT_DEFAULT);
 		parameters[PARAM_FTP_LOGIN]=cvariant_from_string(PARAM_FTP_LOGIN_DEFAULT);
		parameters[PARAM_FTP_PWD]=cvariant_from_string(PARAM_FTP_PWD_DEFAULT);
		parameters[PARAM_FTP_PATH]=cvariant_from_string(PARAM_FTP_PATH_DEFAULT);
		
		//Set up values
		const char* module=cvariant_get_string(parameters[PARAM_MOD_PASSPHRASE]);
		_setPPKModuleForPassphrase(QString::fromUtf8(module));

		const char* path=cvariant_get_string(parameters[PARAM_SAFELOCKER_PATH]);
		_setSafeLockerPath(QString::fromUtf8(path));

		int closingDelay=cvariant_get_int(parameters[PARAM_CLOSING_DELAY]);
		_setClosingDelay(closingDelay);
	}

	void destructor()
	{
		//Free the list of parameter
		delete[] availParams;
		
		//Free the param prototypes
		std::map<std::string, ppk_proto_param*>::const_iterator itr;
		for(itr = proto_params.begin(); itr != proto_params.end(); ++itr)
			ppk_param_proto_free(itr->second);
		
		//Free the setting groups
		std::map<std::string, ppk_settings_group*>::const_iterator itr2;
		for(itr2 = params_group.begin(); itr2 != params_group.end(); ++itr2)
			ppk_settings_group_free(itr2->second);

		//Free the parameters
		std::map<std::string, cvariant>::const_iterator itr3;
		for(itr3 = parameters.begin(); itr3 != parameters.end(); ++itr3)
			cvariant_free(itr3->second);
	}
	
	const char* getModuleID()
	{
		return "PPKSafeLocker";
	}

	const char* getModuleName()
	{
		return "PPK SafeLocker - PPassKeeper's Safe Locker";
	}

	const int getABIVersion()
	{
		return 1;
	}
	
	ppk_security_level securityLevel(const char* module_id)
	{
		return ppk_sec_safe;
	}
	
	ppk_boolean isWritable()
	{
		return PPK_TRUE;
	}

	//Get available flags
	unsigned int readFlagsAvailable()
	{
		return ppk_rf_none|ppk_rf_silent;
	}

	unsigned int writeFlagsAvailable()
	{
		return ppk_wf_none|ppk_wf_silent;
	}
	
	unsigned int listingFlagsAvailable()
	{
		return ppk_lf_none|ppk_lf_silent;
	}

	ppk_error getSimpleEntryList(char*** list, unsigned int flags)
	{
		if(list==NULL)
			return PPK_INVALID_ARGUMENTS;
		
		(*list)=NULL;
		
		QList<QString> entries=_listEntries();
		
		if(entries.size()>0)
		{
			//Copy to a char** list
			(*list)=new char*[entries.size()+1];
			if((*list)!=NULL)
			{
				for(int i=0; i<entries.size(); i++)
				{
					QString entry=entries.at(i);

					(*list)[i]=(char*)malloc((entry.size()+1)*sizeof(char));
					strncpy((*list)[i], qPrintable(entry), entry.size()+1);
				}
				(*list)[entries.size()]=NULL;
			}
		}
		else
			*list=NULL;
		
		return PPK_OK;
	}

	//Get and Set passwords
	ppk_error getEntry(const ppk_entry* entry, ppk_data **edata, unsigned int flags)
	{
		return _getEntry(entry, edata, flags);
	}

	ppk_error setEntry(const ppk_entry* entry, const ppk_data* edata, unsigned int flags)
	{
		return _resetEntry(entry, edata, flags);
	}
	
	ppk_error removeEntry(const ppk_entry* entry, unsigned int flags)
	{
		return _removeEntry(entry, flags);;
	}

	ppk_boolean entryExists(const ppk_entry* entry, unsigned int flags)
	{
		ppk_data* edata=NULL;

		if(getEntry(entry, &edata, flags)==PPK_OK)
		{

			return PPK_TRUE;
		}
		else
			return PPK_FALSE;
	}
	
	unsigned int maxDataSize(ppk_data_type type)
	{
		switch(type)
		{
			case ppk_string:
				return (unsigned int)-1;
			case ppk_blob:
				return (unsigned int)-1;
		}
	
		return 0;
	}
	
	const ppk_proto_param** availableParameters()
	{
		return const_cast<const ppk_proto_param**>(availParams);
	}

	void setParam(const char* paramName, const cvariant value)
	{
		std::string key(paramName);

		if(key == PARAM_MOD_PASSPHRASE)
		{
			if(cvariant_get_type(value)==cvariant_string)
			{
				parameters[PARAM_MOD_PASSPHRASE]=value;
				
				//Update the module
				const char* module=cvariant_get_string(parameters[PARAM_MOD_PASSPHRASE]);
				_setPPKModuleForPassphrase(QString::fromUtf8(module));
			}
			else
				printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
		}
		else if(key == PARAM_CLOSING_DELAY)
		{
			if(cvariant_get_type(value)==cvariant_int)
			{
				parameters[PARAM_CLOSING_DELAY]=value;

				//Update the value
				int closingDelay=cvariant_get_int(parameters[PARAM_CLOSING_DELAY]);
				_setClosingDelay(closingDelay);
			}
			else
				printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
		}
		else if(key == PARAM_SAFELOCKER_PATH)
		{
			if(cvariant_get_type(value)==cvariant_string)
			{
				parameters[PARAM_SAFELOCKER_PATH]=value;
				
				//Update the module
				const char* path=cvariant_get_string(parameters[PARAM_SAFELOCKER_PATH]);
				_setSafeLockerPath(QString::fromUtf8(path));
			}
			else
				printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
		}
		else if(key == PARAM_FTP_USE)
		{
			if(cvariant_get_type(value)==cvariant_boolean)
				parameters[PARAM_FTP_USE]=value;
			else
				printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
		}
		else if(key == PARAM_FTP_HOST)
		{
			if(cvariant_get_type(value)==cvariant_string)
				parameters[PARAM_FTP_HOST]=value;
			else
				printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
		}
		else if(key == PARAM_FTP_PORT)
		{
			if(cvariant_get_type(value)==cvariant_int)
				parameters[PARAM_FTP_PORT]=value;
			else
				printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
		}
		else if(key == PARAM_FTP_LOGIN)
		{
			if(cvariant_get_type(value)==cvariant_string)
				parameters[PARAM_FTP_LOGIN]=value;
			else
				printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
		}
		else if(key == PARAM_FTP_PWD)
		{
			if(cvariant_get_type(value)==cvariant_string)
				parameters[PARAM_FTP_PWD]=value;
			else
				printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
		}
		else if(key == PARAM_FTP_PATH)
		{
			if(cvariant_get_type(value)==cvariant_string)
				parameters[PARAM_FTP_PATH]=value;
			else
				printf("%s: Wrong data type for the parameter '%s' !\n", getModuleID(), paramName);
		}
		else
			printf("%s: Unknown param(%s) !!\n", getModuleID(), paramName);
	}
}
