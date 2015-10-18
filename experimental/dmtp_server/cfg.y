// cfg.y - lemon conf file for dmtpd grammer

%include {
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "dmtpd_cfg.h"
#include "dmtpd_routines.h"
extern con_print();
}

%token_type {char*}
%extra_argument {parse_t *ps}
%syntax_error  {ps->rc=-1;}
//%parse_failure {printf("%s {parse: %s}\n", ERRORSTR_500, ps->lexStr); ps->rc=-1;}
%parse_failure {ps->rc=-1; con_print(ps, ERRORSTR_500,  ps->lexStr);}

start ::= in .
in ::= .
in ::= in cmd .


cmd ::= DATA EOL.                                           { dmtp_data(ps, NULL); }
// NOTE: the following is not to spec, used for testing purposes. KDH
cmd ::= DATA SPC FREEFORM(A) RSQ EOL.                       { dmtp_data(ps, A); }
//cmd ::= DATA EOL.                                           { ps_ctx->dmtp_data(err_ctx, ps); }

cmd ::= HELO SPC HOST(A) RNG EOL.                           { dmtp_helo(ps, A); }
cmd ::= EHLO SPC HOST(A) RNG EOL.                           { dmtp_ehlo(ps, A); }

cmd ::= HELP EOL.                                           { dmtp_help(ps); }

cmd ::= HIST SPC USER(A) RNG EOL.                           { dmtp_hist(ps, A, NULL, NULL); }
cmd ::= HIST SPC USER(A) RNG SPC START(B) RBK EOL.          { dmtp_hist(ps, A, B,    NULL); }
cmd ::= HIST SPC USER(A) RNG SPC STOP(B) RBK EOL.           { dmtp_hist(ps, A, NULL, B); }
cmd ::= HIST SPC USER(A) RNG SPC START(B) RBK SPC STOP(C) RBK EOL.  { dmtp_hist(ps, A, B, C); }

cmd ::= MAIL SPC FROM(A) RNG SPC FINGERPRINT(B) RBK EOL.    { dmtp_mail(ps, A, B); }

cmd ::= MODE EOL.                                           { dmtp_mode(ps); }
cmd ::= NOOP EOL.                                           { dmtp_noop(ps, NULL); }
cmd ::= NOOP SPC FREEFORM(A) RSQ EOL.                       { dmtp_noop(ps, A); }
cmd ::= QUIT EOL.                                           { dmtp_quit(ps); }

cmd ::= RCPT SPC TO(A) RNG SPC FINGERPRINT(B) RBK EOL.      { dmtp_rcpt(ps, A, B); }

cmd ::= RSET EOL.                                           { dmtp_rset(ps); }

cmd ::= SGNT SPC DOMAIN(A) RNG EOL.                         { dmtp_sgnt_domain(ps, A, NULL); }
cmd ::= SGNT SPC DOMAIN(A) RNG SPC FINGERPRINT(B) RBK EOL.  { dmtp_sgnt_domain(ps, A, B); }
cmd ::= SGNT SPC USER(A) RNG EOL.                           { dmtp_sgnt_user(ps, A, NULL); }
cmd ::= SGNT SPC USER(A) RNG SPC FINGERPRINT(B) RBK EOL.    { dmtp_sgnt_user(ps, A, B); }

cmd ::= STARTTLS SPC HOST(A) RNG EOL.                       { dmtp_starttls(ps, A, 0); }
cmd ::= STARTTLS SPC HOST(A) RNG SPC MODESMTP EOL.          { dmtp_starttls(ps, A, 1); }
cmd ::= STARTTLS SPC HOST(A) RNG SPC MODEDMTP EOL.          { dmtp_starttls(ps, A, 2); }

cmd ::= VRFY SPC DOMAIN(A) RNG SPC FINGERPRINT(B) RBK EOL.  { dmtp_sgnt_domain(ps, A, B); }
cmd ::= VRFY SPC USER(A) RNG SPC FINGERPRINT(B) RBK EOL.    { dmtp_sgnt_user(ps, A, B); }

cmd ::= EOL.                                                { dmtp_invalid(ps); }
cmd ::= INVALID EOL.                                        { dmtp_invalid(ps); }

