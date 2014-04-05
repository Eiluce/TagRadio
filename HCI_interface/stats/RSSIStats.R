options(echo=TRUE)
args<-commandArgs(trailingOnly = TRUE)

resultFile=args[1]
figureIndice=args[2]
figureName=paste("./stats/figures/Figure", figureIndice, ".png", sep="")
figureInfoName=paste("./stats/figures/Figure", figureIndice, ".info", sep="")

rssi <- scan(resultFile, sep="\n");
rssi2 <- rssi^2
rssiMean <- mean(rssi)
rssiVar <- mean(rssi2) - mean(rssi)^2
rssiSd <- sqrt(rssiVar)
rssiRVar <- var(rssi)
rssiRSd <- sd(rssi)

infoFile <- file(figureInfoName, open="w")
cat("Simulation parameters :\n", file=infoFile)
cat(" * Range : ", args[5], "m\n", file=infoFile)
cat(" * Scan interval : ", args[3], "ms\n", file=infoFile)
cat(" * Scan window : ", args[4], "ms\n\n", file=infoFile)
cat("Statistics results :\n", file=infoFile)
cat(" * Mean :", rssiMean, "\n", file=infoFile)
cat(" * Variance :", rssiVar, "\n", file=infoFile)
cat(" * Standard Deviation :", rssiSd, "\n", file=infoFile)
cat(" * Variance (R) :", rssiRVar, "\n", file=infoFile)
cat(" * Standard Deviation (R) :", rssiRSd, "\n", file=infoFile)
cat("\n\n", file = infoFile)
cat("Obtained with the following sample : \n", rssi, file=infoFile)
close(infoFile)

png(figureName, width=1000);
  par(mfcol=c(1,2))
  hist(rssi)
  plot(density(rssi), col="red");
dev.off()