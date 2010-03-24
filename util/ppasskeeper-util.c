#include "ppasskeeper.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>

char mode = 0, listing_type = 0;
char *module_id = NULL, *key = NULL, *password = NULL, *ppk_password = NULL, *file = NULL;

void usage()
{
	printf("Usage:\n"
			"ppasskeeper -L [-u <ppk_pwd>]                                         #List modules\n"
			"ppasskeeper -G -m <module> -k <key> [-f <file> -u <ppk_pwd>]          #Get an entry\n"
			"ppasskeeper -G -m <module> -l ani [-u <ppk_pwd>]                      #Lists entries\n"
			"ppasskeeper -S -m <module> -k <key> [-f <file> -p <pwd> -u <ppk_pwd>] #Set an entry\n"
			"ppasskeeper -E -m <module> -k <key> [-u <ppk_pwd>]                    #Erase an entry\n"
			"ppasskeeper -R -m <module> -k <key> [-u <ppk_password>]               #Read a parameter\n"
			"ppasskeeper -W -m <module> -k <key> -p <value> [-u <ppk_pwd>]         #Write a parameter\n"
			"ppasskeeper -I -m <module> [-u <ppk_pwd>]                             #Module information\n"
			"ppasskeeper -D [-m <module> -u <ppk_pwd>]                             #G/S default module\n"
			"ppasskeeper -X -m <module> -f <file> [-u <ppk_pwd>]                   #Export entries\n"
			"ppasskeeper -M -m <module> -f <file> [-u <ppk_pwd>]                   #Import entries\n"
			"See ppasskeeper(1) for details.\n");
	exit(1);
}

#if defined(WIN32) || defined(WIN64)
char* getpass(const char* prompt)
{
	static char* pwd[101];

	printf("Password : ");
	fgets((char*)pwd, sizeof(pwd)-1, stdin);

	//Get rid of the \r\n characters
	char* p;
	if ((p = strchr((char*)pwd, '\n')) != NULL)
	  *p = '\0';
	if ((p = strchr((char*)pwd, '\r')) != NULL)
	  *p = '\0';

	return (char*)pwd;
}
#endif

void parse_cmdline(int argc, char **argv)
{
	int n;
	for (n = 1; n < argc; n++)
	{
		char *arg = argv[n];
		if (arg[0] != '-' || ! arg[1]) usage();
		char *flag;
		for (flag = arg + 1; *flag; flag++)
		{
			switch(*flag)
			{
			case 'L':
			case 'G':
			case 'S':
			case 'R':
			case 'W':
			case 'I':
			case 'D':
			case 'E':
			case 'X':
			case 'M':
				if (mode) usage();
				mode = *flag;
				break;
			case 'm':
				n++;
				if (module_id || *(flag + 1) || n >= argc) usage();
				module_id = argv[n];
				break;
			case 'l':
				n++;
				if (*(flag + 1) || n >= argc) usage();
				int i, arglen=strlen(argv[n]);
				if(arglen>3) usage();
				for(i=0;i<arglen;i++)
				{
					if(argv[n][i]=='a')
						listing_type+=ppk_application;
					else if(argv[n][i]=='n')
						listing_type+=ppk_network;
					else if(argv[n][i]=='i')
						listing_type+=ppk_item;
					else
						usage();
				}
				break;
			case 'k':
				n++;
				if (key || *(flag + 1) || n >= argc) usage();
				key = argv[n];
				break;
			case 'p':
				n++;
				if (password || *(flag + 1) || n >= argc) usage();
				password = argv[n];
				break;
			case 'u':
				n++;
				if (ppk_password || *(flag + 1) || n >= argc) usage();
				ppk_password = argv[n];
				break;
			case 'f':
				n++;
				if (file || *(flag + 1) || n >= argc) usage();
				file = argv[n];
				break;
			default:
				usage();
			}
		}
	}

	if (! mode)
		usage();
}

void die(const char* msg)
{
	printf("Fatal Error : %s\n", msg);
	exit(-1);
}

void listFlags(unsigned int flags)
{
	printf("none");

	if(flags|ppk_rf_silent)
		printf(", silent");

	printf("\n");
}

