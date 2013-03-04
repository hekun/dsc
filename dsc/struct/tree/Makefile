include ../../Makefile.config
CFLAGS=-g -shared -fPIC -Wall -I../include -I../../include
#target_objects=bitree.o bitree_threaded.o
target_objects=bitree.o bitree_unrec.o

all: $(target_objects)
.c.o:
	$(CC) $(CFLAGS) -c -o $(OBJS)/$@ $< $(DEBUG)
clean:
	rm -rf $(OBJS)/*.o

