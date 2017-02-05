#setwd("/home/shared/develop/projects/CEFET/mestrado/SIM");

# include utility file
source(file="R/utils.r", chdir=TRUE);

# loading dependences
loaded_libs = utils$loadLibs(c(
    # basic R packages:
    "datasets:3.1.1",
    "graphics:3.1.1",
    "grDevices:3.1.1",
    "methods:3.1.1",
    "stats:3.1.1",
    "utils:3.1.1",
    # custom packages:
    "compiler:3.1.1",
    "rjson:0.2.15",
    "TSMining:1.0",
    "ggplot2:1.0.0",
    "grid:3.1.1",
    "scales:0.2.4"));



# defining a specific plot function
plotSequencePositionsRangesAndIntervals_src = function(
    x_points, y_points,
    xmin_ranges=c(), xmax_ranges=c(),
    xmin_interval=c(), xmax_interval=c(),
    ymin_interval=c(), ymax_interval=c(),
    lim_x_min=NA, lim_x_max=NA, lim_y_min=-NA, lim_y_max=NA,
    clean=TRUE, scale=1) {

    df_points = data.frame(x=x_points, y=y_points);
    df_rectangles = data.frame(xmin=xmin_ranges, xmax=xmax_ranges);
    df_blocks = data.frame(
        xmin=xmin_interval, xmax=xmax_interval,
        ymin=ymin_interval, ymax=ymax_interval
        );

    # define limits, if not defined
    if(is.na(lim_x_min)) {
        lim_x_min = min(c(df_points$x, df_rectangles$xmin, df_blocks$xmin));
    }

    if(is.na(lim_x_max)) {
        lim_x_max = max(c(df_points$x, df_rectangles$xmax, df_blocks$xmax));
    }

    if(is.na(lim_y_min)) {
        lim_y_min = min(c(df_points$y, df_rectangles$ymin, df_blocks$ymin));
    }

    if(is.na(lim_y_max)) {
        lim_y_max = max(c(df_points$y, df_rectangles$ymax, df_blocks$ymax));
    }

    gg = ggplot();

    # if defined, print the ranges
    if(length(xmin_ranges) > 0 && length(xmax_ranges) > 0) {
        gg = gg + geom_rect(
            data=df_rectangles,
            aes_string(
                xmin="xmin", xmax="xmax",
                ymin=-Inf, ymax=Inf, inherit.ae=FALSE),
            size=0.5 * scale,
            color=alpha("darkgreen", 0.5), fill="darkgreen", alpha=0.1);
    }

    # then print the points
    gg = gg + geom_point(
        data=df_points,
        aes_string("x", "y", inherit.ae=FALSE),
        size=3 * scale, color="darkblue", alpha=0.5);

    # finally, if defined, print the blocks
    if(
        length(xmin_interval) > 0 && length(xmax_interval) > 0 &&
            length(ymin_interval) > 0 && length(ymax_interval) > 0
    ) {
        gg = gg + geom_rect(
            data=df_blocks,
            aes_string(
                xmin="xmin", xmax="xmax",
                ymin="ymin", ymax="ymax", inherit.ae=FALSE),
            size=0.5 * scale,
            color=alpha("darkred", 1), fill="darkred", alpha=0.5);
    }

    # defines the limites
    gg = gg + coord_cartesian(
        xlim=c(lim_x_min, lim_x_max), ylim=c(lim_y_min, lim_y_max));

    if(clean) {
        gg = gg + scale_x_continuous(expand=c(0,0));
        # start from the top to the bottom
        gg = gg + scale_y_continuous(expand=c(0,0), trans="reverse");

        gg = gg + labs(x=NULL, y=NULL, title=NULL);
        gg = gg + theme(panel.background=element_rect(
            fill="transparent", colour=NA))
        gg = gg + theme(plot.background=element_rect(
            fill="transparent", colour=NA))
        gg = gg + theme(panel.grid=element_blank())
        gg = gg + theme(panel.border=element_blank())
        gg = gg + theme(plot.margin=unit(c(0,0,0,0), "null"))
        gg = gg + theme(panel.margin=unit(c(0,0,0,0), "null"))
        gg = gg + theme(axis.ticks=element_blank())
        gg = gg + theme(axis.text=element_blank())
        gg = gg + theme(axis.title=element_blank())
        gg = gg + theme(axis.line=element_blank())
        gg = gg + theme(legend.position="none")
        gg = gg + theme(axis.ticks.length=unit(0, "null"))
        gg = gg + theme(legend.margin=unit(0, "null"))
    } else {
        # start from the top to the bottom
        gg = gg + scale_y_continuous(trans="reverse");
    }

    return(gg);
}
plotSequencePositionsRangesAndIntervals = cmpfun(
    plotSequencePositionsRangesAndIntervals_src);



