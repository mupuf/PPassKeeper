/*
PPassKeeper
Copyright (C) 2008-2010 MùPùF.org

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef PPK_MODULE_H
#define PPK_MODULE_H

/**
 * \file module.h
 * \author MùPùF - Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 27-07-2009
 */

#include "ppasskeeper.h"

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

///Definition of what a parameter is
typedef struct
{
	///Name of the parameter
	const char* name;
	///The value of the parameter
	cvariant value;
} ppk_param;

#include "ppasskeeper/param_prototype.h"

#ifdef __cplusplus 
extern "C"
{
#endif

	/*! \brief Get the count of available modules
	* \return Return the count of available modules */
	unsigned int ppk_module_count();

	/*! \brief Get a list of available modules
	* \return  Returns a null-terminated list of module ID*/
	char** ppk_module_list_new();

	/*! \brief Free a module list
	* \param list the list to be freed*/
	void ppk_module_list_free(char **list);

	/*! \brief Reloads the plugins from the plugin dir and updates the module count and list.
	* WARNING: This function is NOT thread-safe. Make sure no other thread uses ppk while calling this function.
	* \return  An error code is returned*/
	ppk_error ppk_module_reload_list();

	/*! \brief Get the default module. This parameter can be set/updated with ppk_setDefaultModule.
	* \return  Return the default module's name if there is one, NULL otherwise.*/
	const char* ppk_module_get_default();

	/*! \brief Set the default module. This parameter can be retrieved using ppk_getDefaultModule.
	* \param module_id Module's ID.
	* \return  Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_set_default(const char* module_id);

	/*! \brief Tells whether a module is available or not given its ID
	* \param module_id Module's ID.
	* \return  Return PPK_TRUE if the module is available, PPK_FALSE else.*/
	ppk_boolean ppk_module_is_available(const char* module_id);

	/*! \brief Get the display name of a module
	* \param module_id Module's ID.
	* \return  The display name of the module if it exists and if the library is not locked, NULL otherwise*/
	const char* ppk_module_display_name(const char* module_id);

	/*! \brief Returns supported reading flags for a given module
	* \param module_id Module's ID.
	* \param[out] flags Supported flags.
	* \return  Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_read_flags(const char* module_id, unsigned int* flags);

	/*! \brief Returns supported writing flags for a given module
	* \param module_id Module's ID.
	* \param[out] flags Supported flags.
	* \return  Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_write_flags(const char* module_id, unsigned int* flags);

	/*! \brief Returns supported password listing flags for a given module
	* \param module_id Module's ID.
	* \param[out] flags Supported flags.
	* \return  Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_listing_flags(const char* module_id, unsigned int* flags);

	/*! \brief How secure is the module ?
	* \param module_id Module's ID.
	* \return  Return the security level. See security_level for more information about it.*/
	ppk_security_level ppk_module_security_level(const char* module_id);

	/*! \brief Tells whether a module are writable or not. It may be a stupid question given the name of the library,
	* but some modules just allow you to key-in your password. For these modules, you are the keeper.
	* \param module_id Module's ID.
	* \return Return PPK_TRUE if the module is writable, PPK_FALSE else.*/
	ppk_boolean ppk_module_is_writable(const char* module_id);

	/*! \brief Get the maximum data size a module can handle.
	* \param module_id Module's ID.
	* \param type The type you want to know its maximum data size.
	* \return Returns the maximum data size*/
	size_t ppk_module_max_data_size(const char* module_id, ppk_data_type type);

	/*! \brief Get the number of entries stored into a module.
	* \param module_id Module's ID.
	* \param entry_types The entry types that should be taken into account.
	* \param flags You can specify flags which will change the way the entry list will be got. See listingFlag.
	* \return  Returns the number of entries counted.*/
	size_t ppk_module_get_entry_count(const char* module_id, int entry_types, unsigned int flags);

	/*! \brief Get entries's name stored into a module.
	* \param module_id Module's ID.
	* \param entry_types The entry types that should be taken into account.
	* \param[out] entryList Entries name will be stored here.
	* \param[out] nbEntries Number of entries retrieved.
	* \param flags You can specify flags which will change the way the entry list will be got. See listingFlag.
	* \return  Returns the number of entries stored into entryList.*/
	ppk_error ppk_module_get_entry_list(const char* module_id, int entry_types, ppk_entry ***entryList, size_t* nbEntries, unsigned int flags);
	
	/*! \brief Free an entry list
	* \param entry_list The entry list to be freed*/
	void ppk_module_free_entry_list(ppk_entry** entry_list);

	/*! \brief Tells whether a module contains a specific entry or not.
	* \param module_id Module's ID.
	* \param entry The entry to be listed.
	* \param flags You can specify flags which will change the way the entry will be tested as accessible or not. See readFlags.
	* \param [out] result Is set to PPK_TRUE if the module has the entry, PPK_FALSE otherwise
	* \return  Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_has_entry(const char* module_id, const ppk_entry* entry, unsigned int flags, ppk_boolean* result);

	/*! \brief Get an Entry from a module
	* \param module_id Module's ID.
	* \param entry The entry to get.
	* \param[out] edata Result will be stored here.
	* \param flags You can specify flags which will change the way the entry will be retrieved. See readFlag.
	* \return  Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_get_entry(const char *module_id, const ppk_entry* entry, ppk_data** edata, unsigned int flags);

	/*! \brief Set an Entry to a module
	* \param module_id Module's ID.
	* \param entry The entry to be set.
	* \param edata What should be stored.
	* \param flags You can specify flags which will change the way the entry will be set. See writeFlag.
	* \return Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_set_entry(const char *module_id, const ppk_entry* entry, const ppk_data* edata, unsigned int flags);

	/*! \brief Remove an Entry from a module
	* \param module_id Module's ID.
	* \param entry The entry to be removed.
	* \param flags You can specify flags which will change the way the entry will be removed. See writeFlags.
	* \return Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_remove_entry(const char* module_id, const ppk_entry* entry, unsigned int flags);

	/*! \brief Store a module parameter. This parameter can be retrieved using ppk_getParam.
	* \param module_id Module's ID.
	* \param key The name of the parameter.
	* \param value The value to store.
	* \return Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_save_param(const char* module_id, const char* key, const cvariant value);

	/*! \brief Retrieve a module parameter. This parameter can be set/updated with ppk_saveParam.
	* \param module_id Module's ID.
	* \param key The name of the parameter.
	* \return Return the parameter (as a cvariant) if found), a NULL cvariant otherwise (use cvariant_is_null() to test it)*/
	cvariant ppk_module_get_param(const char* module_id, const char* key);

	/*! \brief Retrieve the parameters list of a given module.
	* \param module_id Module's ID.
	* \param[out] list The output list.
	* \return Returns the number of elements in the list.*/
	size_t ppk_module_list_params(const char* module_id, char*** list);
	
	/*! \brief Free a parameter list
	* \param list the list to be freed*/
	void ppk_module_free_params_list(char** list);
	
	/*! \brief Delete a module parameter.
	* \param module_id Module's ID.
	* \param key The name of the parameter to delete.
	* \return Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_remove_param(const char* module_id, const char* key);
	
	/*! \brief Retrieve the list of supported parameters's prototype (type, default value, help text) for a given module
	* \param module_id Module's ID.
	* \return Return a null-terminated list of ppk_proto_param**/
	const ppk_proto_param** ppk_module_available_parameters(const char* module_id);

	/*! \brief Get a parameters's prototype by name for a given module
	* \param module_id Module's ID.
	* \param name the name of the parameter
	* \return Return the concerned ppk_proto_param**/
	const ppk_proto_param* ppk_module_parameter_prototype(const char* module_id, const char* name);
	
	/*! \brief Allow to change the default password prompt message and replace it by your own.
	* \param module_id Module's ID.
	* \param customMessage Message to be used
	* \return  Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_afp_set_custom_prompt_message(const char* module_id, const char* customMessage);
	
	/*! \brief Reset all the parameters concerning a module
	* \param module_id Module's ID.
	* \return  Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_reset_params(const char* module_id);
	
	/*! \brief Clear every entry in a module
	* \param module_id Module's ID.
	* \return  Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_empty(const char* module_id);
    
    /*! \brief Import entries from a dump file to a particular module
	* \param module_id Module's ID.
	* \param importFile The file to import the entries from.
	* \return  Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_import(const char* module_id, const char* importFile);

	/*! \brief Export entries of a module to a dump file
	* \param module_id Module's ID.
	* \param exportFile The file to export the entries to.
	* \return  Return a ppk_error (PPK_OK if it succeeded).*/
	ppk_error ppk_module_export(const char* module_id, const char* exportFile);

#ifdef __cplusplus
}
#endif

#endif
