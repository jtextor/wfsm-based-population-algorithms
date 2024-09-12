
# to be called from its parent folder

source("../settings.R")

M <- readRDS( "data/analysis.rds" )

x <- as.integer( colnames(M) )

pdf_out( "plots/analysis.pdf", width=3.33, height=2 )
par( mar=c(4,4,0,0) )

matplot( x,t(M[1:2,]), type='l', lty=1, bty='l', ylab='AUC', ylim=c(0.5,1), xlab="training samples",
	yaxt="n", log="x" )
axis( 2, las=2 )
legend("bottomleft", lty=1, col=1:2, c("unweighted","weighted"), bty="n" )
polygon( c(x,rev(x)), c(M[3,],rev(M[5,])), col=rgb(0,0,0,.1), border=NA )
polygon( c(x,rev(x)), c(M[4,],rev(M[6,])), col=rgb(1,0,0,.1), border=NA )

dev.off()
