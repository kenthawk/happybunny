/*
 * @file test_cmdparser.c
 *
 *
 * @Author
 * Kent
 */

#include <stdlib.h>     // malloc
#include <stdio.h>      // printf
#include <string.h>
#include "cfg.h"
#include "dmtpd_cfg.h"
#include "lex.h"

/* lemon functions */
void *ParseAlloc();
void Parse();
void ParseFree();

// ps->connfd is required by lex when parsing input from a socket.
// This testcode doesn't use the socket, but the global context
// that contains the socket needs to be declared for the linker's
// benefit.
parse_t *ps;

parse_t *
new_ps () {
    parse_t *ps;
    if ((ps = malloc(sizeof (parse_t))) == NULL) {
        goto error;
    }
    ps->line = 0;
    ps->rc = 0;
    return ps;
error:
    return NULL;
}

int main() {
    parse_t *ps;
    void * pParser;
    int lexCode = 0;
    char * linebuf;
    size_t linecap = MAXCMDLEN;

    linebuf = malloc(MAXCMDLEN + 2); // +2 = '\n\0';

    ps = new_ps();
    pParser = ParseAlloc (malloc);

    // used for printing debug for lexStr
    char intStr[128];
    intStr[0] = '\0';

    while ((getline(&linebuf, &linecap, stdin)) > 0) {
        yy_scan_string (linebuf);

        for (lexCode = yylex(); lexCode > 0; lexCode = yylex()) {
            Parse(pParser, lexCode, strdup(yyget_text()), ps);

            if (lexCode != EOL) {
                sprintf (intStr, "|%d(", lexCode);
                strcat (ps->lexStr, intStr);
                strcat (ps->lexStr, yyget_text());
                strcat (ps->lexStr, ")");
            }
            else {
                strcat (ps->lexStr, "|EOL|"); 
                Parse (pParser, 0, NULL, ps);
                ps->rc = 0;
                ps->line++;
                ps->lexStr[0] = '\0';
            }
        }
    }

    ParseFree(pParser, free );
}
