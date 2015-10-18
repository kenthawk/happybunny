/* Refs: https://en.wikipedia.org/wiki/Flex_lexical_analyser  */
%{
#include <sys/socket.h>
#include "cfg.h"
#include "dmtpd_cfg.h"

/* Current socket to process */
//extern int Connfd;
extern parse_t *ps;

/* Stop scanning on YY_NULL */
/* From: https://forums.freebsd.org/threads/lex-scanner-on-a-socket.36148/ */
#define YY_INPUT(buf,result,max_size) \
  { \
    ssize_t recvSize = recv(ps->connfd, buf, max_size, 0); \
    result = recvSize > 0 ? recvSize : YY_NULL; \
    if (recvSize == 0 || (recvSize == -1 && errno != EAGAIN)) { \
    /* Close socket on error */ \
    } \
  }
%}

%option  noyywrap

%x HOST_QUOTED
%x USER_QUOTED
%x FROM_QUOTED
%x TO_QUOTED
%x DOMAIN_QUOTED
%x START_QUOTED
%x STOP_QUOTED
%x FINGER_QUOTED
%x FREEFORM_QUOTED


%%

DATA                    { return DATA;      }
HELP                    { return HELP;      }
MODE                    { return MODE;      }
QUIT                    { return QUIT;      }
RSET                    { return RSET;      }
EHLO                    { return EHLO;      }
HELO                    { return HELO;      }
HIST                    { return HIST;      }
MAIL                    { return MAIL;      }
NOOP                    { return NOOP;      }
RCPT                    { return RCPT;      }
SGNT                    { return SGNT;      }
STARTTLS                { return STARTTLS;  }
VRFY                    { return VRFY;      }


"HOST=<"                { BEGIN(HOST_QUOTED);                   }
<HOST_QUOTED>[^>\n]*    { return HOST;                          }
<HOST_QUOTED>"\n"       { BEGIN(INITIAL); return EOL;           }
<HOST_QUOTED>">"        { BEGIN(INITIAL); return RNG;           }

"USER=<"                { BEGIN(USER_QUOTED);                   }
<USER_QUOTED>[^>\n]*    { return USER;                          }
<USER_QUOTED>"\n"       { BEGIN(INITIAL); return EOL;           }
<USER_QUOTED>">"        { BEGIN(INITIAL); return RNG;           }

"FROM=<"                { BEGIN(FROM_QUOTED);                   }
<FROM_QUOTED>[^>\n]*      { return FROM;                        }
<FROM_QUOTED>"\n"       { BEGIN(INITIAL); return EOL;           }
<FROM_QUOTED>">"        { BEGIN(INITIAL); return RNG;           }

"TO=<"                  { BEGIN(TO_QUOTED);                     }
<TO_QUOTED>[^>\n]*        { return TO;                          }
<TO_QUOTED>"\n"         { BEGIN(INITIAL); return EOL;           }
<TO_QUOTED>">"          { BEGIN(INITIAL); return RNG;           }

"DOMAIN=<"              { BEGIN(DOMAIN_QUOTED);                 }
<DOMAIN_QUOTED>[^>\n]*    { return DOMAIN;                      }
<DOMAIN_QUOTED>"\n"     { BEGIN(INITIAL); return EOL;           }
<DOMAIN_QUOTED>">"      { BEGIN(INITIAL); return RNG;           }

"START=["               { BEGIN(START_QUOTED);                  }
<START_QUOTED>[^\]\n]*    { return START;                       }
<START_QUOTED>"\n"      { BEGIN(INITIAL); return EOL;           }
<START_QUOTED>"]"       { BEGIN(INITIAL); return RBK;           }

"STOP=["                { BEGIN(STOP_QUOTED);                   }
<STOP_QUOTED>[^\]\n]*     { return STOP;                        }
<STOP_QUOTED>"\n"       { BEGIN(INITIAL); return EOL;           }
<STOP_QUOTED>"]"        { BEGIN(INITIAL); return RBK;           }

"FINGERPRINT=["         { BEGIN(FINGER_QUOTED);                 }
<FINGER_QUOTED>[^\]\n]*   { return FINGERPRINT;                 }
<FINGER_QUOTED>"\n"     { BEGIN(INITIAL); return EOL;           }
<FINGER_QUOTED>"]"      { BEGIN(INITIAL); return RBK;           }

"{"                     { BEGIN(FREEFORM_QUOTED);               }
<FREEFORM_QUOTED>[^}\n]* { return FREEFORM;                     }
<FREEFORM_QUOTED>"\n"   { BEGIN(INITIAL); return EOL;           }
<FREEFORM_QUOTED>"}"    { BEGIN(INITIAL); return RSQ;           }

MODE=SMTP               { return MODESMTP;  }
MODE=DMTP               { return MODEDMTP;  }

'>'                     { return RNG;       }
']'                     { return RBK;       }
'\}'                     { return RSQ;       }
[ \t\r]+                { return SPC;       }
[\n]                    { return EOL;       }
.                       { return INVALID;   }

%%


