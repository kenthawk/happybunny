// dmtpd_cfg.h - header for lemon cfg related user defined structures

#ifndef DMTPD_CFG_H
#define DMTPD_CFG_H

#include <stdbool.h>

/* Darkmail spec, page 84: All implementations must be capable of handling
 * lines which are up to 512 octets in length.
 */
#define MAXCMDLEN 512

/* A debug string describing the tokenized form of an input command string
 * of length MAXCMDLINE.
 */
#define MAXLEXLEN 2048

typedef struct {
    int line;
    int rc;
    int connfd;
    int con_stat;           // 0- not connected, 1- connected, 2- TLS
    bool quit;
    char lexStr[MAXLEXLEN];
} parse_t;

#endif
