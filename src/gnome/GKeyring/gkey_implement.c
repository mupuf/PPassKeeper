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
const char* keyring_name="ppk_gkeyring";

GnomeKeyringPasswordSchema network = {
  GNOME_KEYRING_ITEM_GENERIC_SECRET,
  {
	   { "username", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
	   { "host", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
	   { "port", GNOME_KEYRING_ATTRIBUTE_TYPE_UINT32 },
	   { NULL, 0 }
  }
};

GnomeKeyringPasswordSchema application = {
  GNOME_KEYRING_ITEM_GENERIC_SECRET,
  {
	   { "username", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
	   { "app_name", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
	   { NULL, 0 }
  }
};

GnomeKeyringPasswordSchema item = {
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

ppk_boolean comparePwdSchema(GnomeKeyringPasswordSchema* a, GnomeKeyringPasswordSchema* b)
{
	if(a->item_type==b->item_type)
	{
		int i=0;
		while(i<32 && a->attributes[i].name!=NULL && b->attributes[i].name!=NULL)
		{
			if(strcmp(a->attributes[i].name, b->attributes[i].name)!=0)
				return PPK_FALSE;
			
			if(a->attributes[i].type!=b->attributes[i].type)
				return PPK_FALSE;
				
			i++;
		}
		
		if(a->attributes[i].name==NULL && a->attributes[i].name==b->attributes[i].name)
			return PPK_TRUE;
	}
	
	return PPK_FALSE;
}

ppk_boolean createPpkKeyring(const char* keyring_name)
{
	g_set_application_name("ppk_gkeyring");
	
	//Create the keyring and let the user enter his password for it
	GnomeKeyringResult res=gnome_keyring_create_sync(keyring_name, NULL);
	
	return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
}





/* **************************************************
 *                                                  *
 *                      Functions                   *
 *                                                  *
 * **************************************************/
ppk_boolean getListItems(const char* keyring_name, GList **found)
{
	createPpkKeyring(keyring_name);
	
	GnomeKeyringResult res=gnome_keyring_list_item_ids_sync(keyring_name, found);
	
	if(res!=GNOME_KEYRING_RESULT_OK)
		setError(gnome_keyring_result_to_message(res));
	
	return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
}

unsigned int getEntryListCount(unsigned int entry_types, unsigned int flags)
{
	unsigned int count=0;
	GnomeKeyringItemInfo *info;
	GList* list;
	
	getListItems(keyring_name, &list);
	
	printf("Size of the list = %i\n", g_list_length(list));
	
	GnomeKeyringResult res = gnome_keyring_item_get_info_sync(keyring_name, GPOINTER_TO_UINT(list->data), &info); 
	
	if(res!=GNOME_KEYRING_RESULT_OK)
		setError(gnome_keyring_result_to_message(res));
	
	char* pwd=gnome_keyring_item_info_get_secret(info);
	//printf("secret = %s\n", pwd);
	free(pwd);
	
	g_list_free(list);
	
	return count;
}

unsigned int getEntryList(unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags)
{
	if(0)
	{
	}
	else
		return 0;
}

int setNetworkPassword(const char* host, const char* login, unsigned short port, const ppk_data edata, unsigned int flags)
{
	char displayName[101];
	snprintf(displayName, sizeof(displayName), "%s@%s:%i", login, host, port);
	
	createPpkKeyring(keyring_name);
	GnomeKeyringResult res=gnome_keyring_store_password_sync(&network, keyring_name, displayName, edata.string, "username", login,"host", host, "port", port, NULL);
	
	if(res!=GNOME_KEYRING_RESULT_OK)
		setError(gnome_keyring_result_to_message(res));
									 
	return res==GNOME_KEYRING_RESULT_OK;
}

int getNetworkPassword(const char* host, const char* login, unsigned short port, ppk_data *edata, unsigned int flags)
{
	static char* pwd[501];
	
	edata->type=ppk_string;
	edata->string=(char*)pwd;
	
	createPpkKeyring(keyring_name);
	
	gchar* ret=NULL;
	GnomeKeyringResult res=gnome_keyring_find_password_sync(&network, &ret, "username", login,"host", host, "port", port, NULL, 0);

	if(res!=GNOME_KEYRING_RESULT_OK)
	{
		setError(gnome_keyring_result_to_message(res));
	}
	else
	{
		strncpy(pwd, ret, sizeof(pwd)-1);
		gnome_keyring_free_password(ret);
	}
	
	return res==GNOME_KEYRING_RESULT_OK;
}

int setApplicationPassword(const char* appName, const char* user, const ppk_data edata, unsigned int flags)
{
	char displayName[101];
	snprintf(displayName, sizeof(displayName), "%s@%s", user, appName);
	
	createPpkKeyring(keyring_name);
	GnomeKeyringResult res=gnome_keyring_store_password_sync(&application, keyring_name, displayName, edata.string, "username", user,"app_name", appName, NULL);
	
	if(res!=GNOME_KEYRING_RESULT_OK)
		setError(gnome_keyring_result_to_message(res));
									 
	return res==GNOME_KEYRING_RESULT_OK;
}

int getApplicationPassword(const char* appName, const char* user, ppk_data *edata, unsigned int flags)
{
	static char* pwd[501];
	
	edata->type=ppk_string;
	edata->string=(char*)pwd;
	
	createPpkKeyring(keyring_name);
	
	gchar* ret=NULL;
	GnomeKeyringResult res=gnome_keyring_find_password_sync(&application, keyring_name, &ret, "username", user,"app_name", appName, NULL);

	if(res!=GNOME_KEYRING_RESULT_OK)
	{
		setError(gnome_keyring_result_to_message(res));
	}
	else
	{
		strncpy(pwd, ret, sizeof(pwd)-1);
		gnome_keyring_free_password(ret);
	}
	
	return res==GNOME_KEYRING_RESULT_OK;
}

int setItem(const char* item, const ppk_data edata, unsigned int flags)
{
	char displayName[101];
	snprintf(displayName, sizeof(displayName), "%s", item);
	
	createPpkKeyring(keyring_name);
	GnomeKeyringResult res=gnome_keyring_store_password_sync(&application, keyring_name, displayName, edata.string, "item", item, NULL);
	
	if(res!=GNOME_KEYRING_RESULT_OK)
		setError(gnome_keyring_result_to_message(res));
									 
	return res==GNOME_KEYRING_RESULT_OK;
}

int getItem(const char* item, ppk_data *edata, unsigned int flags)
{
	static char* pwd[501];
	
	edata->type=ppk_string;
	edata->string=(char*)pwd;
	
	createPpkKeyring(keyring_name);
	
	gchar* ret=NULL;
	GnomeKeyringResult res=gnome_keyring_find_password_sync(&application, keyring_name, &ret, "item", item, NULL);

	if(res!=GNOME_KEYRING_RESULT_OK)
	{
		setError(gnome_keyring_result_to_message(res));
	}
	else
	{
		strncpy(pwd, ret, sizeof(pwd)-1);
		gnome_keyring_free_password(ret);
	}
	
	return res==GNOME_KEYRING_RESULT_OK;
}
