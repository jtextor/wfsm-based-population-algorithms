
name := automata-as-populations

figures := \
	../figures/figure-compressed-strings/compressed.pdf \
	../figures/figure-state-explosion/plots/states.pdf \
	../figures/figure-wfsm/wfsm-table.pdf

all: $(name).pdf


$(name).pdf: $(name).tex $(figures) references.bib
	latexmk -pdf $<


clean:
	rm -f $(addprefix $(name).,aux bbl blg dvi fdb_latexmk fls log out xcp)


.phony: clean
