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





/* **************************************************
 *                                                  *
 *                  Local functions                 *
 *                                                  *
 * **************************************************/
void g_setError(const gchar* error)
{
	setError((char*) error);
}

void constructor(void)
{
	g_set_application_name("ppk_gkeyring");
}

void destructor(void)
{
	
}

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
	//Create the keyring and let the user enter his password for it
	GnomeKeyringResult res=gnome_keyring_create_sync(keyring_name, NULL);
	
	return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
}

/* **************************************************
 *                                                  *
 *                      Functions                   *
 *                                                  *
 * **************************************************/
char** getItemList()
{
	GnomeKeyringItemInfo *info;
	GList* list;
	
	//Create the keyring (shall soon move to somewhere else)
	createPpkKeyring(keyring_name);
	
	//List the ids of the ppasskeeper keyring
	GnomeKeyringResult res=gnome_keyring_list_item_ids_sync(keyring_name, &list);
	if(res!=GNOME_KEYRING_RESULT_OK)
		g_setError((char*)gnome_keyring_result_to_message(res));
	
	char** name_list=calloc(sizeof(char*), g_list_length(list)+1);
	unsigned int i=0;
	
	while(list!=NULL)
	{
		//Get the name
		GnomeKeyringResult res = gnome_keyring_item_get_info_sync(keyring_name, GPOINTER_TO_UINT(list->data), &info); 
		if(res!=GNOME_KEYRING_RESULT_OK)
			g_setError(gnome_keyring_result_to_message(res));
		name_list[i]=gnome_keyring_item_info_get_display_name(info);
		
		list=g_list_next(list);
		i++;
	}
	
	name_list[i]=NULL;
	g_list_free(list);
	
	return name_list;
}

ppk_boolean setNetworkPassword(const char* host, const char* login, unsigned short port, const ppk_data edata, unsigned int flags)
{
	char displayName[101];
	snprintf(displayName, sizeof(displayName), "net://%s@%s:%i", login, host, port);
	
	createPpkKeyring(keyring_name);
	GnomeKeyringResult res=gnome_keyring_store_password_sync(&network_schm, keyring_name, displayName, edata.string, "username", login,"host", host, "port", port, NULL);
	
	if(res!=GNOME_KEYRING_RESULT_OK)
		g_setError(gnome_keyring_result_to_message(res));
									 
	return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
}

ppk_boolean getNetworkPassword(const char* host, const char* login, unsigned short port, ppk_data *edata, unsigned int flags)
{
	static char pwd[1024];
	
	edata->type=ppk_string;
	edata->string=(char*)pwd;
	
	createPpkKeyring(keyring_name);
	
	gchar* ret=NULL;
	GnomeKeyringResult res=gnome_keyring_find_password_sync(&network_schm, &ret, "username", login,"host", host, "port", port, NULL, 0);

	if(res!=GNOME_KEYRING_RESULT_OK)
	{
		g_setError(gnome_keyring_result_to_message(res));
	}
	else
	{
		strncpy(pwd, (char*)ret, sizeof(pwd)-1);
		gnome_keyring_free_password(ret);
	}
	
	return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
}

ppk_boolean setApplicationPassword(const char* appName, const char* user, const ppk_data edata, unsigned int flags)
{
	char displayName[101];
	snprintf(displayName, sizeof(displayName), "app://%s@%s", user, appName);
	
	createPpkKeyring(keyring_name);
	GnomeKeyringResult res=gnome_keyring_store_password_sync(&application_schm, keyring_name, displayName, edata.string, "username", user,"app_name", appName, NULL);
	
	if(res!=GNOME_KEYRING_RESULT_OK)
		g_setError(gnome_keyring_result_to_message(res));
									 
	return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
}

ppk_boolean getApplicationPassword(const char* appName, const char* user, ppk_data *edata, unsigned int flags)
{
	static char pwd[1024];
	
	edata->type=ppk_string;
	edata->string=(char*)pwd;
	
	createPpkKeyring(keyring_name);
	
	gchar* ret=NULL;
	GnomeKeyringResult res=gnome_keyring_find_password_sync(&application_schm, &ret, "username", user,"app_name", appName, NULL);

	if(res!=GNOME_KEYRING_RESULT_OK)
	{
		g_setError(gnome_keyring_result_to_message(res));
	}
	else
	{
		strncpy(pwd, (char*)ret, sizeof(pwd)-1);
		gnome_keyring_free_password(ret);
	}
	
	return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
}

ppk_boolean setItem(const char* item, const ppk_data edata, unsigned int flags)
{
	char displayName[101];
	snprintf(displayName, sizeof(displayName), "itm://%s", item);
	
	createPpkKeyring(keyring_name);
	GnomeKeyringResult res=gnome_keyring_store_password_sync(&item_schm, keyring_name, displayName, edata.string, "item", item, NULL);
	
	if(res!=GNOME_KEYRING_RESULT_OK)
		g_setError(gnome_keyring_result_to_message(res));
									 
	return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
}

/*extern "C" ppk_boolean removeNetworkPassword(const char* host, const char* login, unsigned short port, unsigned int flags);
extern "C" ppk_boolean removeApplicationPassword(const char* appName, const char* user, unsigned int flags);
*/

ppk_boolean removeItem(const char* item, unsigned int flags)
{
	createPpkKeyring(keyring_name);
	GnomeKeyringResult res=gnome_keyring_delete_password_sync(&item_schm, "item", item, NULL);
	
	if(res!=GNOME_KEYRING_RESULT_OK)
		g_setError(gnome_keyring_result_to_message(res));
									 
	return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
}

ppk_boolean getItem(const char* item, ppk_data *edata, unsigned int flags)
{
	static char pwd[1024]; 
	
	edata->type=ppk_string;
	edata->string=(char*)pwd;
	
	createPpkKeyring(keyring_name);
	
	gchar* ret=NULL;
	GnomeKeyringResult res=gnome_keyring_find_password_sync(&item_schm, &ret, "item", item, NULL);

	if(res!=GNOME_KEYRING_RESULT_OK)
	{
		g_setError(gnome_keyring_result_to_message(res));
	}
	else
	{
		strncpy(pwd, (char*)ret, sizeof(pwd)-1);
		gnome_keyring_free_password(ret);
	}
	
	return res==GNOME_KEYRING_RESULT_OK?PPK_TRUE:PPK_FALSE;
}
