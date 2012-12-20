include ../Makefile.config
CFLAGS=-g -Wall -I../include $(DEBUG)
STATIC_LIB_FLAG=$(LIBS)/libdsc_sys.a $(LIBS)/libdsc.a
DYNAMIC_LIB_FLAGS=-L$(LIBS) -ldsc_sys -ldsc
target = $(BIN)/test_link $(BIN)/test_stack $(BIN)/test_queue $(BIN)/test_tree
all: $(target)
$(BIN)/test_link: test_link.o
	$(CC) $(CFLAGS) -o $@ $^ $(DYNAMIC_LIB_FLAGS)
$(BIN)/test_stack: test_stack.o
	$(CC) $(CFLAGS) -o $@ $^ $(DYNAMIC_LIB_FLAGS)
$(BIN)/test_queue: test_queue.o
	$(CC) $(CFLAGS) -o $@ $^ $(DYNAMIC_LIB_FLAGS)
$(BIN)/test_tree: test_tree.o
	$(CC) $(CFLAGS) -o $@ $^ $(DYNAMIC_LIB_FLAGS)
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	rm -rf $(target) *.o

