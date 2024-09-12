plots := $(addprefix plots/,constraints-over-time.pdf evaluations-over-time.pdf populationsize-over-time.pdf)

algo2dir := data/algorithm2
algo2cfg := $(algo2dir)/Makefile.cfg
algo2index := $(algo2dir)/index.csv
algo2files = $(addprefix $(algo2dir)/,$(shell cut -d, -f3 < $(algo2index)))

baselinedir := data/1+1ea
baselinecfg := $(baselinedir)/Makefile.cfg
baselineindex := $(baselinedir)/index.csv
baselinefiles = $(addprefix $(baselinedir)/,$(shell cut -d, -f3 < $(baselineindex)))


all: $(plots)
	

$(plots): code/plot.R $(algo2index) $(algo2files) $(baselineindex) $(baselinefiles)
	Rscript $< 

$(algo2index): $(algo2cfg)
	$(MAKE) -C $(algo2dir)

$(baselineindex): $(baselinecfg)
	$(MAKE) -C $(baselinedir)

clean:
	$(MAKE) -C $(algo2dir) clean
	$(MAKE) -C $(baselinedir) clean
