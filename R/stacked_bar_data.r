#setwd("/home/shared/develop/projects/CEFET/mestrado/SIM");

# include utility file
source(file="R/utils.r", chdir=TRUE);

utils$setVerbose();

# loading dependences
loaded_libs = utils$loadLibs(c(
    "rjson:0.2.15",
    "ggplot2:1.0.0",
    "grid:3.1.1",
    "scales:3.1.1"));



# evaluating arguments
args = commandArgs(TRUE);
#cat("Arguments:\n");
#cat(args, "\n");
# examples:
#args = c();
#args[1] = "results_test/inline-100_orientation-original/sax-25/json/I100_Ooriginal_S25_FS100_FB50_MS0.json";
#args[2] = "results_test/inline-100_orientation-original/sax-25/stats/spatial-100/block-50/stretch-0";
#args[3] = 100;
#args[4] = 50;
#args[5] = 0;
#cat("    args:", args, "\n");

vars = utils$newDict();
vars$input_file_json = args[1];
vars$output_stats_dir = args[2];
vars$min_spatial_freq = args[3];
vars$min_block_freq = args[4];
vars$max_stretch = args[5];

vars$base_filename = utils$remove_extension(basename(vars$input_file_json));


config = utils$newDict();
config$max_sequence_len = 20;
#config$min_sequence_length_to_plot = 2;
#config$max_sequence_length_to_plot = Inf;
#config$max_length_plot_limit = 100000;
config$per_sequence_plot_block_requires_min_width_to_be_drawn = 5;



# loading json data
#cat("Loading json data", vars$input_file_json, "...");
json_data = utils$readJSON(vars$input_file_json);
#cat(" [DONE]\n")

if(is.null(json_data) || length(json_data) < 1) {
    cat("Empty json data\n");
    quit(status=1);
}

solid_sequences = json_data$solid_sequences;
solid_blocks = json_data$solid_blocks;

