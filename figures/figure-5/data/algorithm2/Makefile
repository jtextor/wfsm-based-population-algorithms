
include Makefile.cfg
include Makefile.inc

algo2dir := ../../../../wfsms
algo2 := $(algo2dir)/targets/algorithm2

indexfile := index.csv

all: $(logfiles)

$(indexfile): Makefile.cfg | $(algo2)
	rm -f $(indexfile) $(logfileprefix)-*.csv
	touch $(indexfile)

$(logfiles) : $(logfileprefix)-%.csv : $(indexfile)
	echo $(call num2problem,$*),$(call num2radius,$*),$@ >> $(indexfile)
	$(algo2) $(call num2problem,$*) $(timeout) $(call num2radius,$*) $(prunefract) $@

$(algo2):
	$(MAKE) -C $(algo2dir) $(algo2)

clean:
	rm -f $(indexfile) $(logfileprefix)-*.csv
