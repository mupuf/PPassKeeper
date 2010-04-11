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

#include "gnome-keyring.h"
#include <ppasskeeper.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


/* **************************************************
 *                                                  *
 *                      Constants                   *
 *                                                  *
 * **************************************************/
const gchar* keyring_name="ppk_gkeyring";

GnomeKeyringPasswordSchema network_schm = {
  GNOME_KEYRING_ITEM_GENERIC_SECRET,
  {
	   { "username", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
	   { "host", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
	   { "port", GNOME_KEYRING_ATTRIBUTE_TYPE_UINT32 },
	   { "protocol", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
	   { NULL, 0 }
  }
};

GnomeKeyringPasswordSchema application_schm = {
  GNOME_KEYRING_ITEM_GENERIC_SECRET,
  {
	   { "username", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
	   { "app_name", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
	   { NULL, 0 }
  }
};

GnomeKeyringPasswordSchema item_schm = {
  GNOME_KEYRING_ITEM_GENERIC_SECRET,
  { 
	   { "item", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
	   { NULL, 0 }
  }
};


/* **************************************************
 *                                                  *
 *                  Local functions                 *
 *                                                  *
 * **************************************************/

#include <stdio.h>
ppk_boolean openKeyring(unsigned int flags)
{
	GnomeKeyringInfo *info;
	ppk_boolean res=PPK_FALSE;
	
	//Get infos
	GnomeKeyringResult res_info=gnome_keyring_get_info_sync (keyring_name, &info);
	
	if(res_info==GNOME_KEYRING_RESULT_NO_SUCH_KEYRING && (flags&ppk_rf_silent)==0)
	{
		//Create the keyring and let the user enter his password for it
		GnomeKeyringResult created=gnome_keyring_create_sync(keyring_name, NULL);
		
		if(created==GNOME_KEYRING_RESULT_OK)
			res=PPK_TRUE;
	}
	else if(res_info==GNOME_KEYRING_RESULT_OK && gnome_keyring_info_get_is_locked(info)==TRUE && (flags&ppk_rf_silent)==0)
	{
		//Unlock
		GnomeKeyringResult unlocked=gnome_keyring_unlock_sync (keyring_name, NULL);
		
		if(unlocked==GNOME_KEYRING_RESULT_OK)
			res=PPK_TRUE;
	}
	else if(res_info==GNOME_KEYRING_RESULT_OK && gnome_keyring_info_get_is_locked(info)==FALSE)		
		res=PPK_TRUE;
	
	//free the infos
	gnome_keyring_info_free(info);
	
	return res;
}

/* **************************************************
 *                                                  *
 *                      Functions                   *
 *                                                  *
 * **************************************************/
void constructor(void)
{
	const char *application_name = NULL;
	application_name = g_get_application_name();
	if (!application_name)
		g_set_application_name("ppk_gkeyring");
}

void destructor(void)
{

}

//char** getItemList(unsigned int flags)
ppk_error getItemList(char*** name_list, unsigned int flags)
{
	GnomeKeyringItemInfo *info;
	GList* list;
	
	*name_list=NULL;
	
	//Create the keyring (shall soon move to somewhere else)
	if(openKeyring(flags)==PPK_TRUE)
	{
		//List the ids of the ppasskeeper keyring
		GnomeKeyringResult res=gnome_keyring_list_item_ids_sync(keyring_name, &list);
		if(res!=GNOME_KEYRING_RESULT_OK)
			return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
		
		(*name_list)=calloc(sizeof(char*), g_list_length(list)+1);
		
		unsigned int i=0;
		while(list!=NULL)
		{
			//Get the name
			GnomeKeyringResult res = gnome_keyring_item_get_info_sync(keyring_name, GPOINTER_TO_UINT(list->data), &info); 
			if(res==GNOME_KEYRING_RESULT_OK)
			{
				(*name_list)[i]=gnome_keyring_item_info_get_display_name(info);
				list=g_list_next(list);
				i++;
			}
		}
		
		name_list[i]=NULL;
		g_list_free(list);
	}
		
	return PPK_OK;
}

ppk_error setNetworkPassword(const char* host, const char* login, unsigned short port, const char* protocol, const ppk_data* edata, unsigned int flags)
{
	GnomeKeyringResult res=-100;
	char displayName[101];
	snprintf(displayName, sizeof(displayName)-1, "%s://%s@%s:%i", protocol, login, host, port);
	
	if(openKeyring(flags)==PPK_TRUE)
	{
		res=gnome_keyring_store_password_sync(&network_schm, keyring_name, displayName, edata->string, "username", login,"host", host, "port", port, "protocol", protocol, (char *)NULL, (char *)NULL);
		
		if(res!=GNOME_KEYRING_RESULT_OK)
			fprintf(stderr, "Gnome Keyring Error: %s\n", gnome_keyring_result_to_message(res));
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;

	return res==GNOME_KEYRING_RESULT_OK?PPK_OK:PPK_UNKNOWN_ERROR;
}

ppk_error getNetworkPassword(const char* host, const char* login, unsigned short port, const char* protocol, ppk_data** edata, unsigned int flags)
{
	if(openKeyring(flags)==PPK_TRUE)
	{
		gchar* ret_buf;
		GnomeKeyringResult res=gnome_keyring_find_password_sync(&network_schm, &ret_buf, "username", login,"host", host, "port", port, "protocol", protocol, (char *)NULL, (char *)NULL);

		if(res==GNOME_KEYRING_RESULT_OK)
		{
			*edata=ppk_string_data_new(ret_buf);
		}
		else
			fprintf(stderr, "Gnome Keyring Error: %s\n", gnome_keyring_result_to_message(res));

		if(ret_buf!=NULL)
			gnome_keyring_free_password(ret_buf);
	
		return res==GNOME_KEYRING_RESULT_OK?PPK_OK:PPK_UNKNOWN_ERROR;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

ppk_error setApplicationPassword(const char* appName, const char* user, const ppk_data* edata, unsigned int flags)
{
	char displayName[101];
	snprintf(displayName, sizeof(displayName), "%s@%s", user, appName);
	
	if(openKeyring(flags)==PPK_TRUE)
	{
		GnomeKeyringResult res=gnome_keyring_store_password_sync(&application_schm, keyring_name, displayName, edata->string, "username", user,"app_name", appName, (char *)NULL, (char *)NULL);
		
		if(res!=GNOME_KEYRING_RESULT_OK)
			fprintf(stderr, "Gnome Keyring Error: %s\n", gnome_keyring_result_to_message(res));
			
		return res==GNOME_KEYRING_RESULT_OK?PPK_OK:PPK_UNKNOWN_ERROR;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

ppk_error getApplicationPassword(const char* appName, const char* user, ppk_data** edata, unsigned int flags)
{
	if(openKeyring(flags)==PPK_TRUE)
	{
		gchar* ret_buf;
		GnomeKeyringResult res=gnome_keyring_find_password_sync(&application_schm, &ret_buf, "username", user,"app_name", appName, (char *)NULL, (char *)NULL);

		if(res==GNOME_KEYRING_RESULT_OK)
			*edata=ppk_string_data_new(ret_buf);

		if(ret_buf!=NULL)
			gnome_keyring_free_password(ret_buf);
	
		return res==GNOME_KEYRING_RESULT_OK?PPK_OK:PPK_UNKNOWN_ERROR;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

ppk_error setItem(const char* item, const ppk_data* edata, unsigned int flags)
{
	char displayName[101];
	snprintf(displayName, sizeof(displayName), "%s", item);
	
	if(openKeyring(flags)==PPK_TRUE)
	{
		GnomeKeyringResult res=gnome_keyring_store_password_sync(&item_schm, keyring_name, displayName, edata->string, "item", item, (char *)NULL, (char *)NULL);
		
		if(res!=GNOME_KEYRING_RESULT_OK)
			fprintf(stderr, "Gnome Keyring Error: %s\n", gnome_keyring_result_to_message(res));
		
		return res==GNOME_KEYRING_RESULT_OK?PPK_OK:PPK_UNKNOWN_ERROR;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

ppk_error getItem(const char* item, ppk_data** edata, unsigned int flags)
{
	if(openKeyring(flags)==PPK_TRUE)
	{

		gchar* ret_buf;
		GnomeKeyringResult res=gnome_keyring_find_password_sync(&item_schm, &ret_buf, "item", item, (char *)NULL, (char *)NULL);

		if(res==GNOME_KEYRING_RESULT_OK)
			*edata=ppk_string_data_new(ret_buf);
		else
			fprintf(stderr, "Gnome Keyring Error: %s\n", gnome_keyring_result_to_message(res));

		if(ret_buf!=NULL)
			gnome_keyring_free_password(ret_buf);
	
		return res==GNOME_KEYRING_RESULT_OK?PPK_OK:PPK_UNKNOWN_ERROR;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

ppk_error removeNetworkPassword(const char* host, const char* login, unsigned short port, const char* protocol, unsigned int flags)
{
	if(openKeyring(flags)==PPK_TRUE)
	{
		GnomeKeyringResult res=gnome_keyring_delete_password_sync(&network_schm, "username", login,"host", host, "port", port, "protocol", protocol, (char *)NULL, (char *)NULL);
		
		if(res!=GNOME_KEYRING_RESULT_OK)
			fprintf(stderr, "Gnome Keyring Error: %s\n", gnome_keyring_result_to_message(res));
		
		return res==GNOME_KEYRING_RESULT_OK?PPK_OK:PPK_UNKNOWN_ERROR;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

ppk_error removeApplicationPassword(const char* appName, const char* user, unsigned int flags)
{
	if(openKeyring(flags)==PPK_TRUE)
	{
		GnomeKeyringResult res=gnome_keyring_delete_password_sync(&application_schm, "username", user,"app_name", appName, (char *)NULL, (char *)NULL);
		
		if(res!=GNOME_KEYRING_RESULT_OK)
			fprintf(stderr, "Gnome Keyring Error: %s\n", gnome_keyring_result_to_message(res));
		
		return res==GNOME_KEYRING_RESULT_OK?PPK_OK:PPK_UNKNOWN_ERROR;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}

ppk_error removeItem(const char* item, unsigned int flags)
{
	if(openKeyring(flags)==PPK_TRUE)
	{
		GnomeKeyringResult res=gnome_keyring_delete_password_sync(&item_schm, "item", item, (char *)NULL, (char *)NULL);
		
		if(res!=GNOME_KEYRING_RESULT_OK)
			fprintf(stderr, "Gnome Keyring Error: %s\n", gnome_keyring_result_to_message(res));
		
		return res==GNOME_KEYRING_RESULT_OK?PPK_OK:PPK_UNKNOWN_ERROR;
	}
	else
		return PPK_CANNOT_OPEN_PASSWORD_MANAGER;
}
