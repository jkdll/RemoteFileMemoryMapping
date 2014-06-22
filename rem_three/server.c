#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

struct client_t{
	int address;
	int id; // Thread id
	int begin; // Offset Begin
	int end; // Offset End
	int status; // Status
}

main(){
	if(!fork()){
	// Child
	} else {
	// Parent
	}
}
