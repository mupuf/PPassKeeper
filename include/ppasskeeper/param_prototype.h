#ifndef PPK_PARAM_PROTO_H
#define PPK_PARAM_PROTO_H

/**
 * \file param_prototype.h
 * \author MùPùF - Martin Peres (martin<dot>peres<At>ensi-bourges<dot>fr)
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
	ppk_proto_validated_string_param=10
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
	ppk_boolean allow_self;
	ppk_boolean writable_only;
	ppk_security_level min_sec_level;
	unsigned int needed_read_flags;
	unsigned int needed_write_flags;
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
ppk_settings_group* ppk_settings_group_create(const char *display_name, const char *description);
void ppk_settings_group_free(ppk_settings_group* group);

//Prototype of parameters creation
ppk_proto_param* ppk_param_proto_create_string(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group);
ppk_proto_param* ppk_param_proto_create_int(const char* name, const char* help_text, int default_value, const ppk_settings_group *group);
ppk_proto_param* ppk_param_proto_create_float(const char* name, const char* help_text, float default_value, const ppk_settings_group *group);
ppk_proto_param* ppk_param_proto_create_file(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group, const char* file_filter);
ppk_proto_param* ppk_param_proto_create_directory(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group);
ppk_proto_param* ppk_param_proto_create_module(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group, ppk_boolean allow_self, ppk_boolean writable_only, ppk_security_level min_sec_level, unsigned int needed_read_flags, unsigned int needed_write_flags, unsigned int needed_listing_flags);
ppk_proto_param* ppk_param_proto_create_ranged_int(const char* name, const char* help_text, int default_value, const ppk_settings_group *group, int lowest, int greatest);
ppk_proto_param* ppk_param_proto_create_ranged_float(const char* name, const char* help_text, double default_value, const ppk_settings_group *group, double lowest, double greatest);
ppk_proto_param* ppk_param_proto_create_list(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group, const char** list);
ppk_proto_param* ppk_param_proto_create_validated_string(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group, const char* validation_regexp);
void ppk_param_proto_free(ppk_proto_param* proto_param);

//Accessors
cvariant_type ppk_param_proto_expected_type(const ppk_proto_param* proto);
ppk_param_type ppk_param_proto_user_type(const ppk_proto_param* proto);
const char* ppk_param_proto_name(const ppk_proto_param* proto);
const char* ppk_param_proto_help_text(const ppk_proto_param* proto);
cvariant ppk_param_proto_default_value(const ppk_proto_param* proto);
const ppk_settings_group* ppk_param_proto_settings_group(const ppk_proto_param* proto);
const ppk_proto_param_file* ppk_param_proto_file_params(const ppk_proto_param* proto);
const ppk_proto_param_module* ppk_param_proto_module_params(const ppk_proto_param* proto);
const ppk_proto_param_ranged_int* ppk_param_proto_ranged_int_params(const ppk_proto_param* proto);
const ppk_proto_param_ranged_float* ppk_param_proto_ranged_float_params(const ppk_proto_param* proto);
const ppk_proto_param_list* ppk_param_proto_list_params(const ppk_proto_param* proto);
const ppk_proto_param_validated_string* ppk_param_proto_validated_string_params(const ppk_proto_param* proto);


#ifdef __cplusplus
}
#endif

#endif