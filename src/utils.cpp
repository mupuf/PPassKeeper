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
#include "sha512.h"
#include "ppasskeeper-dir.h"
#include "stdio.h"

//local definitions
enum State {undefined, unlocked, locked };

/*********************************************************************
 *                                                                   *
 *                          Local Functions                          *
 *                                                                   *
 *********************************************************************/
void sha512(char* hash, const void* data, unsigned int length)
{
	uint8_t	sha512[SHA512_HASH_SIZE];
	SHA512Context s;

	SHA512Init(&s);
	SHA512Update(&s, data, length);
	SHA512Final(&s,sha512);

	for(int i = 0 ; i< SHA512_HASH_SIZE; i++)
		snprintf(hash + i *2,SHA512_HASH_SIZE+7,"%02x",sha512[i]);
}

State& cState()
{
	static State state=undefined;
	return state;
}

ppk_error grantAccess(const char* pwd="")
{
	char hash_pwd[SHA512_HASH_SIZE*2+1];
	char hash_file[SHA512_HASH_SIZE*2+1];

	std::string path=setting_dir()+"/lock";
	FILE* f=fopen(path.c_str(), "r");
	if(f!=NULL)
	{
		int ret=fread(hash_file, sizeof(char), sizeof(hash_file),f);
		fclose(f);

		if(ret<=0)
		{
			cState()=unlocked;
			return PPK_OK;
		}

		sha512(hash_pwd, pwd, strlen(pwd));

		if(strcmp(hash_pwd, hash_file)==0)
		{
			cState()=unlocked;
			return PPK_OK;
		}
		else
		{
			cState()=locked;
			return PPK_CANNOT_UNLOCK;
		}
	}
	else
	{
		cState()=unlocked;

		/*
		 * Here is the problem for people who wants this "security",
		 * you just have to delete the file and all the security is gone ...
		 */
		return PPK_NOT_LOCKED;
	}
}

bool isLocked()
{
	if(cState() == undefined)
		grantAccess();

	return cState()==locked;
}


//Public functions
extern "C"
{
	ppk_error ppk_set_password(const char* pwd)
	{
		if(!isLocked())
		{
			char hash_pwd[SHA512_HASH_SIZE*2+1];
			
			std::string path=setting_dir()+"/lock";
			FILE* f=fopen(path.c_str(), "w");
			if(f!=NULL)
			{
				sha512(hash_pwd, pwd, strlen(pwd));
				fwrite(hash_pwd, sizeof(char), sizeof(hash_pwd),f);
				fclose(f);

				return PPK_OK;
			}
			else
				return PPK_FILE_CANNOT_BE_ACCESSED;
		}
		else
			return PPK_NOT_LOCKED;
	}

	ppk_boolean ppk_is_locked()
	{
		return isLocked()?PPK_TRUE:PPK_FALSE;
	}

	ppk_error ppk_unlock(const char* password)
	{
		return grantAccess(password);
	}

	const char* ppk_settings_directory()
	{
		static std::string settingDir=setting_dir();
		return settingDir.c_str();
	}


	#ifdef I18N
	#include <libintl.h>
	#endif
	const char *ppk_error_get_string(ppk_error error_code)
	{
		const char *msg = NULL;

		switch(error_code)
		{
			case PPK_OK:
				msg="Everything went fine";
				break;
			case PPK_UNKNOWN_ERROR:
				msg="Unknown error";
				break;
			case PPK_UNSUPPORTED_METHOD:
				msg="Unsupported method";
				break;
			case PPK_INVALID_ARGUMENTS:
				msg="Invalid arguments";
				break;
			case PPK_LOCKED_NO_ACCESS:
				msg="PPassKeeper is locked, access denied";
				break;
			case PPK_ALREADY_LOCKED:
				msg="PPassKeeper is already locked";
				break;
			case PPK_CANNOT_LOCK:
				msg="PPassKeeper could not be locked (check your permissions)";
				break;
			case PPK_CANNOT_UNLOCK:
				msg="PPassKeeper could not be unlocked (it is not locked)";
				break;
			case PPK_NOT_LOCKED:
				msg="PPassKeeper could not be unlocked (it is not locked)";
				break;
			case PPK_MODULE_UNAVAILABLE:
				msg="No such module is available";
				break;
			case PPK_CANNOT_SAVE_PARAM:
				msg="The parameter cannot be saved";
				break;
			case PPK_CANNOT_GET_PARAM:
				msg="The requested parameter could not be retrieved";
				break;
			case PPK_CANNOT_REMOVE_PARAM:
				msg="The requested parameter could not be deleted";
				break;
			case PPK_CANNOT_VALIDATE_PARAM:
				msg="The submited parameter cannot be validated";
				break;
			case PPK_ENTRY_UNAVAILABLE:
				msg="The requested entry could not be retrieved";
				break;
			case PPK_UNKNOWN_ENTRY_TYPE:
				msg="The requested entry has un unknown type";
				break;
			case PPK_FILE_CANNOT_BE_ACCESSED:
				msg="A file cannot be accessed. Check your permissions";
				break;
			case PPK_INSUFFICIENT_MEMORY:
				msg="Insufficient memory";
				break;
			case PPK_CANNOT_OPEN_PASSWORD_MANAGER:
				msg="Cannot open the requested password manager";
				break;
			case PPK_INVALID_PASSWORD:
				msg="The password given to the module is invalid";
				break;
			case PPK_USER_CANCELLED:
				msg="User pressed cancel";
				break;
			case PPK_INCOMPATIBLE_FLAGS:
				msg="The requested flags are incompatible with the chosen method";
				break;
		}

		#ifdef I18N
		return msg ? gettext(msg) : gettext("<Undefined error>");
		#else
		return msg?msg:"<Undefined error>";
		#endif
	}
}
