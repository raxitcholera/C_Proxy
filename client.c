#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[] = 
"GET http://www.geeksforgeeks.org/given-a-sequence-of-words-print-all-anagrams-together/ HTTP/1.1\r\n"
"Host: www.geeksforgeeks.org\r\n"
"Connection: keep-alive\r\n"
"Cache-Control: max-age=0\r\n"
"Upgrade-Insecure-Requests: 1\r\n"
"User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_11_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/53.0.2785.143 Safari/537.36\r\n"
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"
"Referer: https://www.google.com/\r\n"
"Accept-Encoding: gzip, deflate, sdch\r\n"
"Transfer-Encoding: chunked\r\n"
"Accept-Language: en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;q=0.6en-GB,en-US;q=0.8,en;q=0.6\r\n\r\n";

    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    printf("Please enter the message: ");
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,sizeof(buffer));
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    return 0;
}
