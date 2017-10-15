## Copyright (C) 2017 Riccardo Campisano <riccardo.campisano@gmail.com>
##               2017 Fabio Porto
##               2017 Fabio Perosi
##               2017 Esther Pacitti
##               2017 Florent Masseglia
##               2017 Eduardo Ogasawara
##
## This file is part of STSM (Spatio-Temporal Sequence Miner).
##
## STSM is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## STSM is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with STSM.  If not, see <http://www.gnu.org/licenses/>.

#setwd("/home/shared/develop/projects/CEFET/mestrado/STSM");

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
#args[1] = "data/401_sax-10_original.csv";
#args[2] = "results/inline-401_orientation-original/sax-10/json/I401_Ooriginal_S10_FS80_FB20_MS0.json";
#args[3] = "results/inline-401_orientation-original/sax-10/img/spatial-80/block-20/stretch-0";
#args[4] = "data/inline_401_951x462.jpg";
#cat("    args:", args, "\n");

vars = utils$newDict();
vars$csv_database = args[1];
vars$input_file_json = args[2];
vars$output_img_dir = args[3];
vars$background_img_src = args[4];

vars$background_img = basename(vars$background_img_src);
vars$base_filename = utils$remove_extension(basename(vars$input_file_json));



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

total_ranges = 0;
sum_of_range_size = 0;

# start the iterations, for each json data grouped by length
for(iteration in 1:length(solid_sequences)) {

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

    for(j in 1:length(sequence_data)) {
        sequence_data_item = sequence_data[[j]];
        total_ranges = total_ranges + 1;
        sum_of_range_size = sum_of_range_size + (
            sequence_data_item$end - sequence_data_item$start + 1);
    }
}

mean_range_size = sum_of_range_size / total_ranges;
cat("total ranges:", total_ranges)
cat("mean range size:", mean_range_size)

