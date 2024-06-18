library( dplyr, warn.conflict = FALSE )
library( tidyr )

argv <- commandArgs( trailingOnly = TRUE )
outfile <- argv[1]

nsim <- 20
nvec <- 3^(1:8)
n0 <- 250
muvec <- c( seq(0,.75,by=0.05) )


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

dopsel <- function( train, test ){
	command <- paste0( "../../wfsms/targets/ratfa_possel 0 2 5 ",length(train) )
	read.table(text=system( command, input=c(train,test),intern=TRUE), sep=",")
}

getp <- function( ntrain=1000, ntest=100, mut = 0.6 ){
	x <- dopsel( gen("00000000",N=ntrain, mut = mut) , 
		c(gen("00000000",N=ntest, mut = mut),gen("11111111",N=ntest, mut = mut)))
	c(
		wilcox.test( x$V1[1:ntest], x$V1[(ntest+1):(2*ntest)] )$statistic,
		wilcox.test( x$V2[1:ntest], x$V2[(ntest+1):(2*ntest)] )$statistic
	) / ntest / ntest
}


getReplicates <- function( ntrain, nrep, mut = 0.6 ){
	
	message( paste0( "...",nrep," replicates with train size ", ntrain, " and mu = ", mut, " ... " ) )
	xx <- replicate( nrep, getp( ntrain ,mut = mut ) )
	xx <- as.data.frame( t( xx ) )
	colnames(xx) <- c( "unweighted", "weighted" )
	xx$ntrain <- ntrain
	xx$mut <- mut
	xx$sim <- 1:nrow(xx)
	xx <- xx %>% pivot_longer( names_to = "fsa", values_to = "auc", cols = 1:2 ) 
	return(xx)
	
}

sem <- function(x){
	return( sd(x) / sqrt( length(x) ) )
}

data1 <- bind_rows( lapply( nvec, getReplicates, nsim ) ) 

data2 <- bind_rows( lapply( muvec, function(mu) getReplicates(n0, nsim, mu ) ) ) 

out <- list( vary_n = data1, vary_mu = data2 )

saveRDS( out, file = outfile )



