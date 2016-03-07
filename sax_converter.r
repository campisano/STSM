dep = "TSMining"; if (!require(dep, character.only = TRUE, quietly = TRUE)) { install.packages(dep); library(dep, character.only = TRUE); }; rm(dep);

input_data = read.table(file = "100.csv", header = TRUE, fill = TRUE, as.is = TRUE, stringsAsFactors = FALSE, sep = " ", quote = "");
input_data = subset(input_data, select = -c(X1));
input_data = subset(input_data, select = -c(X2));
input_data = subset(input_data, select = -c(X3));
input_data = subset(input_data, select = -c(X4));
input_data = subset(input_data, select = -c(X5));
input_data = subset(input_data, select = -c(X6));
input_data = subset(input_data, select = -c(X7));
input_data = subset(input_data, select = -c(X8));
input_data = subset(input_data, select = -c(X9));
input_data = subset(input_data, select = -c(X10));
input_data = subset(input_data, select = -c(X11));
input_data = subset(input_data, select = -c(X12));

data_sax = data.frame(matrix(ncol = ncol(input_data), nrow = nrow(input_data)));
colnames(data_sax) = colnames(input_data);

for (col in 1 : ncol(input_data))
{
    data_sax[[col]] = Func.SAX(x = input_data[[col]], w = nrow(input_data), a = 26, eps = .01, norm = TRUE);
}

transposed_data = as.data.frame(t(data_sax), row.names = FALSE);

write.table(x = transposed_data, file = "100_transposed_sax.csv", col.names = TRUE, row.names = FALSE, sep = ",", quote = FALSE);
