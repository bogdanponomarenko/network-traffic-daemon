#include "binaryTree.h"
#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <net/ethernet.h>
#include <netdb.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int help(char **args);
int start(char **args);
int stop(char **args);
int showCount(char **args);
int selectIface(char **args);
int stat(char **args);
int num_builtins();
int launch(char **args);
int execute(char **args);
char *read_line(void);
char **split_line(char *line);
void loop(void);
void initCLI(void);

FILE *openGlobalLog(const char *);
