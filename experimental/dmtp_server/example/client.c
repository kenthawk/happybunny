
/**
 * @file example/client.c
 *
 * @brief
 *   Example client, processing line based input
 * @Author
 *   Kent.  Based on example from:
 *   http://www.cs.dartmouth.edu/~campbell/cs50/socketprogramming.html
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXLINE 4096        // max text line length
#define SERV_PORT 3000      // port

int
main(int argc, char **argv) {
    int sockfd, n;
    int estat = 0;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];
	
    if (argc != 2) {
        perror("Usage: <IP address of the server>"); 
        estat = 1;
        goto error;
    }

    // create the socket descriptor
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Problem in creating the socket");
        estat = 2;
        goto error;
    }

    // create the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr= inet_addr(argv[1]);
    servaddr.sin_port =  htons(SERV_PORT);          // big-endian

    // connect the socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        perror("connect failed()");
        estat = 3;
        goto cleanup_sockfd;
    }

    while (fgets(sendline, MAXLINE, stdin) != NULL) {
        if (send(sockfd, sendline, strlen(sendline), 0) == -1) {
            perror("send() to server failed");
            estat = 4;
            goto cleanup_sockfd;
        }
        if ((n = recv(sockfd, recvline, MAXLINE,0)) == 0) {
            //error: server terminated prematurely
            perror("recv() from server failed"); 
            estat = 5;
            goto cleanup_sockfd;
        }
// stub
printf ("Server returned: '%s'\n", recvline);
        if (strcasecmp("quit", recvline) == 0) {
            printf ("Exiting\n");
            goto cleanup_sockfd;
        }
    }
    perror("fgets() failed\n");
    estat = 6;

cleanup_sockfd:
    close(sockfd);
error:
    return estat;
}
