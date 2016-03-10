# loading dependences
dep = "TSMining"; if (!require(dep, character.only = TRUE, quietly = TRUE)) {
    install.packages(dep); library(dep, character.only = TRUE);
}; rm(dep);

# reading input data
input_data = read.table(
    file = "../data/100.csv",
    header = TRUE,
    fill = TRUE,
    as.is = TRUE,
    stringsAsFactors = FALSE,
    sep = " ",
    quote = "");

# sampling columns, for speedup test only
# input_data = t(input_data);
# input_data = input_data[
#     sample(nrow(input_data),replace=F,size=0.1*nrow(input_data)),];
# input_data = t(input_data)

# merging dataframe rows in a vector
# the translate here is needed to join the matrix as vector
# and to convert it back with the same orientation, without translation
vector_data = as.vector(t(input_data));

# applying SAX
sax_data = Func.SAX(
    x = vector_data,
    w = length(vector_data),
    a = 26,
    eps = .01,
    norm = TRUE);

# test data for debugging
#input_data = data.frame(
#    x = 1, y = 1:10, fac = sample(91:93, 10, replace = TRUE))
#output_data = as.vector(t(input_data));
#input_data == output_data

# splitting the vector back in a dataframe
# it need a translated vector, you can test that using the code above
output_data = data.frame(do.call(rbind, split(
    sax_data,
    ceiling(seq_along(sax_data) / ncol(input_data)))))

# writing to disk
write.table(
    x = output_data,
    file = "../data/100_sax.csv",
    col.names = TRUE,
    row.names = FALSE,
    sep = ",",
    quote = FALSE);