if(length(solid_sequences) != length(solid_blocks)) {
    cat("Num of solid sequence lengths",
        "expected to be equal to num of solid blocks length!\n");
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
# for(iteration in 1:length(solid_sequences)) {
#     #cat("Iteration:", iteration);
#
#     sequence_data_by_length = solid_sequences[[iteration]];
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

# count matching postions by sequence in ranged_sequences data
pos_by_seq = utils$newDict();

# count matching postions by length in ranged_sequences data
pos_by_len = utils$newDict();

# count blocks by length in blocked sequences data
blk_by_len = utils$newDict();

# count sequences by length in ranged_sequences data
seq_by_len = utils$newDict();

# count sequences with big blocks by length in blocked sequences data
seq_with_big_blocks_by_len = utils$newDict();

# histogram of block areas by length
# hist_block_stats = utils$newDict();



# sequences

# start the iterations, for each json data grouped by length
for(iteration in 1:length(solid_sequences)) {

    if(iteration <= config$max_sequence_len)
    {
        #cat("Iteration:", iteration);

        sequence_data_by_length = solid_sequences[[iteration]];

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

        len = as.character(sequence_length);
        pos_by_len[[len]] = utils$newDict();
        pos_by_len[[len]]$num_ranges = 0;
        pos_by_len[[len]]$num_pos = 0;

        for(j in 1:length(sequence_data)) {
            sequence_data_item = sequence_data[[j]];
            sequence = sequence_data_item$sequence;

            # count matching postions by sequence
            if(! exists(sequence, pos_by_seq)) {
                pos_by_seq[[sequence]] = utils$newDict();
                pos_by_seq[[sequence]]$num_ranges = 0;
                pos_by_seq[[sequence]]$num_pos = 0;
            }

            pos_by_seq[[sequence]]$num_ranges =
                pos_by_seq[[sequence]]$num_ranges + 1;

            pos_by_seq[[sequence]]$num_pos =
                pos_by_seq[[sequence]]$num_pos +
                length(sequence_data_item$spaces);

            # count matching postions by length

            pos_by_len[[len]]$num_ranges =
                pos_by_len[[len]]$num_ranges + 1;

            pos_by_len[[len]]$num_pos =
                pos_by_len[[len]]$num_pos +
                length(sequence_data_item$spaces);

            # count sequences by length
            if(! exists(len, seq_by_len)) {
                seq_by_len[[len]] = utils$newDict();
                seq_by_len[[len]]$sequences_map = utils$newDict();
            }

            # using a map to get uniques values
            seq_by_len[[len]]$sequences_map[[sequence]] = TRUE;
        }

        # count stored sequences by length
        seq_by_len[[len]]$sequences = length(seq_by_len[[len]]$sequences_map);
    }
}



# blocks

# start the iterations, for each json data grouped by length
for(iteration in 1:length(solid_blocks)) {

    if(iteration <= config$max_sequence_len)
    {
        #cat("Iteration:", iteration);

        solid_blocks_data_by_length = solid_blocks[[iteration]];

        sequence_length = solid_blocks_data_by_length$length;

        len = as.character(sequence_length);
        blk_by_len[[len]] = utils$newDict();
        blk_by_len[[len]]$num_blocks = 0;

#         hist_block_stats[[len]] = utils$newDict();
#         hist_block_stats[[len]]$areas = c();
#         hist_block_stats[[len]]$widths = c();

        if(
            is.null(solid_blocks_data_by_length$length) ||
                is.null(solid_blocks_data_by_length$blocks) ||
                length(solid_blocks_data_by_length$blocks) < 1
        ) {
            cat(
                "\n\tError in block data for iteration ",
                iteration, ".\n", sep="");
        } else if(solid_blocks_data_by_length$length != sequence_length) {
            cat("\n\tThe length of sequences in the same index of solid",
                "sequences and solid blocks must be the same, at iteration ",
                iteration, ".\n", sep="");
        } else {
            solid_blocks_data = solid_blocks_data_by_length$blocks;

            for(j in 1:length(solid_blocks_data)) {
                block_data_item = solid_blocks_data[[j]];
                sequence = block_data_item$sequence;

                # histogram of block areas and widths by length

                blk_by_len[[len]]$num_blocks =
                    blk_by_len[[len]]$num_blocks + 1;

                r_start = block_data_item$r_start;
                r_end = block_data_item$r_end;
                i_start = block_data_item$i_start;
                i_end = block_data_item$i_end;

                if(
                    (r_end - r_start + 1) >=
                    config$per_sequence_plot_block_requires_min_width_to_be_drawn
                ) {
                    # count sequences with blocks by length
                    if(! exists(len, seq_with_big_blocks_by_len)) {
                        seq_with_big_blocks_by_len[[len]] = utils$newDict();
                        seq_with_big_blocks_by_len[[len]]$sequences_map =
                            utils$newDict();
                    }

                    # using a map to get uniques values
                    seq_with_big_blocks_by_len[[
                        len]]$sequences_map[[sequence]] = TRUE;
                }
            }
            # count stored sequences with blocks by length
            seq_with_big_blocks_by_len[[len]]$sequences =
                length(seq_with_big_blocks_by_len[[len]]$sequences_map);
        }
    }
}



#
# # block stats
# #
# areas = data.frame(hist_block_stats[["3"]]$areas);
# colnames(areas) = c("areas");
# widths = data.frame(hist_block_stats[["3"]]$widths);
# colnames(widths) = c("widths");
#
# areas_3000 = data.frame(areas[areas$areas < 3000,]);
# colnames(areas_3000) = c("areas");
# utils$dev_open_file("areas_len3_limits-0-3000_linear.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=areas_3000,
#         aes_string(x="areas"),
#         binwidth=6,
#         drop=TRUE);
# utils$dev_off();
# utils$dev_open_file("areas_len3_limits-0-3000_log.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=areas_3000,
#         aes_string(x="areas"),
#         binwidth=6,
#         drop=TRUE) +
#     scale_y_log10(labels=trans_format('log10',math_format(10^.x)));
# utils$dev_off();
#
# areas_300 = data.frame(areas[areas$areas < 300,]);
# colnames(areas_300) = c("areas");
# utils$dev_open_file("areas_len3_limits-0-300_linear.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=areas_300,
#         aes_string(x="areas"),
#         binwidth=6,
#         drop=TRUE);
# utils$dev_off();
# utils$dev_open_file("areas_len3_limits-0-300_log.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=areas_300,
#         aes_string(x="areas"),
#         binwidth=6,
#         drop=TRUE) +
#     scale_y_log10(labels=trans_format('log10',math_format(10^.x)));
# utils$dev_off();
#
# areas_150 = data.frame(areas[areas$areas < 150,]);
# colnames(areas_150) = c("areas");
# utils$dev_open_file("areas_len3_limits-0-150_linear.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=areas_150,
#         aes_string(x="areas"),
#         binwidth=3,
#         drop=TRUE);
# utils$dev_off();
# utils$dev_open_file("areas_len3_limits-0-150_log.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=areas_150,
#         aes_string(x="areas"),
#         binwidth=3,
#         drop=TRUE) +
#     scale_y_log10(labels=trans_format('log10',math_format(10^.x)));
# utils$dev_off();
#
# areas_20 = data.frame(areas[areas$areas < 20,]);
# colnames(areas_20) = c("areas");
# utils$dev_open_file("areas_len3_limits-0-20_linear.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=areas_20,
#         aes_string(x="areas"),
#         binwidth=1,
#         drop=TRUE);
# utils$dev_off();
# utils$dev_open_file("areas_len3_limits-0-20_log.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=areas_20,
#         aes_string(x="areas"),
#         binwidth=1,
#         drop=TRUE) +
#     scale_y_log10(labels=trans_format('log10',math_format(10^.x)));
# utils$dev_off();
#
# widths_3000 = data.frame(widths[widths$widths < 3000,]);
# colnames(widths_3000) = c("widths");
# utils$dev_open_file("widths_len3_limits-0-3000_linear.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=widths_3000,
#         aes_string(x="widths"),
#         binwidth=6,
#         drop=TRUE);
# utils$dev_off();
# utils$dev_open_file("widths_len3_limits-0-3000_log.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=widths_3000,
#         aes_string(x="widths"),
#         binwidth=6,
#         drop=TRUE) +
#     scale_y_log10(labels=trans_format('log10',math_format(10^.x)));
# utils$dev_off();
#
# widths_300 = data.frame(widths[widths$widths < 300,]);
# colnames(widths_300) = c("widths");
# utils$dev_open_file("widths_len3_limits-0-300_linear.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=widths_300,
#         aes_string(x="widths"),
#         binwidth=6,
#         drop=TRUE);
# utils$dev_off();
# utils$dev_open_file("widths_len3_limits-0-300_log.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=widths_300,
#         aes_string(x="widths"),
#         binwidth=6,
#         drop=TRUE) +
#     scale_y_log10(labels=trans_format('log10',math_format(10^.x)));
# utils$dev_off();
#
# widths_150 = data.frame(widths[widths$widths < 150,]);
# colnames(widths_150) = c("widths");
# utils$dev_open_file("widths_len3_limits-0-150_linear.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=widths_150,
#         aes_string(x="widths"),
#         binwidth=3,
#         drop=TRUE);
# utils$dev_off();
# utils$dev_open_file("widths_len3_limits-0-150_log.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=widths_150,
#         aes_string(x="widths"),
#         binwidth=3,
#         drop=TRUE) +
#     scale_y_log10(labels=trans_format('log10',math_format(10^.x)));
# utils$dev_off();
#
# widths_20 = data.frame(widths[widths$widths < 20,]);
# colnames(widths_20) = c("widths");
# utils$dev_open_file("widths_len3_limits-0-20_linear.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=widths_20,
#         aes_string(x="widths"),
#         binwidth=1,
#         drop=TRUE);
# utils$dev_off();
# utils$dev_open_file("widths_len3_limits-0-20_log.png", 640, 480);
# ggplot() +
#     theme_bw() +
#     geom_histogram(
#         data=widths_20,
#         aes_string(x="widths"),
#         binwidth=1,
#         drop=TRUE) +
#     scale_y_log10(labels=trans_format('log10',math_format(10^.x)));
# utils$dev_off();









# transform to a data frame
to_data_frame_of_3 = function(data, name1, name2, name3) {
    data1 = c();
    data2 = c();
    data3 = c();

    for(key in ls(data)) {
        data1 = c(data1, key);
        data2 = c(data2, data[[key]][[name2]]);
        data3 = c(data3, data[[key]][[name3]]);
    }

    data_frame = data.frame(data1, data2, data3);
    colnames(data_frame) = c(name1, name2, name3);

    return(data_frame);
}

to_data_frame_of_2 = function(data, name1, name2) {
    data1 = c();
    data2 = c();

    for(key in ls(data)) {
        data1 = c(data1, key);
        data2 = c(data2, data[[key]][[name2]]);
    }

    data_frame = data.frame(data1, data2);
    colnames(data_frame) = c(name1, name2);

    return(data_frame);
}



pos_by_seq_frame = to_data_frame_of_3(
    pos_by_seq, "sequence", "num_ranges", "num_pos");

pos_by_len_frame = to_data_frame_of_3(
    pos_by_len, "length", "num_ranges", "num_pos");

blk_by_len_frame = to_data_frame_of_2(
    blk_by_len, "length", "num_blocks");

seq_by_len_frame = to_data_frame_of_2(
    seq_by_len, "length", "sequences");

seq_with_big_blocks_by_len_frame = to_data_frame_of_2(
    seq_with_big_blocks_by_len, "length", "sequences");



# write data to a csv
utils$writeCSV(
    pos_by_seq_frame, file.path( vars$output_stats_dir, paste(
        vars$base_filename, "_matches-by-seq.csv", sep="")));

utils$writeCSV(
    pos_by_len_frame, file=file.path(vars$output_stats_dir, paste(
        vars$base_filename, "_matches-by-len.csv", sep="")));

utils$writeCSV(
    blk_by_len_frame, file=file.path(vars$output_stats_dir, paste(
        vars$base_filename, "_blocks-by-len.csv", sep="")));

utils$writeCSV(
    seq_by_len_frame, file=file.path(vars$output_stats_dir, paste(
        vars$base_filename, "_sequences-by-len.csv", sep="")));

utils$writeCSV(
    seq_with_big_blocks_by_len_frame,
    file=file.path(vars$output_stats_dir, paste(
        vars$base_filename, "_sequences_with_big_blocks-by-len.csv", sep="")));



# # draw a point plot of num_pos by sequence
# utils$dev_open_file(
#     file.path(vars$output_stats_dir,
#               paste(vars$base_filename, "_numpos-by-sequence.png", sep="")),
#     10000, 10000);
# gg = ggplot();
# gg = gg + theme_bw();
# gg = gg + geom_point(
#     data=pos_by_seq_frame, aes_string("sequence", "num_pos"), size=25);
# gg = gg + theme(axis.text.x = element_text(angle=90, hjust=1));
# gg = gg + theme(panel.background=element_rect(fill="white", colour=NA));
# gg = gg + theme(plot.background=element_rect(fill="white", colour=NA));
# gg = gg + theme(panel.grid=element_blank());
# gg = gg + theme(panel.border=element_blank())
# gg = gg + theme(plot.margin=unit(c(0,0,0,0), "null"));
# gg = gg + theme(panel.margin=unit(c(0,0,0,0), "null"));
# gg = gg + labs(
#     title=paste("Range frequency:", vars$min_spatial_freq, "- Block frequency:", vars$min_block_freq),
#     x="Sequence value",
#     y="Num of occurrences");
# plot(gg);
# utils$dev_off();
#
#
#
# # draw a point plot of num_ranges by sequence
# utils$dev_open_file(
#     file.path(vars$output_stats_dir,
#               paste(vars$base_filename, "_numranges-by-sequence.png", sep="")),
#     10000, 10000);
# gg = ggplot();
# gg = gg + theme_bw();
# gg = gg + geom_point(
#     data=pos_by_seq_frame, aes_string("sequence", "num_ranges"), size=25);
# gg = gg + theme(axis.text.x = element_text(angle=90, hjust=1));
# gg = gg + theme(panel.background=element_rect(fill="white", colour=NA));
# gg = gg + theme(plot.background=element_rect(fill="white", colour=NA));
# gg = gg + theme(panel.grid=element_blank());
# gg = gg + theme(panel.border=element_blank())
# gg = gg + theme(plot.margin=unit(c(0,0,0,0), "null"));
# gg = gg + theme(panel.margin=unit(c(0,0,0,0), "null"));
# gg = gg + labs(
#     title=paste("Range frequency:", vars$min_spatial_freq, "- Block frequency:", vars$min_block_freq),
#     x="Sequence value",
#     y="Num of ranges");
# plot(gg);
# utils$dev_off();



# cleanup excess of data
# pos_by_len_frame$length = as.numeric(pos_by_len_frame$length);
# pos_by_len_frame = pos_by_len_frame[(pos_by_len_frame$length <= 50),];
# pos_by_len_frame$length = as.character(pos_by_len_frame$length);

# draw a stacked bar plot of num_pos by length in log scale
pos_by_len_frame = pos_by_len_frame[pos_by_len_frame$num_pos != 0,];
utils$dev_open_file(
    file.path(vars$output_stats_dir,
              paste(vars$base_filename, "_numpos-by-len_log.png", sep="")),
    640, 480);
utils$bar_plot(
    data_frame=pos_by_len_frame,
    x_col="length",
    y_col="num_pos",
    log=TRUE,
    title=paste(
        "Range frequency:", vars$min_spatial_freq,
        "- Block frequency:", vars$min_block_freq),
    x_title="Sequence lengths",
    y_title="Num of occurrences"
);
utils$dev_off();

# draw a stacked bar plot of num_ranges by length in log scale
pos_by_len_frame = pos_by_len_frame[pos_by_len_frame$num_ranges != 0,];
utils$dev_open_file(
    file.path(vars$output_stats_dir,
              paste(vars$base_filename, "_numranges-by-len_log.png", sep="")),
    640, 480);
utils$bar_plot(
    data_frame=pos_by_len_frame,
    x_col="length",
    y_col="num_ranges",
    log=TRUE,
    title=paste(
        "Range frequency:", vars$min_spatial_freq,
        "- Block frequency:", vars$min_block_freq),
    x_title="Sequence lengths",
    y_title="Num of ranges"
);
utils$dev_off();

# draw a stacked bar plot of num_blocks by length in log scale
blk_by_len_frame = blk_by_len_frame[blk_by_len_frame$num_blocks != 0,];
utils$dev_open_file(
    file.path(vars$output_stats_dir,
              paste(vars$base_filename, "_numblocks-by-len_log.png", sep="")),
    640, 480);
utils$bar_plot(
    data_frame=blk_by_len_frame,
    x_col="length",
    y_col="num_blocks",
    log=TRUE,
    title=paste(
        "Range frequency:", vars$min_spatial_freq,
        "- Block frequency:", vars$min_block_freq),
    x_title="Sequence lengths",
    y_title="Num of blocks"
);
utils$dev_off();



# # cleanup excess of data
# pos_by_len_frame$length = as.numeric(pos_by_len_frame$length);
# pos_by_len_frame = pos_by_len_frame[(pos_by_len_frame$length > 1),];
# pos_by_len_frame$length = as.character(pos_by_len_frame$length);

# # draw a stacked bar plot of num_pos by length in linear scale
# utils$dev_open_file(
#     file.path(vars$output_stats_dir,
#               paste(vars$base_filename, "_numpos-by-len_linear.png", sep="")),
#     640, 480);
# bins = as.character(sort(as.numeric(unique(pos_by_len_frame$length))));
# gg = ggplot();
# gg = gg + theme_bw();
# gg = gg + scale_x_discrete(breaks=bins, limits=bins);
# gg = gg + scale_y_continuous(labels=comma);
# gg = gg + geom_bar(
#     data=pos_by_len_frame,
#     aes_string(x="length", y="num_pos"),
#     position="identity",
#     stat="identity");
# gg = gg + labs(
#     title=paste("Range frequency:", vars$min_spatial_freq, "- Block frequency:", vars$min_block_freq),
#     x="Sequence lengths",
#     y="Num of occurrences");
# plot(gg);
# utils$dev_off();

# # draw a stacked bar plot of num_ranges by length in linear scale
# utils$dev_open_file(
#     file.path(vars$output_stats_dir, paste(
#         vars$base_filename, "_numranges-by-len_linear.png", sep="")),
#     640, 480);
# bins = as.character(sort(as.numeric(unique(pos_by_len_frame$length))));
# gg = ggplot();
# gg = gg + theme_bw();
# gg = gg + scale_x_discrete(breaks=bins, limits=bins);
# gg = gg + scale_y_continuous(labels=comma);
# gg = gg + geom_bar(
#     data=pos_by_len_frame,
#     aes_string(x="length", y="num_ranges"),
#     position="identity",
#     stat="identity");
# gg = gg + labs(
#     title=paste("Range frequency:", vars$min_spatial_freq, "- Block frequency:", vars$min_block_freq),
#     x="Sequence lengths",
#     y="Num of ranges");
# plot(gg);
# utils$dev_off();



# draw a stacked bar plot of sequences by length in log scale
seq_by_len_frame = seq_by_len_frame[seq_by_len_frame$sequences != 0,];
utils$dev_open_file(
    file.path(vars$output_stats_dir,
              paste(vars$base_filename, "_sequences-by-len_log.png", sep="")),
    640, 480);
utils$bar_plot(
    data_frame=seq_by_len_frame,
    x_col="length",
    y_col="sequences",
    log=TRUE,
    title=paste(
        "Range frequency:", vars$min_spatial_freq,
        "- Block frequency:", vars$min_block_freq),
    x_title="Sequence lengths",
    y_title="Num of sequence patterns"
);
utils$dev_off();



# # draw a stacked bar plot of sequences by length in linear scale
# utils$dev_open_file(
#     file.path(
#         vars$output_stats_dir,
#         paste(vars$base_filename, "_sequences-by-len_linear.png", sep="")),
#     640, 480);
# bins = as.character(sort(as.numeric(unique(seq_by_len_frame$length))));
# gg = ggplot();
# gg = gg + theme_bw();
# gg = gg + scale_x_discrete(breaks=bins, limits=bins);
# gg = gg + scale_y_continuous(labels=comma);
# gg = gg + geom_bar(
#     data=seq_by_len_frame,
#     aes_string(x="length", y="sequences"),
#     position="identity",
#     stat="identity");
# gg = gg + labs(
#     title=paste("Range frequency:", vars$min_spatial_freq, "- Block frequency:", vars$min_block_freq),
#     x="Sequence lengths",
#     y="Num of sequence patterns");
# plot(gg);
# utils$dev_off();




# draw a stacked bar plot of sequences by length in log scale
seq_with_big_blocks_by_len_frame =
    seq_with_big_blocks_by_len_frame[
        seq_with_big_blocks_by_len_frame$sequences != 0,];
utils$dev_open_file(
    file.path(vars$output_stats_dir,
              paste(vars$base_filename,
                    "_sequences_with_big_blocks-by-len_log.png", sep="")),
    640, 480);
utils$bar_plot(
    data_frame=seq_with_big_blocks_by_len_frame,
    x_col="length",
    y_col="sequences",
    log=TRUE,
    title=paste(
        "Range frequency:", vars$min_spatial_freq,
        "- Block frequency:", vars$min_block_freq),
    x_title="Sequence lengths",
    y_title="Num of sequence patterns containing big blocks"
);
utils$dev_off();



# write a file that grants that this plot is complete for this product
result = file.create(file.path(vars$output_stats_dir, "complete"));