void printModuleDetails(const char* module)
{
	//Get security level
	int security_level=ppk_module_security_level(module);
	const char* security_string=NULL;
	switch(security_level)
	{
		case ppk_sec_lowest:
			security_string="Lowest security, probably a plain-text storage.";
			break;

		case ppk_sec_scrambled:
			security_string="Poor security, probably a scrambled-text storage.";
			break;

		case ppk_sec_safe:
			security_string="Good security, probably your system's secure password storage.";
			break;

		case ppk_sec_perfect:
			security_string="Perfect security, is it stored ? :D";
			break;
	}

	//Get limits
	ppk_boolean supports_writing=ppk_module_is_writable(module);
	size_t max_string_size=ppk_module_max_data_size(module, ppk_string);
	size_t max_blob_size=ppk_module_max_data_size(module, ppk_blob);

	//Get flags
	unsigned int flags_read, flags_write, flags_list;
	ppk_module_read_flags(module, &flags_read);
	ppk_module_write_flags(module, &flags_write);
	ppk_module_listing_flags(module, &flags_list);

	//Print everything out !
	printf("----- %s -----\n", module);
	printf("	Security: %i/3 --> %s\n", security_level, security_string);
	printf("\n	Limits:\n");
	printf("		Supports writing: %s\n", supports_writing==PPK_TRUE?"yes":"no");
	printf("		Max string size (in byte): %d\n", (int)max_string_size);
	printf("		Max blob size (in byte): %d\n", (int)max_blob_size);
	printf("\n	Supported Flags:\n");
	printf("		Read: "); listFlags(flags_read);
	printf("		Write: "); listFlags(flags_write);
	printf("		List: "); listFlags(flags_list);
}

