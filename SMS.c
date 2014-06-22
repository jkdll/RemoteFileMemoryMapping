#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

#define SIZE 1024 // 1K segments
#define BUFFERSIZE 20

int main(int argc, char *argv[])
{
	//key_t key;
	//int shmid;
	char *data;
	int mode;
	int file;
	struct stat buf;
	buf.st_size = getpagesize();
	int offset;

	//key = ftok("/home/patrick/workspace/file.txt", 'R'); //create key
	//shmid = shmget(key, SIZE, 0644 | IPC_CREAT); //permission (rw-r--r--)
	//data = shmat(shmid, (void *)0, 0);
	file = open("file.txt", O_RDONLY); 	
	

	if (stat("file.txt", &buf)== -1) {
		perror("stat");
		exit(1);	
	}

	offset = atoi(argv[1]);
	int pagesize;
	pagesize = getpagesize();
	data = mmap((caddr_t)0, buf.st_size, PROT_READ, MAP_SHARED, file, 0);
	
	char s[BUFFERSIZE];
	int i;	
	for (i = 0; i<BUFFERSIZE; i++){
		s[i] = data[offset+i];	
	} 

	printf("byte at offset %d is '%s'\n", offset, s);
/*
	if (data == (char *)(-1)) //failure condition
		perror("shmat");
		
	if (argc == 2) {
		printf("Writing to segment: \"%s\"\n", argv[1]);
		strncpy(data, argv[1], SIZE);
	} else
		printf("segment contains: \"%s\"\n", data);

	if (shmdt(data) == -1){
		perror("shmdt");
		exit(1);
	}*/	
	return 0;
}
