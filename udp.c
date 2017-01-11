/*
 * udp: packet radio version 1
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define BUFSIZE 1024

void error(char *s)
{
  extern int errno;
  printf(" FATAL ERROR (%s) (errno=%d)\n", s);
  exit(errno);
}

int main(int argc, char **argv) {
    int sockfd, portno, n;
    int serverlen, nr,ns;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    int fd;
    useconds_t delay;
    char buf[BUFSIZE];


    /* check command line arguments */
    if (argc != 5) {
       fprintf(stderr,"usage: %s <hostname> <port> <delay in ms> <filename>\n", argv[0]);
       exit(0);
    } else {
        hostname = argv[1];
        portno = atoi(argv[2]);
    }
        /* set hostname, portno etc from cmdline arguments */

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* build the server's Internet address (same as proxy) */
    /* use getaddrinfo or gethostbyname */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(hostname);
    serveraddr.sin_port = htons((unsigned short)portno);

    printf(" server %x:%d (%d)\n",serveraddr.sin_addr.s_addr,serveraddr.sin_port,portno);


     // server=gethostbyname(host); 
    
    /* open file */
    if ((fd = open(argv[4],O_RDONLY)) < 0)
    {
        printf("%s\n",argv[4] );
        error(" ERROR can't open file!\n");

    }
    /* send the message to the server */
    for(;;) {
        /* read from file: read(fd,buffer,bytes) */
        bzero((char *) &buf, sizeof(buf));
        nr = read(fd,buf,1024);
        /* send UDP packet */
         printf("read %d\n",nr );
        ns =sendto(sockfd,buf,nr,0,(struct sockaddr *)&serveraddr,serverlen);
        /* wait for <delay> msec */
         printf("sent %d\n",ns );
        usleep(atoi(argv[3]));
        /* break when done reading file */
        if (nr<=0) {
            break;
        }


    }

    printf(" done\n");
    return 0;
}
