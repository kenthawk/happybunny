/**
 * @file /brimstone/dmtp/server/dmtpd.h
 *
 * @brief The entry point for the DMTP server module.
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */

#ifndef DMTPD_H
#define DMTPD_H

// dmtpd.c
// function decls
server_t     * server_new(uint32_t port);
connection_t * con_new(server_t * server, int sockd);     // ref protocol_process
bool_t         net_init(server_t * server);

/// commands.c
//void 		dmtp_requeue(connection_t *con);
int_t   dmtp_compare(const void *compare, const void *command);
int_t    dmtp_process(connection_t *con);
void    dmtp_sort(void);

/// dmtp_routines.c
void		dmtp_ehlo(connection_t *con);
void		dmtp_helo(connection_t *con);
void		dmtp_noop(connection_t *con);
void		dmtp_mode(connection_t *con);
void		dmtp_rset(connection_t *con);
void		dmtp_quit(connection_t *con);
void		dmtp_mail(connection_t *con);
void		dmtp_rcpt(connection_t *con);
void		dmtp_data(connection_t *con);
void		dmtp_sgnt(connection_t *con);
void		dmtp_hist(connection_t *con);
void		dmtp_vrfy(connection_t *con);
void		dmtp_help(connection_t *con);
void		dmtp_verb(connection_t *con);

void 		dmtp_init(connection_t *con);
void		dmtp_invalid(connection_t *con);

/// session.c
void    dmtp_session_reset(connection_t *con);
void    dmtp_session_destroy(connection_t *con);

#endif
