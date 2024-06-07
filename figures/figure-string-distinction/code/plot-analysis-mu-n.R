library( dplyr, warn.conflict = FALSE )
library( ggplot2 )
library( patchwork )
library( geomtextpath )
source('./mytheme.R')

colorMap <- c( "weighted" = "red", "unweighted" = "black" )
thresh <- 5
l <- 8
mut <- 0.6
Nt <- 250

argv <- commandArgs( trailingOnly = TRUE )

datafile <- argv[1]
outplot <- argv[2]

all_data <- readRDS( datafile )

sem <- function(x){
	return( sd(x) / sqrt( length(x) ) )
}

data1 <- all_data$vary_n
data1sum <- data1 %>% group_by( ntrain, fsa ) %>% summarise( mu_auc = mean(auc), se_auc = sem( auc ) )


data2 <- all_data$vary_mu
data2sum <- data2 %>% group_by( mut, fsa ) %>% summarise( mu_auc = mean(auc), se_auc = sem( auc ) )

# expected number of times to find the at least t consecutive foreign positions in 
# training data:
refProb <- ( 1 - pgeom( thresh, (1-mut) ) )
nCritical <- 1/refProb
muts <- seq( 0.2,0.6,by=0.01 )
muCritical <- muts[ Nt*(1 - pgeom( thresh, (1-muts) ) ) >= 1 ][1]

p1 <- ggplot( data1sum, aes( x = ntrain, y = mu_auc, color = fsa, fill = fsa, group = fsa ) ) +
	geom_ribbon( aes( ymin = mu_auc - se_auc, ymax = mu_auc + se_auc ), alpha = 0.1, color = NA ) +
	#geom_line() +
	geom_textline( aes( label = fsa ), size = 0.7* ggtextsize, hjust=0.3) +
	labs( x = "# training strings", y = "AUC" ) +
	annotate( "text", x = 3, y = 0.92, label = expression( mu * " = 0.6" ), hjust = 0, vjust =0, size = 0.7*ggtextsize ) +
	annotate("segment", x = nCritical, y = 0.6, xend = nCritical, yend = 0.52, size = 0.3, 
         arrow = arrow(type = "closed", length = unit(0.05, "npc"))) +
	geom_vline( xintercept = 250, lty = 2, size = 0.3, color = "gray" ) +
	scale_x_log10() +
	scale_y_continuous( limits =c(0.5,1.02), expand = c(0,0) ) +
	scale_color_manual( values = colorMap ) +
	scale_fill_manual( values = colorMap ) +
	mytheme + theme(
		#legend.background = element_rect( fill = NA )
		legend.position = "none"
	)



p2 <- ggplot( data2sum, aes( x = mut, y = mu_auc, color = fsa, fill = fsa, group = fsa ) ) +
	geom_ribbon( aes( ymin = mu_auc - se_auc, ymax = mu_auc + se_auc ), alpha = 0.1, color = NA ) +
	#geom_line() +
	geom_textline( aes( label = fsa ), size = 0.7* ggtextsize, hjust=0.5) +
	annotate( "text", x = 0.02, y = 0.92, label = "N = 250", hjust = 0, vjust = 0, size = 0.7*ggtextsize ) +
	annotate("segment", x = muCritical, y = 0.6, xend = muCritical, yend = 0.52, size = 0.3, 
         arrow = arrow(type = "closed", length = unit(0.05, "npc"))) +
	labs( x = expression(mu), y = "AUC" ) +
	scale_x_continuous( limits = c(0,.75), expand = c(0,0) ) +
	scale_y_continuous( limits =c(0.5,1.02), expand = c(0,0) ) +
	scale_color_manual( values = colorMap ) +
	scale_fill_manual( values = colorMap ) +
	geom_vline( xintercept = 0.6, lty = 2, size = 0.3, color = "gray" ) +
	mytheme + theme(
		#legend.background = element_rect( fill = NA )
		legend.position = "none"
	)



p <- p1 + p2 + plot_annotation(tag_levels = 'A') 
ggsave(p, file = outplot, width = 8, height = 3.5, units = "cm", useDingbats = FALSE )
