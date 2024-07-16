library( dplyr, warn.conflict = FALSE )
library( tidyr )


nsim <- 20
nvec <- c(3^(1:9), 50000)
l <- 3
tvec <- 2:3



argv <- commandArgs( trailingOnly = TRUE )

traindata <- argv[1]
testself <- argv[2]
testnonself <- argv[3]
lname <- argv[4]
outfile <- argv[5]


getNmers <- function( f, len ){
	d <- read.table( f )$V1
	return( substr( d, 1, len ) )
}

trainset <- getNmers( traindata, l )
testset1 <- getNmers( testself, l )
testset2 <- getNmers( testnonself , l )

makeTrain <- function( strings, N ){
	return( sample( strings, N, replace = FALSE ) )
}

makeTest <- function( strings1, strings2, N ){
	self <- sample( strings1, N, replace = FALSE )
	nonself <- sample( strings2, N, replace = FALSE )
	return( c( self, nonself ) ) 
}

dopsel <- function( train, test, thresh = 5, len = l ){
	command <- paste0( "../../wfsms/targets/ratfa_possel _ 28 ", thresh, " ", length(train) )
	read.table(text=system( command, input=c(train,test),intern=TRUE), sep=",")
}

getp <- function( ntrain=500, ntest=100, thresh = 5 ){
	x <- dopsel( makeTrain( trainset, N=ntrain ) , makeTest( testset1, testset2 ,N=ntest ), thresh )
	c(
		wilcox.test( x$V1[1:ntest], x$V1[(ntest+1):(2*ntest)] )$statistic,
		wilcox.test( x$V2[1:ntest], x$V2[(ntest+1):(2*ntest)] )$statistic
	) / ntest / ntest
}


getReplicates <- function( ntrain, nrep, thresh = 5 ){
	
	message( paste0( "...",nrep," replicates with train size ", ntrain, " and t = ", thresh, " ... " ) )
	xx <- replicate( nrep, getp( ntrain , thresh = thresh ) )
	xx <- as.data.frame( t( xx ) )
	colnames(xx) <- c( "unweighted", "weighted" )
	xx$ntrain <- ntrain
	xx$thresh <- thresh
	xx$sim <- 1:nrow(xx)
	xx <- xx %>% pivot_longer( names_to = "fsa", values_to = "auc", cols = 1:2 ) 
	return(xx)
	
}

parms <- expand.grid( n = nvec, thresh = tvec )

data1 <- bind_rows( apply( parms, 1, function(p) getReplicates( p[1], nsim, p[2] ) ) ) 
data1$lang <- lname

saveRDS( data1, file = outfile )



