dep = "TSMining"; if (!require(dep, character.only = TRUE, quietly = TRUE)) { install.packages(dep); library(dep, character.only = TRUE); }; rm(dep);

data = read.table(file = "reduced_data.csv", header = TRUE, fill = TRUE, sep = ",", quote="")
data = subset(data, select = -c(X..))  #TODO [CMP] to remove in the future

data_sax = data.frame(matrix(ncol = ncol(data), nrow = nrow(data)))
colnames(data_sax) = colnames(data)
for (col in 1 : ncol(data))
{
    data_sax[[col]] = Func.SAX(x = data[[col]], w = nrow(data), a = 26, eps = .01, norm = TRUE)
}

write.table(x = data_sax, file = "reduced_data_sax.csv", row.names=FALSE, sep = ",", quote=FALSE)