int main(int argc, char **argv)
{
	ppk_entry* entry;
	
	parse_cmdline(argc, argv);

	//Unlock ppk if needed
	if(ppk_is_locked()==PPK_TRUE)
	{
		if(ppk_password==NULL)
		{
			printf("PPassKeeper is locked.\nPlease add -u <ppk_password> in the command line\n");
			return 1;
		}
		else if(ppk_unlock(ppk_password)==PPK_FALSE)
		{
			printf("The password you gave to unlock PPassKeeper is wrong.\n");
			return 1;
		}
	}

	if (mode == 'L')
	{
		if (module_id || key || password) usage();
		
		char** list=ppk_module_list_new();
		if(list)
		{
			unsigned int i;
			for (i = 0; list[i]!=NULL; ++i)
				printf("%s: %s\n", list[i], ppk_module_display_name(list[i]));
			ppk_module_list_free(list);
		}
		else
		{
			fprintf(stderr, "Listing failed for an unknown reason.\n");
			return 1;
		}
	}
	else if (mode == 'G')
	{
		if (module_id && key && !password)
		{
			entry = ppk_entry_new_from_key(key);
			if (entry == NULL)
				// invalid key
				return 1;

			ppk_data *edata;
			ppk_error res=ppk_module_get_entry(module_id, entry , &edata, ppk_rf_none);
			ppk_entry_free(entry);
			if(res==PPK_OK)
			{
				if(file)
				{
					FILE *pFile = fopen(file, "w");
					if(pFile != NULL)
					{
						if(fwrite(edata->blob.data, 1, edata->blob.size, pFile)==0)
							die("Error while writing data to file !");

						fclose(pFile);
					}
					else
						die("The file cannot be openned in writing mode. Check your file permissions");
				}
				else if(edata->type==ppk_string)
					printf("%s\n", edata->string);
				else if(edata->type==ppk_blob)
					fwrite(edata->blob.data, 1, edata->blob.size, stdout);
				ppk_data_free(edata);
			}
			else
				fprintf(stderr, "The entry cannot be retrieved:\n%s\n", ppk_error_get_string(res));
		}
		else if ( module_id && listing_type > 0)
		{
			size_t len, i;
			ppk_entry** list;
			
			ppk_error err = ppk_module_get_entry_list(module_id, listing_type, &list, &len, ppk_lf_none);
			if (err != PPK_OK)
				die(ppk_error_get_string(err));
			else
			{
				printf("Listing %s gave %d results :\n", module_id, (int)len);
				for (i = 0; i < len; i++)
				{
					char* type, entry[201];
					if(list[i]->type==ppk_application)
					{
						type="Application";
						snprintf(entry, 200, "%s@%s", list[i]->app.username, list[i]->app.app_name);
					}
					else if(list[i]->type==ppk_network)
					{
						type="Network";
						snprintf(entry, 200, "%s@%s:%i", list[i]->net.login, list[i]->net.host, list[i]->net.port);
					}
					else if(list[i]->type==ppk_item)
					{
						type="Item";
						snprintf(entry, 200, "%s", list[i]->item);
					}
					else
						return 2;

					printf("	%s : %s\n", type, entry);
				}
				ppk_module_free_entry_list(list);
			}
		}
		else
			usage();
	}
	else if (mode == 'S')
	{
		if (! module_id || ! key) usage();

		entry = ppk_entry_new_from_key(key);
		if (entry == NULL)
		{
			fprintf(stderr, "The key '%s' is invalid.\n", key);
			return 1; // invalid key
		}

		ppk_data* edata;
		if (password)
		{
			edata = ppk_string_data_new(password);
		}
		else if (! password && !file)
		{
			errno = 0;
			password = getpass("Password (will not be echoed): ");
			if (errno == ENXIO) return 1;
			edata = ppk_string_data_new(password);
		}
		else if (file)
		{
			FILE *pFile = fopen(file, "r");
			if(pFile != NULL)
			{
				fseek(pFile, 0, SEEK_END);
				unsigned int fileSize = ftell(pFile);
				rewind(pFile);

				char* blobdata = (char*)calloc(sizeof(char), fileSize+1);
				if(blobdata==NULL)
					die("Alloc didn't manage to allocate the requested buffer.\nCheck the size of the file and your system's free memory.");

				if(fread((void*)blobdata, 1, fileSize, pFile)==0)
					die("Error while reading data from file !");

				fclose(pFile);

				edata = ppk_blob_data_new(blobdata, fileSize);
			}
			else
				die("The file cannot be openned. Check your file permissions");
		}
		else
			die("Shouldn't happen ! Sky is falling on our heads !");

		ppk_error res=ppk_module_set_entry(module_id, entry, edata, ppk_wf_none);
		ppk_entry_free(entry);
		ppk_data_free(edata);

		if(res!=PPK_OK)
		{
			fprintf(stderr, "An error occured while setting the entry :\n	--> %s\n", ppk_error_get_string(res));
			return 1;
		}

	}
	else if (mode == 'R')
	{
		if (!module_id || !key) usage();

		cvariant cv=ppk_module_get_param(module_id, key);
		if(cvariant_get_type(cv)==cvariant_string)
			printf("%s", cvariant_get_string(cv));
		else if(cvariant_get_type(cv)==cvariant_int)
			printf("%i",cvariant_get_int(cv));
		else if(cvariant_get_type(cv)==cvariant_float)
			printf("%f",cvariant_get_float(cv));
		else
			return 2;
	}
	else if (mode == 'W')
	{
		if (!module_id || !key || !password) usage();

		if(ppk_module_save_param(module_id, key, cvariant_from_string(password))==PPK_TRUE)
			return 0;
		else
			return 1;
	}
	else if (mode == 'I')
	{
		if (!module_id) usage();

		if(ppk_module_is_available(module_id)==PPK_TRUE)
		{
			printModuleDetails(module_id);
			return 0;
		}
		else
			printf("The module '%s' doesn't exists\n", module_id);
	}
	else if (mode == 'D')
	{
		if (!module_id)
			printf("%s", ppk_module_get_default());
		else
		{
			if(ppk_module_set_default(module_id)==PPK_OK)
				return 0;
			else
				return 1;
		}
	}
	else if (mode == 'E')
	{
		if (!module_id || !key) usage();

		entry = ppk_entry_new_from_key(key);
		if (entry == NULL)
		{
			fprintf(stderr, "The key '%s' is invalid.\n", key);
			return 1; // invalid key
		}

		ppk_error ret=ppk_module_remove_entry(module_id, entry, ppk_wf_none);
		if(ret==PPK_OK)
			return 0;
		else
			fprintf(stderr, "An error occured while deleting the entry :\n	--> %s\n", ppk_error_get_string(ret));
		
		ppk_entry_free(entry);
	}
	else if (mode == 'X')
	{
		if (!module_id || !file) usage();

		ppk_error res=ppk_module_export(module_id, file);
		if(res==PPK_OK)
		{
			int count=ppk_module_get_entry_count(module_id, ppk_application|ppk_network|ppk_item, ppk_lf_none);
			
			printf("%i %s have been exported from the module %s\n", count, count==1?"entry":"entries", module_id);
		}
		else
		{
			fprintf(stderr, "An error occured while exporting the entries :\n	--> %s\n", ppk_error_get_string(res));
			return 1;
		}
	}
	else if (mode == 'M')
	{
		if (!module_id || !file) usage();

		int before=ppk_module_get_entry_count(module_id, ppk_application|ppk_network|ppk_item, ppk_lf_none);
		ppk_error res=ppk_module_import(module_id, file);
		if(res==PPK_OK)
		{
			int count=ppk_module_get_entry_count(module_id, ppk_application|ppk_network|ppk_item, ppk_lf_none)-before;
			
			printf("%i %s have been imported in the module %s\n", count, count==1?"entry":"entries", module_id);
		}
		else
		{
			fprintf(stderr, "An error occured while importing the entries :\n	--> %s\n", ppk_error_get_string(res));
			return 1;
		}
	}
	else
	{
		//shouldn't happen
		return 1;
	}
	
	return 0;
}
