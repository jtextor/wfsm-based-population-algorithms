
dotfiles := $(wildcard *.dot)
pdfs := $(patsubst %.dot,%.pdf,$(dotfiles))

all: $(pdfs)
	

%.pdf: %.dot
	dot -Tpdf -o $@ $<
