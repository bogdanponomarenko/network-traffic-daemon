#include "shell.h"

int status;
int lines, globalNLines;
node *tree;
char ch;
char buffer[20];
int ip[4];
char iface[20];
char ifaces[20][20];
int nInterfaces;
char *builtin_str[] = {
	"--help", "start", "stop", "show-count", "select-Iface",
};

char *builtin_string[] = {
	"--help", "start", "stop", "show-count [ip]", "select-Iface [iface]",
};

char *descriptionOfBuiltIn[] = {
	"show usage inforation",
	"packets are being sniffed from interface. default interface - eth0",
	"stop sniffing packets",
	"display number of packets come received from ip address",
	"choose interface which interact with packets",
};

int (*builtin_func[])(char **) = {
	&help, &start, &stop, &showCount, &selectIface};

void initCLI() {
	int i;
	printf("Type [--help] for more information\n");
	system("gcc -o daemon daemon.c sniffer.c");
	globalNLines = 0;
	tree = NULL;
	struct ifaddrs *addrs, *tmp;

	getifaddrs(&addrs);
	tmp = addrs;

	i = 0;
	while (tmp) {
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET) {
			strncpy(ifaces[i], tmp->ifa_name, sizeof ifaces[i]);
			i++;
		}
		tmp = tmp->ifa_next;
	}
	nInterfaces = i;
	freeifaddrs(addrs);
}

int num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

int help(char **args) {
	int i;
	printf("\nBuilt-in commands:\n");
	for (i = 0; i < num_builtins(); i++) {
		printf("  %s - %s\n", builtin_string[i], 
			descriptionOfBuiltIn[i]);
	}

	printf("\nFollowing interfaces are available:\n");
	for (i = 0; i < nInterfaces; i++)
		printf("  %s\n", ifaces[i]);

	printf("\nUsed files:\n");
	printf("- IPlog.txt - contains list of ip addresses, writen by deamon\n");
	printf("- log.txt   - contains full information about each packet\n");
	return 1;
}

int start(char **args) {
	char *exec = "./daemon ";
	char *result = malloc(strlen(exec) + strlen(iface) + 1);
	strcpy(result, exec);
	strcat(result, iface);
	printf("Starting daemon...\n");
	printf("%s\n", result);
	system(result);
	return 2;
	free(exec);
	free(result);
}

int stop(char **args) {
	printf("Stopping daemon...\n");
	status = 1;
	system("kill `pidof ./daemon` 2> /dev/null");
	return 1;
}

int showCount(char **args) {
	int nPacks;
	lines = 0;
	FILE *fp = fopen("IPlog.txt", "r");
	if (!fp) {
		printf("Unable to read file of IP addresses.\n");
		return 1;
	}
	while (fgets(buffer, sizeof(buffer), fp)) {
		lines++;
		if (lines > globalNLines && sscanf(buffer, "%d.%d.%d.%d", 
			&ip[0], &ip[1], &ip[2], &ip[3]) == 4)
			btAddNode(ip, &tree);
	}
	globalNLines = lines;

	nPacks = sscanf(args[1], "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]);
	if (nPacks != 4) {
		printf("Wrong format of ip address (MUST BE n.n.n.n)\n");
		return 1;
	}
	if (btSearch(ip, tree) == NULL) {
	printf("Given IP %d.%d.%d.%d doesn't exist in binary search tree\n", 
		ip[0], ip[1], ip[2], ip[3]);
	return 1;
  	}
	printf("Number of packets received from %s is %d\n", args[1],
        	btSearch(ip, tree)->nPackets);
	fclose(fp);
	return 1;
}

int selectIface(char **args) {
	int i;
	for (i = 0; i < nInterfaces; i++) {
		if (args[1] == NULL || strcmp(args[1], ifaces[i]) != 0) {
			strncpy(iface, "eth0", sizeof iface);
	printf("Selected interface not known. Chosen default interface: %s\n",
		iface);
		break;
	} else {
		strncpy(iface, args[1], sizeof iface);
		printf("Selected interface: %s\n", iface);
		break;
	}
	}
	return 1;
}

int launch(char **args) {
	pid_t pid, wpid;
	int status;

	pid = fork();
	if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			perror("cli");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		perror("cli");
	} else {
		do {
			wpid = waitpid(pid, &status, WUNTRACED);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

int execute(char **args) {
	int i;

	if (args[0] == NULL) {
	return 1;
	}

	for (i = 0; i < num_builtins(); i++) {
		if (strcmp(args[0], builtin_str[i]) == 0) {
			return (*builtin_func[i])(args);
		}
	}
	return launch(args);
}

#define LSH_RL_BUFSIZE 1024

char *read_line(void) {
	int bufsize = LSH_RL_BUFSIZE;
  	int position = 0;
	char *buffer = malloc(sizeof(char) * bufsize);
	int c;

	if (!buffer) {
		fprintf(stderr, "cli: allocation error\n");
		exit(EXIT_FAILURE);
	}
	while (1) {
		c = getchar();

		if (c == EOF || c == '\n') {
			buffer[position] = '\0';
			return buffer;
		} else {
			buffer[position] = c;
		}
		position++;

		if (position >= bufsize) {
			bufsize += LSH_RL_BUFSIZE;
			buffer = realloc(buffer, bufsize);
			if (!buffer) {
				fprintf(stderr, "cli: allocation error\n");
        			exit(EXIT_FAILURE);
      			}
    		}
	}
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"

char **split_line(char *line) {
	int bufsize = LSH_TOK_BUFSIZE, position = 0;
	char **tokens = malloc(bufsize * sizeof(char *));
	char *token;

	if (!tokens) {
		fprintf(stderr, "cli: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, LSH_TOK_DELIM);
	while (token != NULL) {
		tokens[position] = token;
		position++;

	if (position >= bufsize) {
		bufsize += LSH_TOK_BUFSIZE;
		tokens = realloc(tokens, bufsize * sizeof(char *));
      		if (!tokens) {
			fprintf(stderr, "cli: allocation error\n");
			exit(EXIT_FAILURE);
		}
	}

	token = strtok(NULL, LSH_TOK_DELIM);
	}
	tokens[position] = NULL;
	return tokens;
}

void loop(void) {
	char *line;
	char **args;
	int status;

	do {
		printf(">>> ");
		line = read_line();
		args = split_line(line);
		status = execute(args);
		free(line);
		free(args);
	} while (status);
}
