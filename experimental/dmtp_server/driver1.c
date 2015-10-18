/*
 * Notes: Problems with testing results on parsed commands:
 * 
 * Command 2: after detecting a syntax error, lemon shouldn't be calling the elho routine
 * Command 5, after detecting a syntax error, lemon shouldn't be calling the dump_hist() routine.
 * Need a mechanism for not performing the function if a syntax error has been detected!
 */

#include <stdlib.h> 	// malloc
#include <stdio.h>		// printf
#include "cfg.h"
#include "dmtpd_cfg.h"

/* lemon functions */
void *ParseAlloc();
void Parse();
void ParseFree();

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

parse_t *ps;

int main() {
    ps = new_ps();
    void* pParser = ParseAlloc (malloc);

ps->line++;
Parse (pParser, HELO, 0, ps);
Parse (pParser, SPC, 0, ps);
Parse (pParser, EOL, 0, ps);
Parse (pParser, 0, NULL, ps);

ps->line++;
Parse (pParser, HELO, 0, ps);
Parse (pParser, SPC, 0, ps);
Parse (pParser, HOST, "helohost1", ps);
Parse (pParser, EOL, 0, ps);
Parse (pParser, 0, NULL, ps);

ps->line++;
Parse (pParser, EHLO, 0, ps);
Parse (pParser, HELO, 0, ps);                   // syntax
Parse (pParser, HOST, "helohost2", ps);
Parse (pParser, EOL, 0, ps);
Parse (pParser, 0, NULL, ps);

ps->line++;
Parse (pParser, EHLO, 0, ps);
Parse (pParser, HOST, "elhohost3", ps);
Parse (pParser, EOL, 0, ps);
Parse (pParser, 0, NULL, ps);

ps->line++;
Parse (pParser, HIST, 0, ps);
Parse (pParser, USER, "user@history.com", ps);
Parse (pParser, EOL, 0, ps);
Parse (pParser, 0, NULL, ps);

ps->line++;
Parse (pParser, HIST, 0, ps);
Parse (pParser, USER, "user@history.com", ps);
Parse (pParser, STOP, "stop.fp", ps);           // syntax
Parse (pParser, START, "start.fp", ps);
Parse (pParser, EOL, 0, ps);
Parse (pParser, 0, NULL, ps);

ps->line++;
Parse (pParser, HIST, 0, ps);
Parse (pParser, USER, "user@history.com", ps);
Parse (pParser, START, "start.fp", ps);
Parse (pParser, STOP, "stop.fp", ps);
Parse (pParser, EOL, 0, ps);
Parse (pParser, 0, NULL, ps);

ps->line++;
Parse (pParser, DATA, 0, ps);
Parse (pParser, EOL, 0, ps);
Parse (pParser, 0, NULL, ps);

ps->line++;
Parse (pParser, DATA, 0, ps);
Parse (pParser, USER, "datateshere!", ps);      // syntax
Parse (pParser, EOL, 0, ps);
Parse (pParser, 0, NULL, ps);


ParseFree(pParser, free );
}
