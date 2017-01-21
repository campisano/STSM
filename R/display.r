# include utility file
source(file="R/utils.r")



# configure detailed error outputs
utils$verbose();



# loading dependences
loaded_libs = utils$loadLibs(
    c("rjson", "TSMining", "ggplot2", "grid", "scales"));



# defining a specific plot function
plotRangedSequences = function(
    x_points, y_points, xmin_ranges=c(), xmax_ranges=c(),
    lim_x_min=NA, lim_x_max=NA, lim_y_min=-NA, lim_y_max=NA,
    clean=TRUE, scale=1) {
    df_points = data.frame(x=x_points, y=y_points);
    df_rectangles = data.frame(xmin=xmin_ranges, xmax=xmax_ranges);

    if(is.na(lim_x_min)) {
        lim_x_min = min(c(df_points$x, df_rectangles$xmin));
    }

    if(is.na(lim_x_max)) {
        lim_x_max = max(c(df_points$x, df_rectangles$xmin));
    }

    if(is.na(lim_y_min)) {
        lim_y_min = min(c(df_points$y, df_rectangles$ymin));
    }

    if(is.na(lim_y_max)) {
        lim_y_max = max(c(df_points$y, df_rectangles$ymin));
    }

    gg = ggplot();

    # if defined, print the range
    if(length(xmin_ranges) > 0 && length(xmax_ranges) > 0) {
        gg = gg + geom_rect(
            data=df_rectangles,
            aes(xmin=xmin, xmax=xmax, ymin=-Inf, ymax=Inf, inherit.ae=FALSE),
            size=0.5 * scale,
            color=alpha("darkgreen", 0.3), fill="darkgreen", alpha=0.2);
    }

    # then print the points
    gg = gg + geom_point(
        data=df_points,
        aes(x, y, inherit.ae=FALSE),
        size=3 * scale, color="darkblue", alpha=0.5);

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
        gg = gg + theme(axis.ticks.margin=unit(0, "null"))
        gg = gg + theme(legend.margin=unit(0, "null"))
    } else {
        # start from the top to the bottom
        gg = gg + scale_y_continuous(trans="reverse");
    }

    return(gg);
}



# test plot funtion
# plotRangedSequences(
#     rnorm(100,2), rnorm(100,2),
#     c(1, 2, 3), c(1.5, 2.5, 4),
#     lim_x_min=0, lim_x_max=10, lim_y_min=0, lim_y_max=10,
#     clean=FALSE, scale=config$plot_scale);



# configuring variables
config = new.env(hash=TRUE, parent=emptyenv());
config$plot_scale = 5;
config$max_length_plot_limit = 50000;
config$min_positions_plot_limit = 50;
config$per_length_plot_image_type = "png";
config$per_sequence_plot_image_type = "png";
config$background_img_size = "800px";

