# configure detailed error outputs
options(warn=2, keep.source=TRUE);

# loading dependences
repos = "http://cran.r-project.org";
lib = paste(Sys.getenv("HOME"), "R", "library", sep="/");
dir.create(lib, showWarnings=FALSE, recursive=TRUE, mode="0777");
.libPaths(c(lib, .libPaths()));

dep = "rjson"; if (!require(dep, character.only=TRUE, quietly=TRUE)) {
    install.packages(dep, repos=repos, lib=lib);
    library(dep, character.only=TRUE);
}; rm(dep);
rm(lib);



# evaluating arguments
args = commandArgs(TRUE);
#print("Arguments:");
#print(args);
# examples:
#args = c();
#args[1] = "results/25_original/json/401_sax-25_original_s80-100_g10.json";
#$MIN_SUPPORT
#$MAX_SUPPORT
#$MAX_TIME_WINDOW


# configuring variables
input_file_json = args[1];
base_json_path_dir = dirname(input_file_json);
base_path_dir = dirname(base_json_path_dir);
base_filename = sub(file.path(base_json_path_dir, ""), "", input_file_json);
base_filename =  sub("[.][^.]*$", "", base_filename, perl=TRUE);

support_range = paste(args[2], args[3], sep="-");
time_window = args[4];

dir.create(
    file.path(base_path_dir, "stackedbar", "time_window", time_window),
    showWarnings=FALSE, recursive=TRUE, mode="0777");

dir.create(
    file.path(base_path_dir, "stackedbar", "support_range", support_range),
    showWarnings=FALSE, recursive=TRUE, mode="0777");

output_file_time = file.path(
    base_path_dir, "stackedbar", "time_window", time_window,
    paste(base_filename, ".csv", sep=""))

output_file_support = file.path(
    base_path_dir, "stackedbar", "support_range", support_range,
    paste(base_filename, ".csv", sep=""))

if(file.exists(output_file_time))
{
    file.remove(output_file_time)
}

if(file.exists(output_file_support))
{
    file.remove(output_file_support)
}


# loading json data
#print(paste("Loading json data", input_file_json, "..."));
json_data = fromJSON(file=input_file_json, method="C");
#print("Load complete")



if(is.null(json_data) || length(json_data) < 1)
{
    print("Empty json data");
} else {
    for(len in 1:length(json_data))
    {
        sequence_data_by_length = json_data[[len]];

        if(
            is.null(sequence_data_by_length$length) ||
            is.null(sequence_data_by_length$sequences) ||
            length(sequence_data_by_length$sequences) < 1
        )
        {
            print(paste("Empty sequence data of len", len));
            next;
        }

        sequence_length = sequence_data_by_length$length;
        sequence_data = sequence_data_by_length$sequences;          # c++ pair

        # Stacked Bar plot per time_window:
        # support_range, num_sequences, seq_length
        write.table(
            as.matrix(t(c(
                support_range, length(sequence_data), sequence_length))),
            file=output_file_time,
            append=TRUE,
            row.names=FALSE,
            col.names=FALSE,
            quote=FALSE,
            sep=",")

        # Stacked Bar plot per support_range:
        # time_window, num_sequences, seq_length
        write.table(
            as.matrix(t(c(
                time_window, length(sequence_data), sequence_length))),
            file=output_file_support,
            append=TRUE,
            row.names=FALSE,
            col.names=FALSE,
            quote=FALSE,
            sep=",")
    }
}
