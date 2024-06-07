
dotfiles := $(wildcard *.dot)
pdfs := $(patsubst %.dot,%.pdf,$(dotfiles))

all: $(pdfs) weave.eps
	

%.pdf: %.dot
	dot -Tpdf -o $@ $<


weave.eps: weave.dvi
	dvips -E -o $@ $<

weave.dvi: weave.tex
	latex $<
