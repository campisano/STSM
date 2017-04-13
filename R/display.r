#setwd("/home/shared/develop/projects/CEFET/mestrado/SIM");

# include utility file
source(file="R/utils.r", chdir=TRUE);

utils$setVerbose();

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
plotSequencePositionsRangesAndBlocks_src = function(
    x_points, y_points,
    xmin_ranges=c(), xmax_ranges=c(),
    xmin_blocks=c(), xmax_blocks=c(),
    ymin_blocks=c(), ymax_blocks=c(),
    lim_x_min=NA, lim_x_max=NA, lim_y_min=-NA, lim_y_max=NA,
    clean=TRUE, scale=1) {

    df_points = data.frame(x=x_points, y=y_points);
    df_rectangles = data.frame(xmin=xmin_ranges, xmax=xmax_ranges);
    df_blocks = data.frame(
        xmin=xmin_blocks, xmax=xmax_blocks,
        ymin=ymin_blocks, ymax=ymax_blocks
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
                ymin=-Inf, ymax=Inf),
            size=0.5 * scale,
            color=alpha("green", 0.5), fill="darkgreen", alpha=0.1);
    }

    # then print the points
    if(length(x_points) > 0 && length(y_points) > 0) {
        gg = gg + geom_point(
            # shape=15,  # square
            shape=17,  # triangle
            # shape=18,  # diamond
            data=df_points,
            aes_string("x", "y"),
            size=2 * scale, color="darkblue");
    }

    # finally, if defined, print the blocks
    if(
        length(xmin_blocks) > 0 && length(xmax_blocks) > 0 &&
            length(ymin_blocks) > 0 && length(ymax_blocks) > 0
    ) {
        gg = gg + geom_rect(
            data=df_blocks,
            aes_string(
                xmin="xmin", xmax="xmax",
                ymin="ymin", ymax="ymax"),
            size=0.5 * scale,
            color=alpha("red", 1), fill="darkred", alpha=0.5);
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
            fill="transparent", colour=NA));
        gg = gg + theme(plot.background=element_rect(
            fill="transparent", colour=NA));
        gg = gg + theme(panel.grid=element_blank());
        gg = gg + theme(panel.border=element_blank());
        gg = gg + theme(plot.margin=unit(c(0,0,0,0), "null"));
        #gg = gg + theme(panel.margin=unit(c(0,0,0,0), "null"));
        gg = gg + theme(axis.ticks=element_blank());
        gg = gg + theme(axis.text=element_blank());
        gg = gg + theme(axis.title=element_blank());
        gg = gg + theme(axis.line=element_blank());
        gg = gg + theme(legend.position="none");
        gg = gg + theme(axis.ticks.length=unit(0, "null"));
        #gg = gg + theme(legend.margin=unit(0, "null"));
    } else {
        # start from the top to the bottom
        gg = gg + scale_y_continuous(trans="reverse");
    }

    return(gg);
}
plotSequencePositionsRangesAndBlocks = cmpfun(
    plotSequencePositionsRangesAndBlocks_src);



# # test plot function
# plotSequencePositionsRangesAndBlocks(
#     rnorm(100,2), rnorm(100,2),
#     xmin_ranges=c(1, 2, 3), xmax_ranges=c(1.5, 2.5, 4),
#     xmin_blocks=c(3, 6), xmax_blocks=c(4, 8),
#     ymin_blocks=c(2, 4), ymax_blocks=c(5, 10),
#     lim_x_min=0, lim_x_max=10, lim_y_min=0, lim_y_max=10,
#     clean=FALSE, scale=5);



# configuring variables
config = utils$newDict();
config$min_sequence_length_to_plot = 2;
config$max_sequence_length_to_plot = Inf;
config$plot_scale_preview = 1;
config$plot_scale = 5;
config$max_length_plot_limit = 100000;
config$block_area_start_count_from_y_frac = 0.1;   # 10%
config$block_area_start_count_from_y = 0;  # will be calculated
config$per_sequence_plot_block_requires_min_width_to_be_drawn = 5;
config$plot_only_ranges_that_contains_blocks = TRUE;
config$per_length_plot_image_type = "png";
config$per_sequence_plot_preview_image_type = "png";
config$per_sequence_plot_image_type = "svg";
config$preview_img_size = "800px";
config$full_img_size = "1440px";



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

vars = utils$newDict();
vars$csv_database = args[1];
vars$input_file_json = args[2];
vars$output_img_dir = args[3];
vars$background_img_src = args[4];

