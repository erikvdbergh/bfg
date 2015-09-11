CC        =   gcc
INCDIR    =   ./inc

# We dont need -Ox, I tried, it matters not.
#CFLAGS    =   -I$(INCDIR) -Wall -Werror 

# debug
CFLAGS    =   -I$(INCDIR) -Wall -ggdb

OBJDIR    =   bin
OBJ       =   $(addprefix $(OBJDIR)/, $(patsubst %.c, %.o, seqc/seqc.c sgrep/sgrep.c subseq/subseq.c))
DEPS      =   util.c

.PHONY: all clean

all: $(OBJDIR) $(OBJ)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o : %.c $(INCDIR)/$(DEPS);
	$(CC) $(CFLAGS) $< $(INCDIR)/$(DEPS) -o $(OBJDIR)/$(notdir $(basename $@))

clean:
	@rm -f $(TARGET) $(wildcard *.o)
	@rm -rf $(OBJDIR) 
