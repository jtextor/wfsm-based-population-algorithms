library( dplyr, warn.conflict = FALSE )
library( ggplot2 )
source('./mytheme.R')

argv <- commandArgs( trailingOnly = TRUE )
tfile <- argv[1]
outfile <- argv[2]

data1 <- readRDS( tfile )

data1b <- data1 %>%
	group_by( ntrain, thresh ) %>%
	summarise( time = mean(usr) )
	
p <- ggplot( data1b, aes( x = ntrain, y = time, group = thresh, color = thresh ) ) +
	geom_line() + 
	labs( x = "# training strings", y = "sys.time" ) +
	scale_y_log10() +
	mytheme + theme(
		legend.position = "right"
	)
	
	
ggsave( p, file = outfile, width = 7, height = 5, units = "cm", useDingbats = FALSE )