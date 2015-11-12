CC = gcc

CFLAGS = -Wall -Werror -ggdb #debug
TESTFLAGS = -Wall -ggdb -I/usr/local/include

DEPS = src/util.c

all: bin/seqc bin/sgrep bin/subseq bin/revcomp 

tests: test/revcomp test/sgrep test/subseq test/seqc

bin/%: src/%.c src/%_func.c $(DEPS)
	$(CC) $(CFLAGS) $^ -o $@

test/revcomp: src/tests/test_revcomp.c src/revcomp_func.c $(DEPS)
	$(CC) $(TESTFLAGS) $^ -lcunit -o $@
test/sgrep: src/tests/test_sgrep.c src/revcomp_func.c $(DEPS)
	$(CC) $(TESTFLAGS) $^ -lcunit -o $@
test/subseq: src/tests/test_subseq.c src/revcomp_func.c $(DEPS)
	$(CC) $(TESTFLAGS) $^ -lcunit -o $@
test/seqc: src/tests/test_seqc.c src/revcomp_func.c $(DEPS)
	$(CC) $(TESTFLAGS) $^ -lcunit -o $@

.PHONY:clean
clean:
	@rm -f bin/*
