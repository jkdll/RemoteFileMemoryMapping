#ifndef FMMAP
#define FMMAP

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
#include "fmmap.h"
struct fileloc {
    struct in_addr ipaddress; // Remote IP
    int port;                 // Remote port
    char *pathname;           // Remote file to be memory mapped
};
typedef struct fileloc fileloc_t;
#define refuse -1
#define accept 0
#define requestRead 1
#define requestWrite 2

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
			printf("Client: Enter Server <ip>:<port> \n");
			fgets (str, 100, stdin);
			ip = strtok(str,":");
			port = strtok(NULL, ":");
			
			printf("Client: IP ADDRESS:");
			printf("%s ",ip);
			printf(" PORT: ");
			printf("%s\n",port);
			// Connect to Server Here

			// Display File List
		} else if(str[0] == '2'){
			printf("Client: Enter a command:");
			fgets (str, 100, stdin);
		}
	}while(str[0] != ':' && str[1] != 'q');
	printf("Success\n");
}

void rmconnect(){
	
}
// Type definition for remote file location
//struct fileloc {
//    struct in_addr ipaddress; // Remote IP
//    int port;                 // Remote port
//    char *pathname;           // Remote file to be memory mapped
//};
//typedef struct fileloc fileloc_t;

// Create a new memory mapping in the virtual address space
// of the calling process         
// location: Remote file to be memory mapped
// offset:   File offset to memory map (from beginning of file)
//
// returns: Pointer to the mapped area. On error the value MAP_FAILED
//          ((void*) -1) is returned and errno is set appropriately
void *rmmap(fileloc_t location, off_t offset){
	int fd, pagesize;
	
	char *data;

	fd = open(location.pathname, O_RDONLY);
	
	pagesize = getpagesize();
	
	data = mmap((caddr_t)0, pagesize, PROT_READ,MAP_SHARED, fd,offset);	
}

// Deletes mapping for the specified address range
// addr:   Address returned by mmap
// length: Length of mapping
//
// returns: 0 on success, -1 on failure and errno set appropriately
int rmunmap(void *addr){
	// 1. Send MSG to Server About Unmap
	// 2. Clear Client Memory
	return munmap(addr,sizeof(addr));
}

// Attempt to read up to count bytes from memory mapped area pointed
// to by addr + offset into buff
//
// returns: Number of bytes read from memory mapped area
ssize_t mread(void *addr, off_t offset, void *buff, size_t count){
	// 1. Send MSG to Server To get Data
	// 2. Recieve Data and Unmap
	return 0;
}

// Attempt to write up to count bytes to memory mapped area pointed
// to by addr + offset from buff
//
// returns: Number of bytes written to memory mapped area
ssize_t mwrite(void *addr, off_t offset, void *buff, size_t count){
	// 1. Send MSG to Server To get Data [WRITE]
	// 2. If can write, Map Data
	// 3. Else go to 1.
	return 0;
}

#endif
