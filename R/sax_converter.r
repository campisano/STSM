# configure detailed error outputs
options(warn=2, keep.source=TRUE);

# loading dependences
repos = "http://cran.r-project.org";
lib = paste(Sys.getenv("HOME"), "R", "library", sep="/");
dir.create(lib, showWarnings=FALSE, recursive=TRUE, mode="0777");
.libPaths(c(lib, .libPaths()));

dep = "TSMining"; if (!require(dep, character.only=TRUE, quietly=TRUE)) {
    install.packages(dep, repos=repos, lib=lib);
    library(dep, character.only=TRUE);
}; rm(dep);



# reading input data
input_data = read.table(
    file="data/100.csv", header=TRUE, fill=TRUE, as.is=TRUE,
    stringsAsFactors=FALSE, sep=" ", quote=""
);

# sampling columns, for speedup test only
# input_data = t(input_data);
# input_data = input_data[
#     sample(nrow(input_data),replace=F,size=0.1*nrow(input_data)),];
# input_data = t(input_data)

# merging dataframe rows in a vector
# the translate here is needed to join the matrix as vector
# and to convert it back with the same orientation, without translation
vector_data = as.vector(t(input_data));

for(alphabet in c(5, 10, 15, 20, 25))
{
    # applying SAX
    sax_data = Func.SAX(
        x=vector_data, w=length(vector_data), a=alphabet, eps=.01, norm=TRUE
    );

    # test data for debugging
    #input_data = data.frame(
    #    x = 1, y = 1:10, fac = sample(91:93, 10, replace = TRUE))
    #output_data = as.vector(t(input_data));
    #input_data == output_data

    # splitting the vector back in a dataframe
    # it need a translated vector, you can test that using the code above
    output_data = data.frame(
        do.call(rbind, split(sax_data, ceiling(
            seq_along(sax_data) / ncol(input_data)
        )))
    );

    for(type in c("original", "transposed"))
    {
        if(type == "original")
        {
            typed_output_data = output_data;
        }
        else if(type == "transposed")
        {
            typed_output_data = data.frame(t(output_data));
        }
        else
        {
            print("bad type, skipped");
            next;
        }

        # writing to disk
        write.table(
            x=typed_output_data, file=paste(
                "data/100_sax-", alphabet, "_", type, ".csv", sep=""),
            col.names=TRUE, row.names=FALSE, sep=",", quote=FALSE
        );

        # filtred_output_data = typed_output_data[13:ncol(typed_output_data)];
        #
        # writing to disk
        # write.table(
        #     x=filtred_output_data,
        #     file=paste(
        #         "data/100_sax-", alphabet, "_", type, "_filtred", ".csv",
        #         sep=""),
        #     col.names=TRUE, row.names=FALSE, sep=",", quote=FALSE
        # );
    }
}