# # test plot function
# plotSequencePositionsRangesAndIntervals(
#     rnorm(100,2), rnorm(100,2),
#     xmin_ranges=c(1, 2, 3), xmax_ranges=c(1.5, 2.5, 4),
#     xmin_interval=c(3, 6), xmax_interval=c(4, 8),
#     ymin_interval=c(2, 4), ymax_interval=c(5, 10),
#     lim_x_min=0, lim_x_max=10, lim_y_min=0, lim_y_max=10,
#     clean=FALSE, scale=5);



# configuring variables
config = new.env(hash=TRUE, parent=emptyenv());
config$max_sequence_length_to_plot = Inf;
config$min_sequence_length_to_plot = 0;
config$plot_scale = 5;
config$max_length_plot_limit = 100000;
#config$per_sequence_plot_requires_a_range_with_min_pos = 5;
#config$per_sequence_plot_requires_a_block_with_min_area = 5;
config$per_sequence_plot_block_requires_min_width_to_be_drawn = 5;
config$per_length_plot_image_type = "png";
config$per_sequence_plot_image_type = "svg";
config$background_img_size = "800px";



# configuring html basics
html = new.env(hash=TRUE, parent=emptyenv());
html$pre_title = c(
    "<!DOCTYPE html>",
    "<html>",
    "  <head>",
    "    <title>");
html$post_title_pre_container = c(
    "    </title>",
    "    <style type=\"text/css\">",
    "      .container {margin: 0 auto; padding: 3px;}",
    "      .content {float: left; margin: 3px;}",
    "      .content.first {clear: left;}",
    "      .clearfix:after {",
    "          content: \".\"; display: block; height: 0;",
    "          clear: both; visibility: hidden;}",
    "    </style>",
    "  </head>",
    "  <body>",
    "    <div class=\"container\">",
    "      <div class=\"clearfix\">");
html$post_container = c(
    "      </div>",
    "    </div>",
    "  </body>",
    "</html>");



# evaluating arguments
args = commandArgs(TRUE);
#cat("Arguments:\n");
#cat(args, "\n");
# examples:
#args = c();
#args[1] = "data/100_sax-25_original.csv";
#args[2] = "results/inline-100_orientation-original/sax-25/json/I100_Ooriginal_S25_FS100_FB100_MS0.json";
#args[3] = "results/inline-100_orientation-original/sax-25/img/spatial-100/block-100/stretch-0";
#args[4] = "data/inline_100_951x462.jpg";
#cat("    args:", args, "\n");

vars = new.env(hash=TRUE, parent=emptyenv());
vars$csv_database = args[1];
vars$input_file_json = args[2];
vars$output_img_dir = args[3];
vars$background_img_src = args[4];

vars$background_img = basename(vars$background_img_src);
vars$base_filename = sub(
    file.path(dirname(vars$input_file_json), ""), "", vars$input_file_json);
vars$base_filename = sub("[.][^.]*$", "", vars$base_filename, perl=TRUE);



# load original database to know it size
vars$database = read.table(
    file=vars$csv_database, header=TRUE, fill=TRUE, as.is=TRUE,
    stringsAsFactors=FALSE, sep=",", quote=""
);
vars$lim_database_x_min = 0;
vars$lim_database_x_max = nrow(vars$database);
vars$database_x_size = vars$lim_database_x_max - vars$lim_database_x_min;
vars$lim_database_y_min = 0;
vars$lim_database_y_max = ncol(vars$database);
vars$database_y_size = vars$lim_database_y_max - vars$lim_database_y_min;



# loading json data
#cat("Loading json data", vars$input_file_json, "...");
json_data = fromJSON(file=vars$input_file_json, method="C");
#cat(" [DONE]\n")

if(is.null(json_data) || length(json_data) < 1) {
    cat("Empty json data!\n");
    quit(status=1);
}

solid_sequences = json_data$solid_sequences;
solid_blocks = json_data$solid_blocks;

if(length(solid_sequences) != length(solid_blocks)) {
    cat("Num of solid sequence lengths",
        "expected to be equal to num of solid blocks length!\n");
    quit(status=1);
}

# copy background image for html
system(paste("cp -f", vars$background_img_src, vars$output_img_dir));

# preparing html initial file
per_length_index_file = file(file.path(
    vars$output_img_dir, "index.html"));
per_length_index_lines = c(
    html$pre_title, vars$base_filename, html$post_title_pre_container);

