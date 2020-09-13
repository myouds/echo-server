OBJS = echo.o listener.o worker.o
SRCDIR = ../src
CFLAGS = -Wall -I../include

.PHONY:	clean

all: $(OBJS)

%.o:	$(SRCDIR)/%.c

clean:
	rm -f *.o