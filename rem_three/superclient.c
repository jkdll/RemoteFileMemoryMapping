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
	int number = 10;
	int xeba_sockets[number];
	int i;
	// Create a fuckload of sockets
	for( i = 0; i<= number-1; i++){
		xeba_sockets[i] = socket(AF_INET, SOCK_STREAM,0);
		if(xeba_sockets[i] < 0) { printf("Super Client: Could not create a Socket\n"); }
		else { printf("Super Client: Created a socket\n"); }	
	}
	// Create a fuckload of addresses
	struct sockaddr_in xeba_addr[number];
	for(i = 0; i<= number-1; i++){
		bzero(&xeba_addr[i],sizeof(xeba_addr[i]));
		xeba_addr[i].sin_family = AF_INET;
		xeba_addr[i].sin_port = htons(8080);
		xeba_addr[i].sin_addr.s_addr = inet_addr("127.0.0.1");
	}
	// Connect...
	int result;
	for(i = 0; i<= number-1; i++){
		result = connect(xeba_sockets[i], (struct sockaddr *) &xeba_addr[i], sizeof(xeba_addr[i]));
		if(result < 0) { printf("Super Client: Error Connecting to Server\n"); }
		else { printf("Super Client: Conntected to Server\n"); }
	}
	return 0;
}

