#include "gnome-keyring.h"
#include "../../ppasskeeper.h"
#include "string.h"
#include "stdio.h"

void setError(char* error);


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


//Global variables
static gchar* ret_buf=NULL;

void free_ret_buf(gchar* ret_buf)
{
	if(ret_buf!=NULL)
		gnome_keyring_free_password(ret_buf);
	ret_buf=NULL;
}


/* **************************************************
 *                                                  *
 *                  Local functions                 *
 *                                                  *
 * **************************************************/
void g_setError(const gchar* error)
{
	if(error!=NULL)
		setError((char*) error);
}

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
		else
			g_setError((char*)gnome_keyring_result_to_message(res));
	}
	else if(res_info==GNOME_KEYRING_RESULT_OK && gnome_keyring_info_get_is_locked(info)==TRUE && (flags&ppk_rf_silent)==0)
	{
		//Unlock
		GnomeKeyringResult unlocked=gnome_keyring_unlock_sync (keyring_name, NULL);
		
		if(unlocked==GNOME_KEYRING_RESULT_OK)
			res=PPK_TRUE;
		else
			g_setError((char*)gnome_keyring_result_to_message(res));
	}
	else if(res_info==GNOME_KEYRING_RESULT_OK && gnome_keyring_info_get_is_locked(info)==FALSE)		
		res=PPK_TRUE;
	else
		g_setError((char*)gnome_keyring_result_to_message(res));
	
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
	g_set_application_name("ppk_gkeyring"); //Seems to be unnecessary
}

void destructor(void)
{
	free_ret_buf(ret_buf);
}

char** getItemList(unsigned int flags)
{
	GnomeKeyringItemInfo *info;
	GList* list;
	char** name_list=NULL;
	
	//Create the keyring (shall soon move to somewhere else)
	if(openKeyring(flags)==PPK_TRUE)
	{
		//List the ids of the ppasskeeper keyring
		GnomeKeyringResult res=gnome_keyring_list_item_ids_sync(keyring_name, &list);
		if(res!=GNOME_KEYRING_RESULT_OK)
		{
			g_setError(gnome_keyring_result_to_message(res));
			return name_list;
		}
		
		name_list=calloc(sizeof(char*), g_list_length(list)+1);
		unsigned int i=0;
		
		while(list!=NULL)
		{
			//Get the name
			GnomeKeyringResult res = gnome_keyring_item_get_info_sync(keyring_name, GPOINTER_TO_UINT(list->data), &info); 
			if(res!=GNOME_KEYRING_RESULT_OK)
				g_setError(gnome_keyring_result_to_message(res));
			else
			{
				name_list[i]=gnome_keyring_item_info_get_display_name(info);
				list=g_list_next(list);
				i++;
			}
		}
		
		name_list[i]=NULL;
		g_list_free(list);
	}
		
	return name_list;
}

ppk_boolean setNetworkPassword(const char* host, const char* login, unsigned short port, const ppk_data edata, unsigned int flags)
{
	GnomeKeyringResult res=-100;
	char displayName[101];
	snprintf(displayName, sizeof(displayName), "net://%s@%s:%i", login, host, port);
	
	if(openKeyring(flags)==PPK_TRUE)
	{
		res=gnome_keyring_store_password_sync(&network_schm, keyring_name, displayName, edata.string, "username", login,"host", host, "port", port, NULL);
		
		if(res!=GNOME_KEYRING_RESULT_OK)
			g_setError(gnome_keyring_result_to_message(res));
	}
									 
	return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
}

ppk_boolean getNetworkPassword(const char* host, const char* login, unsigned short port, ppk_data *edata, unsigned int flags)
{
	if(openKeyring(flags)==PPK_TRUE)
	{
		free_ret_buf(ret_buf);
		GnomeKeyringResult res=gnome_keyring_find_password_sync(&network_schm, &ret_buf, "username", login,"host", host, "port", port, NULL, 0);

		if(res==GNOME_KEYRING_RESULT_OK)
		{
			edata->type=ppk_string;
			edata->string=ret_buf;
		}
		else
			g_setError(gnome_keyring_result_to_message(res));
	
		return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
	}
	else
		return PPK_FALSE;
}

