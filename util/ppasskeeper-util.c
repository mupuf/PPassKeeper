#include <ppasskeeper.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>

#define APP_PASSWORD 1
#define NET_PASSWORD 2
#define ITEM_PASSWORD 3

char mode = 0, pwd_type = 0;
char *module_id = NULL, *key = NULL, *password = NULL;

void usage()
{
	printf("Usage:\n"
			"ppasskeeper -L\n"
			"ppasskeeper -G -m <module> -t <app|net|item> -k <name>\n"
			"ppasskeeper -S -m <module> -t <app|net|item> -k <name> [-p <password>]\n\n"
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
					pwd_type = APP_PASSWORD;
				else if (! strcasecmp(argv[n], "net"))
					pwd_type = NET_PASSWORD;
				else if (! strcasecmp(argv[n], "item"))
					pwd_type = ITEM_PASSWORD;
				else
					usage();
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

int main(int argc, char **argv)
{
	parse_cmdline(argc, argv);

	if (mode == 'L')
	{
		if (pwd_type || module_id || key || password) usage();
		int c = getAvailableModulesCount();
		struct PPassKeeper_Module modules[c];
		getAvailableModules(modules, c);
		int i;
		for (i = 0; i < c; i++)
			printf("%s: %s\n", modules[i].id, modules[i].display_name);
	} else if (mode == 'G')
	{
		if (! pwd_type || ! module_id || ! key || password) usage();
		if (pwd_type == ITEM_PASSWORD)
			printf("%s\n", getItem(module_id, key));
		else if (pwd_type == APP_PASSWORD)
		{
			struct app_params p = appParameters();
			printf("%s\n", getApplicationPassword(module_id, p.name, p.username));
		} else if (pwd_type == NET_PASSWORD)
		{
			struct net_params p = netParameters();
			printf("%s\n", getNetworkPassword(module_id, p.server, p.port, p.username));
		} else {
			//shouldn't happen
			return 1;
		}
	} else if (mode == 'S')
	{
		if (! pwd_type || ! module_id || ! key) usage();
		if (! password)
		{
			errno = 0;
			password = getpass("Password (will not be echoed): ");
			if (errno == ENXIO) return 1;
		}
		if (pwd_type == ITEM_PASSWORD)
			setItem(module_id, key, password);
		else if (pwd_type == APP_PASSWORD)
		{
			struct app_params p = appParameters();
			setApplicationPassword(module_id, p.name, p.username, password);
		} else if (pwd_type == NET_PASSWORD)
		{
			struct net_params p = netParameters();
			setNetworkPassword(module_id, p.server, p.port, p.username, password);
		}
	} else {
		//shouldn't happen
		return 1;
	}
	return 0;
}
