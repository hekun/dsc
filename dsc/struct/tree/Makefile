include ../../Makefile.config
CFLAGS=-g -shared -fPIC -Wall -I../include -I../../include
target_objects=binary_tree.o
all: $(target_objects)
.c.o:
	$(CC) $(CFLAGS) -c -o $(OBJS)/$@ $< -D_DEBUG
clean:
	rm -rf $(OBJS)/*.o


