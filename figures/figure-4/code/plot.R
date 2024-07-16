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
leftdatafile <- argv[3]
outplot <- argv[4]

filevec <- sapply( langvec, function(x) sub( "XX", x, filepattern ) )

data1 <- data.frame()
for( f in filevec ){
	dtmp <- readRDS( f )
	data1 <- rbind( data1, dtmp )
}

dataleft <- readRDS( leftdatafile )
dataleft2 <- filter(dataleft, thresh == 2)
dataleft3 <- filter(dataleft, thresh == 3)

sem <- function(x){
	return( sd(x) / sqrt( length(x) ) )
}

data1sum <- data1 %>% group_by( lang, ntrain, thresh, fsa ) %>% 
	filter( thresh == tt, lang != "la"  ) %>%
	summarise( mu_auc = mean(auc), se_auc = sem( auc ) ) %>%
	mutate( tname = paste0( "t = ", thresh ))

plotLeft <- function( data ) {
    summarized <- data %>% group_by( lang,ntrain, thresh, fsa ) %>% 
	summarise( mu_auc = mean(auc), se_auc = sem( auc ) )
	tname <- paste0( "Latin, t = ", data$thresh[1] )

    ggplot(summarized, aes( x = ntrain, y = mu_auc, group = fsa, color = fsa, fill = fsa ) ) +
	geom_ribbon( aes( ymin = mu_auc - se_auc, ymax = mu_auc + se_auc ), alpha = 0.1, color = NA ) +
	geom_line() +
	scale_x_log10(breaks=c(1e1, 1e2, 1e3, 1e4), labels=c(expression(10^1), expression(10^2), expression(10^3), expression(10^4))) +
	labs( x = "# training strings", y = "AUC" ) +
	scale_color_manual( values = colorMap ) +
	scale_fill_manual( values = colorMap ) +
	annotate( "text", x = 4, y = 0.93, label = tname, hjust = 0, vjust = 1, size = 0.6*ggtextsize ) +
    coord_cartesian(xlim = c(3, 10000), ylim=c(0.5, 0.9), expand=F, clip="off") +
	#facet_wrap( ~tname ) +
	mytheme #+ theme(legend.text=element_text(size=0.55*textsize))
}

noLegend <- function() {
	theme( legend.position = "none" )
}

withLegend <- function() {
	 theme(
        legend.position = c(-0.1,1.2),
        legend.justification = c(0,1)
     )
}

plotLang <- function( l, lname ){
	dd <- data1sum %>% filter( lang == l )
	p <- ggplot( dd, aes( x = ntrain, y = mu_auc, group = fsa, color = fsa, fill = fsa ) ) +
	geom_ribbon( aes( ymin = mu_auc - se_auc, ymax = mu_auc + se_auc ), alpha = 0.2, color = NA ) +
	geom_line() +
	scale_x_log10(breaks=c(1e1, 1e2, 1e3, 1e4), labels=c(expression(10^1), expression(10^2), expression(10^3), expression(10^4))) +
	annotate( "text", x = 4, y = 0.9, label = lname, hjust = 0, vjust = 1, size = 0.55*ggtextsize ) +
	labs( x = "# training strings", y = "AUC" ) +
	scale_color_manual( values = colorMap ) +
	scale_fill_manual( values = colorMap ) +
    coord_cartesian(xlim = c(3, 10000), ylim=c(0.5, 0.9), expand=F) +
	#facet_wrap( ~lang, ncol = 3, scales="free" ) +
	mytheme + 
	theme(
        axis.line=element_line(),
		legend.position = "none",
		plot.margin = unit(c(1,1,1,0.3),"mm")#trbl
	)
}

noX <- theme(axis.text.x=element_blank(), axis.title.x=element_blank())
noY <- theme(axis.text.y=element_blank(), axis.title.y=element_blank())

p1 <- plotLang( "en2", "English-\nEnglish" ) + noY + noX
p2 <- plotLang( "me", "English-\nmedieval English" ) + noY + noX
p3 <- plotLang( "pd", "English-\nPlautdietsch" ) + noY + noX
p4 <- plotLang( "hi", "English-\nHiligaynon") + noY
p5 <- plotLang( "ta", "English-\nTagalog" ) + noY
p6 <- plotLang( "xh", "English-\nXhosa" ) + noY

plt <- plotLeft( dataleft2 ) + withLegend() + noX
plb <- plotLeft( dataleft3 ) + noLegend()

p <- wrap_plots(list(plt,p1,p2,p3,plb,p4,p5,p6), ncol=4, byrow=T) + plot_annotation(tag_levels=list(c("A", "C", "", "", "B", "", "", "")))

ggsave(p, file = outplot, width = 3.1, height = 1.6, units = "in", useDingbats = FALSE, dpi = 1200, bg="transparent", family=textfam, pointsize=textsize, useKerning=T )
