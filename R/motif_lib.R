load_lib <- function() {
  library(TSMining)
  library(ggplot2)
  library(foreach)  
}

PlotSTMotifs <- function(motifs, dataset) {
  table = NULL
  colfunc = colorRampPalette(c("darkblue", "lightblue"))
  colors = colfunc(length(motifs))

  for (i in (1:length(motifs))) {
    motif <- motifs[[i]]
    vecst = motif$vecst
    vecst$isaxcod = motif$isaxcod
    vecst$color = colors[i]
    vecst$pcolor = i
    if (is.null(table)) {
      table = vecst
    }
    else {
      table = rbind(table,vecst)
    }
  }
  table$colorfactor = as.factor(table$color)
  colorlevels = levels(table$colorfactor)
  colornlevels = c(1:length(motifs))
  with(table,plot(
    s,t,col=colorlevels[colorfactor],
    xlim = c(0, ncol(dataset)),
    ylim = c(nrow(dataset),0)))
  return(table)
}
