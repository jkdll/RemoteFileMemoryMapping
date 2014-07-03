#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
 
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
 
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

/* SECTION 2.0 - GLOBAL DEFINITIONS */
#define IP "127.0.0.1"
#define PORT 3048
#define BACKLOG 10
#define CLIENTS 10
 
#define BUFFSIZE 1024
#define PATHLEN 1000
#define SERVERMEM 2147483648

/* SECTION 3.0 - STRUCTS
* PACKET	The MSG that is exchanged between servers and clients
* THREADINFO	Info for a Client on a Thread
* LLNODE	Node for storing thread info on the linked list
* ******************************************************************
*	COMMAND LIST
* ******************************************************************
*	0	MAP
*	1	READ
*	2	WRITE
*	3	UNMAP
* 
*/
struct FILE {
	int fid;
	long floor;
	long ceiling;
};

struct FILENODE {
	struct FILE src;
	struct FILENODE *next;
};

struct FILELIST {
	struct FILENODE *head, *tail;
	int size;
	int fidgen;
};

struct THREADINFO {
	pthread_t thread_ID; // thread's pointer
	int sockfd; // socket file descriptor
};

struct LLNODE {
    struct THREADINFO threadinfo;
    struct LLNODE *next;
};
 
struct LLIST {
    struct LLNODE *head, *tail;
    int size;
};

/* FILE LIST FUNCTIONS */

int FileCompare(struct FILE *a, struct FILE *b) {
    return a->fid == b->fid;
}
struct FILE* MakeFile(long f, long c){
  struct FILE* new = (struct FILE*)malloc(sizeof(struct FILE));
  new->floor = f;
  new->ceiling = c;
  return new;
}

void FileList_init(struct FILELIST *fl) {
    fl->head = fl->tail = NULL;
    fl->size = 0;
    fl->fidgen = 0;
}
 
int FileList_insert(struct FILELIST *fl, struct FILE *somefile) {
    fl->fidgen++;
    somefile->fid = fl->fidgen;
    if(fl->head == NULL) {
        fl->head = (struct FILENODE *)malloc(sizeof(struct FILENODE));
        fl->head->src = *somefile;
        fl->head->next = NULL;
        fl->tail = fl->head;
    }
    else {
        fl->tail->next = (struct FILENODE *)malloc(sizeof(struct FILENODE));
        fl->tail->next->src = *somefile;
        fl->tail->next->next = NULL;
        fl->tail = fl->tail->next;
    }
    fl->size++;
    return 0;
}
 
int FileList_delete(struct FILELIST *fl, struct FILE *somefile) {
    struct FILENODE *curr, *temp;
    if(fl->head == NULL) return -1;
    if(compare(somefile, &fl->head->src) == 0) {
        temp = fl->head;
        fl->head = fl->head->next;
        if(fl->head == NULL) fl->tail = fl->head;
        free(temp);
        fl->size--;
        return 0;
    }
    for(curr = fl->head; curr->next != NULL; curr = curr->next) {
        if(compare(somefile, &curr->next->src) == 0) {
            temp = curr->next;
            if(temp == fl->tail) fl->tail = curr;
            curr->next = curr->next->next;
            free(temp);
            fl->size--;
            return 0;
        }
    }
    return -1;
}
 
void FileList_dump(struct FILELIST *fl) {
    struct FILENODE *curr;
    struct FILE *somefile;
    printf("File count: %d\n", fl->size);
    for(curr = fl->head; curr != NULL; curr = curr->next) {
        somefile = &curr->src;
	printf("(%d) :: %lu to %lu.\n", somefile->fid, somefile->floor, somefile->ceiling);
    }
}


/* CLIENT LIST FUNCTIONS */

int compare(struct THREADINFO *a, struct THREADINFO *b) {
    return a->sockfd - b->sockfd;
}
 
void list_init(struct LLIST *ll) {
    ll->head = ll->tail = NULL;
    ll->size = 0;
}
 
