.SECONDARY:
.DELETE_ON_ERROR:

TRAINFILE=input/kingjames_5.txt
TESTSELF=input/en-unseen.txt

langs=en2 me hi la pd ta xh

all : plots/example-latin.pdf


# unzip
input/kingjames_5.txt : input/kingjames_5.txt.gz
	gunzip -k $< 

input/kingjames_5b.txt : input/kingjames_5b.txt.gz
	gunzip -k $< 


# example plot with latin vs english
plots/example-latin.pdf : code/plot-lang-vary-t-n.R data/lang-la-t-n.rds | plots
	Rscript $^  $@
	
#data/lang-t-n.rds : code/lang-vary-t-n.R | data
#	Rscript $< $(TRAINFILE) $(TESTSELF) input/la-unseen.txt la $@



# deltaAUC for different languages
all-langs : $(foreach l, $(langs), data/lang-$(l)-t-n.rds )
	touch $@
	
data/lang-%-t-n.rds : code/lang-vary-t-n.R $(TRAINFILE) | data
	Rscript $< $(TRAINFILE) $(TESTSELF) input/$*-unseen.txt $* $@

plots/lang-delta.pdf : code/plot-all-delta.R all-langs | plots
	Rscript $< data/lang-XX-t-n.rds '$(langs)' $@



plots/all-lang-n.pdf : code/plot-all-varyn.R all-langs | plots
	Rscript $< data/lang-XX-t-n.rds '$(langs)' $@

plots/all-lang-t-n.pdf : code/plot-all-vary-t-n.R all-langs | plots
	Rscript $< data/lang-XX-t-n.rds '$(langs)' $@


# runtimes
plots/t-times.pdf : code/plot-times.R data/t-times.rds | plots
	Rscript $^ $@

data/t-times.rds : code/time-t.R | data
	Rscript $< $(TRAINFILE) $(TESTSELF) input/la-unseen.txt $@



data : 
	mkdir -p $@
	
plots : 
	mkdir -p $@
