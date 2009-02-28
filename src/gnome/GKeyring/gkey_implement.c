#include "gnome-keyring.h"
#include "../../ppasskeeper.h"
#include "string.h"

GnomeKeyringPasswordSchema network = {
  GNOME_KEYRING_ITEM_GENERIC_SECRET,
  {
	   { "username", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
	   { "host", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
	   { "port", GNOME_KEYRING_ATTRIBUTE_TYPE_STRING },
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

const char* keyring_name="ppk_gkeyring";

bool isKeyringOpen()
{
	
}


int setNetworkPassword(const char* host, const char* login, unsigned short port, const ppk_data edata, unsigned int flags)
{
	guint32 item_id;
	
	GnomeKeyringResult res=gnome_keyring_set_network_password_sync(NULL, login, "ppk", host, NULL, NULL, NULL, port, edata.string, &item_id);
										 
	return res==GNOME_KEYRING_RESULT_OK;
}

int getNetworkPassword(const char* host, const char* login, unsigned short port, ppk_data *edata, unsigned int flags)
{
	static char* pwd[501];
	
	edata->type=ppk_string;
	edata->string=(char*)pwd;
	
	GList *results;
	GnomeKeyringResult res=gnome_keyring_find_network_password_sync(login, "ppk", host, NULL, NULL, NULL, port, &results);
	
	if(res==GNOME_KEYRING_RESULT_OK)
	{
		GnomeKeyringNetworkPasswordData* d=g_list_nth_data(results, 0);
		if(d!=NULL)
			strncpy((char*)pwd, d->password, sizeof(pwd));
		else
			return 0;
			
		//do not forget to release the glist !! see gnome_keyring_find_network_password_sync for more info
	}
		
	return res==GNOME_KEYRING_RESULT_OK?1:0;
}

int setApplicationPassword(const char* appName, const char* user, const ppk_data edata, unsigned int flags)
{
	//A better implementation should be done
	return setNetworkPassword(appName, user, 0, edata, flags);
}

int getApplicationPassword(const char* appName, const char* user, ppk_data *edata, unsigned int flags)
{
	//A better implementation should be done
	return getNetworkPassword(appName, user, 0, edata, flags);
}

int setItem(const char* item, const ppk_data edata, unsigned int flags)
{
	//A better implementation should be done
	return setNetworkPassword(item, NULL, 0, edata, flags);
}

int getItem(const char* item, ppk_data *edata, unsigned int flags)
{
	//A better implementation should be done
	return getNetworkPassword(item, NULL, 0, edata, flags);
}
