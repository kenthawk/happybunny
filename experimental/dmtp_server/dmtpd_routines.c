/**
 * @file dmtpd_routines.c
 */

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "dmtpd_cfg.h"
#include "dmtpd_routines.h"

#define MAXCAP 128
#define MAXARGSTR 256

#define PS_CHECK_SYNTAX(x)   if (ps->rc!=0) {\
                                con_print(ps, ERRORSTR_501, ps->lexStr);\
                                return;\
                             }


void con_print (parse_t *ps, char *msg, char *extra) {
    char outbuf[MAXCAP*2 + 5];
    outbuf[0] = '\0';

    if (msg != NULL)
        strncat (outbuf, msg, MAXCAP);

    if (extra != NULL) {
        strncat(outbuf, " {", 2);
        strncat(outbuf, extra, MAXCAP);
        strncat(outbuf, "}", 1);
    }

    strncat(outbuf, "\n", 1);

    if (ps->con_stat) {
        send(ps->connfd, outbuf, sizeof(outbuf)+1, 0);     // +1 includes the null term in order to
                                                        // be client friendly when it prints the str
    }
    else
        printf("%s", outbuf);

    return;
}

//void dmtp_data (parse_t *ps, char *fname) {
// test only: arg fname is used to write all data content that follows
// to a file named "fname". KDH
void dmtp_data (parse_t *ps, char *fname) {
    FILE *fp;
    int eob_state = 0;
    PS_CHECK_SYNTAX();

    con_print(ps, "354 READY TO RECEIVE MESSAGE", fname);

    if (fname == NULL)
        goto out;

    if ((fp = fopen(fname, "wb+")) == NULL) {
        perror("fopen failed\n");
    } 

#if 0
    for (;;) {
        // read buff from socket

        // scan buffer for \n.\n state and adjust length of data 

        // write buff to file

        // if EndOfBuf state detected, stop writing data, close the file and return
        if (eob_state == 3) break;
    }
#endif

    if (fclose(fp)!= 0) {
        perror("fclose failed\n");
    }

out: 
    return;
}

void dmtp_ehlo (parse_t *ps, char *host) {
    char argstr[MAXARGSTR];
    PS_CHECK_SYNTAX();

    sprintf(argstr, "EHLO host=|%s|", host);
    con_print(ps, OKSTR_250, argstr);
    return;
}

void dmtp_helo (parse_t *ps, char *host) {
    char argstr[MAXARGSTR];

    PS_CHECK_SYNTAX();

    sprintf(argstr, "HELO host=|%s|", host);
    con_print(ps, OKSTR_250, argstr);
    return;
}

