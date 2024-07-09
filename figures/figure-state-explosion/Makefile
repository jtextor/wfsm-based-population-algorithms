.SECONDARY:
.DELETE_ON_ERROR:

AUTOMATA_TYPES := boolfa logfa ratfa
DATA_FILES := $(addprefix data/,$(addsuffix .csv.gz,$(AUTOMATA_TYPES)))

all : plots/states3.pdf

plots/states3.pdf : code/plot.R $(DATA_FILES)
	Rscript $< $@

mytheme.R : ../figure-lang-distinction/mytheme.R
	ln -s $< 

$(DATA_FILES): | ../../wfsms/targets
	../../wfsms/targets/$(subst .csv.gz,,$(@F))_compress 6 | gzip > $@

../../wfsms/targets:
	make -C ../../wfsms
