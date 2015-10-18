
# Reference material

## RUsage

From [gnu.org] (https://ftp.gnu.org/old-gnu/Manuals/glibc-2.2.3/html_chapter/libc_22.html) 
accessing the rusage struct in the waitpid call would be handy for testing resource 
utilization and may be used to detect memory issues. It also gives access to system and 
user time resources used by the exiting child.

Getursage() could be used by any process to return the rusage structure for examination.


## Server Design

From [Linux Howtos] (http://www.linuxhowtos.org/C_C++/socket.htm)

**Designing Servers**

There are a number of different ways to design servers. These models are discussed in detail in a book by Douglas E. Comer and David L. Stevens entiteld Internetworking with TCP/IP Volume III:Client Server Programming and Applications published by Prentice Hall in 1996. These are summarized here.
 
**Concurrent, connection oriented servers**

The typical server in the Internet domain creates a stream socket and forks off a process to handle each new connection that it receives. This model is appropriate for services which will do a good deal of reading and writing over an extended period of time, such as a telnet server or an ftp server. This model has relatively high overhead, because forking off a new process is a time consuming operation, and because a stream socket which uses the TCP protocol has high kernel overhead, not only in establishing the connection but also in transmitting information. However, once the connection has been established, data transmission is reliable in both directions.

**Iterative, connectionless servers**

Servers which provide only a single message to the client often do not involve forking, and often use a datagram socket rather than a stream socket. Examples include a finger daemon or a timeofday server or an echo server (a server which merely echoes a message sent by the client). These servers handle each message as it receives them in the same process. There is much less overhead with this type of server, but the communication is unreliable. A request or a reply may get lost in the Internet, and there is no built-in mechanism to detect and handle this.

**Single Process concurrent servers**

A server which needs the capability of handling several clients simultaneous, but where each connection is I/O dominated (i.e. the server spends most of its time blocked waiting for a message from the client) is a candidate for a single process, concurrent server. In this model, one process maintains a number of open connections, and listens at each for a message. Whenever it gets a message from a client, it replies quickly and then listens for the next one. This type of service can be done with the select system call.


## Examples
###Concurrent, Connection Oriented Server Example (forked model)

Example used to test concurrent server at [cs.rpi.edu](http://www.cs.rpi.edu/~moorthy/Courses/os98/Pgms/socket.html)

[Linux HowTos](http://www.linuxhowtos.org/C_C++/socket.htm) shows another concurrent/forking example with explanation of zombie processes that don't clean up correctly.

[Dartmouth.edu](http://www.linuxhowtos.org/C_C++/socket.htm) contains a precise set of examples of an interative server (one client request at a time) and a concurrent server.  The concurrent server is one of the best I found that demonstrates the forking mechanism to spawn children to handle the client requests.

[Tutorialspoint](http://www.tutorialspoint.com/unix_sockets/socket_server_example.htm)
contains another example of the concurrent server implementation in addition to a wealth of information on all subjects related to socket programming.  [More](http://www.tutorialspoint.com/unix_sockets/socket_quick_guide.htm) coverage on write(), read(), fork() functions and another example of a server implementation.  This one uses slightly different system includes.. why?

## Useful Commands for socket work

Examine the state of the listening socket:

	> sudo lsof -i -P | grep <your socket #>





