#ifndef FMMAP
#define FMMAP

#include <netinet/in.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

// Type definition for remote file location
struct fileloc {
    struct in_addr ipaddress; // Remote IP
    int port;                 // Remote port
    char *pathname;           // Remote file to be memory mapped
};
typedef struct fileloc fileloc_t;

// Create a new memory mapping in the virtual address space
// of the calling process         
// location: Remote file to be memory mapped
// offset:   File offset to memory map (from beginning of file)
//y
// returns: Pointer to the mapped area. On error the value MAP_FAILED
//          ((void*) -1) is returned and errno is set appropriately
void *rmmap(fileloc_t location, off_t offset){	
	printf("\nStruct receive: %d %s %s \n", location.port, location.ipaddress, location.pathname);
	
	int s, t, len;
	struct sockaddr_in remote;
	char * str =(char *) malloc (sizeof(char)) ;
	
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		exit(1);
	}
	
	bzero(&remote,sizeof(remote));
	remote.sin_family = AF_INET;
	remote.sin_port = htons(3050);
	remote.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	int result;
	
	result  = connect(s, (struct sockaddr *)&remote, sizeof(remote));
	if (result == 0) {
		printf("\n *****\nConnected \n *****\n");
	} else {
		//perror("connect");
		printf("%d",errno);
	}	
	
	printf("Result %d \n", result);
	
	if (send(s, location.pathname, sizeof(location.pathname), 0) == -1) {
		perror("send");
	} else {
	printf("sent %d", send);
	
	}
	printf("Waiting for receive\n");
	if ((t = recv(s, str, 100, 0)) > 0) {
		str[t] = '\0';
		printf("received\n");
		return (char*)str;
	} else {
		if (t< 0) perror("recv");
		else printf("Server closed connection\n");
		exit(1);
	}
}

// Deletes mapping for the specified address range
// addr:   Address returned by mmap
// length: Length of mapping
//
// returns: 0 on success, -1 on failure and errno set appropriately
int rmunmap(void *addr){
	
}

// Attempt to read up to count bytes from memory mapped area pointed
// to by addr + offset into buff
// addr: page read from rmap
// returns: Number of bytes read from memory mapped area
ssize_t mread(void *addr, off_t offset, void *buff, size_t count);

// Attempt to write up to count bytes to memory mapped area pointed
// to by addr + offset from buff
//
// returns: Number of bytes written to memory mapped area
ssize_t mwrite(void *addr, off_t offset, void *buff, size_t count);

#endif
