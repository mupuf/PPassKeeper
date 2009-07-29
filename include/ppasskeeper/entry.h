#ifndef PPK_ENTRY_H
#define PPK_ENTRY_H

///Entry type
typedef enum
{
	///Network
	ppk_network=1,
	
	///Application
	ppk_application=2,
		
	///Item
	ppk_item=4
} ppk_entry_type;

///Definition of a Network entry (login\@host:port)
typedef struct
{
	///Host name of the service (UTF8)
	const char* host;
	///Login (UTF8)
	const char* login;
	///Port of the service
	unsigned short port;
	///Protocol name
	const char* protocol;
} ppk_entry_net;

///Definition of an Application entry (username\@app_name)
typedef struct
{
	///Application's name (UTF8)
	const char* app_name;
	///Username (UTF8)
	const char* username;
} ppk_entry_app;

///Definition of what is an entry, it is basically composed a type and then the right structure depending on the type
typedef struct
{
	///Entry type
	ppk_entry_type type;
	union
	{
		///Network entry
		ppk_entry_net net;
		///Application entry
		ppk_entry_app app;
		///Item entry, it is actually just a string
		const char *item;
	};
} ppk_entry;

/*typedef struct
{
	ppk_entry* entries;
	size_t len;
} ppk_entry_list;*/

#ifdef __cplusplus 
extern "C"
{
#endif

	/*! \brief Generate a Network entry given a host, a login and a port.
	* \param host in: The host (example : mupuf.org).
	* \param login in: The login name (example : mupuf).
	* \param port in: The service's port (example : 21, 0 means the default port).
	* \param port in: The protocol name (example : ftp, NULL means unspecified)
	* \return  Return the ppk_Entry corresponding to the parameters*/
	ppk_entry* ppk_network_entry_new(const char* host, const char* login, unsigned int port, const char* protocol);
	
	/*! \brief Generate an application entry given a username and an application name.
	* \param app_name in: The name of the application (example : PPassKeeper).
	* \param username in: The user name (example : mupuf).
	* \return  Return the ppk_Entry corresponding to the parameters*/
	ppk_entry* ppk_application_entry_new(const char* app_name, const char* username);
	
	/*! \brief Generate an item entry given an item name.
	* \param item in: The item (example : MyWPAKey).
	* \return  Return the ppk_Entry corresponding to the parameters*/
	ppk_entry* ppk_item_entry_new(const char* item);

	void ppk_entry_free(ppk_entry* entry);

	/* \brief Generate a Network entry given a host, a login and a port.WARNING : Destroying original variables host, login, and/or protocol will result in data corruption into the created entry as no data are copied in the function (it only uses pointers).
	* \param host in: The host (example : mupuf.org).
	* \param login in: The login name (example : mupuf).
	* \param port in: The service's port (example : 21, 0 means the default port).
	* \param port in: The protocol name (example : ftp, NULL means unspecified)
	* \return  Return the ppk_Entry corresponding to the parameters*/
	ppk_entry ppk_createNetworkEntry(const char* host, const char* login, unsigned int port, const char* protocol);

	/* \brief Generate an application entry given a username and an application name.WARNING : Destroying original variables username and/or app_name will result in data corruption into the created entry as no data are copied in the function (it only uses pointers).
	* \param app_name in: The name of the application (example : PPassKeeper).
	* \param username in: The user name (example : mupuf).
	* \return  Return the ppk_Entry corresponding to the parameters*/
	ppk_entry ppk_createAppEntry(const char* app_name, const char* username);

	/* \brief Generate an item entry given an item name.
		WARNING : Destroying original variable item will result in data corruption into the created entry as no data are copied in the function (it only uses pointers).
	* \param item in: The item (example : MyWPAKey).
	* \return  Return the ppk_Entry corresponding to the parameters*/
	ppk_entry ppk_createItemEntry(const char* item);

#ifdef __cplusplus 
}
#endif

#endif
