#include "sniffer.h"

int main(int argc, char **argv) {
	FILE *logfileIP = openLogIP("IPlog.txt");
	FILE *logfile = openGlobalLog("log.txt");
	if (logfile == NULL) {
		printf("Unable to create log.txt file.");
	}
	fprintf(logfile, "Starting...\n");
	pid_t pid, sid;
	pid = fork();

	if (pid < 0) {
		fprintf(logfile, "pid < 0");
		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		exit(EXIT_SUCCESS);
	}

	sid = setsid();

	if (sid < 0) {
		fprintf(logfile, "pid < 0");
		exit(EXIT_FAILURE);
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	int saddr_size, data_size;
	struct sockaddr saddr;

	unsigned char *buffer = (unsigned char *)malloc(65536);

	int sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_IP));

	if (argc > 1)
		setsockopt(sock_raw, SOL_SOCKET, SO_BINDTODEVICE, argv[1],
		strlen(argv[1]) + 1);

	if (sock_raw < 0) {
		perror("Socket Error");
		return 1;
	}

	struct ifaddrs *addrs, *tmp;

	getifaddrs(&addrs);
	tmp = addrs;

	while (tmp) {
		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET)
			fprintf(logfile, "%s\n", tmp->ifa_name);
			tmp = tmp->ifa_next;
	}

	freeifaddrs(addrs);
	freeifaddrs(tmp);
	
	while (1) {
		saddr_size = sizeof saddr;

		data_size =
        recvfrom(sock_raw, buffer, 65536, 0, &saddr, (socklen_t *)&saddr_size);
 		if (data_size < 0) {
 			fprintf(logfile, "Error , failed to get packets\n");
 			return 1;
 		}
  		ProcessPacket(buffer, data_size);
	}
 	close(sock_raw);
 	free(buffer);
 	fprintf(logfile, "Finished");
 	fclose(logfile);
 	fclose(logfileIP);

 	return 0;
}