int list_insert(struct LLIST *ll, struct THREADINFO *thr_info) {
    if(ll->size == CLIENTS) return -1;
    if(ll->head == NULL) {
        ll->head = (struct LLNODE *)malloc(sizeof(struct LLNODE));
        ll->head->threadinfo = *thr_info;
        ll->head->next = NULL;
        ll->tail = ll->head;
    }
    else {
        ll->tail->next = (struct LLNODE *)malloc(sizeof(struct LLNODE));
        ll->tail->next->threadinfo = *thr_info;
        ll->tail->next->next = NULL;
        ll->tail = ll->tail->next;
    }
    ll->size++;
    return 0;
}
 
int list_delete(struct LLIST *ll, struct THREADINFO *thr_info) {
    struct LLNODE *curr, *temp;
    if(ll->head == NULL) return -1;
    if(compare(thr_info, &ll->head->threadinfo) == 0) {
        temp = ll->head;
        ll->head = ll->head->next;
        if(ll->head == NULL) ll->tail = ll->head;
        free(temp);
        ll->size--;
        return 0;
    }
    for(curr = ll->head; curr->next != NULL; curr = curr->next) {
        if(compare(thr_info, &curr->next->threadinfo) == 0) {
            temp = curr->next;
            if(temp == ll->tail) ll->tail = curr;
            curr->next = curr->next->next;
            free(temp);
            ll->size--;
            return 0;
        }
    }
    return -1;
}
 
void list_dump(struct LLIST *ll) {
    struct LLNODE *curr;
    struct THREADINFO *thr_info;
    printf("Connection count: %d\n", ll->size);
    for(curr = ll->head; curr != NULL; curr = curr->next) {
        thr_info = &curr->threadinfo;
        //printf("[%d] %s\n", thr_info->sockfd, thr_info->alias);
    }
}

/* Global Variables */
 
int sockfd, newfd;
struct THREADINFO thread_info[CLIENTS];
struct LLIST client_list;
struct FILELIST file_list;
pthread_mutex_t clientlist_mutex;
pthread_mutex_t filelist_mutex;
char *servermemory;
 
void *io_handler(void *param);
void *client_handler(void *fd);
 
int main(int argc, char **argv) {
    int err_ret, sin_size;
    struct sockaddr_in serv_addr, client_addr;
    pthread_t interrupt;
    servermemory = (char *)malloc(SERVERMEM);

    /* initialize linked list */
    list_init(&client_list);
    FileList_init(&file_list);

    /* initiate mutex */
    pthread_mutex_init(&clientlist_mutex, NULL);
    pthread_mutex_init(&filelist_mutex,NULL);
 
    /* open a socket */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        err_ret = errno;
        fprintf(stderr, "socket() failed...\n");
        return err_ret;
    }
 
    /* set initial values */
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_addr.s_addr = inet_addr(IP);
    memset(&(serv_addr.sin_zero), 0, 8);
 
    /* bind address with socket */
    if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) == -1) {
        err_ret = errno;
        fprintf(stderr, "bind() failed...\n");
        return err_ret;
    }
 
    /* start listening for connection */
    if(listen(sockfd, BACKLOG) == -1) {
        err_ret = errno;
        fprintf(stderr, "listen() failed...\n");
        return err_ret;
    }
 
    /* initiate interrupt handler for IO controlling */
    printf("Starting admin interface...\n");
    if(pthread_create(&interrupt, NULL, io_handler, NULL) != 0) {
        err_ret = errno;
        fprintf(stderr, "pthread_create() failed...\n");
        return err_ret;
    }
 
    /* keep accepting connections */
    printf("Starting socket listener...\n");
    while(1) {
	printf("ACCEPT LOOP STILL RUNNING\n");
        sin_size = sizeof(struct sockaddr_in);
        if((newfd = accept(sockfd, (struct sockaddr *)&client_addr, (socklen_t*)&sin_size)) == -1) {
            err_ret = errno;
            fprintf(stderr, "accept() failed...\n");
            return err_ret;
        }
        else {
            if(client_list.size == CLIENTS) {
                fprintf(stderr, "Connection full, request rejected...\n");
                continue;
            }
            printf("Connection requested received...\n");
            struct THREADINFO threadinfo;
            threadinfo.sockfd = newfd;
            pthread_mutex_lock(&clientlist_mutex);
            list_insert(&client_list, &threadinfo);
            pthread_mutex_unlock(&clientlist_mutex);
            pthread_create(&threadinfo.thread_ID, NULL, client_handler, (void *)&threadinfo);
        }
    }
    return 0;
}
 
