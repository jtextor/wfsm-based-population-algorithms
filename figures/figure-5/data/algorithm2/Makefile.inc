# Functions and derived values for the main Makefile to make use of

nlogfiles := $(shell echo $$(( $(nreps) * $(nradiuses) * $(nproblems) )) )
nperproblem := $(shell echo $$(( $(nreps) * $(nradiuses) )) )
nperradius := $(nreps)

logfileprefix := timesteps
logfiles := $(foreach i,$(shell seq 0 $$(($(nlogfiles)-1)) ),$(logfileprefix)-$(i).csv)

inc = $(shell echo $$(( $(1) + 1 )) )
index = $(wordlist $(call inc,$(2)),$(call inc,$(2)),$(1))

num2problemidx = $(shell echo $$(( $(1) / $(nperproblem) )) )
num2problem = $(call index,$(problems),$(call num2problemidx,$(1)))

num2radiusidx = $(shell echo $$(( ($(1) % $(nperproblem)) / $(nperradius) )) )
num2radius = $(call index,$(radiuses),$(call num2radiusidx,$(1)))

num2rep = $(shell echo $$(( ($(1) % $(nperproblem)) / $(nperradius) )) )
