include ../../Makefile.config
CFLAGS=-g -shared -fPIC -Wall -I../include -I../../include
target_objects=sig_link.o link.o dob_link.o
all: $(target_objects)
.c.o:
	$(CC) $(CFLAGS) -c -o $(OBJS)/$@ $< -D_DEBUG
clean:
	rm -rf $(OBJS)/*.o
