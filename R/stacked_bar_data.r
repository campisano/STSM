# include utility file
source(file="R/utils.r")



# configure detailed error outputs
utils$verbose();



# loading dependences
loaded_libs = utils$loadLibs(c("rjson", "ggplot2", "grid", "scales"));



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

vars = new.env(hash=TRUE, parent=emptyenv());
vars$input_file_json = args[1];
vars$output_stats_dir = args[2];
vars$min_spatial_freq = args[3];
vars$min_block_freq = args[4];
vars$max_stretch = args[5];

vars$base_filename = sub(
    file.path(dirname(vars$input_file_json), ""), "", vars$input_file_json);
vars$base_filename = sub("[.][^.]*$", "", vars$base_filename, perl=TRUE);



# loading json data
#cat("Loading json data", vars$input_file_json, "...");
json_data = fromJSON(file=vars$input_file_json, method="C");
#cat(" [DONE]\n")

if(is.null(json_data) || length(json_data) < 1) {
    cat("Empty json data\n");
    quit(status=1);
}



# # preparing output folders
# vars$output_stats_dir = file.path(vars$output_stats_dir, vars$base_filename);
# dir.create(vars$output_stats_dir, showWarnings=FALSE, recursive=TRUE, mode="0755");
#
# output_file_spatial = file.path(vars$output_stats_dir, "min_spatial_freq.csv");
# output_file_block = file.path(vars$output_stats_dir, "min_block_freq.csv");
# output_file_stretch = file.path(vars$output_stats_dir, "max_stretch.csv");
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
#             vars$min_spatial_freq, length(sequence_data), sequence_length))),
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
#             vars$min_block_freq, length(sequence_data), sequence_length))),
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
#             vars$max_stretch, length(sequence_data), sequence_length))),
#         file=output_file_stretch,
#         append=TRUE,
#         row.names=FALSE,
#         col.names=FALSE,
#         quote=FALSE,
#         sep=",");
# }



# retrieving number of ranges and positions group by sequence

# organize data by sequence
by_seq_data = new.env(hash=TRUE, parent=emptyenv());

# organize data by length
by_len_data = new.env(hash=TRUE, parent=emptyenv());

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

        # skip sequence containing mmmmm
        if(length(grep("mmmmm", sequence)) == 0) {

            # organize data by sequence
            if(! exists(sequence, by_seq_data)) {
                by_seq_data[[sequence]] = new.env(hash=TRUE, parent=emptyenv());
                by_seq_data[[sequence]][["num_ranges"]] = 0;
                by_seq_data[[sequence]][["num_pos"]] = 0;
            }

            by_seq_data[[sequence]][["num_ranges"]] =
                by_seq_data[[sequence]][["num_ranges"]] + 1;

            by_seq_data[[sequence]][["num_pos"]] =
                by_seq_data[[sequence]][["num_pos"]] +
                length(sequence_data_item$times);

            # organize data by length
            len = as.character(sequence_length);
            if(! exists(len, by_len_data)) {
                by_len_data[[len]] = new.env(hash=TRUE, parent=emptyenv());
                by_len_data[[len]][["num_ranges"]] = 0;
                by_len_data[[len]][["num_pos"]] = 0;
            }

            by_len_data[[len]][["num_ranges"]] =
                by_len_data[[len]][["num_ranges"]] + 1;

            by_len_data[[len]][["num_pos"]] =
                by_len_data[[len]][["num_pos"]] +
                length(sequence_data_item$times);
        }
    }
}



# transform to a data frame
to_data_frame_of_3 = function(data, name1, name2, name3) {
    data1 = c();
    data2 = c();
    data3 = c();

    for (key in ls(data)) {
        data1 = c(data1, key);
        data2 = c(data2, data[[key]][[name2]]);
        data3 = c(data3, data[[key]][[name3]]);
    }

    data_frame = data.frame(data1, data2, data3);
    colnames(data_frame) = c(name1, name2, name3);

    return(data_frame);
}

by_seq_data_frame = to_data_frame_of_3(
    by_seq_data, "sequence", "num_ranges", "num_pos");

by_len_data_frame = to_data_frame_of_3(
    by_len_data, "length", "num_ranges", "num_pos");


# write data to a csv
write.table(
    by_seq_data_frame,
    file=file.path(vars$output_stats_dir,
                   paste(vars$base_filename, "_sequences.csv", sep="")),
    append=FALSE,
    row.names=FALSE,
    col.names=TRUE,
    quote=FALSE,
    sep=",");
