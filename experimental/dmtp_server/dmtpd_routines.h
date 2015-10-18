// dmtpd_routines.h
#ifndef DMTPD_ROUTINES_H
#define DMTPD_ROUTINES_H

#include "dmtpd_cfg.h"

// Canned client replies
#define OKSTR_250    "250 OK"
#define ERRORSTR_500 "500 COMMAND SYNTAX ERROR"
#define ERRORSTR_501 "501 ARGUMENT SYNTAX ERROR"
#define ERRORSTR_502 "502 COMMAND DISABLED"
#define ERRORSTR_503 "503 INVALID COMMAND SEQUENCE"

/// dmtp_routines.c
//void dmtp_data(parse_t *ps);
void dmtp_data(parse_t *ps, char *fname);       // test
void dmtp_ehlo(parse_t *ps, char *host);
void dmtp_helo(parse_t *ps, char *host);
void dmtp_help(parse_t *ps);
void dmtp_hist(parse_t *ps, char *user, char *start, char *stop);
void dmtp_mail(parse_t *ps, char *from, char *fingerprint);
void dmtp_mode(parse_t *ps);
void dmtp_noop(parse_t *ps, char *freeform);
void dmtp_quit(parse_t *ps);
void dmtp_rcpt(parse_t *ps, char *to,   char *fingerprint);
void dmtp_rset(parse_t *ps);
void dmtp_sgnt_domain(parse_t *ps, char *domain, char *fingerprint);
void dmtp_sgnt_user(parse_t *ps, char *user,   char *fingerprint);
void dmtp_starttls(parse_t *ps, char *host, int mode);
void dmtp_vrfy_domain(parse_t *ps, char *domain, char *fingerprint);
void dmtp_vrfy_user(parse_t *ps, char *user, char *fingerprint);

void dmtp_invalid(parse_t *ps);

#endif
