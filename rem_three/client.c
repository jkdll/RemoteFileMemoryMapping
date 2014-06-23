<<<<<<< HEAD
#include "fmmap.h"
#define refuse -1
#define accept 0
#define requestRead 1
#define requestWrite 2
#define PORT 8088
#define IP 192.168.1.105
struct msg_t {
	int msg;
	char* data[];
}


main(){
	char str[100];
	do{
		printf("Client: Welcome to the program \n");
		printf("Client: Please Select an Option\n");
		printf("Client: Press 1 to Connect to Server\n");
		printf("Client: Press 2 to send something\n");
		printf("*****************************\n");
		fgets (str, 100, stdin);
		if(str[0] == '1'){
			char *ip;
			char *port;
			// Get the IP and Port
			printf("Client: Enter Server <ip>:<port>: \n");
			fgets (str, 100, stdin);
			ip = strtok(str,":");
			port = strtok(NULL, ":");
			
			printf("Client: IP ADDRESS:");
			printf("%s ",ip);
			printf(" PORT: ");
			printf("%s\n",port);
			// Connect to Server Here
			fileloc_t server;
			server.ipaddress.s_addr =(int)ip;
			server.port = (int)port;
			server.pathname = "file.txt";
			void * buf = rmmap(server, 0);
			// Display File List
		} else if(str[0] == '2'){
			printf("Client: Enter a command:");
			fgets (str, 100, stdin);
		}
	}while(str[0] != ':' && str[1] != 'q');
	printf("Success\n");
}
=======
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char **argv) {

	/*
	 * Socket creation and connection to server
	 */
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_fd < 0)
		printf("Could not create socket");
	else
		printf("Socket Created\n");

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int res;
	if ((res = connect(socket_fd, (struct sockaddr *) &addr, sizeof(addr)))
			< 0) {
		printf("%s\n", strerror(res));
		return -1;
	} else
		printf("Connected to Server\n");
	close(socket_fd);

	/*
	 * Sending of one struct
	 */
	/*rm_protocol *test = (rm_protocol*) malloc(sizeof(rm_protocol));
	test->type = 1;
	test->data = 0;
	test->data_length = 0;
	test->count = 0;
	test->path = 0;
	test->error_id = 0;
	test->offset = 50;
	sendStruct(socket_fd, test);*/

	

	/*
	 * Test master key
	 */
	/*void* t = attachKey(MASTER_KEY, sizeof(int));
	int m = getMaster(t);
	printf("%i", m);
	setMaster(t, getpid());
	getchar();
	detachKey(t);
	removeMemSeg(MASTER_KEY, sizeof(int));*/

	/*
	 * Test semaphore
	 */
	/*if(initSemaphores() == 0){
		requestWrite(sem_header_set);
		printf("requested write");
		getchar();
		removeWrite(sem_header_set);
	}
	removeSemaphores();*/

	return 0;
}

>>>>>>> ebb640c802b516301dc2c1f0231fae8376d26c8c