# configuring html basics
html = new.env(hash=TRUE, parent=emptyenv());
html$html_pre_title = c(
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
#args[4] = "data/inline_100_4755x2310.jpg";
#setwd("/home/shared/develop/projects/CEFET/mestrado/SIM");
cat("    args:", args, "\n");

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
cat("Loading json data", vars$input_file_json, "...");
json_data = fromJSON(file=vars$input_file_json, method="C");
cat(" [DONE]\n")

if(is.null(json_data) || length(json_data) < 1) {
    cat("Empty json data\n");
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
for(iteration in 1:length(json_data)) {
    cat("Iteration:", iteration);

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

    cat("\tsequence length:", sequence_length,
        "\tnum of ranged sequences:",
        length(sequence_data), "...");

    if(length(sequence_data) > config$max_length_plot_limit) {
        cat("\n", length(sequence_data),
            "are too much sequences to plot just only a single image",
            "with all of them\n");
        next;
    }

    ###
    # plotting all the sequences of same length
    ###

    cat(" per-lenght plotting... ");

    xmin_ranges = c(); # will be empty, no ranges
    xmax_ranges = c(); # will be empty, no ranges
    x_points = c();
    y_points = c();

    for(j in 1:length(sequence_data)) {
        sequence_data_item = sequence_data[[j]];
        sequence = sequence_data_item$sequence;
        #frequency = sequence_data_item$frequency;
        #start = sequence_data_item$start;
        #end = sequence_data_item$end;
        times = sequence_data_item$times;
        spaces = sequence_data_item$spaces;

        x_points = c(x_points, times);
        y_points = c(y_points, spaces);
    }

    filename_by_len = file.path(
        vars$output_img_dir,
        paste(sequence_length, ".",
              config$per_length_plot_image_type, sep="")
    );

    utils$dev_open_file(
        filename_by_len, vars$database_x_size, vars$database_y_size, config$plot_scale);
    plot(plotRangedSequences(
        x_points, y_points, xmin_ranges, xmax_ranges,
        lim_x_min=vars$lim_database_x_min, lim_x_max=vars$lim_database_x_max,
        lim_y_min=vars$lim_database_y_min, lim_y_max=vars$lim_database_y_max,
        scale=config$plot_scale));
    dev.off();

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
    # plot an image with all ranges for each sequence
    ####

    # organize the data by sequence

    cat(" per-sequence data ...");

    seq_plotd = new.env(hash=TRUE, parent=emptyenv());

    for(j in 1:length(sequence_data)) {
        sequence_data_item = sequence_data[[j]];
        sequence = sequence_data_item$sequence;

        if(! exists(sequence, seq_plotd)) {
            seq_plotd[[sequence]] = new.env(hash=TRUE, parent=emptyenv());
            seq_plotd[[sequence]][["xmin_ranges"]] = c();
            seq_plotd[[sequence]][["xmax_ranges"]] = c();
            seq_plotd[[sequence]][["x_points"]] = c();
            seq_plotd[[sequence]][["y_points"]] = c();
        }

        start = sequence_data_item$start;
        end = sequence_data_item$end;
        times = sequence_data_item$times;
        spaces = sequence_data_item$spaces;

        seq_plotd[[sequence]][["xmin_ranges"]] = c(
            seq_plotd[[sequence]][["xmin_ranges"]], start);
        seq_plotd[[sequence]][["xmax_ranges"]] = c(
            seq_plotd[[sequence]][["xmax_ranges"]], end);
        seq_plotd[[sequence]][["x_points"]] = c(
            seq_plotd[[sequence]][["x_points"]], times);
        seq_plotd[[sequence]][["y_points"]] = c(
            seq_plotd[[sequence]][["y_points"]], spaces);
    }

    cat(" plotting ...");

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

    something_plotted = FALSE;

    # producing the entire bunch of plots at one time,
    # a img with all the points and ranges for each sequence
    utils$dev_open_file(
        filename_by_seq, vars$database_x_size, vars$database_y_size, config$plot_scale);

    for(key in ls(seq_plotd)) {
        if(length(seq_plotd[[key]][["x_points"]])
           > config$min_positions_plot_limit) {
            plot(plotRangedSequences(
                seq_plotd[[key]][["x_points"]],
                seq_plotd[[key]][["y_points"]],
                seq_plotd[[key]][["xmin_ranges"]],
                seq_plotd[[key]][["xmax_ranges"]],
                lim_x_min=vars$lim_database_x_min,
                lim_x_max=vars$lim_database_x_max,
                lim_y_min=vars$lim_database_y_min,
                lim_y_max=vars$lim_database_y_max,
                scale=config$plot_scale));
            something_plotted = TRUE;
        }
    }

    dev.off();

    if(something_plotted) {
        # renaming to have the sequence name
        k = 0;

        per_sequence_index_file = file(file.path(
            vars$output_img_dir, "by_length_and_sequence",
            paste(sequence_length, ".html", sep="")));
        per_sequence_index_lines = c(
            html$pre_title, sequence_length, html$post_title_pre_container);

        for(key in ls(seq_plotd)) {
            if(length(seq_plotd[[key]][["x_points"]])
               > config$min_positions_plot_limit) {
                k = k + 1;
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

                per_sequence_index_lines = c(
                    per_sequence_index_lines,
                    "        <div class=\"content first\">",
                    paste(
                        "          <img style=\"",
                        "background:url(../", vars$background_img, ");",
                        "background-size:cover;",
                        "width:800px;\"",
                        " src=\"", sequence_length, "/",
                        key, ".", config$per_sequence_plot_image_type,
                        "\" alt=\"\" />", sep=""),
                    "        </div>",
                    paste("        <div class=\"content\">",
                          key, "</div>", sep=""));
            }
        }

        writeLines(c(per_sequence_index_lines, html$post_container),
                   per_sequence_index_file);
        close(per_sequence_index_file);

    } else {
        # removing unuseful directory
        #unlink(     # [CMP] unlink won't work
        system(paste('rmdir', file.path(
            vars$output_img_dir, "by_length_and_sequence", sequence_length)));
    }

    cat(" [DONE]\n")
}

# completing html file
writeLines(c(per_length_index_lines, html$post_container),
           per_length_index_file);
close(per_length_index_file);

# write a file that grants that this plot is complete for this product
result = file.create(file.path(vars$output_img_dir, "complete"));
