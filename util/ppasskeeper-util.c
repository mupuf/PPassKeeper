#include <ppasskeeper.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>

char mode = 0, pwd_type = 0, listing_type=0;
char *module_id = NULL, *key = NULL, *password = NULL, *ppk_password = NULL, *file = NULL;

void usage()
{
	printf("Usage:\n"
			"ppasskeeper -L [-u <ppk_password>]\n"
			"ppasskeeper -G -m <module> -t <app|net|item> -k <name> [-f file -u <ppk_password>]\n"
			"ppasskeeper -G -m <module> -l ani [-u <ppk_password>]\n"
			"ppasskeeper -S -m <module> -t <app|net|item> -k <name> [-f file -p <password> -u <ppk_password>]\n\n"
			"See ppasskeeper(1) for details.\n");
	exit(1);
}

struct app_params
{
	char *name;
	char *username;
};

struct net_params
{
	char *server;
	int port;
	char *username;
};

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
				if (mode) usage();
				mode = *flag;
				break;
			case 'm':
				n++;
				if (module_id || *(flag + 1) || n >= argc) usage();
				module_id = argv[n];
				break;
			case 't':
				n++;
				if (pwd_type || *(flag + 1) || n >= argc) usage();
				if (! strcasecmp(argv[n], "app"))
					pwd_type = ppk_application;
				else if (! strcasecmp(argv[n], "net"))
					pwd_type = ppk_network;
				else if (! strcasecmp(argv[n], "item"))
					pwd_type = ppk_item;
				else
					usage();
				break;
			case 'l':
				n++;
				if (pwd_type || *(flag + 1) || n >= argc) usage();
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
	if (! mode) usage();
}

struct app_params appParameters()
{
	struct app_params params;

	char *p = strstr(key, ":");
        if (! p) usage();
	*p = '\0';

	params.name = key;
	params.username = p + 1;
	return params;
}

struct net_params netParameters()
{
	struct net_params params;

	char *p1 = strstr(key, "@");
	if (! p1) usage();
        *p1 = '\0';
	char *p2 = strstr(p1 + 1, ":");
	if (! p2) usage();
	*p2 = '\0';
	char *endptr;
	errno = 0;
	int port = strtol(p2 + 1, &endptr, 10);
	if (errno || endptr == p2 + 1) usage();

	params.server = p1 + 1;
	params.port = port;
	params.username = key;
	
	return params;
}

void die(const char* msg)
{
	printf("Fatal Error : %s\n", msg);
	exit(-1);
}

int main(int argc, char **argv)
{
	ppk_entry entry;
	parse_cmdline(argc, argv);
	
	//Unlock ppk if needed
	if(ppk_isLocked()==PPK_TRUE)
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
		if (pwd_type || module_id || key || password) usage();
		int c = ppk_getAvailableModulesCount();
		ppk_module modules[c];
		ppk_getAvailableModules(modules, c);
		int i;
		for (i = 0; i < c; i++)
			printf("%s: %s\n", modules[i].id, modules[i].display_name);
	} else if (mode == 'G')
	{
		if (pwd_type && module_id && key && !password)
		{
			if (pwd_type == ppk_item)
				entry=ppk_createItemEntry(key);
			else if (pwd_type == ppk_application)
			{
				struct app_params p = appParameters();
				entry=ppk_createAppEntry(p.name, p.username);
			} else if (pwd_type == ppk_network)
			{
				struct net_params p = netParameters();
				entry=ppk_createNetworkEntry(p.server, p.username, p.port);
			} else {
				//shouldn't happen
				return 1;
			}
		
			ppk_data edata;
			ppk_boolean res=ppk_getEntry(module_id, entry , &edata, ppk_rf_none);
			if(res==PPK_TRUE)
			{
				if(file)
				{
					FILE *pFile = fopen(file, "w");
					if(pFile != NULL)
					{					
						if(fwrite(edata.blob.data, 1, edata.blob.size, pFile)==0)
							die("Error while writing data to file !");

						fclose(pFile);
					}
					else
						die("The file cannot be openned in writing mode. Check your file permissions");
				}
				else if(edata.type==ppk_string)
					printf("%s\n", edata.string);
				else if(edata.type==ppk_blob)
					fwrite(edata.blob.data, 1, edata.blob.size, stdout);
			}
			else
				die("Password cannot be retrieved. Does the entry really exist ?");
		}
		else if ( module_id && listing_type > 0)
		{
			int len = ppk_getEntryListCount(module_id, listing_type, ppk_lf_none);
			ppk_entry* list=calloc(len, sizeof(ppk_entry));
			if (list)
			{
     			int i, nb=ppk_getEntryList(module_id, listing_type, list, len, ppk_lf_none);
     			printf("Listing %s gave %i results :\n", module_id, nb);
     			for(i=0; i< nb; i++)
     			{
     				char* type, entry[201];
     				if(list[i].type==ppk_application)
     				{
     					type="Application";
     					snprintf(entry, 200, "%s@%s", list[i].app.username, list[i].app.app_name);
     				}
     				else if(list[i].type==ppk_network)
     				{
     					type="Network";
     					snprintf(entry, 200, "%s@%s:%i", list[i].net.login, list[i].net.host, list[i].net.port);
     				}
     				else if(list[i].type==ppk_item)
     				{
     					type="Item";
     					snprintf(entry, 200, "%s", list[i].item);
     				}
					else
						return 2;
     				
     				printf("	%s : %s\n", type, entry);
     			}
     		}
     		else
     			return 2;
		}
		else
			usage();
	} else if (mode == 'S')
	{
		printf("password = %s\n", password);
		if (! pwd_type || ! module_id || ! key) usage();
		
		if (pwd_type == ppk_item)
			entry=ppk_createItemEntry(key);
		else if (pwd_type == ppk_application)
		{
			struct app_params p = appParameters();
			entry=ppk_createAppEntry(p.name, p.username);
		} else if (pwd_type == ppk_network)
		{
			struct net_params p = netParameters();
			entry=ppk_createNetworkEntry(p.server, p.username, p.port);
		}
		
		ppk_data edata;
		edata.type=ppk_string;
		edata.string=password;
		
		if (! password && !file)
		{
			errno = 0;
			password = getpass("Password (will not be echoed): ");
			if (errno == ENXIO)return 1;
			edata.type=ppk_string;
			edata.string=password;
		}
		else if(file)
		{
			FILE *pFile = fopen(file, "r");
			if(pFile != NULL)
			{
				fseek(pFile, 0, SEEK_END);
				unsigned int fileSize = ftell(pFile);
				rewind(pFile);

				edata.type=ppk_blob;
				edata.blob.size=fileSize;
				edata.blob.data = (char*)calloc(sizeof(char), fileSize+1);
				if(edata.blob.data==NULL)
					die("Alloc didn't manage to allocate the requested buffer.\nCheck the size of the file and your system's free memory.");
				
				if(fread((void*)edata.blob.data, 1, fileSize, pFile)==0)
					die("Error while reading data from file !");

				fclose(pFile);
			}
			else
				die("The file cannot be openned. Check your file permissions");
		}
		else
			die("Shouldn't happen ! Sky is falling on our heads !");

		ppk_boolean res=ppk_setEntry(module_id, entry , edata, ppk_wf_none);
		
		if(res!=PPK_TRUE)
			return 1;
		
	} else {
		//shouldn't happen
		return 1;
	}
	return 0;
}
