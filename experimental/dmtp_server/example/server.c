
/**
 * @file example/server.c
 *
 * @brief
 *   Example server, processing line based input
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
#include <unistd.h>
#include <signal.h>

#define MAXLINE 4096            // max text line length
#define SERV_PORT 3000          // port
#define LISTENQ 2               // maximum number of client connections

// sigchld_handler(): parent to call waitpid then the child exits so zombie 
// processes don't build up
void sigchld_handler(int sig) {
    pid_t pid = 0;;
    //while (waitpid(-1, 0, WNOHANG) > 0);

    // waitpid (pit_t pid, int *stat_loc, int options);
    // pig == -1 is wait for any child.
    // pid == 0 is a child in the parent's process group.
    while ((pid = waitpid(0, 0, WNOHANG)) > 0) {
      printf("Parent: handled SIGCHLD signal for childpid %d!\n", pid);
    }
    return;
}

// myline_boil: build a null terminated string from the input line
void
myline_boil (char *in, char *out) {
    int i; 
    for (i = 0; i < MAXLINE; i++)  {
        out[i] = in[i];

        if (in[i] == '\n') {
            out[i] = 0;
            break;
        }
    }
    // null terminate the last char
    if (i == MAXLINE) out[i-1] = 0;
    return;
}

int
main (int argc, char **argv) {
    int listenfd, connfd, n;
    int estat = 0;
    int childcount = 0;
    pid_t childpid;
    socklen_t clilen;
    char buf[MAXLINE];
    char myline[MAXLINE];       // private copy
    struct sockaddr_in cliaddr, servaddr;

    bzero((char *) myline, MAXLINE);
    bzero((char *) buf, MAXLINE);

    // Prevent zombie process by doing necessary cleanup when the 
    // child exits.
    signal(SIGCHLD, sigchld_handler);

    //If sockfd<0 there was an error in the creation of the socket
    if ((listenfd = socket (AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket() failed");
        estat = 1;
        goto error;
    }


    //preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    //bind the socket
    if (bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) == -1) {
        perror("bind() failed");
        estat = 2;
        goto error;
    }

    //listen to the socket by creating a connection queue, then wait for clients
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen() failed");
        estat = 3;
        goto error;
    }

    printf("%s listening on %d\n", argv[0], SERV_PORT);

    for (;;) {
        // the following call to accept can change the value of this var. ?
        clilen = sizeof(cliaddr);

        //accept a connection
        connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
        if (connfd == -1) {
            perror("accept() failed");
            estat = 4;
            goto error;
        }
        childcount++;

        if ((childpid = fork ()) == 0) {   // we are the child
            estat = 0;
            //close listening socket
            close (listenfd);

            while ((n = recv(connfd, buf, MAXLINE,0)) > 0)  {
#if 0
                printf("Child[%d] Recv'ed %d chars\n", childcount, n);
                for (int i = 0; i < n+1; i++)  {    // test one char past the buff
                    if ((buf[i] >= 'a' && buf[i] <= 'z') || 
                        (buf[i] >= 'A' && buf[i] <= 'Z') ||
                        (buf[i] >= '0' && buf[i] <= '9'))
                        printf("%c |", buf[i]);
                    else if (buf[i] == '\n') 
                        printf("\\n|");
                    else
                        printf ("? |");
                }
                printf("\n");

                for (int i = 0; i < n+1; i++)  {      // test one char past the buff
                    printf ("%02x|", buf[i]);
                }
                printf("\n");
#endif
                myline_boil (buf, myline);
                printf("Child[%d] '%s'\n", childcount, myline);

                // +1 is for the null terminator!
                send(connfd, myline, strlen(myline)+1, 0);

                // special token for quitting the client connection.
                if (strcasecmp(myline, "quit") == 0)  break;
            }
            if (n < 0) {
                printf("Child[%d] Read error\n", childcount);
                estat = n;
            }
            // Client gets here on ^C, Do not allow the client to loop up and 
            // call accept!
            //goto client_exit;
            printf("Child[%d] goodbye!\n", childcount);
            goto cleanup_connfd;
        }
        else {                  // server only
            close(connfd);
        }
    }                           // server returns to accept the next connection

cleanup_connfd:
    close(connfd);
error:
    return estat;
}