vars$background_img = basename(vars$background_img_src);
vars$base_filename = sub(
    file.path(dirname(vars$input_file_json), ""), "", vars$input_file_json);
vars$base_filename = sub("[.][^.]*$", "", vars$base_filename, perl=TRUE);



# load original database to know it size
vars$database = utils$readCSV(vars$csv_database);
vars$lim_database_x_min = 0;
vars$lim_database_x_max = nrow(vars$database);
vars$database_x_size = vars$lim_database_x_max - vars$lim_database_x_min;
vars$lim_database_y_min = 0;
vars$lim_database_y_max = ncol(vars$database);
vars$database_y_size = vars$lim_database_y_max - vars$lim_database_y_min;
config$block_area_start_count_from_y =
    vars$lim_database_y_min + (
        vars$database_y_size * config$block_area_start_count_from_y_frac);


# loading json data
#cat("Loading json data", vars$input_file_json, "...");
json_data = utils$readJSON(vars$input_file_json);
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
    utils$html.getHTMLpreContentCode(title=vars$base_filename));

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
            config$min_sequence_length_to_plot,
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
        config$plot_scale_preview);
    plot(plotSequencePositionsRangesAndBlocks(
        x_points, y_points,
        xmin_ranges=xmin_ranges, xmax_ranges=xmax_ranges,
        lim_x_min=vars$lim_database_x_min, lim_x_max=vars$lim_database_x_max,
        lim_y_min=vars$lim_database_y_min, lim_y_max=vars$lim_database_y_max,
        scale=config$plot_scale_preview));
    invisible(dev.off());

    per_length_index_lines = c(per_length_index_lines,
        "        <div class=\"content first\">",
        paste(
            "          <img style=\"",
            "background:url(", vars$background_img, ");",
            "background-size:cover;",
            "width:", config$preview_img_size ,";\"",
            " src=\"", sequence_length, ".",
            config$per_length_plot_image_type,
            "\" alt=\"\"/>", sep=""),
        "        </div>",

        "        <div class=\"content\">",
        "          <div class=\"content first\">",
        paste("            <b>Sequences of length ", sequence_length, "</b>", sep=""),
        "          </div>",
        "          <div class=\"content first\">",
        paste("            <a href=\"by_length_and_sequence/", sequence_length, "_ordered.html\">alphabetically ordered</a>", sep=""),
        "          </div>",
        "          <div class=\"content first\">",
        paste("            <a href=\"by_length_and_sequence/", sequence_length, "_ranked.html\">rank ordered</a>", sep=""),
        "          </div>",
        "          <div class=\"content first\">",
        paste("            <a href=\"by_length_and_sequence/", sequence_length, "_ranked_t10.html\">rank top 10</a>", sep=""),
        "          </div>",
        "          <div class=\"content first\">",
        paste("            <a href=\"by_length_and_sequence/", sequence_length, "_ranked_t25.html\">rank top 25</a>", sep=""),
        "          </div>",
        "          <div class=\"content first\">",
        paste("            <a href=\"by_length_and_sequence/", sequence_length, "_ranked_t50.html\">rank top 50</a>", sep=""),
        "          </div>",
        "          <div class=\"content first\">",
        paste("            <a href=\"by_length_and_sequence/", sequence_length, "_ranked_t100.html\">rank top 100</a>", sep=""),
        "          </div>",
        "        </div>");

    ####
    # plot an image with all ranges and blocks for each sequence
    ####

    # organize the data by sequence

    cat(", computing per-seq. data...");

    seq_plotd = utils$newDict();



    ####
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
                seq_plotd[[sequence]] = utils$newDict();
                seq_plotd[[sequence]]$min_width_to_be_drawn = FALSE;
                seq_plotd[[sequence]]$block_area = 0.0;
                seq_plotd[[sequence]]$block_count = 0;
                seq_plotd[[sequence]]$xmin_ranges = c();
                seq_plotd[[sequence]]$xmax_ranges = c();
                seq_plotd[[sequence]]$xmin_blocks = c();
                seq_plotd[[sequence]]$xmax_blocks = c();
                seq_plotd[[sequence]]$ymin_blocks = c();
                seq_plotd[[sequence]]$ymax_blocks = c();
                seq_plotd[[sequence]]$x_points = c();
                seq_plotd[[sequence]]$y_points = c();
            }

            r_start = block_data_item$r_start;
            r_end = block_data_item$r_end;
            i_start = block_data_item$i_start;
            i_end = block_data_item$i_end;

            if(
                (r_end - r_start + 1) >=
                config$per_sequence_plot_block_requires_min_width_to_be_drawn
                ) {
                seq_plotd[[sequence]]$min_width_to_be_drawn = TRUE;

                seq_plotd[[sequence]]$xmin_blocks = c(
                    seq_plotd[[sequence]]$xmin_blocks, r_start);
                seq_plotd[[sequence]]$xmax_blocks = c(
                    seq_plotd[[sequence]]$xmax_blocks, r_end);
                seq_plotd[[sequence]]$ymin_blocks = c(
                    seq_plotd[[sequence]]$ymin_blocks, i_start);
                seq_plotd[[sequence]]$ymax_blocks = c(
                    seq_plotd[[sequence]]$ymax_blocks, i_end);

                if(i_start >= config$block_area_start_count_from_y) {
                    seq_plotd[[sequence]]$block_area =
                        seq_plotd[[sequence]]$block_area +
                            (r_end - r_start + 1) * (i_end - i_start + 1);
                    seq_plotd[[sequence]]$block_count =
                        seq_plotd[[sequence]]$block_count + 1;
                }
            }
        }
    }



    ####
    # ranges

    for(j in 1:length(sequence_data)) {
        sequence_data_item = sequence_data[[j]];
        sequence = sequence_data_item$sequence;

        start = sequence_data_item$start;
        end = sequence_data_item$end;
        spaces = sequence_data_item$spaces;
        times = sequence_data_item$times;

        # skip ranges that not conains blocks, if configured
        if(config$plot_only_ranges_that_contains_blocks) {
            if ((! exists(sequence, seq_plotd)) ||
                    (! seq_plotd[[sequence]]$min_width_to_be_drawn)
                ) {
                next;
            }

            block_found = FALSE;

            for(idx in which(seq_plotd[[sequence]]$xmin_blocks >= start)) {
                if(seq_plotd[[sequence]]$xmax_blocks[[idx]] <= end) {
                    block_found = TRUE;
                    break;
                }
            }

            if(!block_found) {
                next;
            }
        }

        if(! exists(sequence, seq_plotd)) {
            seq_plotd[[sequence]] = utils$newDict();
            seq_plotd[[sequence]]$min_width_to_be_drawn = FALSE;
            seq_plotd[[sequence]]$block_area = 0.0;
            seq_plotd[[sequence]]$block_count = 0;
            seq_plotd[[sequence]]$xmin_ranges = c();
            seq_plotd[[sequence]]$xmax_ranges = c();
            seq_plotd[[sequence]]$xmin_blocks = c();
            seq_plotd[[sequence]]$xmax_blocks = c();
            seq_plotd[[sequence]]$ymin_blocks = c();
            seq_plotd[[sequence]]$ymax_blocks = c();
            seq_plotd[[sequence]]$x_points = c();
            seq_plotd[[sequence]]$y_points = c();
        }

        seq_plotd[[sequence]]$xmin_ranges = c(
            seq_plotd[[sequence]]$xmin_ranges, start);
        seq_plotd[[sequence]]$xmax_ranges = c(
            seq_plotd[[sequence]]$xmax_ranges, end);
        seq_plotd[[sequence]]$x_points = c(
            seq_plotd[[sequence]]$x_points, spaces);
        seq_plotd[[sequence]]$y_points = c(
            seq_plotd[[sequence]]$y_points, times);
    }



    cat(", plotting...");

    # plot all sequence data

    dir.create(
        file.path(
            vars$output_img_dir, "by_length_and_sequence", sequence_length
        ),
        showWarnings=FALSE, recursive=TRUE, mode="2755"
    );

    # producing the entire bunch of plots at one time,
    # any image with all the points, ranges and block for the related sequence
    # preview quality images

    filename_by_seq = file.path(
        vars$output_img_dir, "by_length_and_sequence", sequence_length,
        paste("%d", ".", config$per_sequence_plot_preview_image_type, sep="")
    );

    k = 0;

    something_plotted = FALSE;

    utils$dev_open_file(
        filename_by_seq, vars$database_x_size, vars$database_y_size,
        config$plot_scale_preview);

    for(key in ls(seq_plotd)) {
        k = k + 1;
        if(seq_plotd[[key]]$min_width_to_be_drawn) {
            plot(plotSequencePositionsRangesAndBlocks(
                seq_plotd[[key]]$x_points,
                seq_plotd[[key]]$y_points,
                xmin_ranges=seq_plotd[[key]]$xmin_ranges,
                xmax_ranges=seq_plotd[[key]]$xmax_ranges,
                xmin_blocks=seq_plotd[[key]]$xmin_blocks,
                xmax_blocks=seq_plotd[[key]]$xmax_blocks,
                ymin_blocks=seq_plotd[[key]]$ymin_blocks,
                ymax_blocks=seq_plotd[[key]]$ymax_blocks,
                lim_x_min=vars$lim_database_x_min,
                lim_x_max=vars$lim_database_x_max,
                lim_y_min=vars$lim_database_y_min,
                lim_y_max=vars$lim_database_y_max,
                scale=config$plot_scale_preview));
            something_plotted = TRUE;
        }
    }

    invisible(dev.off());

    # producing the entire bunch of plots at one time,
    # any image with all the points, ranges and block for the related sequence
    # full quality images

    filename_by_seq = file.path(
        vars$output_img_dir, "by_length_and_sequence", sequence_length,
        paste("%d", ".", config$per_sequence_plot_image_type, sep="")
    );

    k = 0;

    something_plotted = FALSE;

    utils$dev_open_file(
        filename_by_seq, vars$database_x_size, vars$database_y_size,
        config$plot_scale);

    for(key in ls(seq_plotd)) {
        k = k + 1;
        if(seq_plotd[[key]]$min_width_to_be_drawn) {
            plot(plotSequencePositionsRangesAndBlocks(
                seq_plotd[[key]]$x_points,
                seq_plotd[[key]]$y_points,
                xmin_ranges=seq_plotd[[key]]$xmin_ranges,
                xmax_ranges=seq_plotd[[key]]$xmax_ranges,
                xmin_blocks=seq_plotd[[key]]$xmin_blocks,
                xmax_blocks=seq_plotd[[key]]$xmax_blocks,
                ymin_blocks=seq_plotd[[key]]$ymin_blocks,
                ymax_blocks=seq_plotd[[key]]$ymax_blocks,
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

    k = 0;

    if(something_plotted) {
        # renaming to have the sequence name

        per_sequence_index_file = file(file.path(
            vars$output_img_dir, "by_length_and_sequence",
            paste(sequence_length, "_ordered.html", sep="")));
        per_sequence_index_lines = c(
            utils$html.getHTMLpreContentCode(title=sequence_length));

        for(key in ls(seq_plotd)) {
            if(seq_plotd[[key]]$min_width_to_be_drawn) {
                k = k + 1;

                # rename to final sequence preview image file
                file.rename(
                    file.path(
                        vars$output_img_dir, "by_length_and_sequence",
                        sequence_length,
                        paste(k, ".",
                              config$per_sequence_plot_preview_image_type,
                              sep="")),
                    file.path(
                        vars$output_img_dir, "by_length_and_sequence",
                        sequence_length,
                        paste(key, ".",
                              config$per_sequence_plot_preview_image_type,
                              sep="")));

                # rename to final sequence full quality image file
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

                # create a separated html for this sequence image
                per_len_sequence_index_file = file(file.path(
                    vars$output_img_dir, "by_length_and_sequence",
                    paste(sequence_length, "/", key, ".html", sep="")));
                writeLines(
                    c(
                        utils$html.getHTMLpreContentCode(title=key),
                        paste(
                            "        <div class=\"content first\">",
                            key, "</div>"),
                        paste(
                            "        <div class=\"content first\">",
                            "<img style=\"",
                            "background:url(../../", vars$background_img, ");",
                            "background-size:cover;",
                            "width:", config$full_img_size ,";\"",
                            " src=\"",
                            key, ".", config$per_sequence_plot_image_type,
                            "\" alt=\"\" /></div>", sep=""),
                        utils$html.getHTMLpostContentCode()),
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
                        "width:", config$preview_img_size,";\"",
                        " src=\"", sequence_length, "/",
                        key, ".", config$per_sequence_plot_preview_image_type,
                        "\" alt=\"\" />", sep=""),
                    "          </a>",
                    "        </div>",
                    paste("        <div class=\"content\">",
                          key, "</div>", sep=""));
            }
        }

        writeLines(
            c(
                per_sequence_index_lines,
                utils$html.getHTMLpostContentCode()),
            per_sequence_index_file);
        close(per_sequence_index_file);

        # write an html with link of sequence images ranked by mean_area
        {
            sequences = c();
            mean_areas = c();

            for(key in ls(seq_plotd)) {
                if(seq_plotd[[key]]$min_width_to_be_drawn) {
                    sequences = c(sequences, key);

                    if(seq_plotd[[key]]$block_count == 0) {
                        mean_areas = c(mean_areas, 0);
                    } else {
                        mean_areas = c(
                            mean_areas,
                                seq_plotd[[key]]$block_area
                                / seq_plotd[[key]]$block_count);
                    }
                }
            }

            data_frame = data.frame(sequences, mean_areas);
            colnames(data_frame) = c("sequences", "mean_areas");
            data_frame = data_frame[with(data_frame, order(-mean_areas)), ];

            # create the html file with all ranks
            per_sequence_ranked_index_file = file(file.path(
                vars$output_img_dir, "by_length_and_sequence",
                paste(sequence_length, "_ranked.html", sep="")));
            per_sequence_ranked_index_lines = c(
                utils$html.getHTMLpreContentCode(title=sequence_length));

            for(i in 1:nrow(data_frame)) {
                sequence = data_frame[i,]$sequences;
                mean_area = data_frame[i,]$mean_areas;

                # add an entry to the per-length html
                per_sequence_ranked_index_lines = c(
                    per_sequence_ranked_index_lines,
                    "        <div class=\"content first\">",
                    paste(
                        "          <a href=\"", sequence_length, "/",
                        sequence, ".html\">", sep=""),
                    paste(
                        "            <img style=\"",
                        "background:url(../", vars$background_img, ");",
                        "background-size:cover;",
                        "width:", config$preview_img_size,";\"",
                        " src=\"", sequence_length, "/",
                        sequence, ".",
                        config$per_sequence_plot_preview_image_type,
                        "\" alt=\"\" />", sep=""),
                    "          </a>",
                    "        </div>",
                    paste("        <div class=\"content\">",
                          sequence, " - ", mean_area, "</div>", sep=""));
            }

            writeLines(
                c(
                    per_sequence_ranked_index_lines,
                    utils$html.getHTMLpostContentCode()),
                per_sequence_ranked_index_file);
            close(per_sequence_ranked_index_file);

            # create the html file with top 10 ranks
            per_sequence_ranked_index_file = file(file.path(
                vars$output_img_dir, "by_length_and_sequence",
                paste(sequence_length, "_ranked_t10.html", sep="")));
            per_sequence_ranked_index_lines = c(
                utils$html.getHTMLpreContentCode(title=sequence_length));

            for(i in 1:nrow(data_frame)) {
                if(i > 10) {
                    break;
                }
                sequence = data_frame[i,]$sequences;
                mean_area = data_frame[i,]$mean_areas;

                # add an entry to the per-length html
                per_sequence_ranked_index_lines = c(
                    per_sequence_ranked_index_lines,
                    "        <div class=\"content first\">",
                    paste(
                        "          <a href=\"", sequence_length, "/",
                        sequence, ".html\">", sep=""),
                    paste(
                        "            <img style=\"",
                        "background:url(../", vars$background_img, ");",
                        "background-size:cover;",
                        "width:", config$preview_img_size,";\"",
                        " src=\"", sequence_length, "/",
                        sequence, ".",
                        config$per_sequence_plot_preview_image_type,
                        "\" alt=\"\" />", sep=""),
                    "          </a>",
                    "        </div>",
                    paste("        <div class=\"content\">",
                          sequence, " - ", mean_area, "</div>", sep=""));
            }

            writeLines(
                c(
                    per_sequence_ranked_index_lines,
                    utils$html.getHTMLpostContentCode()),
                per_sequence_ranked_index_file);
            close(per_sequence_ranked_index_file);

            # create the html file with top 25 ranks
            per_sequence_ranked_index_file = file(file.path(
                vars$output_img_dir, "by_length_and_sequence",
                paste(sequence_length, "_ranked_t25.html", sep="")));
            per_sequence_ranked_index_lines = c(
                utils$html.getHTMLpreContentCode(title=sequence_length));

            for(i in 1:nrow(data_frame)) {
                if(i > 25) {
                    break;
                }
                sequence = data_frame[i,]$sequences;
                mean_area = data_frame[i,]$mean_areas;

                # add an entry to the per-length html
                per_sequence_ranked_index_lines = c(
                    per_sequence_ranked_index_lines,
                    "        <div class=\"content first\">",
                    paste(
                        "          <a href=\"", sequence_length, "/",
                        sequence, ".html\">", sep=""),
                    paste(
                        "            <img style=\"",
                        "background:url(../", vars$background_img, ");",
                        "background-size:cover;",
                        "width:", config$preview_img_size,";\"",
                        " src=\"", sequence_length, "/",
                        sequence, ".",
                        config$per_sequence_plot_preview_image_type,
                        "\" alt=\"\" />", sep=""),
                    "          </a>",
                    "        </div>",
                    paste("        <div class=\"content\">",
                          sequence, " - ", mean_area, "</div>", sep=""));
            }

            writeLines(
                c(
                    per_sequence_ranked_index_lines,
                    utils$html.getHTMLpostContentCode()),
                per_sequence_ranked_index_file);
            close(per_sequence_ranked_index_file);

            # create the html file with top 50 ranks
            per_sequence_ranked_index_file = file(file.path(
                vars$output_img_dir, "by_length_and_sequence",
                paste(sequence_length, "_ranked_t50.html", sep="")));
            per_sequence_ranked_index_lines = c(
                utils$html.getHTMLpreContentCode(title=sequence_length));

            for(i in 1:nrow(data_frame)) {
                if(i > 50) {
                    break;
                }
                sequence = data_frame[i,]$sequences;
                mean_area = data_frame[i,]$mean_areas;

                # add an entry to the per-length html
                per_sequence_ranked_index_lines = c(
                    per_sequence_ranked_index_lines,
                    "        <div class=\"content first\">",
                    paste(
                        "          <a href=\"", sequence_length, "/",
                        sequence, ".html\">", sep=""),
                    paste(
                        "            <img style=\"",
                        "background:url(../", vars$background_img, ");",
                        "background-size:cover;",
                        "width:", config$preview_img_size,";\"",
                        " src=\"", sequence_length, "/",
                        sequence, ".",
                        config$per_sequence_plot_preview_image_type,
                        "\" alt=\"\" />", sep=""),
                    "          </a>",
                    "        </div>",
                    paste("        <div class=\"content\">",
                          sequence, " - ", mean_area, "</div>", sep=""));
            }

            writeLines(
                c(
                    per_sequence_ranked_index_lines,
                    utils$html.getHTMLpostContentCode()),
                per_sequence_ranked_index_file);
            close(per_sequence_ranked_index_file);

            # create the html file with top 100 ranks
            per_sequence_ranked_index_file = file(file.path(
                vars$output_img_dir, "by_length_and_sequence",
                paste(sequence_length, "_ranked_t100.html", sep="")));
            per_sequence_ranked_index_lines = c(
                utils$html.getHTMLpreContentCode(title=sequence_length));

            for(i in 1:nrow(data_frame)) {
                if(i > 100) {
                    break;
                }
                sequence = data_frame[i,]$sequences;
                mean_area = data_frame[i,]$mean_areas;

                # add an entry to the per-length html
                per_sequence_ranked_index_lines = c(
                    per_sequence_ranked_index_lines,
                    "        <div class=\"content first\">",
                    paste(
                        "          <a href=\"", sequence_length, "/",
                        sequence, ".html\">", sep=""),
                    paste(
                        "            <img style=\"",
                        "background:url(../", vars$background_img, ");",
                        "background-size:cover;",
                        "width:", config$preview_img_size,";\"",
                        " src=\"", sequence_length, "/",
                        sequence, ".",
                        config$per_sequence_plot_preview_image_type,
                        "\" alt=\"\" />", sep=""),
                    "          </a>",
                    "        </div>",
                    paste("        <div class=\"content\">",
                          sequence, " - ", mean_area, "</div>", sep=""));
            }

            writeLines(
                c(
                    per_sequence_ranked_index_lines,
                    utils$html.getHTMLpostContentCode()),
                per_sequence_ranked_index_file);
            close(per_sequence_ranked_index_file);
        }

    } else {
        # removing unuseful directory
        #unlink(     # [CMP] unlink won't work
        system(paste('rmdir', file.path(
            vars$output_img_dir, "by_length_and_sequence", sequence_length)));
    }

    cat(", plotted:", k);

    cat(" [DONE]\n");
}

# completing html file
writeLines(
    c(
        per_length_index_lines,
        utils$html.getHTMLpostContentCode()),
    per_length_index_file);
close(per_length_index_file);

# write a file that grants that this plot is complete for this product
result = file.create(file.path(vars$output_img_dir, "complete"));
