
/**
 * @file example/server.c
 *
 * @brief
 *   Example server, processing line based input
 * @Author
 *   Kent.  Based on example from:
 *   http://www.cs.dartmouth.edu/~campbell/cs50/socketprogramming.html
 *
 * TODO: where does con->protocol.violations get init?  And updated?
 * - violations gets bumped on invalid dmtp commands
 *
 * TOD: where in magma does con->protocol.violations and spins get initialized?
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
#include <stdbool.h>


#include "./stub.h"
#include "./socket_options.h"
#include "./dmtpd.h"

#define SERV_PORT 3000          // port
#define LISTENQ 128               // maximum number of client connections

#define MAX_LINE_BUFFER         2048 
#if 0
#define MAXLINE 4096            // max text line length
#endif

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

server_t * server_new (uint32_t port) {
    server_t *s;
    s = malloc (sizeof(server_t));
  
    // Note: default values from src/engine/config/servers/keys.h and
    //                           src/engine/config/servers/servers.c:servers_set_value 
    //s->ssl.context = NULL;
    //s->ssl.certificate = NULL;

    s->network.sockd        = -1;       // reset after call to bind, listen
    s->network.ipv6         = false;
    s->network.port         = port;
    s->network.timeout      = 600;
    s->network.listen_queue = LISTENQ;
    s->network.type         = TCP_PORT;

    s->violations.delay     = 1000;
    s->violations.cutoff     = 100;

    s->enabled = true;
    s->name = "STUB_NAME";
    s->domain = "STUB_DOMAIN";

    s->protocol = DMTP;

    return s;
}   // server_new()

bool_t
net_init(server_t *server) {
    int sd;
    struct sockaddr_in sin4;
    struct sockaddr_in6 sin6;

    // Create the socket.
    if ((sd = socket(server->network.ipv6 ? AF_INET6 : AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket: ");
        goto error;
    }

    // Set non-blocking IO.
    if (!net_set_non_blocking(sd, false)) {
        goto error;
    }

    // Make this a reusable socket.
    if (!net_set_reuseable_address(sd, true)) {
        goto error;
    }

#if TODO    // TODO: track down what magma thinks is a good idea
    if (!net_set_buffer_length(sd, magma.system.network_buffer, magma.system.network_buffer)) {
        fprintf(stderr,"Could not configure the socket buffer size.");
        goto error;

    }
#endif
    if (server->network.ipv6) {             // Init for ipv6 socket support
        bzero(&sin6, sizeof(sin6));
        sin6.sin6_family = AF_INET6;
        sin6.sin6_addr = in6addr_any;
        sin6.sin6_port = htons(server->network.port);

        // Bind the socket.
        if (bind(sd, (struct sockaddr *)&sin6, sizeof(sin6)) == -1) {
            perror("bind: ");
            goto error;
        }
    }
    else {                                  // Init for ipv4 socket supprot
        bzero(&sin4, sizeof(sin4));
        sin4.sin_family = AF_INET;
        sin4.sin_addr.s_addr = INADDR_ANY;
        sin4.sin_port = htons(server->network.port);

        // Bind the socket.
        if (bind(sd, (struct sockaddr *)&sin4, sizeof(sin4)) == -1) {
            perror("bind: ");
            goto error;
        }
    }

    // Start listening for incoming connections. We set the queue to our config file listen queue value.
    if (listen(sd, server->network.listen_queue) == -1) {
        perror("listen: ");
        goto error;
    }

    // Store the socket descriptor elsewhere, so it can be shutdown later.
    server->network.sockd = sd;

    return true;
error:
    return false;
}   // net_init()

connection_t *
con_new(server_t *server, int sockd) {
    connection_t *con;

    // magma ref: protocol_process
    if (server == NULL) {
        fprintf (stderr, "con_new: server arg is NULL\n");
        goto error;
    }
    if (sockd == -1) {
        fprintf (stderr, "con_new: sockd not valid\n");
        goto error;
    }
    if (!net_set_timeout(sockd, server->network.timeout, server->network.timeout)) {
        fprintf (stderr, "con_new: net_set_timeout failed\n");
        goto error;
    }
    // magma ref: con_init
    if ((con = malloc(sizeof(connection_t))) == NULL) {
        perror("malloc: ");
        goto error;
    }       
    con->network.status = 0;            // init


    con->protocol.spins = 0;
    con->protocol.violations = 0;

    con->network.sockd = sockd;
    con->network.line = NULL;
    con->network.buffer = malloc (MAX_LINE_BUFFER);

    con->server = server;
    con->command = NULL;

    con->network.status = 1;            // we are connected!

    return con;
error:
    return NULL;
}   // con_new()


int
main (int argc, char **argv) {
    int sockd;
    int childcount = 0;
    pid_t childpid;
    struct sockaddr_in cliaddr, servaddr;
    //socklen_t clilen;
    //struct sockaddr_in cliaddr;

    server_t *server;
    connection_t *con;

    server = server_new(SERV_PORT);

    // Init all things network related
    if (!net_init(server)) {
        goto error;
    }

    // Prevent zombie processes by processing signal when children exit
    signal(SIGCHLD, sigchld_handler);

    // sorting the commands happens once
    dmtp_sort();

    for (;;) {
        if ((sockd = accept (server->network.sockd, (struct sockaddr *) NULL, NULL)) == -1) {
            perror("accept() failed");
            goto error;
        }
    
        childcount++;
        // TODO: remember to free all heap memory allocated in server and con 
        //structures on exit
        if ( (childpid = fork ()) == 0 ) {   // we are the child
            //close listening socket
            close (server->network.sockd);

            // Create connection object
            con = con_new(server, sockd);

            dmtp_init(con);

            for (;con->command && con->command->function != &dmtp_quit;) {

                if ((dmtp_process(con) == -1) ||
                    (con_status(con) < 0)) {
                    con->command->function = &dmtp_quit;
                }

                if (con->command == NULL) {
                    continue;
                }

                // perform the command
                con->command->function(con);

                if (con->command->function == &dmtp_quit) {
                    break;
                }
            }
            goto out;           // child must exit
        }
        else {                  // server only
            close(sockd);
        }
    }                           // server returns to accept the next connection

out:
    return 1;
error:
    return 0;
}
