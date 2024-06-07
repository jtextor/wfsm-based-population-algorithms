.SECONDARY:
.DELETE_ON_ERROR:

all : plots/states2.pdf

plots/states.pdf : code/plot-states.R
	Rscript $<

mytheme.R : ../figure-lang-distinction/mytheme.R
	ln -s $< 

plots/states2.pdf : code/plot-states-2.R | mytheme.R
	Rscript $< $@