void dmtp_help (parse_t *ps) {
    ssize_t recvSize;
    char buf[2];

    PS_CHECK_SYNTAX();
    con_print(ps, "214-DATA", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-DATA {fname}", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-EHLO HOST=<host.domain.tld>", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-HELO HOST=<host.domain.tld>", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-HELP", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-HIST USER=<mbox@domain.tld>", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-HIST USER=<mbox@domain.tld> START=[fingerprint]", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-HIST USER=<mbox@domain.tld> STOP=[fingerprint]", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-HIST USER=<mbox@domain.tld> START=[fingerprint] STOP=[fingerprint]", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-MAIL FROM=<domain.tld> FINGERPRINT=[fingerprint]", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-MODE", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-NOOP", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-NOOP {freeform}", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-QUIT", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-RCPT TO=<domain.tld> FINGERPRINT=[fingerprint]", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-RSET", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-SGNT DOMAIN=<domain.tld>", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-SGNT DOMAIN=<domain.tld> FINGERPRINT=[fingerprint]", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-SGNT USER=<mbox@domain.tld>", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-SGNT USER=<mbox@domain.tld> FINGERPRINT=[fingerprint]", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-STARTTLS HOST=<domain.tld>", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-STARTTLS HOST=<domain.tld> MODE=SMTP", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-STARTTLS HOST=<domain.tld> MODE=DMTP", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214-VRFY DOMAIN=<domain.tld> FINGERPRINT=[fingerprint]", NULL);
    recvSize = recv(ps->connfd, buf, 1, 0);
    con_print(ps, "214 VRFY USER=<mbox@domain.tld> FINGERPRINT=[fingerprint]", NULL);
    return;
}

void dmtp_hist (parse_t *ps, char *user, char *start, char *stop) {
    char argstr[MAXARGSTR];
    PS_CHECK_SYNTAX();
    sprintf(argstr, "HIST user=|%s| start=|%s| stop=|%s|", user, start, stop);
    con_print(ps, OKSTR_250, argstr);
    return;
}

void dmtp_mail (parse_t *ps, char *from, char *fingerprint) {
    char argstr[MAXARGSTR];
    PS_CHECK_SYNTAX();
    sprintf(argstr, "MAIL from=|%s| fingerprint=|%s|}", from, fingerprint);
    con_print(ps, OKSTR_250, argstr);
    return;
}

void dmtp_mode (parse_t *ps) {
    PS_CHECK_SYNTAX();
    con_print(ps, OKSTR_250, "MODE");
    return;
}

void dmtp_noop (parse_t *ps, char *freeform) {
    char argstr[MAXARGSTR];
    PS_CHECK_SYNTAX();
    sprintf(argstr, "NOOP |%s|", freeform);
    con_print(ps, OKSTR_250, argstr);
    return;
}

void dmtp_quit (parse_t *ps) {
    PS_CHECK_SYNTAX();
    con_print(ps, OKSTR_250, "QUIT");
    ps->quit = true;
    return;
}

void dmtp_rcpt (parse_t *ps, char *to, char *fingerprint) {
    char argstr[MAXARGSTR];
    PS_CHECK_SYNTAX();
    sprintf(argstr, "RCPT to=|%s| fingerprint=|%s|", to, fingerprint);
    con_print(ps, OKSTR_250, argstr);
    return;
}

void dmtp_rset (parse_t *ps) {
    PS_CHECK_SYNTAX();
    con_print(ps, OKSTR_250, "RSET");
    return;
}

void dmtp_sgnt_domain (parse_t *ps, char *domain, char *fingerprint) {
    char argstr[MAXARGSTR];
    PS_CHECK_SYNTAX();
    sprintf(argstr, "SGNT domain=|%s| fingerprint=|%s|", domain, fingerprint);
    con_print(ps, OKSTR_250, argstr);
    return;
}

void dmtp_sgnt_user (parse_t *ps, char *user,   char *fingerprint) {
    char argstr[MAXARGSTR];
    PS_CHECK_SYNTAX();
    sprintf(argstr, "SGNT user=|%s| fingerprint=|%s|", user, fingerprint);
    con_print(ps, OKSTR_250, argstr);
    return;
}

void dmtp_starttls (parse_t *ps, char *host, int mode) {
    char argstr[MAXARGSTR];
    PS_CHECK_SYNTAX();
    sprintf(argstr, "STARTTLS host=|%s| mode=%d", host, mode);
    con_print(ps, OKSTR_250, argstr);
    return;
}

void dmtp_vrfy_domain (parse_t *ps, char *domain, char *fingerprint) {
    char argstr[MAXARGSTR];
    PS_CHECK_SYNTAX();
    sprintf(argstr, "VRFY domain=|%s| fingerprint=|%s|", domain, fingerprint);
    con_print(ps, OKSTR_250, argstr);
    return;
}

void dmtp_vrfy_user (parse_t *ps, char *user, char *fingerprint) {
    char argstr[MAXARGSTR];
    PS_CHECK_SYNTAX();
    sprintf(argstr, "VRFY user=|%s| fingerprint=|%s|", user, fingerprint);
    con_print(ps, OKSTR_250, argstr);
    return;
}

void dmtp_invalid(parse_t *ps) {
    ps->rc = -1;            // force the syntax err condition in this special case
    PS_CHECK_SYNTAX();
    con_print(ps, "INVALID!!", NULL);
    return;
}
