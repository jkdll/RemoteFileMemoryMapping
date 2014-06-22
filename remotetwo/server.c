#include <my_sock.h>
#include <errno.h>
    
#define PORT 6969
#define BUFSIZE 8192
#define PERM (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define MAXTHREADS 25

void * f00(void *data)
{   
    char    buf[BUFSIZE];
    int asock = (int) data, nread = 0, fd = 0;
    
    //open the file
    if( (fd = open("tmp", O_CREAT | O_TRUNC | O_WRONLY, PERM)) < 0)
        err_quit("open");
    
    //read from socket, write to file
    while( (nread = read(asock, buf, BUFSIZE)) > 0) 
        if(write(fd, buf, nread) != nread)
            err_quit("write");
        
    close(fd);
    return;
};          

int main(int argc, char **argv)
{       
    u_int   rbufsz = 64000;
    int     on = 1, fd = 0, lsock = 0, asock = 0, nread = 0, nthreads = 0, x = 0;
    char    buf[BUFSIZE];
    struct  sockaddr_in sa, csa;
    struct in_addr  *inptr = NULL;
    socklen_t len = sizeof(sa);
    pthread_t   threadid[MAXTHREADS];
    
    //initialize the socket -- returns < 0 on error
    if ( (lsock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        perr_quit("socket");
    
    if(setsockopt(lsock, SOL_SOCKET, SO_RCVBUF, &rbufsz, sizeof(rbufsz)) != 0)
        perr_quit("setsockopt");
    
    if(setsockopt(lsock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        perr_quit("setsockopt");

    //setup the sock_addrin structure
    memset(&sa, 0, sizeof(sa));
    memset(&csa, 0, sizeof(csa));
    sa.sin_family = AF_INET;
    sa.sin_port   = htons(PORT);    
    sa.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //bind socket 
    if( (bind(lsock, (struct sockaddr *) &sa, sizeof(sa))) < 0)
        err_quit("bind");
    
    //go into listen mode
    if( (listen(lsock, 5)) < 0)
        err_quit("listen");
    
    while(1){
        //accept a connection
        if( (asock = accept(lsock, (struct sockaddr *)&csa, &len)) < 0)
            perr_quit("accept");
        
        printf("address is %d.%d.%d.%d\n", NIPQUAD(csa.sin_addr.s_addr) );
        
        if(setsockopt(lsock, SOL_SOCKET, SO_RCVBUF, &rbufsz, sizeof(rbufsz)) != 0)
            perr_quit("setsockopt");
        
        //spawn a thread for each clien
        if(pthread_create(&threadid[x++], NULL, (void *)&f00, (int)asock) != 0)
            perr_quit("pthread");
        
        if(nthreads++ >= MAXTHREADS)
            break;
    }
    
    //wait for all threads
    for(x = 0; x < nthreads; x++)
        if(pthread_join(threadid[x], NULL) < 0)
            perr_quit("pthread join");
    
    //cleanup
    close(lsock);
    
    return 0;
}
