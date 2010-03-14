#include <ppasskeeper.h>
#include <ppasskeeper/param_prototype.h>
#include <stdlib.h>

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

static ppk_proto_param_module ppk_param_proto_create_ranged_int_parameters(ppk_boolean allow_self, ppk_boolean writable_only, ppk_security_level min_sec_level, unsigned int needed_flags)
{
	ppk_proto_param_module param_module;
	param_module.allow_self=allow_self;
	param_module.writable_only=writable_only;
	param_module.min_sec_level=min_sec_level;
	param_module.needed_flags=needed_flags;
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
															cvariant_from_string(default_value), 
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
															cvariant_from_string(default_value), 
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
															cvariant_from_string(default_value), 
															group);
		
		proto->user_type=ppk_proto_directory_param;
		
		return proto;
	}

	ppk_proto_param* ppk_param_proto_create_module(const char* name, const char* help_text, const char* default_value, const ppk_settings_group *group, ppk_boolean allow_self, ppk_boolean writable_only, ppk_security_level min_sec_level, unsigned int needed_flags)
	{
		ppk_proto_param* proto=ppk_param_proto_create_empty(cvariant_string, 
															name, 
															help_text, 
															cvariant_from_string(default_value), 
															group);
		
		proto->user_type=ppk_proto_module_param;
		proto->module_params=ppk_param_proto_create_ranged_int_parameters(allow_self, writable_only, min_sec_level, needed_flags);
		
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
															cvariant_from_string(default_value), 
															group);
		
		proto->user_type=ppk_proto_list_param;
		proto->list_params=ppk_param_proto_create_list_parameters(list);
		
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
		
		free(proto_param);
	}

#ifdef __cplusplus
}
#endif
