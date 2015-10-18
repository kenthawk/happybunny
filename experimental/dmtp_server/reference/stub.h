/**
 * @file /brimstone/dmtp/server/stub.h
 *
 * @brief
 *  This is a stub of structures defined in the magma code base that the 
 *  existing dmtp codebase was depending on. Plans are to replace all reliance
 *  on these structures, abstraction layers, and other magma dependencies
 *  with non-magma dependent support.  Ifdef or remove anything there that
 *  the dmtp server doesn't require.
 *
 * @Author
 *   Kent
 *
 */

#ifndef STUB_H
#define STUB_H
// ====================================================================
// from: src/core/core.h
typedef char chr_t;
typedef bool bool_t;
typedef int32_t int_t;
typedef uint32_t uint_t;
typedef unsigned char uchr_t;
typedef unsigned char byte_t;

// ====================================================================
// from: src/engine/config/servers/servers.h
typedef enum {
    TCP_PORT = 1,
    SSL_PORT
} M_PORT;

typedef enum {
    MOLTEN = 1,
    HTTP,       
    POP,        
    IMAP,   
    SMTP, 
    DMTP,
    SUBMISSION
} M_PROTOCOL;

typedef struct {
#if 0
    struct {
        SSL_CTX *context;
        char *certificate;
    } ssl; 
#endif
    struct {
        int sockd;
        bool_t ipv6;
        uint32_t port;
        uint32_t timeout;
        uint32_t listen_queue;
        M_PORT type;
    } network;  
    struct {    
        uint32_t delay;
        uint32_t cutoff;
    } violations;
    bool_t enabled;
    // stub for STS
    char *name, *domain;
    M_PROTOCOL protocol;
} server_t; 

// from: src/network/network.h
typedef struct {
    char *string;
    size_t length;
    void *function;
} command_t;

typedef struct {
#if 0
    void *ssl; /* The SSL connection object. */
    int sockd; /* The socket connection. */
    int status; /* Track whether the last network generated an error. */
    placer_t line; /* The current line being processed. */
    // stub for STS
    char *buffer; /* The connection buffer. */
#endif
} client_t;

#if 0
typedef struct {
    union {
        pop_session_t pop;
        imap_session_t imap;
        smtp_session_t smtp;
        dmtp_session_t dmtp;
        http_session_t http;
    };

    struct {
        uint32_t spins;
        uint32_t violations;
    } protocol;

    struct {
        void *ssl; /* The SSL connection object. */
        int sockd; /* The socket connection. */
        int status; /* Track whether the last network operation generated an error. */
        placer_t line; /* The current line being processed. */
        stringer_t *buffer; /* The connection buffer. */
    
        struct {
            int_t status;
            stringer_t *domain;
        } reverse;

    } network;
    uint64_t refs; /* The number of memory references or threads pointing at this structure. */
    pthread_mutex_t lock; /* The mutex used for locking during non-thread save operations. */
    server_t *server; /* The server instance that accepted the connection. */
    command_t *command; /* The command structure. */
} connection_t;
#endif
typedef struct {
    char *todo;             // TODO: any dmtp session related allocation goes here
} dmtp_session_t;

typedef struct {
    dmtp_session_t dmtp;
    struct {
        uint32_t spins;
        uint32_t violations;
    } protocol;
    struct {
        int sockd;          // sockd returned from accept()
        int status;         // 0: in process, 1: success
        char *line;         // replace with SDS string
        char *buffer;       // connection buffer.  needs to be allocated!!!

        struct {
            int_t status;   // TODO: init??
            char *domain;   // TODO: SDS!  needs to be allocated
        } reverse;

    } network;
    server_t *server;       // server that accepted the connection
    command_t *command;     // command structure for this protocol
} connection_t;
        

#endif
