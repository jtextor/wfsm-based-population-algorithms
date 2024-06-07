library( dplyr, warn.conflict = FALSE )
library( ggplot2 )
library( patchwork )
library( geomtextpath )
source('./mytheme.R')

colorMap <- c( "weighted" = "red", "unweighted" = "black" )
tt <- 3

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
	filter( thresh == tt, lang != "la"  ) %>%
	summarise( mu_auc = mean(auc), se_auc = sem( auc ) ) %>%
	mutate( tname = paste0( "t = ", thresh ))

plotLang <- function( l, lname ){
	dd <- data1sum %>% filter( lang == l )
	p <- ggplot( dd, aes( x = ntrain, y = mu_auc, group = fsa, color = fsa, fill = fsa ) ) +
	geom_ribbon( aes( ymin = mu_auc - se_auc, ymax = mu_auc + se_auc ), alpha = 0.2, color = NA ) +
	geom_line() +
	scale_x_log10() +
	scale_y_continuous( limits =c(0.49,0.9), expand = c(0,0) ) +
	annotate( "text", x = 2, y = 0.9, label = lname, hjust = 0, vjust = 1, size = 0.69*ggtextsize ) +
	labs( x = "# training strings", y = "AUC" ) +
	scale_color_manual( values = colorMap ) +
	scale_fill_manual( values = colorMap ) +
	#facet_wrap( ~lang, ncol = 2 ) +
	mytheme + 
	theme(
		legend.position = "none",
		plot.margin = unit(c(1,1,1,0.3),"mm")#trbl
	)
}

p1 <- plotLang( "en2", "English-\nEnglish" )
p2 <- plotLang( "me", "English-\nmedieval English" )
p3 <- plotLang( "pd", "English-\nPlautdietsch" )
p5 <- plotLang( "ta", "English-\nTagalog" )
p4 <- plotLang( "hi", "English-\nHiligaynon")
p6 <- plotLang( "xh", "English-\nXhosa" )

p <- ( p1 + p2 ) / (p3 +p4 ) / (p5 + p6)


ggsave(p, file = outplot, width = 8, height = 10, units = "cm", useDingbats = FALSE )
