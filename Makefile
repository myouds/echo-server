SRCDIR = src
BIN    = echo

.PHONY:	clean $(SRCDIR)

all:	$(BIN)

$(BIN):	$(SRCDIR)
	$(CC) -o $@ $(SRCDIR)/*.o

$(SRCDIR):
	$(MAKE) -C $(SRCDIR)

clean:
	rm -f $(BIN)
	$(MAKE) -C $(SRCDIR) clean