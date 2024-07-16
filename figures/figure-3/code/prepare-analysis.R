

flipbits <- function( x, i ){
	for( j in i ){
		substr(x,j,j) <- as.character(1-as.numeric(substr(x,j,j)))
	}
	x
}

gen <- function( center="00000000", N = 10, mut=0.999 ){
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
	command <- paste0( "./possel_train -a binary.txt -l 8 -n ",length(train)," -t 5" )
	read.table(text=system( command, input=c(train,test),intern=TRUE), sep=",")
}

getp <- function( ntrain=1000, ntest=100 ){
	x <- dopsel( gen("00000000",N=ntrain) , c(gen("00000000",N=ntest),gen("11111111",N=ntest)))
	#print(x)
	c(
		wilcox.test( x$V1[1:ntest], x$V1[(ntest+1):(2*ntest)] )$statistic,
		wilcox.test( x$V2[1:ntest], x$V2[(ntest+1):(2*ntest)] )$statistic
	) / ntest / ntest
}

x <- 3^(1:8)

if(T){
M <- sapply( x, function(x){
		cat(x,"\n")
		xx <- replicate(10,getp(x))
		rmx <- rowMeans(xx)
		sdx <- apply( xx, 1, function(x)sd(x)/sqrt(length(x)) )
	 	c( rmx , rmx-sdx, rmx+sdx ) #  quantile(y,c(0.025,0.975))) )
	}
)

colnames(M) <- x

saveRDS( M, file="data/analysis.rds" )
}
