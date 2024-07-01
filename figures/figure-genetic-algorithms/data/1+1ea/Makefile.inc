# Functions and derived values for the main Makefile to make use of

nlogfiles := $(shell echo $$(( $(nreps) * $(nmutprobs) * $(nproblems) )) )
nperproblem := $(shell echo $$(( $(nreps) * $(nmutprobs) )) )
npermutprob := $(nreps)

logfileprefix := timesteps
logfiles := $(foreach i,$(shell seq 0 $$(($(nlogfiles)-1)) ),$(logfileprefix)-$(i).csv)

inc = $(shell echo $$(( $(1) + 1 )) )
index = $(wordlist $(call inc,$(2)),$(call inc,$(2)),$(1))

num2problemidx = $(shell echo $$(( $(1) / $(nperproblem) )) )
num2problem = $(call index,$(problems),$(call num2problemidx,$(1)))

num2mutprobidx = $(shell echo $$(( ($(1) % $(nperproblem)) / $(npermutprob) )) )
num2mutprob = $(call index,$(mutprobs),$(call num2mutprobidx,$(1)))

num2rep = $(shell echo $$(( ($(1) % $(nperproblem)) / $(nperradius) )) )
