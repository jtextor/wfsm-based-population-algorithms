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

data1sum <- data1 %>% group_by( lang,ntrain, thresh, fsa ) %>% 
	summarise( mu_auc = mean(auc), se_auc = sem( auc ) ) %>%
	mutate( tname = paste0( "latin, t = ", thresh ))

datat2 <- data1sum %>% filter( thresh == 2 )
datat3 <- data1sum %>% filter( thresh == 3 )

nmax <- max( datat2$ntrain )
aucs <- range( datat2[ datat2$ntrain == nmax, "mu_auc" ] )
print(nmax)
print(aucs)
	
# p1 <- ggplot( datat2, aes( x = ntrain, y = mu_auc, group = fsa, color = fsa, fill = fsa ) ) +
# 	geom_ribbon( aes( ymin = mu_auc - se_auc, ymax = mu_auc + se_auc ), alpha = 0.1, color = NA ) +
# 	geom_line() +
# 	#geom_textline( aes( label = fsa ), size = 0.6* ggtextsize, hjust = 0.93 ) +
# 	# annotate( "segment", x = nmax, xend = nmax, y = aucs[1], yend = aucs[2], 
# # 		lty = 2, size = 0.2, color = "gray50" ) +
# # 	annotate( "text", x = nmax, y = aucs[2]-0.03, hjust = 1, size = 0.6*ggtextsize, 
# # 		label = expression( Delta * "AUC "), color = "gray50" ) +
# 	annotate( "text", x = 4, y = 0.9, label = "t = 2", 
# 		hjust = 0, vjust =1, size = 0.7*ggtextsize ) +
# 	scale_x_log10() +
# 	scale_y_continuous( limits =c(0.49,.9), expand = c(0,0) ) +
# 	labs( x = "# training strings", y = "AUC" ) +
# 	scale_color_manual( values = colorMap ) +
# 	scale_fill_manual( values = colorMap ) +
# 	#facet_wrap( ~tname ) +
# 	mytheme + 
# 	#theme( legend.position = "none" )
# 	 theme(
#  		legend.position = c(0,1),
#  		legend.justification = c(0,1)
#          )
# 
# ggsave(p1, file="example-latin-t2.pdf", width=1.21, height=1.1, units="in", bg="transparent", family=textfam, pointsize=textsize, dpi=1200, useDingbats=F, useKerning=T)
# 
# p2 <- ggplot( datat3, aes( x = ntrain, y = mu_auc, group = fsa, color = fsa, fill = fsa ) ) +
# 	geom_ribbon( aes( ymin = mu_auc - se_auc, ymax = mu_auc + se_auc ), alpha = 0.1, color = NA ) +
# 	geom_line() +
# 	#geom_textline( aes( label = fsa ), size = 0.6* ggtextsize, hjust = 0.93 ) +
# 	annotate( "text", x = 4, y = 0.9, label = "t = 3", 
# 		hjust = 0, vjust =1, size = 0.7*ggtextsize ) +
# 	scale_x_log10() +
# 	scale_y_continuous( limits =c(0.49,.9), expand = c(0,0) ) +
# 	labs( x = "# training strings", y = "AUC" ) +
# 	scale_color_manual( values = colorMap ) +
# 	scale_fill_manual( values = colorMap ) +
# 	#facet_wrap( ~tname ) +
# 	mytheme + theme( legend.position = "none" )
# 	# theme(
# # 		legend.position = c(0,1),
# # 		legend.justification = c(0,1)
# # 	)
# 
# ggsave(p2, file="example-latin-t3.pdf", width=1.21, height=1.1, units="in", bg="transparent", family=textfam, pointsize=textsize, dpi=1200, useDingbats=F, useKerning=T)

we <- data1sum %>% filter( fsa == "weighted" )
un <- data1sum %>% filter( fsa == "unweighted" )

p1 <- ggplot( we, aes( x = ntrain, y = mu_auc, group = thresh ) ) +
	geom_textline( aes( alpha = thresh, label = thresh, hjust = thresh*0.07 ), 
		color = colorMap["weighted"], size = 0.5* ggtextsize ) +
	scale_x_log10() +
	labs( x = "# training strings", y = "AUC" ) +
	scale_y_continuous( limits =c(0.45,1.02), expand = c(0,0) ) +
	scale_alpha_continuous( range = c(0.3, 1 ) ) +
	facet_wrap( ~fsa ) +
	mytheme + theme(
		legend.position = "none"
	)

p2 <- ggplot( un, aes( x = ntrain, y = mu_auc, group = thresh ) ) +
	geom_textline( aes( alpha = thresh, label = thresh, hjust = thresh*0.07 ), 
		color = colorMap["unweighted"], size = 0.5* ggtextsize ) +
	scale_x_log10() +
	labs( x = "# training strings", y = "AUC" , alpha = "t" ) +
	scale_y_continuous( limits =c(0.45,1.02), expand = c(0,0) ) +
	scale_alpha_continuous( range = c(0.3, 1 ) ) +
	facet_wrap( ~fsa ) +
	mytheme + theme(
		legend.position = "none"
	)

p <- p1 + p2 + plot_annotation(tag_levels = 'A') 

ggsave(p, file = outplot, width = 8, height = 3.5, units = "cm", useDingbats = FALSE )
