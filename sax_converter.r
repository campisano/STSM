dep = "TSMining"; if (!require(dep, character.only = TRUE, quietly = TRUE)) { install.packages(dep); library(dep, character.only = TRUE); }; rm(dep);

input_data = read.table(file = "100.csv", header = TRUE, fill = TRUE, as.is = TRUE, stringsAsFactors = FALSE, sep = " ", quote = "");

inv_transposed_data = as.data.frame(t(input_data), row.names = FALSE);

data_sax = data.frame(matrix(ncol = ncol(inv_transposed_data), nrow = nrow(inv_transposed_data)));
colnames(data_sax) = colnames(inv_transposed_data);

for (col in 1 : ncol(inv_transposed_data))
{
    data_sax[[col]] = Func.SAX(x = inv_transposed_data[[col]], w = nrow(inv_transposed_data), a = 26, eps = .01, norm = TRUE);
}

transposed_data = as.data.frame(t(data_sax), row.names = FALSE);

write.table(x = transposed_data, file = "100_transposed_sax.csv", col.names = TRUE, row.names = FALSE, sep = ",", quote = FALSE);
