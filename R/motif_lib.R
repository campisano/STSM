load_lib <- function() {
  library(TSMining)
  library(ggplot2)
  library(foreach)  
}

STSAdjustBlock  <- function(dataset, tslice, sslice) {
  c = ncol(dataset)
  r = nrow(dataset)
  ec = c %% sslice
  er = r %% tslice
  dataset = dataset[1:(r-er), 1:(c-ec)]  
  return (dataset)
}

#Compute blocks
STSComputeBlocks <- function(dataset, tslice, sslice) {
  datasets <- list()
  rectangles <- list()
  
  c = ncol(dataset)
  r = nrow(dataset)
  nc = c / sslice
  nr = r / tslice
  i = 1
  j = 1
  n = 1
  for (i in 1:nc) {
    sc = (i-1)*sslice + 1
    ec = (i)*sslice
    for (j in 1:nr) {
      sr = (j-1)*tslice + 1
      er = (j)*tslice
      ds = dataset[sr:er, sc:ec]  
      datasets[[n]] = ds
      rect = c(sS = sc, eS = ec, sT = sr, eT = er, nr = j, nc = i)
      rectangles[[n]] = rect
      n = n + 1
    }
  }
  blocks = list(datasets = datasets, nrows = nr, ncols = nc, rectangles = rectangles)
  return (blocks)
}

STSSaxEncode <- function(dataset, a, eps=0.1, norm = FALSE) {
  c = ncol(dataset)
  n = nrow(dataset)
  vector = as.matrix(dataset)
  vector = as.vector(vector)
  saxvector = Func.SAX(x = vector, w = n*c, a = a, eps = eps, norm = norm)    
  saxvector = matrix(saxvector, nrow = n, ncol = c)
  saxvector = data.frame(saxvector)
  colnames(saxvector) = colnames(dataset)
  return(saxvector)
}

#Identify Motifs
STSIdentifyCandidateSTMotifs <- function(dataset, tslide, sslide, alpha, mask_size) {
  #Build blocks
  dataset = STSAdjustBlock(dataset, tslice, sslice)
  blocks <- STSComputeBlocks(dataset, tslice, sslice)
  nrows = blocks$nrows
  ncols = blocks$ncols
  rectangles = blocks$rectangles
  blocks$rectangles <- NULL
  
  saxdataset <- STSSaxEncode(dataset, a = alpha)
  saxblocks <- STSComputeBlocks(saxdataset, tslice, sslice)
  saxblocks$rectangles <- NULL
  
  #Identify Motifs
  motifs<-list()
  size=length(blocks$datasets)
  for (i in 1:(length(blocks$datasets))) {
    block = blocks$datasets[[i]]
    saxblock = saxblocks$datasets[[i]]
    block = as.vector(as.matrix(block))
    saxblock = as.vector(as.matrix(saxblock))
    
    motifs[[i]] <- identifyMotifsInBlock(block, saxblock, window_size, alpha, mask_size)
    
    message(i, "/", size," - ", i/size*100,"%")
  }
  candidates = list(motifs = motifs, nrows = nrows, ncols = ncols, rectangles = rectangles, blocks = blocks, saxblocks = saxblocks)
  return (candidates)
}


identifyMotifsInBlock <- function(block, saxblock, window_size, alpha, mask_size) {
  res <- tryCatch(
    {
      STS.Func.motif(ts = block, tss = saxblock, window.size = window_size, a = alpha, mask.size = mask_size)
    },
    error=function(cond) {
      message(cond)
      return(NA)
    },
    warning=function(cond) {
      message(cond)
      return(NA)
    },
    finally={
    }
  )
  return(res)
}


