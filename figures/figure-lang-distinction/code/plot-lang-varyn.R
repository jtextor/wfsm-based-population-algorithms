library( dplyr, warn.conflict = FALSE )
library( ggplot2 )
library( patchwork )
library( geomtextpath )
source('./mytheme.R')

colorMap <- c( "weighted" = "red", "unweighted" = "black" )

argv <- commandArgs( trailingOnly = TRUE )

datafile <- argv[1]
outplot <- argv[2]

data1 <- readRDS( datafile )

sem <- function(x){
	return( sd(x) / sqrt( length(x) ) )
}

data1b <- data1 %>% group_by( lang,ntrain, thresh, fsa ) %>% 
	summarise( mu_auc = mean(auc), se_auc = sem( auc ) ) %>%
	mutate( tname = paste0( "latin, t = ", thresh ))

p <- ggplot( data1b, aes( x = ntrain, y = mu_auc, group = fsa, color = fsa, fill = fsa ) ) +
	geom_ribbon( aes( ymin = mu_auc - se_auc, ymax = mu_auc + se_auc ), alpha = 0.1, color = NA ) +
	geom_line() +
	#geom_textline( aes( label = fsa ), size = 0.6* ggtextsize, hjust = 0.93 ) +
	scale_x_log10() +
	scale_y_continuous( limits =c(0.49,NA), expand = c(0,0) ) +
	labs( x = "# training strings", y = "AUC" ) +
	scale_color_manual( values = colorMap ) +
	scale_fill_manual( values = colorMap ) +
	#facet_wrap( ~tname ) +
	mytheme + 
	#theme( legend.position = "none" )
	 theme(
 		legend.position = c(0,1),
 		legend.justification = c(0,1)
 	)

ggsave(p, file = outplot, width = 1.21, height = 1.4, units = "in", useDingbats = FALSE, family=textfam, pointsize=textsize, dpi=1200, bg="transparent", useKerning=T )
