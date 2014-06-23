
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
			ip = "192.168.1.105"; //strtok(str,":");
			port = "3049"; //strtok(NULL, ":");
			
			printf("Client: IP ADDRESS:");
			printf("%s ",ip);
			printf(" PORT: ");
			printf("%s\n",port);
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

