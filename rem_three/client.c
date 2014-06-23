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

	/*
	 * Socket creation and connection to server
	 */
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (socket_fd < 0)
		printf("Could not create socket");
	else
		printf("Socket Created\n");

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int res;
	if ((res = connect(socket_fd, (struct sockaddr *) &addr, sizeof(addr)))
			< 0) {
		printf("%s\n", strerror(res));
		return -1;
	} else
		printf("Connected to Server\n");
	close(socket_fd);

	/*
	 * Sending of one struct
	 */
	/*rm_protocol *test = (rm_protocol*) malloc(sizeof(rm_protocol));
	test->type = 1;
	test->data = 0;
	test->data_length = 0;
	test->count = 0;
	test->path = 0;
	test->error_id = 0;
	test->offset = 50;
	sendStruct(socket_fd, test);*/

	

	/*
	 * Test master key
	 */
	/*void* t = attachKey(MASTER_KEY, sizeof(int));
	int m = getMaster(t);
	printf("%i", m);
	setMaster(t, getpid());
	getchar();
	detachKey(t);
	removeMemSeg(MASTER_KEY, sizeof(int));*/

	/*
	 * Test semaphore
	 */
	/*if(initSemaphores() == 0){
		requestWrite(sem_header_set);
		printf("requested write");
		getchar();
		removeWrite(sem_header_set);
	}
	removeSemaphores();*/

	return 0;
}

