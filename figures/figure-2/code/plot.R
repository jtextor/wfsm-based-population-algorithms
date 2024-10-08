#!/usr/bin/env Rscript

library( dplyr, warn.conflict = FALSE )
library( ggplot2 )
source("mytheme.R")

outfile <- commandArgs( trailingOnly = TRUE ) [1]

family <- "sans"
size <- 7

getData <- function( f = "without_push_quantize/real-encode", tag = "reals" ){
	d <- read.csv(paste0("data/",f,".csv.gz"))
	d$fsa <- tag
	return(d)
}

files <- c( 
	"reals" = "logfa",# "without_push_quantize/real__6_6",
	"rationals" = "ratfa", #"with_push_quantize/rational_6_6",
	"unweighted" = "boolfa"#"without_push_quantize/tpl__6_6"
)

all.data <- bind_rows( lapply( 1:3, function(i){
	getData( files[i], names(files)[i] )
})) %>%
mutate( size = nstates+narcs )	%>%
mutate( nwords2 = nstrings * rnorm( length(nstrings), mean = 1, sd =  0.05))

d1 <- all.data %>% 
	group_by( fsa, nstrings ) %>%
	summarize( msize = mean(size), sd = sd(size) )


p <- ggplot( all.data, aes( x = nstrings, y =size, group = fsa, color = fsa, fill = fsa, lty = fsa ) ) +
	stat_smooth( size = .5, se=FALSE ) +
	#stat_smooth(data = all.data[ all.data$fsa == "rationals", ], size = 0.3, se=FALSE ) +
	geom_jitter( size = .2, show.legend = FALSE, aes( x = nwords2 ), alpha=0.3) +
	#geom_line( size = .3, data = d1, aes( y = msize ) ) +
	scale_x_log10( expand = c(0,0) ) +
	scale_y_continuous( expand = c(0,0) ) +
	scale_color_manual( values = c("unweighted" = "grey", "reals" = "red", "rationals" = "blue" ) ) +
	scale_fill_manual( values = c("unweighted" = "grey", "reals" = "red", "rationals" = "blue" ) ) +
	scale_linetype_manual( values = c("unweighted" = 2, "reals" = 1, "rationals" = 1 ) ) +
    coord_cartesian( ylim=c(0, 320) ) +
	labs( x = "# training strings", y = "FSM size (|V| + |E|)") +
	mytheme +
     theme(
         text=element_text(family=family, size=size),
         axis.text=element_text(family=family, size=size),
         legend.text=element_text(family=family, size=size),
         panel.background = element_blank(),
         plot.background = element_blank(),
         legend.position = c(0,1),
         legend.justification = c(0,1 ),
         legend.key.width = unit( 6, "mm" ),
         #plot.margin = unit(c(0.3,0.5,0.3,0.3),"cm")#trbl
     )

ggsave( p, file = outfile, device="pdf", width = 2.33, height = 2, units="in", bg="transparent", family=family, pointsize=size, dpi=1200, useDingbats=F, useKerning=T)
#ggsave( p, file = outfile, device=cairo_ps, width = 2.4, height = 2, units="in", bg="transparent", pointsize = size, family=family, fallback_resolution=1200)
