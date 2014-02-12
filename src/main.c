#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "file_system.h"

#define PROGRAM "jaggrab-c"

int main(int argc, char* argv[])
{
	char* root_dir = NULL;
	uint16_t port = 43595;
	int error = 0;

	for (int i = 1; i < argc; ++i)
	{
		if (strncmp("--root", argv[i], 6) == 0 && argc >= i + 1)
			root_dir = argv[++i];
		else if (strncmp("--port", argv[i], 6) == 0 && argc >= i + 1)
			port = atoi(argv[++i]);
		else if (strncmp("--help", argv[i], 6) == 0)
			error = 1;
		else
		{
			printf("unknown argument: %s\n", argv[i]);
			error = 1;
			break;
		}
	}

	error = error || root_dir == NULL;
	if (error == 1)
	{
		printf("usage: " PROGRAM " --root <root_dir> [--port <operating_port:default=43595>]\n");
		return 0;
	}

	int flags = file_sys_init(root_dir);
	
	if (flags)
	{
		if (flags & 0x1)
			printf("\terror: no data file present\n");
		if (flags & 0x2)
			printf("\terror: no index file(s) present\n");
		puts("exiting..");
	}
	else
	{
		printf("starting server on port %d..\n", port);
		if (!server_init(port))
		{
			printf("success! awaiting connections..\n");
			server_loop();
		}
		else
		{
			printf("quitting..\n");
		}
	}
	file_sys_dealloc();
	return 0;
}