# start the iterations, for each json data grouped by length
for(iteration in 1:length(solid_sequences)) {
    cat("Iteration:", iteration);

    sequence_data_by_length = solid_sequences[[iteration]];

    if(
        is.null(sequence_data_by_length$length) ||
            is.null(sequence_data_by_length$sequences) ||
            length(sequence_data_by_length$sequences) < 1
    ) {
        cat("\nEmpty sequence data iteration", iteration,
            "of length", sequence_data_by_length$length, "\n");
        #cat("Data:\n");
        #dput(sequence_data_by_length);
        next;
    }

    sequence_length = sequence_data_by_length$length;
    sequence_data = sequence_data_by_length$sequences;

    # limit sequence plot to a length sample

    if(sequence_length < config$min_sequence_length_to_plot) {
        cat(
            "\t[WARN] Skipping sequences data of sequence length <",
            config$mi_sequence_length_to_plot,
            ".\n");
        next;
    }

    if(sequence_length > config$max_sequence_length_to_plot) {
        cat(
            "\t[WARN] Skipping sequences data of sequence length >",
            config$max_sequence_length_to_plot,
            "[BREAK]\n");
        break;
    }

    cat(", length:", sequence_length);
    cat(", ranged sequences:", length(sequence_data));

    if(length(sequence_data) > config$max_length_plot_limit) {
        cat("\n", length(sequence_data),
            "are too much sequences to plot just only a single image",
            "with all of them\n");
        next;
    }

    ###
    # plotting all the sequences of same length
    ###

    cat(", computing per-len data...");

    xmin_ranges = c(); # will be empty, no ranges
    xmax_ranges = c(); # will be empty, no ranges
    x_points = c();
    y_points = c();

    for(j in 1:length(sequence_data)) {
        sequence_data_item = sequence_data[[j]];
        sequence = sequence_data_item$sequence;
        spaces = sequence_data_item$spaces;
        times = sequence_data_item$times;

        x_points = c(x_points, spaces);
        y_points = c(y_points, times);
    }

    filename_by_len = file.path(
        vars$output_img_dir,
        paste(sequence_length, ".",
              config$per_length_plot_image_type, sep="")
    );

    utils$dev_open_file(
        filename_by_len, vars$database_x_size, vars$database_y_size,
        config$plot_scale);
    plot(plotSequencePositionsRangesAndIntervals(
        x_points, y_points,
        xmin_ranges=xmin_ranges, xmax_ranges=xmax_ranges,
        lim_x_min=vars$lim_database_x_min, lim_x_max=vars$lim_database_x_max,
        lim_y_min=vars$lim_database_y_min, lim_y_max=vars$lim_database_y_max,
        scale=config$plot_scale));
    invisible(dev.off());

    per_length_index_lines = c(per_length_index_lines,
        "        <div class=\"content first\">",
        paste(
            "          <img style=\"",
            "background:url(", vars$background_img, ");",
            "background-size:cover;",
            "width:", config$background_img_size ,";\"",
            " src=\"", sequence_length, ".",
            config$per_length_plot_image_type,
            "\" alt=\"\"/>", sep=""),
        "        </div>",
        paste("        <div class=\"content\">",
              "<a href=\"by_length_and_sequence/", sequence_length, ".html\">",
              "sequences of length ", sequence_length, "</a></div>", sep=""));

    ####
    # plot an image with all ranges and blocks for each sequence
    ####

    # organize the data by sequence

    cat(", computing per-seq. data...");

    seq_plotd = new.env(hash=TRUE, parent=emptyenv());

    # ranges

    for(j in 1:length(sequence_data)) {
        sequence_data_item = sequence_data[[j]];
        sequence = sequence_data_item$sequence;

        if(! exists(sequence, seq_plotd)) {
            seq_plotd[[sequence]] = new.env(hash=TRUE, parent=emptyenv());
#           seq_plotd[[sequence]]$range_with_min_pos = FALSE;
#           seq_plotd[[sequence]]$block_with_min_area = FALSE;
            seq_plotd[[sequence]]$min_width_to_be_drawn = FALSE;
            seq_plotd[[sequence]]$xmin_ranges = c();
            seq_plotd[[sequence]]$xmax_ranges = c();
            seq_plotd[[sequence]]$xmin_interval = c();
            seq_plotd[[sequence]]$xmax_interval = c();
            seq_plotd[[sequence]]$ymin_interval = c();
            seq_plotd[[sequence]]$ymax_interval = c();
            seq_plotd[[sequence]]$x_points = c();
            seq_plotd[[sequence]]$y_points = c();
        }

        start = sequence_data_item$start;
        end = sequence_data_item$end;
        spaces = sequence_data_item$spaces;
        times = sequence_data_item$times;

        # per_sequence_plot_requires_a_range_with_min_pos condition
#         if(
#             (! seq_plotd[[sequence]]$range_with_min_pos) &&
#                 (length(spaces) >=
#                      config$per_sequence_plot_requires_a_range_with_min_pos)
#             ) {
#             seq_plotd[[sequence]]$range_with_min_pos = TRUE;
#         }

        seq_plotd[[sequence]]$xmin_ranges = c(
            seq_plotd[[sequence]]$xmin_ranges, start);
        seq_plotd[[sequence]]$xmax_ranges = c(
            seq_plotd[[sequence]]$xmax_ranges, end);
        seq_plotd[[sequence]]$x_points = c(
            seq_plotd[[sequence]]$x_points, spaces);
        seq_plotd[[sequence]]$y_points = c(
            seq_plotd[[sequence]]$y_points, times);
    }

    # blocks

    solid_blocks_data_by_length = solid_blocks[[iteration]];

    if(
        is.null(solid_blocks_data_by_length$length) ||
            is.null(solid_blocks_data_by_length$blocks) ||
            length(solid_blocks_data_by_length$blocks) < 1
        ) {
        cat("\n\tError in block data.\n");
    } else if(solid_blocks_data_by_length$length != sequence_length) {
        cat("\n\tThe length of sequences in the same index of solid sequences",
            "and solid blocks must be the same.\n");
    } else {
        solid_blocks_data = solid_blocks_data_by_length$blocks;

        for(j in 1:length(solid_blocks_data)) {
            block_data_item = solid_blocks_data[[j]];
            sequence = block_data_item$sequence;

            if(! exists(sequence, seq_plotd)) {
                cat("\t[WARN] STRANGE!!!",
                    "Sequence is in a block and is not in a range...\n");
                seq_plotd[[sequence]] = new.env(hash=TRUE, parent=emptyenv());
#               seq_plotd[[sequence]]$range_with_min_pos = FALSE;
#               seq_plotd[[sequence]]$block_with_min_area = FALSE;
                seq_plotd[[sequence]]$min_width_to_be_drawn = FALSE;
                seq_plotd[[sequence]]$xmin_ranges = c();
                seq_plotd[[sequence]]$xmax_ranges = c();
                seq_plotd[[sequence]]$xmin_interval = c();
                seq_plotd[[sequence]]$xmax_interval = c();
                seq_plotd[[sequence]]$ymin_interval = c();
                seq_plotd[[sequence]]$ymax_interval = c();
                seq_plotd[[sequence]]$x_points = c();
                seq_plotd[[sequence]]$y_points = c();
            }

            r_start = block_data_item$r_start;
            r_end = block_data_item$r_end;
            i_start = block_data_item$i_start;
            i_end = block_data_item$i_end;

            # per_sequence_plot_requires_a_block_with_min_area condition
#             if(
#                 (! seq_plotd[[sequence]]$block_with_min_area) &&
#                     (((r_end - r_start + 1) * (i_end - i_start + 1)) >=
#                     config$per_sequence_plot_requires_a_block_with_min_area)
#                 ) {
#                 seq_plotd[[sequence]]$block_with_min_area = TRUE;
#             }

            if(
                (r_end - r_start + 1) >=
                config$per_sequence_plot_block_requires_min_width_to_be_drawn
                ) {
                seq_plotd[[sequence]]$min_width_to_be_drawn = TRUE;

                seq_plotd[[sequence]]$xmin_interval = c(
                    seq_plotd[[sequence]]$xmin_interval, r_start);
                seq_plotd[[sequence]]$xmax_interval = c(
                    seq_plotd[[sequence]]$xmax_interval, r_end);
                seq_plotd[[sequence]]$ymin_interval = c(
                    seq_plotd[[sequence]]$ymin_interval, i_start);
                seq_plotd[[sequence]]$ymax_interval = c(
                    seq_plotd[[sequence]]$ymax_interval, i_end);
            }
        }
    }


    cat(", plotting...");

    # plot all sequence data

    dir.create(
        file.path(
            vars$output_img_dir, "by_length_and_sequence", sequence_length
        ),
        showWarnings=FALSE, recursive=TRUE, mode="2755"
    );

    filename_by_seq = file.path(
        vars$output_img_dir, "by_length_and_sequence", sequence_length,
        paste("%d", ".", config$per_sequence_plot_image_type, sep="")
    );

    k = 0;

    something_plotted = FALSE;

    # producing the entire bunch of plots at one time,
    # a img with all the points and ranges for each sequence
    utils$dev_open_file(
        filename_by_seq, vars$database_x_size, vars$database_y_size,
        config$plot_scale);

    for(key in ls(seq_plotd)) {
        k = k + 1;
        if(
#           seq_plotd[[key]]$range_with_min_pos &&
#           seq_plotd[[key]]$block_with_min_area &&
            seq_plotd[[sequence]]$min_width_to_be_drawn
            ) {
            plot(plotSequencePositionsRangesAndIntervals(
                seq_plotd[[key]]$x_points,
                seq_plotd[[key]]$y_points,
                xmin_ranges=seq_plotd[[key]]$xmin_ranges,
                xmax_ranges=seq_plotd[[key]]$xmax_ranges,
                xmin_interval=seq_plotd[[key]]$xmin_interval,
                xmax_interval=seq_plotd[[key]]$xmax_interval,
                ymin_interval=seq_plotd[[key]]$ymin_interval,
                ymax_interval=seq_plotd[[key]]$ymax_interval,
                lim_x_min=vars$lim_database_x_min,
                lim_x_max=vars$lim_database_x_max,
                lim_y_min=vars$lim_database_y_min,
                lim_y_max=vars$lim_database_y_max,
                scale=config$plot_scale));
            something_plotted = TRUE;
        }
    }

    invisible(dev.off());

    cat(", sequences:", k);

    if(something_plotted) {
        # renaming to have the sequence name
        k = 0;

        per_sequence_index_file = file(file.path(
            vars$output_img_dir, "by_length_and_sequence",
            paste(sequence_length, ".html", sep="")));
        per_sequence_index_lines = c(
            html$pre_title, sequence_length, html$post_title_pre_container);

        for(key in ls(seq_plotd)) {
            if(
#               seq_plotd[[key]]$range_with_min_pos &&
#               seq_plotd[[key]]$block_with_min_area &&
                seq_plotd[[sequence]]$min_width_to_be_drawn
                ) {
                k = k + 1;

                # rename to final image file
                file.rename(
                    file.path(
                        vars$output_img_dir, "by_length_and_sequence",
                        sequence_length,
                        paste(k, ".",
                              config$per_sequence_plot_image_type, sep="")),
                    file.path(
                        vars$output_img_dir, "by_length_and_sequence",
                        sequence_length,
                        paste(key, ".",
                              config$per_sequence_plot_image_type, sep="")));

                # create a separated html
                per_len_sequence_index_file = file(file.path(
                    vars$output_img_dir, "by_length_and_sequence",
                    paste(sequence_length, "/", key, ".html", sep="")));
                writeLines(
                    c(
                        html$pre_title, key,
                        html$post_title_pre_container,
                        paste(
                            "        <div class=\"content first\">",
                            key, "</div>"),
                        paste(
                            "        <div class=\"content first\">",
                            "<img style=\"",
                            "background:url(../../", vars$background_img, ");",
                            "background-size:cover;",
                            "width:", config$background_img_size ,";\"",
                            " src=\"",
                            key, ".", config$per_sequence_plot_image_type,
                            "\" alt=\"\" /></div>", sep=""),
                        html$post_container),
                    per_len_sequence_index_file);
                close(per_len_sequence_index_file);

                # add an entry to the per-length html
                per_sequence_index_lines = c(
                    per_sequence_index_lines,
                    "        <div class=\"content first\">",
                    paste(
                        "          <a href=\"", sequence_length, "/",
                        key, ".html\">", sep=""),
                    paste(
                        "            <img style=\"",
                        "background:url(../", vars$background_img, ");",
                        "background-size:cover;",
                        "width:800px;\"",
                        " src=\"", sequence_length, "/",
                        key, ".", config$per_sequence_plot_image_type,
                        "\" alt=\"\" />", sep=""),
                    "          </a>",
                    "        </div>",
                    paste("        <div class=\"content\">",
                          key, "</div>", sep=""));
            }
        }

        cat(", plotted:", k);

        writeLines(c(per_sequence_index_lines, html$post_container),
                   per_sequence_index_file);
        close(per_sequence_index_file);

    } else {
        # removing unuseful directory
        #unlink(     # [CMP] unlink won't work
        system(paste('rmdir', file.path(
            vars$output_img_dir, "by_length_and_sequence", sequence_length)));
    }

    cat(" [DONE]\n");
}

# completing html file
writeLines(c(per_length_index_lines, html$post_container),
           per_length_index_file);
close(per_length_index_file);

# write a file that grants that this plot is complete for this product
result = file.create(file.path(vars$output_img_dir, "complete"));
