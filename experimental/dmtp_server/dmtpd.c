
/**
 * @file example/server.c
 *
 * @brief
 * Example server, processing line based input
 *
 * @Author
 * Kent.  Based on example from:
 * http://www.cs.dartmouth.edu/~campbell/cs50/socketprogramming.html
 *
 * TODO: why do I have to declare 'extern int Connfd;' here versus
 * declaring it 'int Connfd;' here and extern in the lex.l file?  Seems
 * like it would be the same.  ie: One global decl and one extern global
 * decl.  Zach help!
 *
 * TODO: how is max input line length controlled?  There's a define for the 
 * length maintained in dmtpd_cfg.h @ define MAXCMDLEN
 */

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "cfg.h"
#include "dmtpd_cfg.h"
#include "lex.h"

// Global context
parse_t *ps;

// lemon functions
void *ParseAlloc();
void Parse();
void ParseFree();


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

// new_ps: allocate a new parse context structure
parse_t *
new_ps () {
    parse_t *ps;
    if ((ps = malloc(sizeof (parse_t))) == NULL) {
        goto error;
    }
    ps->line = 1;
    ps->rc = 0;
    ps->connfd = 0;
    ps->con_stat = 0;       // not connected
    ps->quit = false;
    ps->lexStr[0] = '\0';
    return ps;
error:
    return NULL;
}

int
main (int argc, char **argv) {
    int listenfd;
    int estat = 0;
    int childcount = 0;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    int lexCode;
    char intStr[128];        // debug
    void * pParser;

    // global context - constructor
    ps = new_ps();

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
        ps->connfd = accept (listenfd, (struct sockaddr *) &cliaddr, &clilen);
        if (ps->connfd == -1) {
            perror("accept() failed");
            estat = 4;
            goto error;
        }
        ps->con_stat = 1;
        childcount++;

        if ((childpid = fork ()) == 0) {   // we are the child
            estat = 0;
            //close listening socket
            close (listenfd);

            // Setup parser
            pParser = ParseAlloc(malloc);

/*
 * Three things to set/check during the following lex/scan operations.
 * 1) ps parse context needs to record/report when a QUIT command or
 *    error condition has occurred to direct this loop to end and
 *    close the socket.
 * 2) ps parse context needs to record any error state in order
 *    to report and communicate with/close down the client connection
 *    and socket.
 * 3) If we're in a data state, bypass the lex part and slurp all data
 *    until a \n.\n condition has been satified.
 */
            // reset lexStr to null
            ps->lexStr[0] = '\0';

            for (;;) {
                lexCode = yylex();
if (lexCode != EOL) {
    sprintf (intStr, "|%d(", lexCode);
    strcat (ps->lexStr, intStr);
    strcat (ps->lexStr, yyget_text());
    strcat (ps->lexStr, ")");
}
                if (lexCode > 0) {
                    Parse(pParser, lexCode, strdup(yyget_text()), ps);
                }

                if (lexCode == EOL) {
                    strcat (ps->lexStr, "|EOL|");
                    Parse (pParser, 0, NULL, ps);       // Process the line
                    ps->rc = 0;
                    ps->line++;
                    // reset the lexStr
                    ps->lexStr[0] = '\0';

#if 0
                    if (ps->quit) {
                        send(ps->connfd, "QUIT\n", 6, 0);
                        break;
                    }
                    send(ps->connfd, "OK\n", 4, 0);
#endif
                }

                if (lexCode <= 0 || ps->quit) {
                    break;
                }
            }

            estat = lexCode;        // remote this
            if (lexCode == -1) {
                if (errno == EAGAIN) {
                // scanner error, complain and bail
                    printf ("Child[%d]: EAGAIN! (timeout?)\n", childcount);
                }
                else {
                    printf ("Child[%d]: scanner error on %s\n", childcount, yyget_text());
                }
            }
            else {
                printf ("Child[%d]: lexCode == %d\n", childcount, lexCode);
            }

            // Client gets here on ^C, Do not allow the client to loop up and 
            // call accept!
            //goto client_exit;
            printf("Child[%d] goodbye!\n", childcount);
            //yylex_destroy(scanner);
            ParseFree(pParser, free);
            goto cleanup_connfd;
        }
        else {                  // server only
            close(ps->connfd);
        }
    }                           // server returns to accept the next connection

cleanup_connfd:
    close(ps->connfd);
error:
    return estat;
}
