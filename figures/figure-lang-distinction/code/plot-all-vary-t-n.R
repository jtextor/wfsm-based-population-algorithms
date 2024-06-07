library( dplyr, warn.conflict = FALSE )
library( ggplot2 )
library( patchwork )
library( geomtextpath )
source('./mytheme.R')

colorMap <- c( "weighted" = "red", "unweighted" = "black" )

argv <- commandArgs( trailingOnly = TRUE )

filepattern <- argv[1]
langvec <- unlist( strsplit( argv[2], " ") )
outplot <- argv[3]

filevec <- sapply( langvec, function(x) sub( "XX", x, filepattern ) )

data1 <- data.frame()
for( f in filevec ){
	dtmp <- readRDS( f )
	data1 <- rbind( data1, dtmp )
}

sem <- function(x){
	return( sd(x) / sqrt( length(x) ) )
}

data1sum <- data1 %>% group_by( lang, ntrain, thresh, fsa ) %>% 
	summarise( mu_auc = mean(auc), se_auc = sem( auc ) ) %>%
	mutate( tname = paste0( "t = ", thresh ))
	
p <- ggplot( data1sum, aes( x = ntrain, y = mu_auc, group = fsa, color = fsa, fill = fsa ) ) +
	geom_ribbon( aes( ymin = mu_auc - se_auc, ymax = mu_auc + se_auc ), alpha = 0.2, color = NA ) +
	geom_line() +
	scale_x_log10() +
	scale_y_continuous( limits =c(0.45,NA), expand = c(0,0) ) +
	labs( x = "# training strings", y = "AUC" ) +
	scale_color_manual( values = colorMap ) +
	scale_fill_manual( values = colorMap ) +
	facet_grid( lang~tname ) +
	mytheme + 
	theme(
		legend.position = c(0,1),
		legend.justification = c(0,1)
	)

# we <- data1sum %>% filter( fsa == "weighted" )
# un <- data1sum %>% filter( fsa == "unweighted" )
# 
# p1 <- ggplot( we, aes( x = ntrain, y = mu_auc, group = thresh ) ) +
# 	geom_textline( aes( alpha = thresh, label = thresh, hjust = thresh*0.07 ), 
# 		color = colorMap["weighted"], size = 0.5* ggtextsize ) +
# 	scale_x_log10() +
# 	labs( x = "# training strings", y = "AUC" ) +
# 	scale_y_continuous( limits =c(0.45,1.02), expand = c(0,0) ) +
# 	scale_alpha_continuous( range = c(0.3, 1 ) ) +
# 	facet_wrap( ~fsa ) +
# 	mytheme + theme(
# 		legend.position = "none"
# 	)
# 
# p2 <- ggplot( un, aes( x = ntrain, y = mu_auc, group = thresh ) ) +
# 	geom_textline( aes( alpha = thresh, label = thresh, hjust = thresh*0.07 ), 
# 		color = colorMap["unweighted"], size = 0.5* ggtextsize ) +
# 	scale_x_log10() +
# 	labs( x = "# training strings", y = "AUC" , alpha = "t" ) +
# 	scale_y_continuous( limits =c(0.45,1.02), expand = c(0,0) ) +
# 	scale_alpha_continuous( range = c(0.3, 1 ) ) +
# 	facet_wrap( ~fsa ) +
# 	mytheme + theme(
# 		legend.position = "none"
# 	)
# p <- p2 + p1 

ggsave(p, file = outplot, width = 12, height = 20, units = "cm", useDingbats = FALSE )
