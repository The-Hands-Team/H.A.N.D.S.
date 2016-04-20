all:
	if [ -e makefiles/temp.mk ] ; \
	then rm makefiles/temp.mk ; \
	fi;
	make -f makefiles/Makefile




### Cleanup

clean:
	rm -rf Run $(TEMPFILE) lib/_output/*
    
.PHONY: clean

.SILENT: all
