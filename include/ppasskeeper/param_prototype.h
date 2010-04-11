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

#ifndef PPK_PARAM_PROTO_H
#define PPK_PARAM_PROTO_H

/**
 * \file param_prototype.h
 * \author MùPùF - Martin Peres (martin dot peres at ensi-bourges dot fr)
 * \date 13-03-2010
 */

#include "ppasskeeper.h"

///Parameter type
typedef enum
{
	ppk_proto_string_param=1,
	ppk_proto_int_param=2,
	ppk_proto_float_param=3,
	ppk_proto_file_param=4,
	ppk_proto_directory_param=5,
	ppk_proto_module_param=6,
	ppk_proto_ranged_int_param=7,
	ppk_proto_ranged_float_param=8,
	ppk_proto_list_param=9,
	ppk_proto_validated_string_param=10,
	ppk_proto_boolean_param=11
} ppk_param_type;

///Parameters of the 'ppk_file_param' parameter type
typedef struct
{
	///File filter. Syntax: "Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml);;All files (*.*)"
	const char* file_filter;
} ppk_proto_param_file;

///Parameters of the 'ppk_module_param' parameter type
typedef struct
{
	///Allow this parameter to reference its module_id
	ppk_boolean allow_self;
	
	///Only allow writable modules
	ppk_boolean writable_only;
	
	///Only allow modules with a security level above or equal min_sec_level
	ppk_security_level min_sec_level;
	
	///Only allow modules having at least this read flags
	unsigned int needed_read_flags;
	
	///Only allow modules having at least this write flags
	unsigned int needed_write_flags;
	
	///Only allow modules having at least this listing flags
	unsigned int needed_listing_flags;
} ppk_proto_param_module;

///Parameters of the 'ppk_ranged_int_param' parameter type
typedef struct
{
	///The lowest integer of the range
	int lowest;
	
	///The greatest integer of the range
	int greatest;
} ppk_proto_param_ranged_int;

///Parameters of the 'ppk_ranged_float_param' parameter type
typedef struct
{
	///The lowest integer of the range
	int lowest;
	
	///The greatest integer of the range
	int greatest;
} ppk_proto_param_ranged_float;

///Parameters of the 'ppk_proto_list_param' parameter type
typedef struct
{
	///Null terminated string list
	const char** list;
} ppk_proto_param_list;

///Parameters of the 'ppk_validated_string_param' parameter type
typedef struct
{
	///The regexp which will validate the string
	const char* validation_regexp;
} ppk_proto_param_validated_string;

///Definition of the prototype of a parameter
typedef struct
{
	///The expected type of the final parameter (string, float or integer)
	cvariant_type expected_type;
	///The final use of the parameter (file, directory, module, ranged_int, ranged_float, ...)
	ppk_param_type user_type;
	
	///Name of the parameter
	const char* name;
	///Contextual help of the parameter
	const char* help_text;
	///Default value of the parameter
	cvariant default_value;

	///Group in which belongs this parameter
	const ppk_settings_group* group;
	
	///parameters type
	union
	{
		///Parameters of the 'ppk_file_param' parameter type
		ppk_proto_param_file file_params;
		///Parameters of the 'ppk_module_param' parameter type
		ppk_proto_param_module module_params;
		///Parameters of the 'ppk_ranged_int_param' parameter type
		ppk_proto_param_ranged_int ranged_int_params;
		///Parameters of the 'ppk_ranged_float_param' parameter type
		ppk_proto_param_ranged_float ranged_float_params;
		///Parameters of the 'ppk_ranged_float_param' parameter type
		ppk_proto_param_list list_params;
		///Parameters of the 'ppk_validated_string_param' parameter type
		ppk_proto_param_validated_string validated_string_params;
	};
	
} ppk_proto_param;