STS.Func.motif <- function(ts, tss, window.size, a, mask.size, eps=0.1, iter=25, max.dist.ratio=1.2, count.ratio.1=1.5, count.ratio.2=1.2) {
    i=j=q=m=k=l=g=h=u=NULL

    overlap = 0
    w = window.size

    #Create the subsequence data frame
    b <- ifelse(overlap==1, yes = 1, no = round((1-overlap)*window.size, digits = 0))
    ts.subs <- foreach::foreach(i=seq(from = 1, to = length(ts), by = b), .combine = rbind) %do% {
        c(i,subs.temp <- ts[i:(i+window.size-1)])
    }
    ts.subs <- na.omit(ts.subs)
    
    ts.sax <- foreach::foreach(i=seq(from = 1, to = length(tss), by = b), .combine = rbind) %do% {
      c(i,sax.temp <- tss[i:(i+window.size-1)])
    }
    ts.sax <- na.omit(ts.sax)

    ts.sax <- as.data.frame(ts.sax, stringsAsFactors = FALSE)
    colnames(ts.sax) <- c("StartP", 1:w)
    ts.sax$StartP <- as.numeric(ts.sax$StartP)
    
    #Perform the random projection
    col.mat <- matrix(data = 0, nrow = dim(ts.sax)[1], ncol = dim(ts.sax)[1])
    for(i in 1:iter) {
        col.pos <- sort(sample(x = 2:dim(ts.sax)[2], size = mask.size, replace = F), decreasing = F)
        sax.mask <- ts.sax[,col.pos]
        
        unique.lab <- unique(sax.mask)
        
        mat <- foreach::foreach(j = 1:dim(unique.lab)[1], .combine = rbind) %do% {
            indices <- foreach::foreach(q = 1:dim(sax.mask)[1], .combine = c) %do% {
                identical(as.character(sax.mask[q,]), as.character(unique.lab[j,]))
            }
        }
        
        for(m in 1:dim(mat)[1]) {
            if(length(which(mat[m,]==TRUE))>1) {
                com <- t(combn(x = which(mat[m,]==TRUE), m = 2))
                col.mat[com] <- col.mat[com] + 1    
            }
        }   
    }
    
    #Extact the tentative motif pair
    counts <- sort(col.mat, decreasing=TRUE)
    counts.sel <- counts[which(counts>=(iter/count.ratio.1))]
    
    motif.pair <- foreach::foreach(k = 1:length(unique(counts.sel)), .combine = rbind) %do% {
        arrayInd(which(col.mat==unique(counts.sel)[k]), .dim = dim(col.mat))    
    }
    
    indices <- foreach::foreach(l = 1:dim(motif.pair)[1]) %do% {
        pair <- c(ts.sax[motif.pair[l,1],1], ts.sax[motif.pair[l,2],1])
        
        cand.1 <- ts.subs[motif.pair[l,1],-1]
        cand.2 <- ts.subs[motif.pair[l,2],-1]
        dist.raw <- sqrt(sum((cand.1 - cand.2)^2))
        
        col.no <- col.mat[motif.pair[l,1],]
        ind.cand <- which(col.no > (max(col.no)/count.ratio.2))
        if(length(ind.cand)>1) {
            ind.temp <- ind.cand[-which(ind.cand == motif.pair[l,2])]
            if(length(ind.temp)==1) {
                df.cand.sel <- as.matrix(ts.subs[ind.temp,-1])
                dist.res <- plyr::aaply(.data = df.cand.sel, .margins = 2, .fun = function(x) sqrt(sum((cand.1-x)^2)))
                ind.final <- ts.sax[ind.temp[which(dist.res<=max.dist.ratio*dist.raw)],1]   
            } else {
                df.cand.sel <- ts.subs[ind.temp,-1]
                dist.res <- plyr::aaply(.data = df.cand.sel, .margins = 1, .fun = function(x) sqrt(sum((cand.1-x)^2)))
                ind.final <- ts.sax[ind.temp[which(dist.res<=max.dist.ratio*dist.raw)],1]  
            }} else {
                ind.final <- NULL
            }
        
        pair.final <- c(pair, ind.final)
    }
    
    #Combine the indices if there is any overlap
    vec.subset <- rep(0, length(indices))
    foreach::foreach(g = 1:(length(indices)-1), .combine = rbind) %do% {
        for (h in (g+1):length(indices)) {
            if(length(which(indices[[g]] %in% indices[[h]]))>0) {
                indices[[h]] <- unique(c(indices[[g]], indices[[h]]))
                vec.subset[g] <- 1
            }
        }
    }
    indices <- indices[vec.subset==0]
    
    motif.raw <- foreach::foreach(u = 1:length(indices)) %do% {
        ts.subs[which(ts.subs[,1] %in% indices[[u]]),]
    }
    
    motif.sax <- foreach::foreach(u = 1:length(indices)) %do% {
        ts.sax[which(ts.sax[,1] %in% indices[[u]]),]
    }
    
    return(list(Subs=ts.subs, Subs.SAX=ts.sax, Motif.raw=motif.raw, Motif.SAX=motif.sax, Collision.matrix=col.mat, Indices=indices))
}


