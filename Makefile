CC = gcc
INCDIR = ./inc
CFLAGS = -I$(INCDIR) -Wall -ggdb
DEPS = $(INCDIR)/*

all: bin/seqc bin/sgrep bin/subseq

bin/seqc: src/seqc.c $(DEPS)
	$(CC) $(CFLAGS) $(DEPS) src/seqc.c -o bin/seqc

bin/sgrep: src/sgrep.c $(DEPS)
	$(CC) $(CFLAGS) $(DEPS) src/sgrep.c -o bin/sgrep

bin/subseq: src/subseq.c $(DEPS)
	$(CC) $(CFLAGS) $(DEPS) src/subseq.c -o bin/subseq

.PHONY:clean
clean:
	@rm -f bin/*
