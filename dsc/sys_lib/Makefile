include ../Makefile.config
CFLAGS=-g -shared -fPIC -Wall -I../include
static_target =$(LIBS)/libdbs_sys.a
dynamic_target =$(LIBS)/libdbs_sys.so
all: $(static_target) $(dynamic_target)
target_objects=log.o wap_mem.o
$(static_target): ${target_objects}
	$(AR) -rv $@ $^
	$(RANLIB) $@
	@echo $@ Build OK.
$(dynamic_target):${target_objects}
	$(CC) $(LDFLAGS) -I../include -o $@ $^ 
.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
clean:
	rm -rf $(static_target) $(target_objects) $(dynamic_target)

