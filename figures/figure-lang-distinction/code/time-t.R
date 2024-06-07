library( dplyr, warn.conflict = FALSE )
library( tidyr )


nsim <- 3
nvec <- c(1:10,15,20,30)
tvec <- 1:6


argv <- commandArgs( trailingOnly = TRUE )

traindata <- argv[1]
testself <- argv[2]
testnonself <- argv[3]
outfile <- argv[4]


trainset <- read.table( traindata )$V1
testset1 <- read.table( testself )$V1
testset2 <- read.table( testnonself )$V1


makeTrain <- function( strings, N ){
	return( sample( strings, N, replace = FALSE ) )
}

makeTest <- function( strings1, strings2, N ){
	self <- sample( strings1, N, replace = FALSE )
	nonself <- sample( strings2, N, replace = FALSE )
	return( c( self, nonself ) ) 
}

dopsel <- function( train, test, thresh = 5 ){
	command <- paste0( "./possel_train -a lang.txt -l 6 -n ",length(train)," -t ", thresh )
	read.table(text=system( command, input=c(train,test),intern=TRUE), sep=",")
}

getp <- function( ntrain=500, ntest=100, thresh = 5 ){
	x <- system.time( dopsel( makeTrain( trainset, N=ntrain ) , 
		makeTest( testset1, testset2 ,N=ntest ), thresh ) )
	return( x[["user.self"]] )
}


getReplicates <- function( ntrain, nrep, thresh = 5 ){
	
	message( paste0( "...",nrep," replicates with train size ", ntrain, " and t = ", thresh, " ... " ) )
	xx <- replicate( nrep, getp( ntrain , thresh = thresh ) )
	print(xx)
	df <- data.frame( usr = xx, sim = 1:length(xx), ntrain = ntrain, thresh = thresh )
	return(df)
	
}

parms <- expand.grid( n = nvec, thresh = tvec )

data1 <- bind_rows( apply( parms, 1, function(p) getReplicates( p[1], nsim, p[2] ) ) ) 

saveRDS( data1, file = outfile )



