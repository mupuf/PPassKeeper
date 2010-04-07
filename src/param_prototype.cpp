#include <ppasskeeper.h>
#include <ppasskeeper/param_prototype.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

//local functions
static ppk_proto_param* ppk_param_proto_create_empty(cvariant_type type, const char* name, const char* help_text, cvariant default_value, const ppk_settings_group *group)
{
	ppk_proto_param* proto=(ppk_proto_param*)malloc(sizeof(ppk_proto_param));
	
	proto->expected_type=type;
	proto->name=strdup(name);
	proto->help_text=strdup(help_text);
	proto->default_value=default_value;
	proto->group=group;
	
	return proto;
}

static ppk_proto_param_file ppk_param_proto_create_file_parameters(const char* file_filter)
{
	ppk_proto_param_file param_file;
	param_file.file_filter=strdup(file_filter);
	return param_file;
}

static void ppk_param_proto_free_file_parameters(ppk_proto_param_file param_file)
{
	free(const_cast<char*>(param_file.file_filter));
}

static ppk_proto_param_module ppk_param_proto_create_ranged_int_parameters(ppk_boolean allow_self, ppk_boolean writable_only, ppk_security_level min_sec_level, unsigned int needed_read_flags, unsigned int needed_write_flags, unsigned int needed_listing_flags)
{
	ppk_proto_param_module param_module;
	param_module.allow_self=allow_self;
	param_module.writable_only=writable_only;
	param_module.min_sec_level=min_sec_level;
	param_module.needed_read_flags=needed_read_flags;
	param_module.needed_write_flags=needed_write_flags;
	param_module.needed_listing_flags=needed_listing_flags;
	return param_module;
}

static ppk_proto_param_ranged_int ppk_param_proto_create_ranged_int_parameters(int lowest, int greatest)
{
	ppk_proto_param_ranged_int param_ranged_int;
	param_ranged_int.lowest=lowest;
	param_ranged_int.greatest=greatest;
	return param_ranged_int;
}

static ppk_proto_param_ranged_float ppk_param_proto_create_ranged_float_parameters(int lowest, int greatest)
{
	ppk_proto_param_ranged_float param_ranged_float;
	param_ranged_float.lowest=lowest;
	param_ranged_float.greatest=greatest;
	return param_ranged_float;
}

static ppk_proto_param_list ppk_param_proto_create_list_parameters(const char** list)
{
	ppk_proto_param_list param_list;
	
	//Get the size of the list
	int len=0;
	while(list!=NULL && list[len]!=NULL)
		len++;
	
	//Copy the list
	char** n_list=(char**)malloc((len+1)*sizeof(char*));
	for(int i=0; i<len; i++)
		n_list[i]=strdup(list[i]);
	n_list[len]=NULL;
	
	//Set the list to param_file.list
	param_list.list=const_cast<const char**>(n_list);
	
	return param_list;
}

static void ppk_param_proto_free_list_parameters(ppk_proto_param_list param_list)
{
	const char** list=param_list.list;
	
	//Get the size of the list
	int i=0;
	while(list!=NULL && list[i]!=NULL)
	{
		free(const_cast<char*>(param_list.list[i]));
		i++;
	}
}

static ppk_proto_param_validated_string ppk_param_proto_create_validated_string_parameters(const char* validation_regexp)
{
	ppk_proto_param_validated_string param_validated_string;
	param_validated_string.validation_regexp=strdup(validation_regexp);
	return param_validated_string;
}

static void ppk_param_proto_free_validated_string_parameters(ppk_proto_param_validated_string param_validated_string)
{
	free(const_cast<char*>(param_validated_string.validation_regexp));
}