void *io_handler(void *param) {
/*
    char option[OPTLEN];
    while(scanf("%s", option)==1) {
        if(!strcmp(option, "exit")) {
		CLEAN UP
            printf("Terminating server...\n");
            pthread_mutex_destroy(&clientlist_mutex);
            close(sockfd);
            exit(0);
        }
        else if(!strcmp(option, "list")) {
            pthread_mutex_lock(&clientlist_mutex);
            list_dump(&client_list);
            pthread_mutex_unlock(&clientlist_mutex);
        }
        else {
            fprintf(stderr, "Unknown command: %s...\n", option);
        }
    }*/
    return NULL;
}
/* This Function contains the main logic for a thread sending and recieving messages */

void *client_handler(void *fd) {
    struct THREADINFO threadinfo = *(struct THREADINFO *)fd;
    char msg[1024];
    struct LLNODE *curr;
    struct FILENODE *currentfile;

    int bytes, sent;
    while(1) {
	printf("Server: [%d] Waiting for Another Message...\n",threadinfo.sockfd);
	bytes = recv(threadinfo.sockfd, msg, BUFFSIZE ,0);
	msg[bytes] = '\0';
        // bytes = recv(threadinfo.sockfd, (void *)&packet, sizeof(struct PACKET), 0);
        if(!bytes) {
            //fprintf(stderr, "Connection lost from [%d] %s...\n", threadinfo.sockfd, threadinfo.alias);
            pthread_mutex_lock(&clientlist_mutex);
            list_delete(&client_list, &threadinfo);
            pthread_mutex_unlock(&clientlist_mutex);
            break;
        }
	char *token;
	char *search = " ";
	printf("THE MESSAGE RECIEVED IS: %s\n", msg);
	token = strtok(msg, "|");
	int type = atoi(token);
	printf("THE TYPE RECIEVED IS: %d\n", type);
	if(type == 1){
		token = strtok(NULL, "|");
		printf("Server: [%d] RECV MAP\n", threadinfo.sockfd);
		long floor_pointer = strtol(token,NULL,0);
		long ceiling_pointer = floor_pointer + BUFFSIZE;
		token = strtok(NULL, "|");
		pthread_mutex_lock(&filelist_mutex);
		int found = 0;
		printf("BEFORE ACTION: GOT HERE!\n");
		struct FILE * newfile = MakeFile(floor_pointer,ceiling_pointer);
		printf("AFTER NEW CREATION: GOT HERE!\n");
		for(currentfile = file_list.head; currentfile != NULL; currentfile = currentfile->next) {
			if(FileCompare(&currentfile->src, newfile) == 0) {
				if((currentfile->src.floor < floor_pointer) && (currentfile->src.ceiling < ceiling_pointer)){
					currentfile->src.floor = floor_pointer;
					currentfile->src.ceiling = ceiling_pointer;
				} else if(currentfile->src.floor < floor_pointer){
					currentfile->src.ceiling = ceiling_pointer;
				} else if(currentfile->src.ceiling < ceiling_pointer){
					currentfile->src.floor = floor_pointer;
				}
				found == 1;
				break;
			}
		}
		printf("AFTER SEARCH: GOT HERE!\n");
		if(found == 0){
			FileList_insert(&file_list, newfile);
		}
		FileList_dump(&file_list);
		printf("AFTER INSERT: GOT HERE!\n");
		pthread_mutex_unlock(&filelist_mutex);
	
		int fd = open((char*)token, O_RDWR);
		if(fd < 0) { //send back error message
		 } else { printf("File Opened"); }
		//char *reply = malloc(sizeof(char) * 1024);
		//reply = strdup("Jake Itfa Page hawnekk\0");
		//sent = send(curr->threadinfo.sockfd,reply, 1024, 0);
		printf("!!!");
		printf(" Floor: %lu , Ceiling: %lu , Filename: %s", floor_pointer, ceiling_pointer, token);
	}
	else if(type == 2){
		printf("Server: [%d] RECV READ %s\n", threadinfo.sockfd, msg);
	}
	else if(type == 3){
		printf("Server: [%d] RECV WRITE %s\n", threadinfo.sockfd, msg);
	}
	else if(type == 4){
		token = strtok(NULL, "|");
		printf("Server: [%d] RECV UNMAP \n", threadinfo.sockfd);
		long m_offset = strtol(token,NULL,0);
		token = strtok(NULL, "|");
		long m_pagesize = strtol(token,NULL,0);
		token = strtok(NULL, "|");
		int reply;
		// If Successful.
		reply = 1;
		reply = 0;
		printf("Offset: %lu , Pagesize: %lu, Filename: %s",m_offset,m_pagesize,token);
	}
	else if(type == 5){
		break;	
	} 
	else {
		printf("Recieved Something funny! : %d AND %s \n", type, msg);
	}
	printf("END: GOT HERE!\n");
	FileList_dump(&file_list);
	/*
        if(!strcmp(packet.option, "alias")) {
            printf("Set alias to %s\n", packet.alias);
            pthread_mutex_lock(&clientlist_mutex);
            for(curr = client_list.head; curr != NULL; curr = curr->next) {
                if(compare(&curr->threadinfo, &threadinfo) == 0) {
                    strcpy(curr->threadinfo.alias, packet.alias);
                    strcpy(threadinfo.alias, packet.alias);
                    break;
                }
            }
            pthread_mutex_unlock(&clientlist_mutex);
        }
        else if(!strcmp(packet.option, "whisp")) {
            int i;
            char target[ALIASLEN];
            for(i = 0; packet.buff[i] != ' '; i++); packet.buff[i++] = 0;
            strcpy(target, packet.buff);
            pthread_mutex_lock(&clientlist_mutex);
            for(curr = client_list.head; curr != NULL; curr = curr->next) {
                if(strcmp(target, curr->threadinfo.alias) == 0) {
                    struct PACKET spacket;
                    memset(&spacket, 0, sizeof(struct PACKET));
                    if(!compare(&curr->threadinfo, &threadinfo)) continue;
                    strcpy(spacket.option, "msg");
                    strcpy(spacket.alias, packet.alias);
                    strcpy(spacket.buff, &packet.buff[i]);
                    sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct PACKET), 0);
                }
            }
            pthread_mutex_unlock(&clientlist_mutex);
        }
        else if(!strcmp(packet.option, "send")) {
            pthread_mutex_lock(&clientlist_mutex);
            for(curr = client_list.head; curr != NULL; curr = curr->next) {
                struct PACKET spacket;
                memset(&spacket, 0, sizeof(struct PACKET));
                if(!compare(&curr->threadinfo, &threadinfo)) continue;
                strcpy(spacket.option, "msg");
                strcpy(spacket.alias, packet.alias);
                strcpy(spacket.buff, packet.buff);
                sent = send(curr->threadinfo.sockfd, (void *)&spacket, sizeof(struct PACKET), 0);
            }
            pthread_mutex_unlock(&clientlist_mutex);
        }
        else if(!strcmp(packet.option, "exit")) {
            printf("[%d] %s has disconnected...\n", threadinfo.sockfd, threadinfo.alias);
            pthread_mutex_lock(&clientlist_mutex);
            list_delete(&client_list, &threadinfo);
            pthread_mutex_unlock(&clientlist_mutex);
            break;
        }
        else {
            fprintf(stderr, "Garbage data from [%d] %s...\n", threadinfo.sockfd, threadinfo.alias);
        }
	*/
    }
 
    /* clean up */
    close(threadinfo.sockfd);
 
    return NULL;
}