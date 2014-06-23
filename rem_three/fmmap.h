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
//
// returns: Pointer to the mapped area. On error the value MAP_FAILED
//          ((void*) -1) is returned and errno is set appropriately
void *rmmap(fileloc_t location, off_t offset){	
	int fd, pagesize, s, r;
	char *data;
	socklen_t clilen;
	char buf[1024];			
	printf("hola");
	struct hostent *server;
	struct sockaddr_in serv_addr;	

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0)
		perror("socket");	
	const char * addr = (char*)location.ipaddress.s_addr;
	server =(struct hostent *) gethostbyaddr(addr, sizeof(struct in_addr), AF_INET);
	if (server == NULL)
	{
		fprintf(stderr, "ERROR, host does not exist");
		exit(0);
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(location.ipaddress.s_addr);
	serv_addr.sin_port = htons(location.port);
	printf("trying to bind");
	if (bind(s,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		perror("bind");
		exit(1);
	}	

	if(connect(s, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	{
		error("ERROR connecting");
		exit(1);
	}

	
	int sent = write(s, location.pathname, sizeof(location.pathname)); 
	if (sent <= 0)
		error("ERROR writing to socket\n");
	
	int nread = read(s, buf, sizeof(buf));
	
	printf(buf);
	
	return NULL;	
	/*
	clilen = sizeof(cli_addr);
	int done = 0;
	while(!done){
		printf("waiting for response");
		r = accept(s, (struct sockaddr *) &cli_addr, &clilen);
	}
	
	
	

	fd = open(location.pathname, O_RDONLY);
	
	pagesize = getpagesize();
	
	return data = mmap((caddr_t)0, pagesize, PROT_READ,MAP_SHARED, fd,offset);*/	
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
//
// returns: Number of bytes read from memory mapped area
ssize_t mread(void *addr, off_t offset, void *buff, size_t count);

// Attempt to write up to count bytes to memory mapped area pointed
// to by addr + offset from buff
//
// returns: Number of bytes written to memory mapped area
ssize_t mwrite(void *addr, off_t offset, void *buff, size_t count);

#endif
