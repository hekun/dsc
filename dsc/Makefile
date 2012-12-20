DIRS=sys_lib virtual struct obj test
all:
	@for i in $(DIRS); do \
		echo Making all in $${i}; \
		cd $${i}; \
		make; \
		cd ..; \
	done

clean:
	@for i in $(DIRS); do \
		echo Cleaning $${i}; \
		cd $${i}; \
		make -s clean; \
		cd ..; \
	done


