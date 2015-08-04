CC        =   gcc
INCDIR    =   ./inc
#CFLAGS    =   -I$(INCDIR) -O3 -Wall -Werror -g
CFLAGS    =   -I$(INCDIR) -Wall -ggdb
OBJDIR    =   bin
OBJ       =   $(addprefix $(OBJDIR)/, $(patsubst %.c, %.o, seqc/seqc.c sgrep/sgrep.c subseq/subseq.c))
DEPS      =   util.c
#OBJ       =   $(addprefix $(OBJDIR)/, seqc sgrep subseq)

.PHONY: all clean

all: $(OBJDIR) $(OBJ)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o : %.c $(INCDIR)/$(DEPS);
	$(CC) $(CFLAGS) $< $(INCDIR)/$(DEPS) -o $(OBJDIR)/$(notdir $(basename $@))

clean:
	@rm -f $(TARGET) $(wildcard *.o)
	@rm -rf $(OBJDIR) 
