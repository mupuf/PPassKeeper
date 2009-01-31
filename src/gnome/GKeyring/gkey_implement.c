#include "gnome-keyring.h"
#include "../../ppasskeeper.h"
#include "string.h"

int setNetworkPassword(const char* host, const char* login, unsigned short port, const ppk_data edata, unsigned int flags)
{
	guint32 item_id;
	//g_set_application_name("PPassKeeper");
	
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
