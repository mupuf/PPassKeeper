#ifndef PPASSWORDKEEPER_H
#define PPASSWORDKEEPER_H
//
#ifdef __cplusplus 
extern "C"
{
#endif
	//Create some enums
	typedef enum
	{
		PPK_FALSE=0,PPK_TRUE=1
	} ppk_boolean;

	typedef enum
	{
		ppk_sec_lowest=0,		//Example: Stored in a plain-text file
		ppk_sec_scrambled=1,		//Example: Stored in a poorly-encrypted file
		ppk_sec_safe=2,			//Example: Keyrings/Wallets
		ppk_sec_perfect=3		//Example: Prompt users to key-in their password
	} ppk_security_level;

	typedef enum
    {
            ppk_network=1,
            ppk_application=2,
            ppk_item=4
    } ppk_entry_type;

	typedef enum
    {
            ppk_string=1,
            ppk_blob=2
    } ppk_data_type;

	typedef enum {
		ppk_rf_none=0,
		ppk_rf_silent=1			//Will retrieve the password silently or abort, but won't prompt anything to users
	} ppk_readFlag;

	typedef enum {
		ppk_wf_none=0,
		ppk_wf_silent=1			//Will save the password silently or abort, but won't prompt anything to users
	} ppk_writeFlag;

	typedef enum {
		ppk_lf_none=0,
		ppk_lf_silent=1			//Will list passwords silently or abort, but won't prompt anything to users
	} ppk_listingFlag;

	///Module's definition
	typedef struct
	{
		///Module's id
		const char* id;

		///Module's description to be displayed to user
		const char* display_name;
	} ppk_module;

	typedef struct
        {
                const char* host;          
                const char* login;         
                unsigned short int port;   
        } ppk_entry_net;

        typedef struct
        {
                const char* app_name;      
                const char* username;      
        } ppk_entry_app;                                 

        typedef struct
        {
                ppk_entry_type type;
                union
                {
                        ppk_entry_net net;
                        ppk_entry_app app;
                        const char *item;
                };
        } ppk_entry;

        typedef struct
        {
                void *data;
                unsigned long size;
        } ppk_data_blob;

        typedef struct
        {
                ppk_data_type type;
                union
                {
                        const char *string;
                        ppk_data_blob blob;
                };
        } ppk_data;


	/*! \brief Get the count of available modules
	* \return Return the count of available modules */
	unsigned int ppk_getAvailableModulesCount();

	/*! \brief Get a list of available modules
	* \param modules out: Array of PPassKeeper_Module that will store the list of modules.
	* \param y in: Size of the array modules.
	* \return  Return the count of available modules*/
	unsigned int ppk_getAvailableModules(ppk_module* modules, unsigned int nbModules); //returns the number of modules

	/*! \brief Tells whether a module is available or not given its ID
	* \param module_id in: Module's ID.
	* \return  Return BTRUE if the module is available, BFALSE else.*/
        ppk_boolean ppk_moduleAvailable(const char* module_id);

	
        /*! \brief returns supported reading flags for a given module
	* \param module_id in: Module's ID.
	* \return  Return available reading flags. See readFlag for more information about flags.*/
	ppk_readFlag ppk_readFlagsAvailable(const char* module_id);

	/*! \brief returns supported writing flags for a given module
	* \param module_id in: Module's ID.
	* \return  Return available read flags. See writeFlag for more information about flags.*/
	ppk_writeFlag ppk_writeFlagsAvailable(const char* module_id);

	/*! \brief returns supported password listing flags for a given module
	* \param module_id in: Module's ID.
	* \return  Return available listing flags. See listingFlag for more information about flags.*/
	ppk_listingFlag ppk_listingFlagsAvailable(const char* module_id);

	ppk_boolean ppk_getEntry(const char *module_id, const ppk_entry entry, ppk_data *edata, unsigned int flags);
	ppk_boolean ppk_setEntry(const char *module_id, const ppk_entry entry, const ppk_data edata, unsigned int flags);
	ppk_boolean ppk_removeEntry(const char* module_id, const ppk_entry entry, unsigned int flags);

    unsigned int ppk_getEntryListCount(const char* module_id, unsigned int entry_types, unsigned int flags);
    unsigned int ppk_getEntryList(const char* module_id, unsigned int entry_types, ppk_entry *entryList, unsigned int nbEntries, unsigned int flags);

	ppk_boolean ppk_entryExists(const char* module_id, const ppk_entry entry, unsigned int flags);
        
	/*! \brief Tells whether a module are writable or not. It may be a stupid question given the name of the library,
	/* but some modules just allow you to key-in your password. For these modules, you are the keeper.
	* \param module_id in: Module's ID.
	* \return  Return BTRUE if the module is writable, BFALSE else.*/
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
	* \return  Return BTRUE if the module is compatible with changing the prompt message, BFALSE else.*/
	ppk_boolean ppk_setCustomPromptMessage(const char* module_id, const char* customMessage);
	
	
	/****************************************************************************************************/
	/****************************************************************************************************/
	/*                                                                                                  */
	/*                                       Convenient functions                                       */
	/*                                                                                                  */
	/****************************************************************************************************/
	/****************************************************************************************************/
	//host, login, app_name and item must be const char* and must last in time
	ppk_entry createNetworkEntry(const char* host, const char* login, unsigned int port);
	ppk_entry createAppEntry(const char* app_name, const char* username);
	ppk_entry createItemEntry(const char* item);
     
	ppk_data createStringData(const char* string);
	ppk_data createBlobData(void* data, unsigned long size);

#ifdef __cplusplus 
}
#endif

//
#endif
