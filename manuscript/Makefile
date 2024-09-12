SHELL := /bin/sh
.SUFFIXES:

name := automata-as-populations

figures := $(addprefix ../figures/,\
	figure-2/plots/states3.pdf \
	figure-3/plots/bitstring-mu-ntrain.pdf \
	figure-3/plots/bitstring-t-n.pdf \
	figure-4/plots/fig4.pdf \
	figure-5/plots/constraints-over-time.pdf \
	figure-5/plots/populationsize-over-time.pdf \
	figure-5/plots/evaluations-over-time.pdf \
)

all: $(name).pdf


$(name).pdf: $(name).tex $(figures) references.bib
	latexmk -use-make -pdf $<

$(figures):
	make -C $(dir $(@D))

clean:
	rm -f $(addprefix $(name).,aux bbl blg dvi fdb_latexmk fls log out xcp)

cleanall: clean
	rm -f $(figures)

.phony: clean cleanall

