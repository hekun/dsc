include ../Makefile.config
CFLAGS=-g -shared -fPIC -Wall -I../include
target_objects=v_data.o
all: $(target_objects)
.c.o:
	$(CC) $(CFLAGS) -c -o $(OBJS)/$@ $< -D_DEBUG
clean:
	rm -rf $(OBJS)/*.o

