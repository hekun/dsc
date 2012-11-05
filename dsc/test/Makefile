include ../Makefile.config
CFLAGS=-g -Wall -I../include -D_DEBUG_
STATIC_LIB_FLAG=$(LIBS)/libdsc_sys.a $(LIBS)/libdsc.a
DYNAMIC_LIB_FLAGS=-L$(LIBS) -ldsc_sys -ldsc
target = $(BIN)/test_link $(BIN)/test_stack
all: $(target)
$(BIN)/test_link: test_link.o
	$(CC) $(CFLAGS) -o $@ $^ $(DYNAMIC_LIB_FLAGS) -D_DEBUG_
$(BIN)/test_stack: test_stack.o
	$(CC) $(CFLAGS) -o $@ $^ $(DYNAMIC_LIB_FLAGS) -D_DEBUG_
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	rm -rf $(target) *.o

