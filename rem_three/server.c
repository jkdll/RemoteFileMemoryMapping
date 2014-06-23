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
#include <sys/ipc.h>
#include <sys/shm.h>

struct client_t{
	int address;
	int id; // Thread id
	int begin; // Offset Begin
	int end; // Offset End
	int status; // Status
}

#define msg_Accpted -1 
#define msg_Refused 1
#define msg_Wrt 2
#define msg_Rd 2


main(){

    signal(SIGCHLD, SIG_IGN);
    
    struct client_t clientlist[50];

    // Declare variables
    int sockfd, newsockfd, portno, clilen;
    //char buffer[256];
    //char s_buf[1];
    struct sockaddr_in serv_addr, cli_addr;
    //int32_t  n;

    // Create server socket (AF_INET, SOCK_STREAM)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        exit(1);
    }

    // Initialize socket structure
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 3049;  // Server port
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any address
    serv_addr.sin_port = htons(portno);
 
    // Bind the host address
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(1);
    }
    // CREATE SHARED MEMORY FOR CLIENT LIST
    
    key_t key;
    int shmid;
    char *data;

    key = ftok("/workspace/RemoteMemoryMapping/rem_three/server.c", 'A');
    shmid = shmget(key, sizeof(clientlist), 0644 | IPC_CREAT);
    data = shmat(shmid, clientlist, 0);

    listen(sockfd,5);
    while(1){
	printf("#S [Process %i]: Waiting for connection...\n",getppid());
	newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t*) &clilen);
	if(newsockfd == -1){ perror("#S [Parent Process]: Accept"); } 
	else { printf("#S [Process %i]: New Client Accepted\n",getpid()); }
	if(!fork()){
		
		printf("#S [Process %i]: Client Placed on New Process\n",getpid());
		
	}
    }
    //wait(0);
    //close(newsockfd);
    //close(sockfd);
}
