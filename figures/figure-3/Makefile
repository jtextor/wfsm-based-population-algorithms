.SECONDARY:
.DELETE_ON_ERROR:

all : plots/bitstring-mu-ntrain.pdf plots/bitstring-t-n.pdf


# screening some hyperparameters
plots/bitstring-mu-ntrain.pdf : code/plot-analysis-mu-n.R data/analysis-mu-n.rds
	Rscript $^ $@


data/analysis-mu-n.rds : code/bitstring-vary-mu-n.R
	Rscript $< $@


plots/bitstring-t-n.pdf : code/plot-analysis-t-n.R data/analysis-t-n.rds
	Rscript $^ $@

data/analysis-t-n.rds : code/bitstring-vary-t-n.R
	Rscript $< $@