#ifdef __cplusplus 
extern "C"
{
#endif

//Groups creation and deletion
/*! \brief Create a settings group
* \param display_name The name of the group which will be shown to the user
* \param description The description of the purpose of the group
* \return  Return the newly created group*/
ppk_settings_group* ppk_settings_group_create(const char *display_name, const char *description);

/*! \brief Free a settings group
* \param group The group to be freed*/
void ppk_settings_group_free(ppk_settings_group* group);

//Prototype of parameters creation
/*! \brief Create a prototype of a string parameter
* \param name The name of the parameter (must be unique)
* \param help_text The help text to be displayed to the user
* \param default_value The default value of the parameter
* \param group The settings group of the parameter
* \return  Return the prototype of the parameter*/
ppk_proto_param* ppk_param_proto_create_string(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group);

/*! \brief Create a prototype of an int parameter
* \param name The name of the parameter (must be unique)
* \param help_text The help text to be displayed to the user
* \param default_value The default value of the parameter
* \param group The settings group of the parameter
* \return  Return the prototype of the parameter*/
ppk_proto_param* ppk_param_proto_create_int(const char* name, const char* help_text, int default_value, const ppk_settings_group *group);

/*! \brief Create a prototype of a float parameter
* \param name The name of the parameter (must be unique)
* \param help_text The help text to be displayed to the user
* \param default_value The default value of the parameter
* \param group The settings group of the parameter
* \return  Return the prototype of the parameter*/
ppk_proto_param* ppk_param_proto_create_float(const char* name, const char* help_text, float default_value, const ppk_settings_group *group);

/*! \brief Create a prototype of a file parameter (get a filepath from the user)
* \param name The name of the parameter (must be unique)
* \param help_text The help text to be displayed to the user
* \param default_value The default value of the parameter
* \param group The settings group of the parameter
* \param file_filter The file filter. Syntax: "Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml);;All files (*.*)"
* \return  Return the prototype of the parameter*/
ppk_proto_param* ppk_param_proto_create_file(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group, const char* file_filter);

/*! \brief Create a prototype of a directory parameter (get a directory path from the user)
* \param name The name of the parameter (must be unique)
* \param help_text The help text to be displayed to the user
* \param default_value The default value of the parameter
* \param group The settings group of the parameter
* \return  Return the prototype of the parameter*/
ppk_proto_param* ppk_param_proto_create_directory(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group);

/*! \brief Create a prototype of a directory parameter (get a directory path from the user)
* \param name The name of the parameter (must be unique)
* \param help_text The help text to be displayed to the user
* \param default_value The default value of the parameter
* \param group The settings group of the parameter
* \param allow_self Should the module having this parameter be listed ?
* \param writable_only Should only writable module be listed ?
* \param min_sec_level The minimum security level required for a module to be listed
* \param needed_read_flags The needed read flags required for a module to be listed
* \param needed_write_flags The needed write flags required for a module to be listed
* \param needed_listing_flags The needed listing flags required for a module to be listed
* \return  Return the prototype of the parameter*/
ppk_proto_param* ppk_param_proto_create_module(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group, ppk_boolean allow_self, ppk_boolean writable_only, ppk_security_level min_sec_level, unsigned int needed_read_flags, unsigned int needed_write_flags, unsigned int needed_listing_flags);

/*! \brief Create a prototype of a ranged-int parameter
* \param name The name of the parameter (must be unique)
* \param help_text The help text to be displayed to the user
* \param default_value The default value of the parameter
* \param group The settings group of the parameter
* \param lowest The minimum value the int should have
* \param greatest The maximum value the int should have
* \return  Return the prototype of the parameter*/
ppk_proto_param* ppk_param_proto_create_ranged_int(const char* name, const char* help_text, int default_value, const ppk_settings_group *group, int lowest, int greatest);

/*! \brief Create a prototype of a ranged-float parameter
* \param name The name of the parameter (must be unique)
* \param help_text The help text to be displayed to the user
* \param default_value The default value of the parameter
* \param group The settings group of the parameter
* \param lowest The minimum value the int should have
* \param greatest The maximum value the int should have
* \return  Return the prototype of the parameter*/
ppk_proto_param* ppk_param_proto_create_ranged_float(const char* name, const char* help_text, double default_value, const ppk_settings_group *group, double lowest, double greatest);

/*! \brief Create a prototype of a list parameter
* \param name The name of the parameter (must be unique)
* \param help_text The help text to be displayed to the user
* \param default_value The default value of the parameter
* \param group The settings group of the parameter
* \param list A null terminated list (eg char** list={"object 1", "object2", NULL};)
* \return  Return the prototype of the parameter*/
ppk_proto_param* ppk_param_proto_create_list(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group, const char** list);

/*! \brief Create a prototype of a validated string parameter
* \param name The name of the parameter (must be unique)
* \param help_text The help text to be displayed to the user
* \param default_value The default value of the parameter
* \param group The settings group of the parameter
* \param validation_regexp The regexp that should be matched in order for the param. to be valid.
* \return  Return the prototype of the parameter*/
ppk_proto_param* ppk_param_proto_create_validated_string(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group, const char* validation_regexp);

/*! \brief Create a prototype of a boolean parameter
* \param name The name of the parameter (must be unique)
* \param help_text The help text to be displayed to the user
* \param default_value The default value of the parameter
* \param group The settings group of the parameter
* \return  Return the prototype of the parameter*/
ppk_proto_param* ppk_param_proto_create_boolean(const char* name, const char* help_text, cvariant_bool default_value, const ppk_settings_group *group);

/*! \brief Free a prototype
* \param proto_param The prototype to be freed*/
void ppk_param_proto_free(ppk_proto_param* proto_param);

//Accessors
/*! \brief Get the expected type of a prototype parameter(int, float or string)
* \param proto The prototype to get the info from
* \return The expected type of a prototype parameter(int, float or string)*/
cvariant_type ppk_param_proto_expected_type(const ppk_proto_param* proto);

/*! \brief Get the user type of a prototype parameter(file, module, list, etc...)
* \param proto The prototype to get the info from
* \return The user type of a prototype parameter*/
ppk_param_type ppk_param_proto_user_type(const ppk_proto_param* proto);

/*! \brief Get the name of a prototype parameter
* \param proto The prototype to get the info from
* \return The name of a prototype parameter*/
const char* ppk_param_proto_name(const ppk_proto_param* proto);

/*! \brief Get the help text of a prototype parameter
* \param proto The prototype to get the info from
* \return The help text of a prototype parameter*/
const char* ppk_param_proto_help_text(const ppk_proto_param* proto);

/*! \brief Get the default value of a prototype parameter
* \param proto The prototype to get the info from
* \return The default value of a prototype parameter*/
cvariant ppk_param_proto_default_value(const ppk_proto_param* proto);

/*! \brief Get the settings group of a prototype parameter
* \param proto The prototype to get the info from
* \return The settings group of a prototype parameter*/
const ppk_settings_group* ppk_param_proto_settings_group(const ppk_proto_param* proto);

/*! \brief Get the file parameter of a file prototype parameter
* \param proto The prototype to get the info from
* \return the file parameters or NULL if proto is not a file prototype parameter*/
const ppk_proto_param_file* ppk_param_proto_file_params(const ppk_proto_param* proto);

/*! \brief Get the module parameters of a module prototype parameter
* \param proto The prototype to get the info from
* \return the module parameters or NULL if proto is not a module prototype parameter*/
const ppk_proto_param_module* ppk_param_proto_module_params(const ppk_proto_param* proto);

/*! \brief Get the ranged int parameters of a ranged int prototype parameter
* \param proto The prototype to get the info from
* \return the ranged int parameters or NULL if proto is not a ranged int prototype parameter*/
const ppk_proto_param_ranged_int* ppk_param_proto_ranged_int_params(const ppk_proto_param* proto);

/*! \brief Get the ranged float parameters of a ranged float prototype parameter
* \param proto The prototype to get the info from
* \return the ranged float parameters or NULL if proto is not a ranged float prototype parameter*/
const ppk_proto_param_ranged_float* ppk_param_proto_ranged_float_params(const ppk_proto_param* proto);

/*! \brief Get the list parameters of a list prototype parameter
* \param proto The prototype to get the info from
* \return the list parameters or NULL if proto is not a list prototype parameter*/
const ppk_proto_param_list* ppk_param_proto_list_params(const ppk_proto_param* proto);

/*! \brief Get the validated string parameters of a validated-string prototype parameter
* \param proto The prototype to get the info from
* \return the validated string parameters or NULL if proto is not a validated-string prototype parameter*/
const ppk_proto_param_validated_string* ppk_param_proto_validated_string_params(const ppk_proto_param* proto);

//Validation
/*! \brief Validate a module parameter (does it comply with its prototype ?)
* \param module_id The module that contains this parameter
* \param params The prototype's parameter
* \param module_value The value to be validated
* \return PPK_TRUE if the parameter is valid, PPK_FALSE otherwise*/
ppk_boolean ppk_param_module_validation(const char* module_id, const ppk_proto_param_module* params, const char* module_value);

/*! \brief Validate a ranged-int parameter (does it comply with its prototype ?)
* \param module_id The module that contains this parameter
* \param params The prototype's parameter
* \param value The value to be validated
* \return PPK_TRUE if the parameter is valid, PPK_FALSE otherwise*/
ppk_boolean ppk_param_ranged_int_validation(const char* module_id, const ppk_proto_param_ranged_int* params, int value);

/*! \brief Validate a ranged-float parameter (does it comply with its prototype ?)
* \param module_id The module that contains this parameter
* \param params The prototype's parameter
* \param value The value to be validated
* \return PPK_TRUE if the parameter is valid, PPK_FALSE otherwise*/
ppk_boolean ppk_param_ranged_float_validation(const char* module_id, const ppk_proto_param_ranged_float* params, double value);

/*! \brief Validate a list parameter (does it comply with its prototype ?)
* \param module_id The module that contains this parameter
* \param params The prototype's parameter
* \param value The value to be validated
* \return PPK_TRUE if the parameter is valid, PPK_FALSE otherwise*/
ppk_boolean ppk_param_list_validation(const char* module_id, const ppk_proto_param_list* params, const char* value);

/*! \brief Validate a parameter (does it comply with its prototype ?)
* \param module_id The module that contains this parameter
* \param name The name of the parameter to be validated
* \param value The value of the parameter to be validated
* \return PPK_TRUE if the parameter is valid, PPK_FALSE otherwise*/
ppk_boolean ppk_param_validation(const char* module_id, const char* name, const cvariant value);


#ifdef __cplusplus
}
#endif

#endif