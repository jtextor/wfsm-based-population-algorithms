#!/usr/bin/env Rscript

library(tidyverse)
library(ggplot2)

wagadir <- "data/algorithm2/"
eadir <- "data/1+1ea/"

family <- "sans"
size <- 7

wi <- read_csv(paste0(wagadir, "index.csv"),
        col_types="cic", col_names=c("problem", "radius", "outcsv")) |>
    mutate(problem=paste0(wagadir, problem), outcsv=paste0(wagadir, outcsv)) |>
    rowid_to_column(var="key")

ei <- read_csv(paste0(eadir, "index.csv"),
        col_types="cdc", col_names=c("problem", "mu", "outcsv")) |>
    mutate(problem=paste0(eadir, problem), outcsv=paste0(eadir, outcsv)) |>
    rowid_to_column(var="key")

wd <- reduce(wi$outcsv,
    function(a, o)
        bind_rows(a,
            read_csv(o, col_types="iiici") |>
                mutate(outcsv=o) |>
                rowid_to_column("step")),
    .init=NULL)

ed <- reduce(ei$outcsv,
    function(a, o)
        bind_rows(a,
            read_csv(o, col_types="dici") |> mutate(outcsv=o)),
    .init=NULL)

wj <- inner_join(wi, wd, by="outcsv")
ej <- inner_join(ei, ed, by="outcsv")

both <- bind_rows(
    select(filter(wj, radius==3), duration_ms, fitness, key, problem, step) |>
        mutate(Algorithm="Algorithm 2"),
    select(filter(ej, mu==0.02), duration_ms, fitness, key, problem, step) |>
        mutate(Algorithm="1+1 EA")
)

k17a <- filter(wj, key==17)
k17b <- filter(both, key==17, Algorithm=="Algorithm 2")

allelse <- filter(both, Algorithm!="Algorithm 2" | key!=17)

mytheme <- theme_minimal(base_size=size) +
    theme(
        text=element_text(family=family, size=size),
        axis.text=element_text(family=family, size=size),
        legend.text=element_text(family=family, size=size),
        axis.line=element_line(),
        axis.ticks=element_line(),
        panel.background=element_blank(),
        plot.background=element_blank(),
        legend.title=element_blank(),
        panel.grid=element_blank(),
        strip.text=element_blank()
    )

p1 <- ggplot(both, aes(x=duration_ms*1e-3, y=fitness, group=interaction(key, Algorithm), color=Algorithm)) +
    geom_line(linewidth=0.2, position=position_jitter(w=0, h=0.25)) +
    geom_point(data=filter(allelse, fitness==218), size=1) +
    geom_point(data=filter(k17b, fitness==218), size=2, shape=5, show.legend=F) +
    facet_wrap(~basename(problem), nrow=1) +
    xlab("Time elapsed (s)") +
    ylab("Clauses satisfied") +
    scale_color_manual(values=c("#000000", "#ff0000")) +
    scale_y_continuous(breaks=c(210, 214, 218)) +
    coord_cartesian(ylim=c(210, 218.5)) +
    mytheme + theme(legend.position=c(0.15, 0.9))

ggsave(p1, file="plots/constraints-over-time.pdf", width=4.2, height=1.0, units="in", bg="transparent", family=family, pointsize=size, dpi=1200, useDingbats=F, useKerning=T)

p2 <- ggplot(k17a, aes(x=duration_ms*1e-3, y=nstrings)) +
    geom_point(size=0.1) +
    geom_vline(
        xintercept=1e-3 * k17a$duration_ms[k17a$fitness - lag(k17a$fitness) > 0],
        linetype=3,
        linewidth=0.3) +
    xlab("Time elapsed (seconds)") +
    ylab("Population size") +
    mytheme

ggsave(p2, file="plots/populationsize-over-time.pdf", width=2.1, height=1.0, units="in", bg="transparent", family=family, pointsize=size, dpi=1200, useDingbats=F, useKerning=T)

m <- mutate(both, nevals=ifelse(Algorithm=="Algorithm 2", 20876*step, step))

p3 <- ggplot( filter(m, basename(problem)=="uf50-0748.cnf"),
    aes(x=1e-3*duration_ms, y=nevals, group=interaction(key, Algorithm), color=Algorithm)
    ) +
    geom_line(position=position_jitter(h=0.2, w=0), linewidth=0.2) +
    scale_color_manual(values=c("#000000", "#ff0000")) +
    scale_y_log10() +
    xlab("Time elapsed (seconds)") +
    ylab("Evaluations") +
    mytheme + theme(legend.position="none")

ggsave(p3, file="plots/evaluations-over-time.pdf", width=2.1, height=1.0, units="in", bg="transparent", family=family, pointsize=size, dpi=1200, useDingbats=F, useKerning=T)