#Build spatial-Time Motifs

STSIdentifySTMotifs <- function(candidates, ka, si) {
  stmotifs <- list()
  motifs <- candidates$motifs
  nrows = candidates$nrows
  ncols = candidates$ncols
  rectangles <- candidates$rectangles
  blocks <- candidates$blocks
  saxblocks <- candidates$saxblocks
  for (i in 1:length(motifs)){
    stmotifs <- STSIdentifySTMotif(stmotifs, motifs[[i]], nrows, ncols, rectangles[[i]], ka, si)
  }
  for (i in 1:length(stmotifs)) {
    stmotif = stmotifs[[i]]
    s = stmotif$vecs
    t = stmotif$vect
    stmotif$vecst = data.frame(s, t)
    stmotif$vecs <- NULL
    stmotif$vect <- NULL
    stmotifs[[i]] = stmotif
  }
  return(stmotifs)
}

STSIdentifySTMotif <- function(stmotifs, motif, nrows, ncols, rectangle, sigma, ka) {
  k <- length(stmotifs)
  sS = rectangle["sS"]
  eS = rectangle["eS"]
  sT = rectangle["sT"]
  eT = rectangle["eT"]
  nr = rectangle["nr"]
  nc = rectangle["nc"]
  
  recMatrix = matrix(rep(0, nrows*ncols), nrow = nrows, ncol = ncols)
  tslice <- eT - sT + 1
  sslice <- eS - sS + 1
  if(!is.na(motif$Indices)){
    for(a in 1:length(motif$Indices)) {
      vec <- motif$Indices[[a]]
      if(length(vec) >= sigma) {
        saxvec <- motif$Motif.SAX[[a]]
        scount <- rep(0, sslice)
        for(z in 1: length(vec)) {
          i <- as.integer(vec[z] / tslice) + 1
          scount[i] <- 1
        }
        count <- sum(scount)
        if(count >= ka) {
          saxcod <- saxvec[1,2:(length(motif$Subs.SAX))]
          isaxcod <- paste(saxcod, collapse = "")
          vect <- as.integer(vec %% tslice) + sT
          vecs <- as.integer(vec / tslice) + sS
          i <- match(isaxcod, names(stmotifs))
          if (is.na(i)) {
            k = k + 1
            stmotifs[[k]] <- list(saxcod=saxcod, isaxcod=isaxcod, vecs=vecs, vect=vect, recmatrix=recMatrix)
            stmotifs[[k]]$recmatrix[nr, nc] = 1
            names(stmotifs)[k] = isaxcod
          }
          else {
            list <- stmotifs[[i]]
            list$recmatrix[nr, nc] = max(list$recmatrix)+1
            list$vect <- c(list$vect, vect)
            list$vecs <- c(list$vecs, vecs)
            stmotifs[[i]] <- list
          }
        }
      }
    }
  }
    
  return (stmotifs)
}


STSIdentifyTightSTMotifs <- function(stmotifs, rectangles) {
  sttightmotifs <- list()
  for(stmotif in (stmotifs)) {
    list <- STSIdentifyTightSTMotif(stmotif, rectangles)
    for (item in (list)) {
      pos = length(sttightmotifs)+1
      sttightmotifs[[pos]] <- item
      names(sttightmotifs)[pos] = item$isaxcod
    }
  }
  return (sttightmotifs)
}

