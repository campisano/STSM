# include utility file
source(file="R/utils.r")



# configure detailed error outputs
utils.verbose();



# loading dependences
loaded_libs = utils.loadLibs(c("rjson"));



# evaluating arguments
args = commandArgs(TRUE);
#cat("Arguments:\n");
#cat(args, "\n");
# examples:
#args = c();
#args[1] = "results/inline-100_orientation-original/sax-25/json/I100_Ooriginal_S25_FS100_FB100_MS0.json";
#args[2] = "results/inline-100_orientation-original/sax-25/stats";
#args[3] = 100;
#args[4] = 100;
#args[5] = 0;
#cat("    args:", args, "\n");

input_file_json = args[1];
output_stats_dir = args[2];
min_spatial_freq = args[3];
min_block_freq = args[4];
max_stretch = args[5];

base_filename = sub(
    file.path(dirname(input_file_json), ""), "", input_file_json);
base_filename = sub("[.][^.]*$", "", base_filename, perl=TRUE);



# loading json data
#cat("Loading json data", input_file_json, "...");
json_data = fromJSON(file=input_file_json, method="C");
#cat(" [DONE]\n")

if(is.null(json_data) || length(json_data) < 1) {
    cat("Empty json data\n");
    quit(status=1);
}



# # preparing output folders
# output_stats_dir = file.path(output_stats_dir, base_filename);
# dir.create(output_stats_dir, showWarnings=FALSE, recursive=TRUE, mode="0755");
#
# output_file_spatial = file.path(output_stats_dir, "min_spatial_freq.csv");
# output_file_block = file.path(output_stats_dir, "min_block_freq.csv");
# output_file_stretch = file.path(output_stats_dir, "max_stretch.csv");
#
#
#
# # cleanup file and create header
# empty_data = matrix(ncol=3, nrow=0);
# colnames(empty_data) = c(
#     "min_spatial_freq", "num_sequences", "seq_length");
# write.table(empty_data, file=output_file_spatial, append=FALSE,
#             row.names=FALSE, col.names=TRUE, quote=FALSE, sep=",");
#
# # cleanup file and create header
# empty_data = matrix(ncol=3, nrow=0);
# colnames(empty_data) = c(
#     "min_block_freq", "num_sequences", "seq_length");
# write.table(empty_data, file=output_file_block, append=FALSE,
#             row.names=FALSE, col.names=TRUE, quote=FALSE, sep=",");
#
# # cleanup file and create header
# empty_data = matrix(ncol=3, nrow=0);
# colnames(empty_data) = c(
#     "max_stretch", "num_sequences", "seq_length");
# write.table(empty_data, file=output_file_stretch, append=FALSE,
#             row.names=FALSE, col.names=TRUE, quote=FALSE, sep=",");
#
#
#
# # [CMP] not optimized code, but pretty fast
#
# # start the iterations, for each json data grouped by length
# for(iteration in 1:length(json_data)) {
#     #cat("Iteration:", iteration);
#
#     sequence_data_by_length = json_data[[iteration]];
#
#     if(
#         is.null(sequence_data_by_length$length) ||
#             is.null(sequence_data_by_length$sequences) ||
#             length(sequence_data_by_length$sequences) < 1
#     ) {
#         cat("Empty sequence data iteration", iteration,
#             "of length", sequence_data_by_length$length, "\n");
#         #cat("Data:\n");
#         #dput(sequence_data_by_length);
#         next;
#     }
#
#     sequence_length = sequence_data_by_length$length;
#     sequence_data = sequence_data_by_length$sequences;
#
#     # APPEND Stacked Bar plot DATA per min_spatial_freq:
#     # min_spatial_freq, num_sequences, seq_length
#     write.table(
#         as.matrix(t(c(
#             min_spatial_freq, length(sequence_data), sequence_length))),
#         file=output_file_spatial,
#         append=TRUE,
#         row.names=FALSE,
#         col.names=FALSE,
#         quote=FALSE,
#         sep=",");
#
#     # APPEND Stacked Bar plot DATA per min_block_freq:
#     # min_block_freq, num_sequences, seq_length
#     write.table(
#         as.matrix(t(c(
#             min_block_freq, length(sequence_data), sequence_length))),
#         file=output_file_block,
#         append=TRUE,
#         row.names=FALSE,
#         col.names=FALSE,
#         quote=FALSE,
#         sep=",");
#
#     # APPEND Stacked Bar plot DATA per max_stretch:
#     # max_stretch, num_sequences, seq_length
#     write.table(
#         as.matrix(t(c(
#             max_stretch, length(sequence_data), sequence_length))),
#         file=output_file_stretch,
#         append=TRUE,
#         row.names=FALSE,
#         col.names=FALSE,
#         quote=FALSE,
#         sep=",");
# }



