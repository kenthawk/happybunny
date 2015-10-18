# Kent Hawk
# <kent.hawk@gmail.com>
# 2015
#
CFLAGS = -Wall
CFLAGS = -Wall -g
LEX=/usr/bin/flex
LEMON=./lemon
CC=/usr/bin/gcc
RM=/bin/rm

all: lemon dmtpd stub_client test_cmdparser driver1

lemon: lemon.c

stub_client: stub_client.c

dmtpd: cfg.o lex.o dmtpd_routines.o dmtpd.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

test_cmdparser: cfg.o lex.o dmtpd_routines.o test_cmdparser.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

driver1: cfg.o lex.o dmtpd_routines.o driver1.o
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $^

cfg.o: cfg.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $^

cfg.c: cfg.y
	$(LEMON) $^

lex.o: lex.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $^

lex.c: lex.l
	$(LEX) $(LFLAGS) --outfile=$@ --header-file=lex.h $^

clean:
	$(RM) -rf *dSYM
	$(RM) lemon dmtpd stub_client test_cmdparser driver1 *.o lex.c lex.h cfg.c cfg.h cfg.out

