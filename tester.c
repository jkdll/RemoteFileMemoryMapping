#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>


int main(int argc, char *argv[])
{
	int fd, pagesize;
	
	char *data;

	fd = open("MapTestFile.txt", O_RDONLY);
	
	pagesize = getpagesize();
	
	data = mmap((caddr_t)0, pagesize, PROT_READ,MAP_SHARED, fd,0);
	int i;	
	for(i = 0; i<= sizeof(data); i++){
		printf("%c",data[i]);	
	}
	return 0;
}
