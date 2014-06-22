#include <my_sock.h>
#include <sys/stat.h>

#define PORT 6969
#define BUFSIZE 8192

int main(int argc, char **argv)
{
    int     sock = 0, fd = 0, sendret = 0, nread = 0;
    char    buf[BUFSIZE];
    struct  sockaddr_in servaddr;
    struct stat     statbuf;

    if (argc != 3){
        printf("Usage: %s <server ip> <file>\n", argv[0]);
        return 1;
    }

    //initialize the socket -- returns < 0 on error
    if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_quit("socket");

    //open the file
    if( (fd = open(argv[2], O_RDONLY)) < 0)
        err_quit("open");

    //setup the sock_addrin structure
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port   = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);

    //connect to the server, returns < 0 on error
    if (connect(sock, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0)
        err_quit("connect");

    //send the file
    while( (nread = read(fd, buf, BUFSIZE)) > 0)
        if(write(sock, buf, nread) < 0)
            err_quit("write");

    close(fd);
    close(sock);

    return 0;
}
