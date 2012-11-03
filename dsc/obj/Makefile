include ../Makefile.config
CFLAGS=-g -shared -fPIC -Wall -I../include
static_target =$(LIBS)/libdsc.a
dynamic_target =$(LIBS)/libdsc.so
all: $(static_target) $(dynamic_target)
target_objects=link.o sig_link.o v_data.o dob_link.o cic_link.o
$(static_target): ${target_objects}
	$(AR) -rv $@ $^
	$(RANLIB) $@
	@echo $@ Build OK.
$(dynamic_target):${target_objects}
	$(CC) $(LDFLAGS) -I../include -I../../include -o $@ $^ 
clean:
	rm -rf $(static_target) $(dynamic_target)