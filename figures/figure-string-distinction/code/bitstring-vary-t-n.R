library( dplyr, warn.conflict = FALSE )
library( tidyr )

argv <- commandArgs( trailingOnly = TRUE )
outfile <- argv[1]

nsim <- 20
nvec <- 3^(1:8)
tvec <- 1:8


flipbits <- function( x, i ){
	for( j in i ){
		substr(x,j,j) <- as.character(1-as.numeric(substr(x,j,j)))
	}
	x
}

gen <- function( center="00000", N = 10, mut=0.6 ){
	l <- nchar(center)
	nflips <- rgeom( N, 1-mut )
	nflips[nflips > l] <- l
	r <- rep(center,N)
	for( i in seq_len(N) ){
		if( nflips[i] > 0 ){
			r[i] <- flipbits( r[i], sample( l, nflips[i] ) )
		}
	}
	r
}

dopsel <- function( train, test, thresh = 5 ){
	command <- paste0( "../../wfsms/targets/ratfa_possel 0 2 ",thresh," ",length(train))
	read.table(text=system( command, input=c(train,test),intern=TRUE), sep=",")
}

getp <- function( ntrain=1000, ntest=100, mut = 0.6, thresh = 5 ){
	x <- dopsel( gen("00000000",N=ntrain, mut = mut) , 
		c(gen("00000000",N=ntest, mut = mut),gen("11111111",N=ntest, mut = mut)), thresh )
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

saveRDS( data1, file = outfile )



