# configure detailed error outputs
options(warn=2, keep.source=TRUE);

# loading dependences
repos = "http://cran.r-project.org";
lib = paste(Sys.getenv("HOME"), "R", "library", sep="/");
dir.create(lib, showWarnings=FALSE, recursive=TRUE, mode="0777");
.libPaths(c(lib, .libPaths()));

dep = "ggplot2"; if (!require(dep, character.only=TRUE, quietly=TRUE)) {
    install.packages(dep, repos=repos, lib=lib);
    library(dep, character.only=TRUE);
}; rm(dep);

dep = "plyr"; if (!require(dep, character.only=TRUE, quietly=TRUE)) {
    install.packages(dep, repos=repos, lib=lib);
    library(dep, character.only=TRUE);
}; rm(dep);

dep = "scales"; if (!require(dep, character.only=TRUE, quietly=TRUE)) {
    install.packages(dep, repos=repos, lib=lib);
    library(dep, character.only=TRUE);
}; rm(dep);

rm(lib, repos);




# evaluating arguments
args = commandArgs(TRUE);
#print("Arguments:");
#print(args);
# examples:
#args = c();
#args[1] = "results/25_original/stackedbar/support_range/80-100";
#args[2] = 80-100


# configuring variables
base_path_dir = args[1];
support_range = args[2]
rm(args);

base_super_path_dir = dirname(base_path_dir);
base_filename = sub(file.path(base_super_path_dir, ""), "", base_path_dir);



# reading input data
input_data = NULL;
file_names = dir(base_path_dir, pattern =".csv");

for(i in 1:length(file_names))
{
    input_data = rbind(input_data, read.table(
        file = file.path(base_path_dir, file_names[i]),
        header = FALSE, fill = TRUE, as.is = TRUE,
        stringsAsFactors = FALSE, sep=",", quote = ""));
}

rm(i, file_names);

colnames(input_data) = c("time_window", "num_sequences", "seq_length");



# sort by time_window and seq_length
input_data_sorted = arrange(input_data, time_window, desc(seq_length));



# add y label positions [CMP] does not work well with scale log
input_data_cumsum = ddply(
    input_data_sorted, "time_window", transform,
    label_ypos=cumsum(num_sequences) - 0.5 * num_sequences);



# transform data
bins = as.character(sort(unique(input_data_cumsum$time_window)));
legend = as.character(sort(unique(input_data_cumsum$seq_length)));
input_data_cumsum$time_window = as.character(input_data_cumsum$time_window);
input_data_cumsum$seq_length = as.character(input_data_cumsum$seq_length);



# plot
png(
    file.path(base_super_path_dir, paste(base_filename, ".png", sep="")),
    bg="transparent");

ggplot() +
    theme_bw() +
    geom_bar(
        data = input_data_cumsum,
        aes(
            x = time_window, y = num_sequences, fill = seq_length
        ),
        stat="identity",
        position = "stack"
    ) +
    geom_text(
        data = input_data_cumsum,
        aes(
            x = time_window, y = label_ypos, label = num_sequences
        ),
    ) +
    scale_x_discrete(breaks=bins, limits=bins) +
    scale_y_log10(labels=trans_format('log10',math_format(10^.x))) +
    scale_fill_discrete(
        breaks = legend, limits = legend, guide = guide_legend()) +
    labs(
        title = paste("Support Range:", support_range),
        y = "Num of Sequences",
        x = "Time Window",
        fill = "Sequence Size"
    );

dev.off();
