dep = "TSMining"; if (!require(dep, character.only = TRUE, quietly = TRUE)) { install.packages(dep); library(dep, character.only = TRUE); }; rm(dep);

input_data = read.table(file = "reduced_data.csv", header = TRUE, fill = TRUE, as.is = TRUE, stringsAsFactors = FALSE, sep = ",", quote = "");
input_data = subset(input_data, select = -c(X..));
input_data = subset(input_data, select = -c(X.V1.));
input_data = subset(input_data, select = -c(X.V2.));
input_data = subset(input_data, select = -c(X.V3.));
input_data = subset(input_data, select = -c(X.V4.));
input_data = subset(input_data, select = -c(X.V5.));
input_data = subset(input_data, select = -c(X.V6.));
input_data = subset(input_data, select = -c(X.V7.));
input_data = subset(input_data, select = -c(X.V8.));
input_data = subset(input_data, select = -c(X.V9.));
input_data = subset(input_data, select = -c(X.V10.));
input_data = subset(input_data, select = -c(X.V11.));
input_data = subset(input_data, select = -c(X.V12.));
input_data = subset(input_data, select = -c(X.V13.));

data_sax = data.frame(matrix(ncol = ncol(input_data), nrow = nrow(input_data)));
colnames(data_sax) = colnames(input_data);

for (col in 1 : ncol(input_data))
{
    data_sax[[col]] = Func.SAX(x = input_data[[col]], w = nrow(input_data), a = 26, eps = .01, norm = TRUE);
}

transposed_data = as.data.frame(t(data_sax), row.names = FALSE);

write.table(x = transposed_data, file = "reduced_data_transposed_sax.csv", col.names = TRUE, row.names = FALSE, sep = ",", quote = FALSE);
