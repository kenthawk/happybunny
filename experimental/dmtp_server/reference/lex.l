/* Refs: https://en.wikipedia.org/wiki/Flex_lexical_analyser  */
%{
#include "cfg.h"
%}

%x HOST_QUOTED
%x USER_QUOTED
%x FROM_QUOTED
%x TO_QUOTED
%x DOMAIN_QUOTED
%x START_QUOTED
%x STOP_QUOTED
%x FINGER_QUOTED

%x BRACK_QUOTED

digit         [0-9]
letter        [a-zA-Z]

%%

"DATA"                  { return DATA;          }
"EHLO"                  { return EHLO;          }
"HELO"                  { return HELO;          }
"HELP"                  { return HELP;          }
"HIST"                  { return HIST;          }
"MAIL"                  { return MAIL;          }
"MODE"                  { return MODE;          }
"NOOP"                  { return NOOP;          }
"QUIT"                  { return QUIT;          }
"RCPT"                  { return RCPT;          }
"RSET"                  { return RSET;          }
"SGNT"                  { return SGNT;          }
"STARTTLS"              { return STARTTLS;      }
"VRFY"                  { return VRFY;          }


"HOST=<"                { BEGIN(HOST_QUOTED);  }
<HOST_QUOTED>[^>]+      { }
<HOST_QUOTED>">"        { BEGIN(INITIAL); return HOST; }

"USER=<"                { BEGIN(USER_QUOTED);  }
<USER_QUOTED>[^>]+      { }
<USER_QUOTED>">"        { BEGIN(INITIAL); return USER; }

"FROM=<"                { BEGIN(FROM_QUOTED);  }
<FROM_QUOTED>[^>]+      { }
<FROM_QUOTED>">"        { BEGIN(INITIAL); return FROM; }

"TO=<"                  { BEGIN(TO_QUOTED);  }
<TO_QUOTED>[^>]+        { }
<TO_QUOTED>">"          { BEGIN(INITIAL); return TO; }

"DOMAIN=<"              { BEGIN(DOMAIN_QUOTED);  }
<DOMAIN_QUOTED>[^>]+    { }
<DOMAIN_QUOTED>">"      { BEGIN(INITIAL); return DOMAIN; }

"START=["               { BEGIN(START_QUOTED);  }
<START_QUOTED>[^\]]+    { }
<START_QUOTED>"]"       { BEGIN(INITIAL); return START; }

"STOP=["                { BEGIN(STOP_QUOTED);  }
<STOP_QUOTED>[^\]]+     { }
<STOP_QUOTED>"]"        { BEGIN(INITIAL); return STOP; }

"FINGERPRINT=["         { BEGIN(FINGER_QUOTED);  }
<FINGER_QUOTED>[^\]]+   { }
<FINGER_QUOTED>"]"      { BEGIN(INITIAL); return FINGERPRINT; }

"MODE=SMTP"             { return MODESMTP;          }
"MODE=DMTP"             { return MODEDMTP;          }

{letter}({letter}|{digit})* {
                       yylval.id = strdup(yytext);
                       return IDENT;      }
{digit}+             { yylval.num = atoi(yytext);
                       return NUMBER;     }
[ \t\r]            /* skip whitespace */
[\n]                { return EOL;           }
.                    { return -1; }

%%

int yywrap(void){return 1;}
