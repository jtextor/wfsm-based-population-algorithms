library( ggplot2, warn.conflicts = FALSE )

textsize <- 7
ggtextsize <- textsize * (7/15)

mytheme <-  theme_classic() +
  theme(
	line=element_line(size=2),
	text=element_text(size=textsize),
	axis.text=element_text(size=textsize),
	legend.position=c(0,0),
	legend.justification = c(0,0),
	legend.title=element_blank(),
	legend.background = element_rect( fill = NA ),
	legend.key.height = unit(3, 'mm'), 
	legend.key.width = unit(4, 'mm'), 
	axis.line.x=element_line(size=0.25),
	axis.line.y=element_line(size=0.25),
	axis.ticks=element_line(size=0.25),
	strip.text = element_text(size=textsize),
	strip.background = element_rect(fill=NA,color=NA),
	plot.margin = unit(c(0.3,0.5,0.3,0.3),"mm")#trbl
  )		