# retrieving number of ranges and positions group by sequence

# organize the data by sequence
seq_data = new.env(hash=TRUE, parent=emptyenv());

# start the iterations, for each json data grouped by length
for(iteration in 1:length(json_data)) {
    #cat("Iteration:", iteration);

    sequence_data_by_length = json_data[[iteration]];

    if(
        is.null(sequence_data_by_length$length) ||
            is.null(sequence_data_by_length$sequences) ||
            length(sequence_data_by_length$sequences) < 1
    ) {
        cat("Empty sequence data iteration", iteration,
            "of length", sequence_data_by_length$length, "\n");
        #cat("Data:\n");
        #dput(sequence_data_by_length);
        next;
    }

    sequence_length = sequence_data_by_length$length;
    sequence_data = sequence_data_by_length$sequences;

    for(j in 1:length(sequence_data)) {
        sequence_data_item = sequence_data[[j]];
        sequence = sequence_data_item$sequence;

        #if(is.null(seq_data[[sequence]])) {
        if(! exists(sequence, seq_data)) {
            #seq_data[[sequence]] = list();
            seq_data[[sequence]] = new.env(hash=TRUE, parent=emptyenv());
            seq_data[[sequence]][["num_ranges"]] = 0;
            seq_data[[sequence]][["num_pos"]] = 0;
        }

        seq_data[[sequence]][["num_ranges"]] =
            seq_data[[sequence]][["num_ranges"]] + 1;

        seq_data[[sequence]][["num_pos"]] =
            seq_data[[sequence]][["num_pos"]] +
            length(sequence_data_item$times);
    }
}

# transform in a matrix
seq_matrix = new.env(hash=TRUE, parent=emptyenv());
seq_matrix[["sequences"]] = c();
seq_matrix[["num_ranges"]] = c();
seq_matrix[["num_pos"]] = c();

for (key in ls(seq_data)) {
    seq_matrix[["sequences"]] = c(seq_matrix[["sequences"]], key);
    seq_matrix[["num_ranges"]] = c(
        seq_matrix[["num_ranges"]], seq_data[[key]][['num_ranges']]);
    seq_matrix[["num_pos"]] = c(
        seq_matrix[["num_pos"]], seq_data[[key]][['num_pos']]);
}

seq_data_frame = data.frame(
    seq_matrix[["sequences"]],
    seq_matrix[["num_ranges"]],
    seq_matrix[["num_pos"]]);
colnames(seq_data_frame) = c("sequences", "num_ranges", "num_pos");


# write data to a csv
write.table(
    seq_data_frame,
    file=file.path(output_stats_dir, paste(base_filename, ".csv", sep="")),
    append=FALSE,
    row.names=FALSE,
    col.names=TRUE,
    quote=FALSE,
    sep=",");

#
# # draw a stacked bar plot
# utils.dev_open_file(
#     file.path(output_stats_dir, paste(base_filename, ".png", sep="")),
#     640, 480);
#
# bins = as.character(sort(unique(seq_data_frame$sequences)));
# legend = as.character(sort(unique(seq_data_frame$num_ranges)));
#
# ggplot() +
#     theme_bw() +
#     geom_bar(
#         data = seq_data_frame,
#         aes(
#             x=factor(sequences), fill=num_pos
#         ),
#         stat="identity"
#     ) +
# #     geom_text(
# #         data = input_data_cumsum,
# #         aes(
# #             x = sequences, y = label_ypos, label = num_pos
# #         ),
# #     ) +
#     scale_x_discrete(breaks=bins, limits=bins) +
#     scale_y_log10(labels=trans_format('log10',math_format(10^.x))) +
#     scale_fill_discrete(
#         breaks = legend, limits = legend, guide = guide_legend()) +
#     labs(
#         title = paste("Support Range:", 0),
#         y = "Num of Sequences",
#         x = "Time Window",
#         fill = "Sequence Size"
#     );
#
# dev.off();
