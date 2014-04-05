histogramme <- function(x) {
	y <- sort(x)
	k <- 5
	a <- y[1] - 0.025*(y[length(x)]-y[1])
	b <- y[length(x)] + 0.025*(y[length(x)]-y[1])
	h <- (b-a)/k
	pdf("hist.pdf")
	hist(y,prob=T,breaks=seq(a,b,h))
	lines(density(y))
	dev.off()
}

hisMEffectif <- function (x) {
	y <- sort(x)
	k <- 1 + log(length(x))
	if( k < 5 ) {
		k <- 5
	}
	a <- y[1] - 0.025*(y[length(x)]-y[1])
	b <- y[length(x)] + 0.025*(y[length(x)]-y[1])
	h <- (b-a)/k
	bornes <- c(a, quantile(y, seq(1,k-1)/k),b)
	pdf("histmeff.pdf")
	hist(y,prob=T,breaks=bornes)
	lines(density(y))
	dev.off()
}

simu <- scan("test.txt")

histogramme(simu)
hisMEffectif(simu)

pdf("graphe proba.pdf")
plot(qqnorm(simu))
dev.off()

#Estimateur méthode des moments
m <- mean(simu)
sigma <- var(simu)


