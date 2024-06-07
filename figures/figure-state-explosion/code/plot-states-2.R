library( dplyr, warn.conflict = FALSE )
library( ggplot2 )
source("./mytheme.R")

outfile <- commandArgs( trailingOnly = TRUE ) [1]

getData <- function( f = "without_push_quantize/real-encode", tag = "reals" ){
	d <- read.csv(paste0("data/",f,".csv.gz"))
	d$fsa <- tag
	return(d)
}

files <- c( 
	"reals" = "without_push_quantize/real__6_6",
	"rationals" = "with_push_quantize/rational_6_6",
	"unweighted" = "without_push_quantize/tpl__6_6"
)

all.data <- bind_rows( lapply( 1:3, function(i){
	getData( files[i], names(files)[i] )
})) %>%
mutate( size = nnodes+nedges )	%>%
mutate( nwords2 = nwords * rnorm( length(nwords), mean = 1, sd =  0.05))

d1 <- all.data %>% 
	group_by( fsa, nwords ) %>%
	summarize( msize = mean(size), sd = sd(size) )


p <- ggplot( all.data, aes( x = nwords, y =size, group = fsa, color = fsa, fill = fsa, lty = fsa ) ) +
	stat_smooth( size = .5, se=FALSE ) +
	#stat_smooth(data = all.data[ all.data$fsa == "rationals", ], size = 0.3, se=FALSE ) +
	geom_jitter( size = .2, show.legend = FALSE, aes( x = nwords2 ), alpha = 0.3 ) +
	#geom_line( size = .3, data = d1, aes( y = msize ) ) +
	scale_x_log10( expand = c(0,0) ) +
	scale_y_continuous( limits=c(0,NA), expand = c(0,0) ) +
	scale_color_manual( values = c("unweighted" = "gray", "reals" = "black", "rationals" = "red" ) ) +
	scale_fill_manual( values = c("unweighted" = "gray", "reals" = "black", "rationals" = "red" ) ) +
	scale_linetype_manual( values = c("unweighted" = 2, "reals" = 1, "rationals" = 1 ) ) +
	labs( x = "# training strings", y = "FSM size (|V|+|E|)") +
	mytheme +
	theme(
		legend.position = c(0,1),
		legend.justification = c(0,1 ),
		legend.key.width = unit( 6, "mm" ),
		plot.margin = unit(c(0.3,0.5,0.3,0.3),"cm")#trbl
	)

ggsave( p, file = outfile, width = 8, height = 4, units = "cm", useDingbats = FALSE )