ppk_boolean setApplicationPassword(const char* appName, const char* user, const ppk_data edata, unsigned int flags)
{
	char displayName[101];
	snprintf(displayName, sizeof(displayName), "app://%s@%s", user, appName);
	
	if(openKeyring(flags))
	{
		GnomeKeyringResult res=gnome_keyring_store_password_sync(&application_schm, keyring_name, displayName, edata.string, "username", user,"app_name", appName, NULL);
	
		if(res!=GNOME_KEYRING_RESULT_OK)
			g_setError(gnome_keyring_result_to_message(res));
										 
		return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
	}
	else
		return PPK_FALSE;
}

ppk_boolean getApplicationPassword(const char* appName, const char* user, ppk_data *edata, unsigned int flags)
{
	if(openKeyring(flags)==PPK_TRUE)
	{
		free_ret_buf(ret_buf);
		GnomeKeyringResult res=gnome_keyring_find_password_sync(&application_schm, &ret_buf, "username", user,"app_name", appName, NULL);

		if(res==GNOME_KEYRING_RESULT_OK)
		{
			edata->type=ppk_string;
			edata->string=ret_buf;
		}
		else
			g_setError(gnome_keyring_result_to_message(res));
	
		return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
	}
	else
		return PPK_FALSE;
}

ppk_boolean setItem(const char* item, const ppk_data edata, unsigned int flags)
{
	char displayName[101];
	snprintf(displayName, sizeof(displayName), "itm://%s", item);
	
	if(openKeyring(flags)==PPK_TRUE)
	{
		GnomeKeyringResult res=gnome_keyring_store_password_sync(&item_schm, keyring_name, displayName, edata.string, "item", item, NULL);
	
		if(res!=GNOME_KEYRING_RESULT_OK)
			g_setError(gnome_keyring_result_to_message(res));
										 
		return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
	}
	else
		return PPK_FALSE;
}

ppk_boolean getItem(const char* item, ppk_data *edata, unsigned int flags)
{
	if(openKeyring(flags)==PPK_TRUE)
	{
		free_ret_buf(ret_buf);
		GnomeKeyringResult res=gnome_keyring_find_password_sync(&item_schm, &ret_buf, "item", item, NULL);

		if(res==GNOME_KEYRING_RESULT_OK)
		{
			edata->type=ppk_string;
			edata->string=ret_buf;
		}
		else
			g_setError(gnome_keyring_result_to_message(res));
	
		return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
	}
	else
		return PPK_FALSE;
}

ppk_boolean removeNetworkPassword(const char* host, const char* login, unsigned short port, unsigned int flags)
{
	if(openKeyring(flags)==PPK_TRUE)
	{
		GnomeKeyringResult res=gnome_keyring_delete_password_sync(&network_schm, "username", login,"host", host, "port", port, NULL);
	
		if(res!=GNOME_KEYRING_RESULT_OK)
			g_setError(gnome_keyring_result_to_message(res));
										 
		return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
	}
	else
		return PPK_FALSE;
}

ppk_boolean removeApplicationPassword(const char* appName, const char* user, unsigned int flags)
{
	if(openKeyring(flags)==PPK_TRUE)
	{
		GnomeKeyringResult res=gnome_keyring_delete_password_sync(&application_schm, "username", user,"app_name", appName, NULL);
	
		if(res!=GNOME_KEYRING_RESULT_OK)
			g_setError(gnome_keyring_result_to_message(res));
										 
		return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
	}
	else
		return PPK_FALSE;
}

ppk_boolean removeItem(const char* item, unsigned int flags)
{
	if(openKeyring(flags)==PPK_FALSE)
	{
		GnomeKeyringResult res=gnome_keyring_delete_password_sync(&item_schm, "item", item, NULL);
	
		if(res!=GNOME_KEYRING_RESULT_OK)
			g_setError(gnome_keyring_result_to_message(res));
										 
		return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
	}
	else
		return PPK_FALSE;
}
