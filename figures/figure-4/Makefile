.SECONDARY:
.DELETE_ON_ERROR:

TRAINFILE=input/kingjames_5.txt
TESTSELF=input/en-unseen.txt

langs=en2 me hi la pd ta xh

all : plots/fig4.pdf
#all : plots/example-latin.pdf plots/all-lang-n.pdf

# unzip
input/kingjames_5.txt : input/kingjames_5.txt.gz
	gunzip -k $< 


plots/fig4.pdf : code/plot.R all-langs
	Rscript $< data/lang-XX-t-n.rds 'en2 me hi la pd ta xh' data/lang-la-t-n.rds $@

# deltaAUC for different languages
all-langs : $(foreach l, $(langs), data/lang-$(l)-t-n.rds )
	touch $@
	
data/lang-%-t-n.rds : code/lang-vary-t-n.R $(TRAINFILE) | data
	Rscript $< $(TRAINFILE) $(TESTSELF) input/$*-unseen.txt $* $@

data plots : 
	mkdir -p $@