STSIdentifyTightSTMotif <- function(stmotif, rectangles) {
  tight <- list()
  mat <- stmotif$recmatrix
  vecst <- stmotif$vecst
  for (i in 1:nrow(mat)) {
    for (j in 1:ncol(mat)) {
      if (mat[i,j] != 0) {
        iA <- i - 1
        if ((iA >= 1) && (mat[iA,j] != 0)) {
          k <- min(mat[iA,j], mat[i,j])
          mat[mat == mat[iA,j] | mat == mat[i,j]] = k
        }
        iP <- i + 1
        if ((iP <= nrow(mat)) && (mat[iP,j] != 0)) {
          k <- min(mat[iP,j], mat[i,j])
          mat[mat == mat[iP,j] | mat == mat[i,j]] = k
        }
        jA <- j - 1
        if ((jA >= 1) && (mat[i,jA] != 0)) {
          k <- min(mat[i,jA], mat[i,j])
          mat[mat == mat[i,jA] | mat == mat[i,j]] = k
        }
        jP <- j + 1
        if ((jP <= ncol(mat)) && (mat[i,jP] != 0)) {
          k <- min(mat[i,jP], mat[i,j])
          mat[mat == mat[i,jP] | mat == mat[i,j]] = k
        }
      }
    }
  }
  vec <- as.vector(mat)
  vec <- vec[vec > 0]
  vec <- unique(vec)
  k <- 1
  gkappa = length(unique(vecst$s))
  gsigma = nrow(vecst)
  for (i in (vec)) {
    stmotif$recmatrix[mat != i] <- 0
    stmotif$recmatrix[mat == i] <- k
    vecrects <- as.vector(stmotif$recmatrix)
    rects <- rectangles[vecrects>0]
    stmotif$vecst <- vecst
    conds = rep(FALSE, nrow(stmotif$vecst))
    for (rect in (rects)) {
      sS = rect["sS"]
      eS = rect["eS"]
      sT = rect["sT"]
      eT = rect["eT"]
      conds = conds | (stmotif$vecst$s >= sS & stmotif$vecst$s <= eS & stmotif$vecst$t >= sT & stmotif$vecst$t <= eT)
    }
    stmotif$vecst <- stmotif$vecst[conds,]

    stmotif$kappa = length(unique(stmotif$vecst$s))
    stmotif$sigma = nrow(stmotif$vecst)
    stmotif$gkappa = gkappa 
    stmotif$gsigma = gsigma 
    gsigma = nrow(vecst)
    
    tight[[k]] <- stmotif
    k <- k + 1
  }
  return(tight)
}

STSRankTightSTMotifs <- function(sttightmotifs) {
  kappa = rep(0, length(sttightmotifs))
  gkappa = rep(0, length(sttightmotifs))
  sigma = rep(0, length(sttightmotifs))
  gsigma = rep(0, length(sttightmotifs))
  objfunc = rep(0, length(sttightmotifs))
  
  ranktbl = data.frame(kappa, gkappa, sigma, gsigma)
  for (i in (1:length(sttightmotifs))) {
    motif = sttightmotifs[[i]]
    motif$objfunc = (motif$gsigma + motif$gkappa) * 1000 + motif$sigma + motif$kappa
    ranktbl$kappa[i] = motif$kappa
    ranktbl$gkappa[i] = motif$gkappa
    ranktbl$sigma[i] = motif$sigma
    ranktbl$gsigma[i] = motif$gsigma
    ranktbl$objfunc[i] = motif$objfunc
    sttightmotifs[[i]] = motif
  }
  myrankorder = with(ranktbl, order(objfunc))
  
  myranklist <- sttightmotifs[myrankorder]  
  return(myranklist)
}

PlotSTMotifs <- function(motifs) {
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
  with(table,plot(s,t,col=colorlevels[colorfactor], ylim = c(nrow(dataset),0)))
  return(table)
}



