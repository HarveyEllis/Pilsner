#### Example code ####
# This is an example as instructed by Pete Dodd in original repo.

library(Rcpp) # assuming in dir above src
sourceCpp('src/pilsner.cpp') #compiles: only needed once per session

NN <- 1e4 #cohort size
AM <- matrix(15,ncol=1,nrow=NN); colnames(AM) <- "age" # age data <0 will not be advanced
CM <- matrix(0,ncol=2,nrow=NN); colnames(CM) <- c("timeofdeath","timeofinfection")
DM <- matrix(1,ncol=2,nrow=NN); colnames(DM) <- c("alive","infected")
DM[,2] <- 0 #start uninfected

## parameter examples
PP <- data.frame(mu=rep(.05,NN),lambda=rep(0.01,NN))
df2l <- function(x) lapply(split(x,1:nrow(x)),unlist) #fn for converting dataframe
PP0 <- list(unlist(PP[1,])) # take the top row as a parameter input
PPL <- df2l(PP) # split the whole dataframe into a list with separate parameter for everyone

out <- simulator(AM,CM,DM,100,PPL,recording=FALSE) #returns by side effect

library(ggplot2)  # plotting library 
P <- ggplot(as.data.frame(cbind(AM,DM)),
            aes(x=age,fill=factor(infected))) +
  geom_histogram() + theme_classic()
print(P)
