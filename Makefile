SHELL := /bin/sh
.SUFFIXES:

all: manuscript/automata-as-populations.pdf

manuscript/automata-as-populations.pdf:
	make -C manuscript
