#ifndef PPASSWORDKEEPER_H
#define PPASSWORDKEEPER_H
//

/**
 * \file ppasskeeper.h
 * \author MùPùF
 * \date 11-23-2008
 */
 
#include <wchar.h>

///Maximum size of saved parameters
#define PPK_PARAM_MAX 4096
 
#ifdef __cplusplus 
extern "C"
{
#endif

	///Boolean 
	typedef enum
	{
		///FALSE
		PPK_FALSE=0,
		///TRUE
		PPK_TRUE=1
	} ppk_boolean;

	///Security Level
	typedef enum
	{
		///Lowest (eg Stored in a plain-text file)
		ppk_sec_lowest=0,
		///Scrambled (eg Stored in a poorly-encrypted file)
		ppk_sec_scrambled=1,
		///Safe (eg Keyrings/Wallets)	
		ppk_sec_safe=2,		
		///Perfect (eg Prompt users to key-in their password)	
		ppk_sec_perfect=3
	} ppk_security_level;

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

	///Data type
	typedef enum
	{
		///String
		ppk_string=1,
		
		///Binary
		ppk_blob=2
	} ppk_data_type;

	///Reading flags
	typedef enum {
		///Retrieve the password by the default way
		ppk_rf_none=0,
		///Will retrieve the password silently or abort, but won't prompt anything to users
		ppk_rf_silent=1
	} ppk_readFlag;

	///Writing flags
	typedef enum {
		///Retrieve the password by the default way
		ppk_wf_none=0,
		///Will retrieve the password silently or abort, but won't prompt anything to users
		ppk_wf_silent=1
	} ppk_writeFlag;

	///Listing flags
	typedef enum {
		///Retrieve the password by the default way
		ppk_lf_none=0,
		///Will retrieve the password silently or abort, but won't prompt anything to users
		ppk_lf_silent=1
	} ppk_listingFlag;

	///Module's definition
	typedef struct
	{
		///Module's id
		const char* id;

		///Module's description to be displayed to user
		const char* display_name;
	} ppk_module;

	///Definition of a Network entry (login\@host:port)
	typedef struct
	{
		///Host name of the service
		const char* host;      
		///Login
		const char* login;    
		///Port of the service
		unsigned short port;   
	} ppk_entry_net;

	///Definition of an Application entry (username\@app_name)
	typedef struct
	{
		///Application's name
		const char* app_name;     
		///Username 
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

	///Definition of what a blob is (basically just binary data)
	typedef struct
	{
		///A pointer
		const void *data;
		///And the size of the memory zone to store
		unsigned long size;
	} ppk_data_blob;

	///Definition of what a storable data is (Basically, binary or string data)
	typedef struct
	{
		///The type
		ppk_data_type type;
		union
		{
			///Basic text
			const char *string;
			///Binary value
			ppk_data_blob blob;
		};
	} ppk_data;

	/*! \brief Returns whether the library is locked, waiting for the right password, or not
	* \return PPK_TRUE is the library is locked, PPK_FALSE else */
	ppk_boolean ppk_isLocked();
	
	/*! \brief Lock the library with the given password.
	 * \param [in] pwd The password that will lock the library
	* \return PPK_TRUE is the library has been locked with the password, PPK_FALSE else */
	ppk_boolean ppk_setPassword(wchar_t* pwd);
	
	/*! \brief Try to unlock the library using the right password
	 * \param [in] password The password to try to unlock the library
	* \return PPK_TRUE is the library is unlocked, PPK_FALSE else */
	ppk_boolean ppk_unlock(wchar_t* password);

	/*! \brief Get the count of available modules
	* \return Return the count of available modules */
	unsigned int ppk_getAvailableModulesCount();

	/*! \brief Get a list of available modules
	* \param modules out: Array of PPassKeeper_Module that will store the list of modules.
	* \param nbModules in: Size of the array modules.
	* \return  Return the count of available modules*/
	unsigned int ppk_getAvailableModules(ppk_module* modules, unsigned int nbModules); //returns the number of modules

	/*! \brief Tells whether a module is available or not given its ID
	* \param module_id in: Module's ID.
	* \return  Return PPK_TRUE if the module is available, PPK_FALSE else.*/
    ppk_boolean ppk_moduleAvailable(const char* module_id);

	
    /*! \brief Returns supported reading flags for a given module
	* \param module_id in: Module's ID.
	* \return  Return available reading flags. See readFlag for more information about flags.*/
	unsigned int ppk_readFlagsAvailable(const char* module_id);

	/*! \brief Returns supported writing flags for a given module
	* \param module_id in: Module's ID.
	* \return  Return available read flags. See writeFlag for more information about flags.*/
	unsigned int  ppk_writeFlagsAvailable(const char* module_id);

	/*! \brief Returns supported password listing flags for a given module
	* \param module_id in: Module's ID.
	* \return  Return available listing flags. See listingFlag for more information about flags.*/
	unsigned int ppk_listingFlagsAvailable(const char* module_id);

	/*! \brief Get an Entry from a module
	* \param module_id in: Module's ID.
	* \param entry in: The entry to get.
	* \param *edata out: Result will be stored here.
	* \param flags in: You can specify flags which will change the way the entry will be retrieved. See readFlag.
	* \return  Returns PPK_TRUE if getEntry worked fine, PPK_FALSE else.*/
	ppk_boolean ppk_getEntry(const char *module_id, const ppk_entry entry, ppk_data *edata, unsigned int flags);
	
	/*! \brief Set an Entry to a module
	* \param module_id in: Module's ID.
	* \param entry in: The entry to be set.
	* \param edata in: What should be stored.
	* \param flags in: You can specify flags which will change the way the entry will be set. See writeFlag.
	* \return  Returns PPK_TRUE if setEntry worked fine, PPK_FALSE else.*/
	ppk_boolean ppk_setEntry(const char *module_id, const ppk_entry entry, const ppk_data edata, unsigned int flags);
	
	/*! \brief Remove an Entry from a module
	* \param module_id in: Module's ID.
	* \param entry in: The entry to be removed.
	* \param flags in: You can specify flags which will change the way the entry will be removed. See writeFlags.
	* \return  Returns PPK_TRUE if setEntry worked fine, PPK_FALSE else.*/
	ppk_boolean ppk_removeEntry(const char* module_id, const ppk_entry entry, unsigned int flags);

	/*! \brief Get the number of entries stored into a module.
	* \param module_id in: Module's ID.
	* \param entry_types in: The entry types that should be taken into account.
	* \param flags in: You can specify flags which will change the way the entry list will be got. See listingFlag.
	* \return  Returns the number of entries counted.*/
	unsigned int ppk_getEntryListCount(const char* module_id, unsigned int entry_types, unsigned int flags);
	
	/*! \brief Get entries's name stored into a module.
	* \param module_id in: Module's ID.
	* \param entry_types in: The entry types that should be taken into account.
	* \param entryList out: Entries name will be stored here.
	* \param nbEntries in: Maximum number of entries to be retrieved.
	* \param flags in: You can specify flags which will change the way the entry list will be got. See listingFlag.
	* \return  Returns the number of entries stored into entryList.*/
	unsigned int ppk_getEntryList(const char* module_id, unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags);

	/*! \brief Tells whether a module contains a specific entry or not.
	* \param module_id in: Module's ID.
	* \param entry in: The entry to be listed.
	* \param flags in: You can specify flags which will change the way the entry will be removed. See readFlags.
	* \return  Returns PPK_TRUE if the entry exists, PPK_FALSE else.*/
	ppk_boolean ppk_entryExists(const char* module_id, const ppk_entry entry, unsigned int flags);
	
	/*! \brief Get the maximum data size a module can handle.
	* \param module_id in: Module's ID.
	* \param type in: The type you want to know its maximum data size.
	* \return  Returns the maximum data size*/
	unsigned int ppk_maxDataSize(const char* module_id, ppk_data_type type);
        
	/*! \brief Tells whether a module are writable or not. It may be a stupid question given the name of the library,
	* but some modules just allow you to key-in your password. For these modules, you are the keeper.
	* \param module_id in: Module's ID.
	* \return  Return PPK_TRUE if the module is writable, PPK_FALSE else.*/
	ppk_boolean ppk_isWritable(const char* module_id);

	/*! \brief How secure is the module ?
	* \param module_id in: Module's ID.
	* \return  Return the security level. See security_level for more information about it.*/
	ppk_security_level ppk_securityLevel(const char* module_id);

	//Errors
	const char* ppk_getLastError(const char* module_id);

	/****************************************************************************************************/
	/****************************************************************************************************/
	/*                                                                                                  */
	/*                                              OPTIONAL                                            */
	/*                                                                                                  */
	/****************************************************************************************************/
	/****************************************************************************************************/
	
	/*! \brief Allow to change the default password prompt message and replace it by your own.
	* \param module_id in: Module's ID.
	* \param customMessage in: Message to be used
	* \return  Return PPK_TRUE if the module is compatible with changing the prompt message, PPK_FALSE else.*/
	ppk_boolean ppk_setCustomPromptMessage(const char* module_id, const char* customMessage);
	
	
	/****************************************************************************************************/
	/****************************************************************************************************/
	/*                                                                                                  */
	/*                                       Convenient functions                                       */
	/*                                                                                                  */
	/****************************************************************************************************/
	/****************************************************************************************************/

	/*! \brief Generate a Network entry given a host, a login and a port. WARNING : Destroying original variables host and/or login will result in data corruption into the created entry as no data are copied in the function (it only uses pointers).  
	* \param host in: The host (example : mupuf.org).
	* \param login in: The login name (example : mupuf).
	* \param port in: The service's port (example : 21).
	* \return  Return the ppk_Entry corresponding to the parameters*/
	ppk_entry ppk_createNetworkEntry(const char* host, const char* login, unsigned int port);
	
	/*! \brief Generate an application entry given a username and an application name. WARNING : Destroying original variables username and/or app_name will result in data corruption into the created entry as no data are copied in the function (it only uses pointers).  
	* \param app_name in: The name of the application (example : PPassKeeper).
	* \param username in: The user name (example : mupuf).
	* \return  Return the ppk_Entry corresponding to the parameters*/
	ppk_entry ppk_createAppEntry(const char* app_name, const char* username);
	
	/*! \brief Generate an item entry given an item name. WARNING : Destroying original variable item will result in data corruption into the created entry as no data are copied in the function (it only uses pointers).  
	* \param item in: The item (example : MyWPAKey).
	* \return  Return the ppk_Entry corresponding to the parameters*/
	ppk_entry ppk_createItemEntry(const char* item);
     
     /*! \brief Generate a string ppk_data. WARNING : Destroying original variable string will result in data corruption into the created ppk_data as no data are copied in the function (it only uses pointers).  
	* \param string in: The string to be stored (example : MySecretPassword).
	* \return  Return the ppk_data corresponding to the parameters*/
	ppk_data ppk_createStringData(const char* string);
	
	/*! \brief Generate a blob ppk_data. WARNING : Destroying original variable data will result in data corruption into the created ppk_data as no data are copied in the function (it only uses pointers).  
	* \param data in: The blob to be stored (example : 01110101011100101010101011010000101101111011010 (anything that's not human readable ;) ).
	* \return  Return the ppk_data corresponding to the parameters*/
	ppk_data ppk_createBlobData(void* data, unsigned long size);

	ppk_boolean ppk_saveParam(const char* module_id, const char* key, const char* value);
	ppk_boolean ppk_getParam(const char* module_id, const char* key, char* returnedString, size_t maxSize);

#ifdef __cplusplus 
}
#endif

//
#endif
