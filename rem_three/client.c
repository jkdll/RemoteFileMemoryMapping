
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
			int port;
			// Get the IP and Port
			printf("Client: Enter Server <ip>:<port>: \n");
			fgets (str, 100, stdin);
			ip = "127.0.0.1"; 
			port = 3050; 
			
			printf("Client: IP ADDRESS:");
			printf("%s ",ip);
			printf(" PORT: ");
			printf("%d\n",port);
			
			fileloc_t *server =(struct fileloc*)malloc(sizeof(struct fileloc));
			server->ipaddress.s_addr =(unsigned long)ip;
			server->port = port;
			printf("port is %d", server->port);
			server->pathname = "file.txt";
			char * s;
			s = rmmap(*server, 0);
			// Display File List
		} else if(str[0] == '2'){
			printf("Client: Enter a command:");
			fgets (str, 100, stdin);
		}
	}while(str[0] != ':' && str[1] != 'q');
	printf("Success\n");
}