#ifdef __cplusplus 
extern "C"
{
#endif
	//Exported functions
	//Groups creation and deletion
	ppk_settings_group* ppk_settings_group_create(const char *display_name, const char *description)
	{
		ppk_settings_group* group=(ppk_settings_group*)malloc(sizeof(ppk_settings_group));
		
		group->display_name=strdup(display_name);
		group->description=strdup(description);
		
		return group;
	}
	void ppk_settings_group_free(ppk_settings_group* group)
	{
		free(const_cast<char*>(group->display_name));
		free(const_cast<char*>(group->description));
	}

	ppk_proto_param* ppk_param_proto_create_string(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group)
	{
		ppk_proto_param* proto=ppk_param_proto_create_empty(cvariant_string, 
															name, 
															help_text, 
															cvariant_from_string_copy(default_value, 0), 
															group);
		
		proto->user_type=ppk_proto_string_param;
		
		return proto;
	}

	ppk_proto_param* ppk_param_proto_create_int(const char* name, const char* help_text, int default_value, const ppk_settings_group *group)
	{
		ppk_proto_param* proto=ppk_param_proto_create_empty(cvariant_int, 
															name, 
															help_text, 
															cvariant_from_int(default_value), 
															group);
		
		proto->user_type=ppk_proto_int_param;
		
		return proto;
	}

	ppk_proto_param* ppk_param_proto_create_float(const char* name, const char* help_text, float default_value, const ppk_settings_group *group)
	{
		ppk_proto_param* proto=ppk_param_proto_create_empty(cvariant_float, 
															name, 
															help_text, 
															cvariant_from_float(default_value), 
															group);
		
		proto->user_type=ppk_proto_float_param;
		
		return proto;
	}

	ppk_proto_param* ppk_param_proto_create_file(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group, const char* file_filter)
	{
		ppk_proto_param* proto=ppk_param_proto_create_empty(cvariant_string, 
															name, 
															help_text, 
															cvariant_from_string_copy(default_value, 0), 
															group);
		
		proto->user_type=ppk_proto_file_param;
		proto->file_params=ppk_param_proto_create_file_parameters(file_filter);
		
		return proto;
	}

	ppk_proto_param* ppk_param_proto_create_directory(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group)
	{
		ppk_proto_param* proto=ppk_param_proto_create_empty(cvariant_string, 
															name, 
															help_text, 
															cvariant_from_string_copy(default_value, 0), 
															group);
		
		proto->user_type=ppk_proto_directory_param;
		
		return proto;
	}

	ppk_proto_param* ppk_param_proto_create_module(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group, ppk_boolean allow_self, ppk_boolean writable_only, ppk_security_level min_sec_level, unsigned int needed_read_flags, unsigned int needed_write_flags, unsigned int needed_listing_flags)
	{
		ppk_proto_param* proto=ppk_param_proto_create_empty(cvariant_string, 
															name, 
															help_text, 
															cvariant_from_string_copy(default_value, 0), 
															group);
		
		proto->user_type=ppk_proto_module_param;
		proto->module_params=ppk_param_proto_create_ranged_int_parameters(allow_self, writable_only, min_sec_level, needed_read_flags, needed_write_flags, needed_listing_flags);
		
		return proto;
	}

	ppk_proto_param* ppk_param_proto_create_ranged_int(const char* name, const char* help_text, int default_value, const ppk_settings_group *group, int lowest, int greatest)
	{
		ppk_proto_param* proto=ppk_param_proto_create_empty(cvariant_int, 
															name, 
															help_text, 
															cvariant_from_int(default_value), 
															group);
		
		proto->user_type=ppk_proto_ranged_int_param;
		proto->ranged_int_params=ppk_param_proto_create_ranged_int_parameters(lowest, greatest);
		
		return proto;
	}

	ppk_proto_param* ppk_param_proto_create_ranged_float(const char* name, const char* help_text, double default_value, const ppk_settings_group *group, double lowest, double greatest)
	{
		ppk_proto_param* proto=ppk_param_proto_create_empty(cvariant_float, 
															name, 
															help_text, 
															cvariant_from_float(default_value), 
															group);
		
		proto->user_type=ppk_proto_ranged_float_param;
		proto->ranged_float_params=ppk_param_proto_create_ranged_float_parameters(lowest, greatest);
		
		return proto;
	}
	
	ppk_proto_param* ppk_param_proto_create_list(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group, const char** list)
	{
		ppk_proto_param* proto=ppk_param_proto_create_empty(cvariant_string, 
															name, 
															help_text, 
															cvariant_from_string_copy(default_value, 0), 
															group);
		
		proto->user_type=ppk_proto_list_param;
		proto->list_params=ppk_param_proto_create_list_parameters(list);
		
		return proto;
	}
	
	ppk_proto_param* ppk_param_proto_create_validated_string(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group, const char* validation_regexp)
	{
		ppk_proto_param* proto=ppk_param_proto_create_empty(cvariant_string, 
															name, 
															help_text, 
															cvariant_from_string_copy(default_value, 0), 
															group);
		
		proto->user_type=ppk_proto_validated_string_param;
		proto->validated_string_params=ppk_param_proto_create_validated_string_parameters(validation_regexp);
		
		return proto;
	}
	
	ppk_proto_param* ppk_param_proto_create_boolean(const char* name, const char* help_text, cvariant_bool default_value, const ppk_settings_group *group)
	{
		ppk_proto_param* proto=ppk_param_proto_create_empty(cvariant_boolean, 
															name, 
															help_text, 
															cvariant_from_bool(default_value), 
															group);
		
		proto->user_type=ppk_proto_boolean_param;
		
		return proto;
	}
	
	void ppk_param_proto_free(ppk_proto_param* proto_param)
	{
		free(const_cast<char*>(proto_param->name));
		free(const_cast<char*>(proto_param->help_text));
		cvariant_free(proto_param->default_value);
		
		//type specific
		if(proto_param->user_type==ppk_proto_file_param)
			ppk_param_proto_free_file_parameters(proto_param->file_params);
		else if(proto_param->user_type==ppk_proto_list_param)
			ppk_param_proto_free_list_parameters(proto_param->list_params);
		else if(proto_param->user_type==ppk_proto_validated_string_param)
			ppk_param_proto_free_validated_string_parameters(proto_param->validated_string_params);
		
		free(proto_param);
	}
	
//Accessors
	cvariant_type ppk_param_proto_expected_type(const ppk_proto_param* proto)
	{
		assert(proto!=NULL);
		return proto->expected_type;
	}
	
	ppk_param_type ppk_param_proto_user_type(const ppk_proto_param* proto)
	{
		assert(proto!=NULL);
		return proto->user_type;
	}
	
	const char* ppk_param_proto_name(const ppk_proto_param* proto)
	{
		assert(proto!=NULL);
		return proto->name;
	}
	
	const char* ppk_param_proto_help_text(const ppk_proto_param* proto)
	{
		assert(proto!=NULL);
		return proto->help_text;
	}
	
	cvariant ppk_param_proto_default_value(const ppk_proto_param* proto)
	{
		assert(proto!=NULL);
		return proto->default_value;
	}
	
	const ppk_settings_group* ppk_param_proto_settings_group(const ppk_proto_param* proto)
	{
		assert(proto!=NULL);
		return proto->group;
	}
	
	const ppk_proto_param_file* ppk_param_proto_file_params(const ppk_proto_param* proto)
	{
		assert(proto!=NULL);
		if(proto->user_type==ppk_proto_file_param)
			return &proto->file_params;
		else
			return NULL;
	}
	
	const ppk_proto_param_module* ppk_param_proto_module_params(const ppk_proto_param* proto)
	{
		assert(proto!=NULL);
		if(proto->user_type==ppk_proto_module_param)
			return &proto->module_params;
		else
			return NULL;
	}
	
	const ppk_proto_param_ranged_int* ppk_param_proto_ranged_int_params(const ppk_proto_param* proto)
	{
		assert(proto!=NULL);
		if(proto->user_type==ppk_proto_ranged_int_param)
			return &proto->ranged_int_params;
		else
			return NULL;
	}
	
	const ppk_proto_param_ranged_float* ppk_param_proto_ranged_float_params(const ppk_proto_param* proto)
	{
		assert(proto!=NULL);
		if(proto->user_type==ppk_proto_ranged_float_param)
			return &proto->ranged_float_params;
		else
			return NULL;
	}
	
	const ppk_proto_param_list* ppk_param_proto_list_params(const ppk_proto_param* proto)
	{
		assert(proto!=NULL);
		if(proto->user_type==ppk_proto_list_param)
			return &proto->list_params;
		else
			return NULL;
	}
	
	const ppk_proto_param_validated_string* ppk_param_proto_validated_string_params(const ppk_proto_param* proto)
	{
		assert(proto!=NULL);
		if(proto->user_type==ppk_proto_file_param)
			return &proto->validated_string_params;
		else
			return NULL;
	}
	
	//Validation
	ppk_boolean ppk_param_module_validation(const char* module_id, const ppk_proto_param_module* params, const char* module_value)
	{
		//If one of the param is NULL
		if(module_id==NULL || params==NULL || module_value==NULL)
			return PPK_FALSE;
		
		//Check if the module exists
		ppk_boolean avail=ppk_module_is_available(module_value);
		if(avail==PPK_FALSE)
			return PPK_FALSE;
		
		//get the flags
		unsigned int read_flags, write_flags, listing_flags;
		ppk_module_read_flags(module_value, &read_flags);
		ppk_module_write_flags(module_value, &write_flags);
		ppk_module_listing_flags(module_value, &listing_flags);
		
		//check if the value complies with what the module expects
		if(params->allow_self==PPK_FALSE && strcmp(module_value, module_id)==0)
			return PPK_FALSE;
		else if(params->writable_only==PPK_TRUE && ppk_module_is_writable(module_value)==PPK_FALSE)
			return PPK_FALSE;
		else if(params->min_sec_level > ppk_module_security_level(module_value))
			return PPK_FALSE;
		else if((read_flags&params->needed_read_flags)==read_flags)
			return PPK_FALSE;
		else if(params->writable_only==PPK_TRUE && (write_flags&params->needed_write_flags)==write_flags)
			return PPK_FALSE;
		else if(params->writable_only==PPK_TRUE && (listing_flags&params->needed_listing_flags)==listing_flags)
			return PPK_FALSE;
		else
			return PPK_TRUE;
	}
	
	ppk_boolean ppk_param_ranged_int_validation(const char* /*module_id*/, const ppk_proto_param_ranged_int* param, int value)
	{
		return value>=param->lowest && value<=param->greatest?PPK_TRUE:PPK_FALSE;;
	}
	
	ppk_boolean ppk_param_ranged_float_validation(const char* /*module_id*/, const ppk_proto_param_ranged_float* param, double value)
	{
		return value>=param->lowest && value<=param->greatest?PPK_TRUE:PPK_FALSE;;
	}
	
	ppk_boolean ppk_param_list_validation(const char* /*module_id*/, const ppk_proto_param_list* param, const char* value)
	{
		const char** list=param->list;
		
		//Is the item selected in the list ?
		int i=0;
		while(list!=NULL && list[i]!=NULL)
		{
			if(strcmp(list[i], value)==0)
				return PPK_TRUE;
			
			i++;
		}
		
		return PPK_FALSE;
	}
	
	ppk_boolean ppk_param_validation(const char* module_id, const char* param_name, const cvariant param_value)
	{
		//if the module_id or the param is NULL, just go out
		if(module_id==NULL || param_name==NULL)
			return PPK_FALSE;
		
		//Read the prototype corresponding to param
		const ppk_proto_param* proto=ppk_module_parameter_prototype(module_id, param_name);
		
		//get the expected type
		cvariant_type expected_type=ppk_param_proto_expected_type(proto);
		if(cvariant_get_type(param_value)!=expected_type)
			return PPK_FALSE;
		
		//get the expected user type
		ppk_param_type type=ppk_param_proto_user_type(proto);
		
		//Cases
		if(type==ppk_proto_module_param)
		{
			//Get the proto parameters
			const ppk_proto_param_module* params=ppk_param_proto_module_params(proto);
			if(params==NULL)
				return PPK_FALSE;
			
			return ppk_param_module_validation(module_id, params, cvariant_get_string(param_value));
		}
		else if(type==ppk_proto_ranged_int_param)
		{
			//Get the proto parameters
			const ppk_proto_param_ranged_int* params=ppk_param_proto_ranged_int_params(proto);
			if(params==NULL)
				return PPK_FALSE;
			
			return ppk_param_ranged_int_validation(module_id, params, cvariant_get_int(param_value));
		}
		else if(type==ppk_proto_ranged_float_param)
		{
			//Get the proto parameters
			const ppk_proto_param_ranged_float* params=ppk_param_proto_ranged_float_params(proto);
			if(params==NULL)
				return PPK_FALSE;
			
			return ppk_param_ranged_float_validation(module_id, params, cvariant_get_float(param_value));
		}
		else if(type==ppk_proto_list_param)
		{
			//Get the proto parameters
			const ppk_proto_param_list* params=ppk_param_proto_list_params(proto);
			if(params==NULL)
				return PPK_FALSE;
			
			return ppk_param_list_validation(module_id, params, cvariant_get_string(param_value));
		}
		else if(type==ppk_proto_validated_string_param)
		{
			//TODO, need advices on this (which library should I use, etc ...)
			return PPK_TRUE;
		}
		
		return PPK_TRUE;
	}

#ifdef __cplusplus
}
#endif