write.table(
    by_len_data_frame,
    file=file.path(vars$output_stats_dir,
                   paste(vars$base_filename, "_lengths.csv", sep="")),
    append=FALSE,
    row.names=FALSE,
    col.names=TRUE,
    quote=FALSE,
    sep=",");



# draw a point plot of sequences vs num_pos
utils$dev_open_file(
    file.path(vars$output_stats_dir,
              paste(vars$base_filename, "_sequence-num_pos.png", sep="")),
    10000, 10000);
gg = ggplot();
gg = gg + theme_bw();
gg = gg + geom_point(
    data=by_seq_data_frame, aes(sequence, num_pos), size=25);
gg = gg + theme(axis.text.x = element_text(angle=90, hjust=1));
gg = gg + theme(panel.background=element_rect(fill="white", colour=NA));
gg = gg + theme(plot.background=element_rect(fill="white", colour=NA));
gg = gg + theme(panel.grid=element_blank());
gg = gg + theme(panel.border=element_blank())
gg = gg + theme(plot.margin=unit(c(0,0,0,0), "null"));
gg = gg + theme(panel.margin=unit(c(0,0,0,0), "null"));
gg = gg + labs(
    title=paste("Frequency:", vars$min_spatial_freq),
    x="Sequence value",
    y="Num of positions");
plot(gg);
dev.off();



# draw a point plot of sequences vs num_ranges
utils$dev_open_file(
    file.path(vars$output_stats_dir,
              paste(vars$base_filename, "_sequence-num_ranges.png", sep="")),
    10000, 10000);
gg = ggplot();
gg = gg + theme_bw();
gg = gg + geom_point(
    data=by_seq_data_frame, aes(sequence, num_ranges), size=25);
gg = gg + theme(axis.text.x = element_text(angle=90, hjust=1));
gg = gg + theme(panel.background=element_rect(fill="white", colour=NA));
gg = gg + theme(plot.background=element_rect(fill="white", colour=NA));
gg = gg + theme(panel.grid=element_blank());
gg = gg + theme(panel.border=element_blank())
gg = gg + theme(plot.margin=unit(c(0,0,0,0), "null"));
gg = gg + theme(panel.margin=unit(c(0,0,0,0), "null"));
gg = gg + labs(
    title=paste("Frequency:", vars$min_spatial_freq),
    x="Sequence value",
    y="Num of ranges");
plot(gg);
dev.off();


# cleanup
by_len_data_frame$length = as.numeric(by_len_data_frame$length);
by_len_data_frame = by_len_data_frame[(by_len_data_frame$length <= 50),];
by_len_data_frame$length = as.character(by_len_data_frame$length);

# draw a stacked bar plot
utils$dev_open_file(
    file.path(vars$output_stats_dir,
              paste(vars$base_filename, "_length-num_pos-log.png", sep="")),
    640, 480);
bins = as.character(sort(as.numeric(unique(by_len_data_frame$length))));
legend = as.character(sort(unique(by_len_data_frame$num_ranges)));
gg = ggplot();
gg = gg + theme_bw();
gg = gg + scale_x_discrete(breaks=bins, limits=bins);
gg = gg + scale_y_log10(labels=trans_format('log10', math_format(10^.x)));
gg = gg + geom_bar(data=by_len_data_frame, aes(length, num_pos), stat="identity");
gg = gg + labs(
    title=paste("Frequency:", vars$min_spatial_freq),
    x="Sequence lengths",
    y="Num of positions");
plot(gg);
dev.off();


# cleanup
by_len_data_frame$length = as.numeric(by_len_data_frame$length);
by_len_data_frame = by_len_data_frame[(by_len_data_frame$length > 1),];
by_len_data_frame$length = as.character(by_len_data_frame$length);

# draw a stacked bar plot
utils$dev_open_file(
    file.path(vars$output_stats_dir,
              paste(vars$base_filename, "_length-num_pos-linear.png", sep="")),
    640, 480);
bins = as.character(sort(as.numeric(unique(by_len_data_frame$length))));
legend = as.character(sort(unique(by_len_data_frame$num_ranges)));
gg = ggplot();
gg = gg + theme_bw();
gg = gg + scale_x_discrete(breaks=bins, limits=bins);
gg = gg + scale_y_continuous();
gg = gg + geom_bar(data=by_len_data_frame, aes(length, num_pos), stat="identity");
gg = gg + labs(
    title=paste("Frequency:", vars$min_spatial_freq),
    x="Sequence lengths",
    y="Num of positions");
plot(gg);
dev.off();

