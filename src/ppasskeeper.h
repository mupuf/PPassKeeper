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
		ppk_network, ppk_application, ppk_item
	};

	enum ppk_readFlag {
		ppk_rd_none=0,
		ppk_rd_silent=1			//Will retrieve the password silently or abort, but won't prompt anything to users
	};

	enum ppk_writeFlag {
		ppk_wt_none=0,
		ppk_wt_silent=1			//Will save the password silently or abort, but won't prompt anything to users
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

	//Get and Set passwords
	const char* ppk_getNetworkPassword(const char* module_id, const char* server, int port, const char* username, unsigned int flags);
	enum ppk_boolean ppk_setNetworkPassword(const char* module_id, const char* server, int port, const char* username,  const char* pwd, unsigned int flags);
	const char* ppk_getApplicationPassword(const char* module_id, const char* application_name, const char* username, unsigned int flags);
	enum ppk_boolean ppk_setApplicationPassword(const char* module_id, const char* application_name, const char* username,  const char* pwd, unsigned int flags);
	const char* ppk_getItem(const char* module_id, const char* key, unsigned int flags);
	enum ppk_boolean ppk_setItem(const char* module_id, const char* key, const char* item, unsigned int flags);

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
	unsigned int ppk_getPasswordListCount(const char* module_id, enum ppk_password_type type);

	/*! \brief Tells how many passwords are stored in a particulier module with a particular type.
	* \param module_id in: Module's ID.
	* \param type in: Type you would like to be counted.
	* \return  Return how many password are available (WARNING : It can be greater than maxPasswordCount !).*/
	unsigned int ppk_getPasswordList(const char* module_id, enum ppk_password_type type, void* pwdList, unsigned int maxPasswordCount);

	//Errors
	const char* ppk_getLastError(const char* module_id);

#ifdef __cplusplus 
}
#endif

//
#endif
