#ifndef PPK_MODULE_H
#define PPK_MODULE_H

///Maximum size of saved parameters
#define PPK_PARAM_MAX 4096

///Module's definition
typedef struct
{
	///Module's id
	const char* id;

	///Module's description to be displayed to user
	const char* display_name;
} ppk_module;

///Definition of a group of parameter
typedef struct
{
	///Name of the group (meant to be displayed)
	const char *display_name;
	///Description of the group
	const char *description;
} ppk_settings_group;

///Definition of the prototype of a parameter
typedef struct
{
	///The expected type of the final parameter
	cvariant_type expected_type;
	///Name of the parameter
	const char* name;
	///Contextual help of the parameter
	const char* help_text;
	///Default value of the parameter
	cvariant default_value;

	///Group in which belongs this parameter
	const ppk_settings_group *group;
} ppk_proto_param;

///Definition of what a parameter is
typedef struct
{
	///Name of the parameter
	const char* name;
	///The value of the parameter
	cvariant value;
} ppk_param;

#ifdef __cplusplus 
extern "C"
{
#endif

	/*! \brief Get the count of available modules
	* \return Return the count of available modules */
	unsigned int ppk_module_count();

	/*! \brief Get a list of available modules
	* \param modules out: Array of PPassKeeper_Module that will store the list of modules.
	* \param nbModules in: Size of the array modules.
	* \return  Return the count of available modules*/
	size_t ppk_module_list(ppk_module* modules, unsigned int nbModules); //returns the number of modules

	/*! \brief Reloads the plugins from the plugin dir and updates the module count and list.
	* WARNING: This function is NOT thread-safe. Make sure no other thread uses ppk while calling this function.
	* \return  An error code is returned*/
	ppk_error ppk_module_reload_list();

	/*! \brief Get the default module. This parameter can be set/updated with ppk_setDefaultModule.
	* \return  Return the default module's name if there is one, NULL otherwise.*/
	const char* ppk_module_get_default();

	/*! \brief Set the default module. This parameter can be retrieved using ppk_getDefaultModule.
	* \param module_id in: Module's ID.
	* \return  Return PPK_TRUE if the module the module exists and has been set as the new default module, PPK_FALSE else.*/
	ppk_error ppk_module_set_default(const char* module_id);

	/*! \brief Tells whether a module is available or not given its ID
	* \param module_id in: Module's ID.
	* \return  Return PPK_TRUE if the module is available, PPK_FALSE else.*/
	ppk_boolean ppk_module_is_available(const char* module_id);

	/*! \brief Returns supported reading flags for a given module
	* \param module_id in: Module's ID.
	* \param flags out: Supported flags.
	* \return  Return available reading flags. See readFlag for more information about flags.*/
	ppk_error ppk_module_read_flags(const char* module_id, unsigned int* flags);

	/*! \brief Returns supported writing flags for a given module
	* \param module_id in: Module's ID.
	* \param flags out: Supported flags.
	* \return  Return available read flags. See writeFlag for more information about flags.*/
	ppk_error ppk_module_write_flags(const char* module_id, unsigned int* flags);

	/*! \brief Returns supported password listing flags for a given module
	* \param module_id in: Module's ID.
	* \param flags out: Supported flags.
	* \return  Return available listing flags. See listingFlag for more information about flags.*/
	ppk_error ppk_module_listing_flags(const char* module_id, unsigned int* flags);

	/*! \brief How secure is the module ?
	* \param module_id in: Module's ID.
	* \return  Return the security level. See security_level for more information about it.*/
	ppk_security_level ppk_module_security_level(const char* module_id);

	/*! \brief Tells whether a module are writable or not. It may be a stupid question given the name of the library,
	* but some modules just allow you to key-in your password. For these modules, you are the keeper.
	* \param module_id in: Module's ID.
	* \return  Return PPK_TRUE if the module is writable, PPK_FALSE else.*/
	ppk_boolean ppk_module_is_writable(const char* module_id);

	/*! \brief Get the maximum data size a module can handle.
	* \param module_id in: Module's ID.
	* \param type in: The type you want to know its maximum data size.
	* \return  Returns the maximum data size*/
	size_t ppk_module_max_data_size(const char* module_id, ppk_data_type type);

	/*! \brief Get the number of entries stored into a module.
	* \param module_id in: Module's ID.
	* \param entry_types in: The entry types that should be taken into account.
	* \param flags in: You can specify flags which will change the way the entry list will be got. See listingFlag.
	* \return  Returns the number of entries counted.*/
	size_t ppk_module_get_entry_count(const char* module_id, int entry_types, unsigned int flags);

	/* \brief Get entries's name stored into a module.
	* \param module_id in: Module's ID.
	* \param entry_types in: The entry types that should be taken into account.
	* \param entryList out: Entries name will be stored here.
	* \param nbEntries in: Maximum number of entries to be retrieved.
	* \param flags in: You can specify flags which will change the way the entry list will be got. See listingFlag.
	* \return  Returns the number of entries stored into entryList.
	ppk_entry_list* ppk_module_get_entry_list(const char* module_id, int entry_types, unsigned int flags);
	void ppk_module_free_entry_list(ppk_entry_list* entry_list);*/

	/*! \brief Get entries's name stored into a module.
	* \param module_id in: Module's ID.
	* \param entry_types in: The entry types that should be taken into account.
	* \param entryList out: Entries name will be stored here.
	* \param nbEntries in: Maximum number of entries to be retrieved.
	* \param flags in: You can specify flags which will change the way the entry list will be got. See listingFlag.
	* \return  Returns the number of entries stored into entryList.*/
	size_t ppk_module_get_entry_list(const char* module_id, int entry_types, ppk_entry *entryList, size_t nbEntries, unsigned int flags);

	/*! \brief Tells whether a module contains a specific entry or not.
	* \param module_id in: Module's ID.
	* \param entry in: The entry to be listed.
	* \param flags in: You can specify flags which will change the way the entry will be tested as accessible or not. See readFlags.
	* \return  Returns PPK_TRUE if the entry exists, PPK_FALSE else.*/
	ppk_error ppk_module_has_entry(const char* module_id, const ppk_entry* entry, unsigned int flags);

	/*! \brief Get an Entry from a module
	* \param module_id in: Module's ID.
	* \param entry in: The entry to get.
	* \param edata out: Result will be stored here.
	* \param flags in: You can specify flags which will change the way the entry will be retrieved. See readFlag.
	* \return  Returns PPK_TRUE if getEntry worked fine, PPK_FALSE else.*/
	ppk_error ppk_module_get_entry(const char *module_id, const ppk_entry* entry, ppk_data** edata, unsigned int flags);

	/*! \brief Set an Entry to a module
	* \param module_id in: Module's ID.
	* \param entry in: The entry to be set.
	* \param edata in: What should be stored.
	* \param flags in: You can specify flags which will change the way the entry will be set. See writeFlag.
	* \return  Returns PPK_TRUE if setEntry worked fine, PPK_FALSE else.*/
	ppk_error ppk_module_set_entry(const char *module_id, const ppk_entry* entry, const ppk_data* edata, unsigned int flags);

	/*! \brief Remove an Entry from a module
	* \param module_id in: Module's ID.
	* \param entry in: The entry to be removed.
	* \param flags in: You can specify flags which will change the way the entry will be removed. See writeFlags.
	* \return  Returns PPK_TRUE if setEntry worked fine, PPK_FALSE else.*/
	ppk_error ppk_module_remove_entry(const char* module_id, const ppk_entry* entry, unsigned int flags);

	/*! \brief Store a module parameter. This parameter can be retrieved using ppk_getParam.
	* \param module_id in: Module's ID.
	* \param key in: The name of the parameter.
	* \param value in: The value to store.
	* \return  Return PPK_TRUE if the parameter has been saved, PPK_FALSE else.*/
	ppk_error ppk_module_save_param(const char* module_id, const char* key, const cvariant value);

	/*! \brief Retrieve a module parameter. This parameter can be set/updated with ppk_saveParam.
	* \param module_id in: Module's ID.
	* \param key in: The name of the parameter.
	* \return  Return the parameter (as a cvariant) if found), a NULL cvariant otherwise (use cvariant_is_null() to test it)*/
	cvariant ppk_module_get_param(const char* module_id, const char* key);
	
	/* \brief Retrieve the parameters list of a given module.
	* \param module_id in: Module's ID.
	* \param list out: The output list.
	* \param maxEntries in: The maximum size of the list.
	* \return  Returns the number of elements in the list.
	ppk_variant_list* ppk_module_list_params(const char* module_id);*/

	/*! \brief Retrieve the parameters list of a given module.
	* \param module_id in: Module's ID.
	* \param list out: The output list.
	* \param maxEntries in: The maximum size of the list.
	* \return  Returns the number of elements in the list.*/
	size_t ppk_module_list_params(const char* module_id, const char*** list, size_t maxEntries);
	
	/*! \brief Delete a module parameter.
	* \param module_id in: Module's ID.
	* \param key in: The name of the parameter to delete.
	* \return  Return PPK_TRUE if the module is compatible with changing the prompt message, PPK_FALSE else.*/
	ppk_error ppk_module_remove_param(const char* module_id, const char* key);

	/*! \brief Retrieve the list of supported parameters's prototype (type, default value, help text) for a given module
	* \param module_id in: Module's ID.
	* \return  Return a null-terminated list of ppk_proto_param**/
	const ppk_proto_param** ppk_module_available_parameters(const char* module_id);

	/*! \brief Allow to change the default password prompt message and replace it by your own.
	* \param module_id in: Module's ID.
	* \param customMessage in: Message to be used
	* \return  Return PPK_TRUE if the module is compatible with changing the prompt message, PPK_FALSE else.*/
	ppk_error ppk_afp_set_custom_prompt_message(const char* module_id, const char* customMessage);

#ifdef __cplusplus
}
#endif

#endif
