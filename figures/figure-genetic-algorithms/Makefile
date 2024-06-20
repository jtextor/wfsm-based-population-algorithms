#radiuses := 3
#problemfiles := $(wildcard input/*.cnf)
#nreps := 6
#nradiuses := $(words $(radiuses))
#nproblems := $(words $(problemfiles))
#nlogfiles := $(shell echo $$(($(nreps) * $(nradiuses) * $(nproblems))))

plots := $(addprefix plots/,constraints-over-time.pdf evaluations-over-time.pdf populationsize-over-time.pdf)

algo2index := data/algorithm2/index.csv
algo2files := $(shell cut -d, -f3 < $(algo2index))

baselineindex := data/1+1ea/index.csv
baselinefiles := $(shell cut -d, -f3 < $(baselineindex))


all: $(plots)
	

$(plots): code/plot.R $(algo2index) $(algo2files) $(baselineindex) $(baselinefiles)
	Rscript $< 

data:
	mkdir -p $@
