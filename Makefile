CC = gcc
INCDIR = ./inc
CFLAGS = -I$(INCDIR) -Wall -ggdb
DEPS = $(INCDIR)/*

all: bin/seqc bin/sgrep bin/subseq

bin/seqc: seqc/seqc.c $(DEPS)
	$(CC) $(CFLAGS) $(DEPS) seqc/seqc.c -o bin/seqc

bin/sgrep: sgrep/sgrep.c $(DEPS)
	$(CC) $(CFLAGS) $(DEPS) sgrep/sgrep.c -o bin/sgrep

bin/subseq: subseq/subseq.c $(DEPS)
	$(CC) $(CFLAGS) $(DEPS) subseq/subseq.c -o bin/subseq

.PHONY:clean
clean:
	@rm -f bin/*
