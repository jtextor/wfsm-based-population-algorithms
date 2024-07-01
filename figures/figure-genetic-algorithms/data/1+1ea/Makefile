
include Makefile.cfg
include Makefile.inc

baselinedir := ../../../../wfsms
baseline := $(baselinedir)/targets/1+1ea

indexfile := index.csv

all: $(logfiles)
	

$(indexfile): Makefile.cfg | $(baseline)
	rm -f $(indexfile) $(logfileprefix)-*.csv
	touch $(indexfile)

$(logfiles) : $(logfileprefix)-%.csv : $(indexfile)
	echo $(call num2problem,$*),$(call num2mutprob,$*),$@ >> $(indexfile)
	$(baseline) $(call num2problem,$*) $(call num2mutprob,$*) $(timeout) $@ 1000

$(baseline):
	$(MAKE) -C $(baselinedir) $(baseline)

clean:
	rm -f $(indexfile) $(logfileprefix)-*.csv

.PHONY: $(indexfile)
