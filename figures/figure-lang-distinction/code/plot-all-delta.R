library( dplyr, warn.conflict = FALSE )
library( tidyr )
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

sim <- read.table( "input/similarities.txt", header = TRUE, sep = "\t" )
map <- c( "eng" = "en2", "pdc" = "pd", "lat" = "la", "tgl" = "ta", "xho" = "xh", "hil" = "hi" )
sim$lang <- map[ sim$iso2 ]

sim.map <- setNames( sim$sim, sim$lang )

data1 <- data.frame()
for( f in filevec ){
	dtmp <- readRDS( f )
	data1 <- rbind( data1, dtmp )
}

data1$similarity <- sim.map[ data1$lang ]
data1$lang <- factor( data1$lang, levels = c("en2", "me", "la", "pd", "ta" , "hi", "xh" ) )

sem <- function(x){
	return( sd(x) / sqrt( length(x) ) )
}

nmax <- max( data1$ntrain )

data1sum <- data1 %>% 
	filter( thresh == 2 ) %>%
	group_by( lang, thresh ) %>% 
	pivot_wider( names_from = "fsa", values_from = "auc" ) %>%
	mutate( delta = weighted - unweighted, auc = max( weighted ) ) %>%
	filter( ntrain == nmax, thresh == 2 ) %>%
	summarise( mu_delta = mean(delta), se_delta = sem( delta ), mu_auc = mean(auc), 
		se_auc = sem( auc ), similarity = similarity[1] )
	
p1 <- ggplot( data1sum, aes( x = lang, y = mu_delta ) ) +
	geom_col( fill="black" ) +
	annotate( "text", x = "en2", y = 0.12, label = "t = 2", size = 0.8*ggtextsize, hjust = 0.3 ) +
	scale_y_continuous( limits =c(0,NA) ) +
	labs( x = "test language", y = expression( Delta * "AUC" ) ) +
	mytheme 

p1b <- ggplot( data1sum, aes( x = similarity, y = mu_delta ) ) +
	geom_point( size = .5 ) +
	geom_text( aes( label = lang ), size = 0.6 * ggtextsize, hjust = -.5 ) +
	annotate( "text", x = 0.01, y = 0, label = "t = 2", size = 0.8*ggtextsize, hjust = 0 ) +
	scale_y_continuous( limits =c(0,NA) ) +
	scale_x_continuous( limits =c(0,4) ) +
	labs( x = "lexical similarity", y = expression( Delta * "AUC" ) ) +
	mytheme 

p2 <- ggplot( data1sum, aes( x = similarity, y = mu_auc ) ) +
	geom_point( size = .5 ) +
	geom_text( aes( label = lang ), size = 0.6 * ggtextsize, hjust = -.4 ) +
	annotate( "text", x = 0.01, y = 0.5, label = "t = 2", size = 0.8*ggtextsize, hjust = 0 ) +
	scale_y_continuous( limits =c(0.5,NA) ) +
	scale_x_continuous( limits =c(0,4) ) +
	labs( x = "lexical similarity", y = "max AUC" ) +
	mytheme 
	
	
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

p <- p1b + p2 

ggsave(p, file = outplot, width = 12, height = 5, units = "cm", useDingbats = FALSE )
