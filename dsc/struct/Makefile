DIRS=link stack queue tree
all:
	@for i in $(DIRS); do \
		echo Making all in $${i}; \
		cd $${i}; \
		make -s; \
		cd ..; \
	done

clean:
	@for i in $(DIRS); do \
		echo Cleaning $${i}; \
		cd $${i}; \
		make -s clean; \
		cd ..; \
	done



