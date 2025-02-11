CC 	= gcc
LD	= gcc
LEX	= flex

OPT	= 
DEBUG	= -g -O1 -Wall

CFLAGS	= $(OPT) $(DEBUG) $(INCLUDES) $(DEFINES)
INCLUDES=
DEFINES =
LDFLAGS	= -ll

LEXFLAGS= -I
HOSTNAME=`hostname`
OBJS= ish_main.o scanner.o ish_sig.o ish_environ.o ish_job.o \
      ish_command.o ish_alias.o

ish: $(OBJS)
	$(CC) $(OPT) $(OBJS) -o $@ $(LDFLAGS) 

clean: 
	rm -f $(OBJS) lex.yy.c scanner.c ish

scanner.o: scanner.c scanner.h

scanner.c: scanner.lex
	$(LEX) $(LEXFLAGS) scanner.lex
	mv lex.yy.c scanner.c

test:
	shelltest -DSHELL=./ish -DPROMPT="${HOSTNAME}%" -DHOME="${HOME}" .
