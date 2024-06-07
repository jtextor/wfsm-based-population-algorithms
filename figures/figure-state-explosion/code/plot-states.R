

source("../settings.R")

pdf_out( "plots/states.pdf", width=3.33, height=2.2 )
par( mar=c(4,4,0.2,0), mgp=default.mgp )

d <- read.csv("data/without_push_quantize/real__6_6.csv.gz")

plot( NA, xlab="strings", ylab="", yaxt="n", 
	xlim=c(0,3^6), ylim=c(0,350), pch=19, bty="l" )

addp <- function(f="without_push_quantize/real-encode", col=2){
	d <- read.csv(paste0("data/",f,".csv.gz"))
	nw <- sort(unique(d$nwords))
	xx <- by( d$nnodes+d$nedges, d$nwords, median )
	lines( loess.smooth( nw, xx ), col=col )
	points( d$nwords, d$nnodes+d$nedges, col=adjustcolor(col,alpha.f=.4), pch=19 )
}

addp("without_push_quantize/real__6_6",col="black")

#addp()
addp("without_push_quantize/tpl__6_6",col="blue")
#addp("without_push_quantize/sgl__6_6",col=4)

#addp("with_push_quantize/tpl_6_6",col=4)
#addp("with_push_quantize/sgl_6_6",col=5)
addp("with_push_quantize/rational_6_6",col="red")


mtext( "FSM size (|V|+|E|)", 2, line=3 )

axis( 2, las=2 )

legend( "topleft", c("reals","rationals","unweighted"), lty=1, col=c(1,2,4), bty="n" )

dev.off()


