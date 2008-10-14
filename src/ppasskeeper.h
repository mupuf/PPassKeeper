#ifndef PPASSWORDKEEPER_H
#define PPASSWORDKEEPER_H
//
#ifdef __cplusplus 
extern "C"
{
#endif
	//Create some enums
	enum ppk_boolean
	{
		PPK_FALSE=0,PPK_TRUE=1
	};

	enum ppk_security_level
	{
		ppk_sec_lowest=0,		//Example: Stored in a plain-text file
		ppk_sec_scrambled=1,	//Example: Stored in a poorly-encrypted file
		ppk_sec_safe=2,			//Example: Keyrings/Wallets
		ppk_sec_perfect=3		//Example: Prompt users to key-in their password
	};

	enum ppk_password_type
	{
		ppk_network=0, ppk_application=1, ppk_item=2
	};

	enum ppk_readFlag {
		ppk_rf_none=0,
		ppk_rf_silent=1			//Will retrieve the password silently or abort, but won't prompt anything to users
	};

	enum ppk_writeFlag {
		ppk_wf_none=0,
		ppk_wf_silent=1			//Will save the password silently or abort, but won't prompt anything to users
	};

	enum ppk_listingFlag {
		ppk_lf_none=0,
		ppk_lf_silent=1			//Will list passwords silently or abort, but won't prompt anything to users
	};

	///Module's definition
	struct PPassKeeper_Module
	{
		///Module's id
		const char* id;

		///Module's description to be displayed to user
		const char* display_name;
	};

	///Passwords's listing
	struct PPassKeeper_module_entry_net
	{
		const char* host;
		const char* login;
		unsigned short int port;
	};

	struct PPassKeeper_module_entry_app
	{
		const char* app_name;
		const char* username;
	};

	struct PPassKeeper_module_entry_item
	{
		const char* key;
	};

	/*! \brief Get the count of available modules
	* \return Return the count of available modules */
	unsigned int ppk_getAvailableModulesCount();

	/*! \brief Get a list of available modules
	* \param modules out: Array of PPassKeeper_Module that will store the list of modules.
	* \param y in: Size of the array modules.
	* \return  Return the count of available modules*/
	unsigned int ppk_getAvailableModules(struct PPassKeeper_Module* modules, unsigned int nbModules); //returns the number of modules

	/*! \brief Tells whether a module is available or not given its ID
	* \param module_id in: Module's ID.
	* \return  Return BTRUE if the module is available, BFALSE else.*/
	enum ppk_boolean ppk_moduleAvailable(const char* module_id);

	/*! \brief returns supported reading flags for a given module
	* \param module_id in: Module's ID.
	* \return  Return available reading flags. See readFlag for more information about flags.*/
	enum ppk_readFlag ppk_readFlagsAvailable(const char* module_id);

	/*! \brief returns supported writing flags for a given module
	* \param module_id in: Module's ID.
	* \return  Return available read flags. See writeFlag for more information about flags.*/
	enum ppk_writeFlag ppk_writeFlagsAvailable(const char* module_id);

	/*! \brief returns supported password listing flags for a given module
	* \param module_id in: Module's ID.
	* \return  Return available listing flags. See listingFlag for more information about flags.*/
	enum ppk_listingFlag ppk_listingFlagsAvailable(const char* module_id);



	/*! \brief returns the password corresponding to username@server:port for a given module and retrieval flags
	* \param module_id in: Module's ID.
	* \param server in: Server's hostname/IP.
	* \param port in: Port where the service is listenning.
	* \param username in: Granted username on the service.
	* \param flags in: See ppk_readFlag in order to know which flags are supported.
	* \return  Returns the password whenever it succeed, NULL else.*/
	const char* ppk_getNetworkPassword(const char* module_id, const char* server, int port, const char* username, unsigned int flags);
	
	/*! \brief Save the password corresponding to username@server:port.
	* \param module_id in: Module's ID.
	* \param server in: Server's hostname/IP.
	* \param port in: Port where the service is listenning.
	* \param username in: Granted username on the service.
	* \param pwd in: Password to be stored.
	* \param flags in: See ppk_writeFlag in order to know which flags are supported.
	* \return  Returns PPK_TRUE in case of success, PPK_FALSE else.*/
	enum ppk_boolean ppk_setNetworkPassword(const char* module_id, const char* server, int port, const char* username,  const char* pwd, unsigned int flags);
	
	/*! \brief Delete the password corresponding to username@server:port.
	* \param module_id in: Module's ID.
	* \param server in: Server's hostname/IP.
	* \param port in: Port where the service is listenning.
	* \param username in: Granted username on the service.
	* \param flags in: See ppk_listingFlag in order to know which flags are supported.
	* \return  Returns PPK_TRUE in case of success, PPK_FALSE else.*/
	enum ppk_boolean ppk_removeNetworkPassword(const char* module_id, const char* server, int port, const char* username, unsigned int flags);
	



	/*! \brief returns the password corresponding to username@application_name for a given module and retrieval flags
	* \param module_id in: Module's ID.
	* \param application_name in: Application's name.
	* \param flags in: See ppk_readFlag in order to know which flags are supported.
	* \return  Returns the password whenever it succeed, NULL else.*/
	const char* ppk_getApplicationPassword(const char* module_id, const char* application_name, const char* username, unsigned int flags);
	
	/*! \brief Save the password corresponding to username@application_name.
	* \param module_id in: Module's ID.
	* \param application_name in: Application's name.
	* \param username in: Granted username on the service.
	* \param pwd in: Password to be stored.
	* \param flags in: See ppk_writeFlag in order to know which flags are supported.
	* \return  Returns PPK_TRUE in case of success, PPK_FALSE else.*/
	enum ppk_boolean ppk_setApplicationPassword(const char* module_id, const char* application_name, const char* username,  const char* pwd, unsigned int flags);
	
	/*! \brief Delete the password corresponding to username@application_name.
	* \param module_id in: Module's ID.
	* \param application_name in: Application's name.
	* \param username in: Granted username on the service.
	* \param flags in: See ppk_listingFlag in order to know which flags are supported.
	* \return  Returns PPK_TRUE in case of success, PPK_FALSE else.*/
	enum ppk_boolean ppk_removeApplicationPassword(const char* module_id, const char* application_name, const char* username, unsigned int flags);




	/*! \brief Returns the item corresponding to the key for a given module and retrieval flags
	* \param module_id in: Module's ID.
	* \param key in: The Key.
	* \param flags in: See ppk_readFlag in order to know which flags are supported.
	* \return  Returns the password whenever it succeed, NULL else.*/
	const char* ppk_getItem(const char* module_id, const char* key, unsigned int flags);
	
	/*! \brief Save an item corresponding to a key.
	* \param module_id in: Module's ID.
	* \param key in: The Key.
	* \param item in: Item to be stored.
	* \param flags in: See ppk_writeFlag in order to know which flags are supported.
	* \return  Returns PPK_TRUE in case of success, PPK_FALSE else.*/
	enum ppk_boolean ppk_setItem(const char* module_id, const char* key, const char* item, unsigned int flags);
	
	/*! \brief Delete the password corresponding to the key.
	* \param module_id in: Module's ID.
	* \param key in: The Key.
	* \param flags in: See ppk_listingFlag in order to know which flags are supported.
	* \return  Returns PPK_TRUE in case of success, PPK_FALSE else.*/
	enum ppk_boolean ppk_removeItem(const char* module_id, const char* key, unsigned int flags);
	
	
	

	/*! \brief Tells whether a module are writable or not. It may be a stupid question given the name of the library,
	/* but some modules just allow you to key-in your password. For these modules, you are the keeper.
	* \param module_id in: Module's ID.
	* \return  Return BTRUE if the module is writable, BFALSE else.*/
	enum ppk_boolean ppk_isWritable(const char* module_id);

	/*! \brief How secure is the module ?
	* \param module_id in: Module's ID.
	* \return  Return the security level. See security_level for more information about it.*/
	enum ppk_security_level ppk_securityLevel(const char* module_id);


	/*! \brief Tells how many passwords are stored in a particulier module with a particular type.
	* \param module_id in: Module's ID.
	* \param type in: Type you would like to be counted.
	* \return  Return how many password are available.*/
	unsigned int ppk_getPasswordListCount(const char* module_id, enum ppk_password_type type, unsigned int flags);

	/*! \brief Tells how many passwords are stored in a particulier module with a particular type.
	* \param module_id in: Module's ID.
	* \param type in: Type you would like to be counted.
	* \return  Return how many password are available (WARNING : It can be greater than maxPasswordCount !).*/
	unsigned int ppk_getPasswordList(const char* module_id, enum ppk_password_type type, void* pwdList, unsigned int maxPasswordCount, unsigned int flags);

	//Errors
	const char* ppk_getLastError(const char* module_id);

	/****************************************************************************************************/
	/****************************************************************************************************/
	/*																									*/
	/*											OPTIONNAL !												*/
	/*																									*/
	/****************************************************************************************************/
	/****************************************************************************************************/
	
	/*! \brief Allow to change the default password prompt message and replace it by your own.
	* \param module_id in: Module's ID.
	* \param customMessage in: Message to be used
	* \return  Return BTRUE if the module is compatible with changing the prompt message, BFALSE else.*/
	enum ppk_boolean ppk_setCustomPromptMessage(const char* module_id, const char* customMessage);

#ifdef __cplusplus 
}
#endif

//
#endif
