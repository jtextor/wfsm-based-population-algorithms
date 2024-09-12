library( dplyr, warn.conflict = FALSE )
library( ggplot2 )
library( patchwork )
library( geomtextpath )
source('./mytheme.R')

colorMap <- c( "weighted" = "red", "unweighted" = "black" )
tvec <- c(1,2,3,4,5,6,7,8)

argv <- commandArgs( trailingOnly = TRUE )

datafile <- argv[1]
outplot <- argv[2]

data1 <- readRDS( datafile )

sem <- function(x){
	return( sd(x) / sqrt( length(x) ) )
}

data1sum <- data1 %>% group_by( ntrain, thresh, fsa ) %>% 
	filter( is.element( thresh, tvec ) ) %>%
	summarise( mu_auc = mean(auc), se_auc = sem( auc ) ) 

mapw <- c( 0.1, 0.07, 0.07, 0.4, 0.5, 0.6, 0.7, 0.8 )
mapu <- c( 0.1, 0.25, 0.35, 0.5, 0.55, 0.66, 0.76, 0.86 )


we <- data1sum %>% filter( fsa == "weighted" ) %>%
	mutate( hj = mapw[thresh] )
un <- data1sum %>% filter( fsa == "unweighted" ) %>%
	mutate( hj = mapu[thresh] )


p1 <- ggplot( we, aes( x = ntrain, y = mu_auc, group = thresh ) ) +
	geom_textline( aes( alpha = thresh, label = thresh, hjust = hj ), 
		color = colorMap["weighted"], size = 0.55* ggtextsize,
		padding = unit( 0.5,"mm") ) +
	scale_x_log10() +
	annotate( "text", x = 3, y = 1, vjust = 0.7, hjust = 0, size = 0.7*ggtextsize, 
		label="weighted" ) +
	labs( x = "# training strings", y = "AUC" ) +
	scale_y_continuous( limits =c(0.45,1.02), expand = c(0,0) ) +
	scale_alpha_continuous( range = c(0.3, 1 ) ) +
	#facet_wrap( ~fsa ) +
	mytheme + theme(
		legend.position = "none"
	)

p2 <- ggplot( un, aes( x = ntrain, y = mu_auc, group = thresh ) ) +
	geom_textline( aes( alpha = thresh, label = thresh, hjust = hj ), 
		color = colorMap["unweighted"], size = 0.55* ggtextsize,
		padding = unit( 0.5,"mm") ) + 
	scale_x_log10() +
	annotate( "text", x = 3, y = 1, vjust = 0.7, hjust = 0, size = 0.7*ggtextsize, 
		label="unweighted" ) +
	labs( x = "# training strings", y = "AUC" , alpha = "t" ) +
	scale_y_continuous( limits =c(0.45,1.02), expand = c(0,0) ) +
	scale_alpha_continuous( range = c(0.3, 1 ) ) +
	#facet_wrap( ~fsa ) +
	mytheme + theme(
		legend.position = "none"
	)
p <- p2 + p1 + plot_annotation(tag_levels = 'A') 

ggsave(p, file = outplot, width = 8, height = 3.5, units = "cm", useDingbats = FALSE )
