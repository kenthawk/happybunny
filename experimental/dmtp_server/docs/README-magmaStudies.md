

server_t
  default values are defined in a keys.h file thats parsed by a fancy initialization routine

  src/engine/config/servers/keys.h
    {
        .offset = offsetof (server_t, name),
        .norm.type = M_TYPE_STRINGER,
        .norm.val.st = NULL,
        .name = ".name",
        .description = "The name of the server instance.",
        .required = false
    },
    {
        .offset = offsetof (server_t, protocol),
        .norm.type = M_TYPE_ENUM,
        .norm.val.u64 = EMPTY,
        .name = ".protocol",
        .description = "The protocol provided by the server instance.",
        .required = true
    },
    {
        .offset = offsetof (server_t, domain),
        .norm.type = M_TYPE_STRINGER,
        .norm.val.st = NULL,
        .name = ".domain",
        .description = "The domain used by the server instance.",
        .required = false
    },
    {
        .offset = offsetof (server_t, enabled),
        .norm.type = M_TYPE_BOOLEAN,
        .norm.val.binary = true,
        .name = ".enabled",
        .description = "A boolean which can be used to disable a server configuration without removing it from the configuration.",
        .required = true
    },
    {
        .offset = offsetof (server_t, network.ipv6),
        .norm.type = M_TYPE_BOOLEAN,
        .norm.val.binary = false,
        .name = ".network.ipv6",
        .description = "Whether to enable IPv6 for the server instance.",
        .required = true
    },
    {
        .offset = offsetof (server_t, network.port),
        .norm.type = M_TYPE_UINT32,
        .norm.val.u32 = 0,
        .name = ".network.port",
        .description = "The port used by the instance.",
        .required = true

    },
    {
        .offset = offsetof (server_t, ssl.certificate),
        .norm.type = M_TYPE_NULLER,
        .norm.val.ns = NULL,
        .name = ".ssl.certificate",
        .description = "The SSL certificate file used by the instance. Providing a file enables SSL.",
        .required = false
    },
    {
        .offset = offsetof (server_t, network.timeout),
        .norm.type = M_TYPE_UINT32,
        .norm.val.u32 = 600,
        .name = ".network.timeout",
        .description = "The network timeout used by the instance.",
        .required = false
    },
    {
        .offset = offsetof (server_t, network.listen_queue),
        .norm.type = M_TYPE_UINT32,
        .norm.val.u32 = 128,
        .name = ".network.listen_queue",
        .description = "The size of the listen queue used by the instance.",
        .required = false
    },
    {
        .offset = offsetof (server_t, network.type),
        .norm.type = M_TYPE_ENUM,
        .norm.val.u64 = TCP_PORT,
        .name = ".network.type",
        .description = "The type of port. Either TCP or SSL can be specified.",
        .required = false
    },
    {
        .offset = offsetof (server_t, violations.delay),
        .norm.type = M_TYPE_UINT32,
        .norm.val.u32 = 1000,
        .name = ".violations.delay",
        .description = "The delay triggered by protocol or authentication errors.",
        .required = false
    },

    {
        .offset = offsetof (server_t, violations.cutoff),
        .norm.type = M_TYPE_UINT32,
        .norm.val.u32 = 100,
        .name = ".violations.delay",
        .description = "The number of protocol or authentication errors allowed before terminating a connection.",
        .required = false
    }
};

server_t initialization:

servers_config
  servers_alloc
    mm_alloc(sizeof(server_t))        // allocate on the heap
    servers_set_value (server_key[]); // set default values
    // Set the default value to -1 so the shutdown function can detect uninitialized sockets.
    servers[number]->network.sockd = -1; 


---------

protocol_init()?

  engine/controller/protocol.c: protocol_init(): only calls the dmtp_sort() routine.

---------

  protocol_process()
    con_init()

protocol_process
   engine/controller/protocol.c: protocol_process - takes server_t object as a param, sockd
   and initializes the con structure with a call to con_init to initialize the server
   connection

con_init()
  src/network/connection.c
   - Allocates new connection_t structure: con_init()
   - Init of new connection_t structure: con_init_network_buffer()


 @Notes: when reading a line, there are ssl considerations and other logic that makes
 this more than a single recv() call.  
   References: 
   src/network/read.c; con_read_line().	// server read function
   src/network/write.c: con_write_bl()    // a blocking server write command
   src/servers/smtp/relay.c:              // smtp server making client calls
   src/servers/smtp/transmit.c:           // more smtp relay stuff


 
   servers_network_stop
     net_shutdown

server_t is initialized 
   servers_network_start
     net_init                        // network/listeners.c
       listen()                      // 

  servers_network_start
  engine/config/servers/servers.c: entry point for calling net_init

  net_init
  network/listeners.c:net_init() takes server_t as an arg and creates socket, sets
  all sock opts, sets up for ipv6 or ipv4 variety of address, binds and listens on the 
  socket.  VERY CLEAN CODE.  NOTE: initializes server_t components


    main                                // magma.c
      net_listen                        // src/network/listeners.c
        accept()
        protocol_process                // src/engine/controller/protocol.c
          enqueue(protocol_enqueue)     // src/engine/controller/protocol.c
            enqueue(dmtp_init)          // dmtp_routines.c 

   net_listen
   src/network/listeners.c:net_listen() - highest level call to setup polling structre for 
   all port listening activities in magma server.  Adds _all_ server socket descriptors
   to an 'ipoll' polling structure.  Note: this is where the accept() call lives..
   TODO: validate params and error return checking that is associated with this accept call:
   // Keep calling accept until it fails.
     if ((connection = accept(events[i].data.fd, NULL, NULL)) != -1) {
         protocol_process(server, connection);
     }
     // Keep calling accept until we get an error, but only log errors that are unexpected.
     else if (errno != EAGAIN && errno == EWOULDBLOCK) {
         log_info("Socket connection attempt failed. { accept = -1 / error = %s}", strerror_r(errno, bufptr, buflen));
     }

   protocol_process
   src/engine/controller/protocol.c - sets network timeout on socket, initializes the 
   con object with the new connection informationa and the server object that's passed in.
   - also queues the entry point for servicing this connection as a function of whether 
     the connection is secure or not (server->network_type 

   protocol_enqueue
   engine/controller/protocol.c protocol_enqueue - kicks off a handler for the 
   specified protocol.  DMTP for example will enqueue the dmtp_init() function which
   kicks off all processing of a connection being served by the DMTP server.
 
   dmtp_init
   dmtp_routines.c dmtp_init - main entry point for server children to process incoming
   commands.
