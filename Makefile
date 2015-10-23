CC = gcc
INCDIR = ./inc

CFLAGS = -I$(INCDIR) -Wall -Werror -ggdb #debug
#CFLAGS = -I$(INCDIR) -Wall -Werror

DEPS = $(INCDIR)/*

all: bin/seqc bin/sgrep bin/subseq bin/revcomp

bin/%: src/%.c $(DEPS)
	$(CC) $(CFLAGS) $(DEPS) $< -o $@

.PHONY:clean
clean:
	@rm -f bin/*
