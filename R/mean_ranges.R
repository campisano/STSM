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



## include utility file
source(file="R/utils.R", chdir=TRUE);

## set verbose mode
utils$setVerbose();



## loading dependences
loaded_libs = utils$loadLibs(c(
    ## basic R packages:
    "datasets:3.1.1",
    "methods:3.1.1",
    "stats:3.1.1",
    "utils:3.1.1",
    "rjson:0.2.15",
    "compiler:3.1.1",
    ## plot packages:
    "graphics:3.1.1",
    "grDevices:3.1.1",
    "grid:3.1.1",
    "scales:0.2.4",
    "ggplot2:1.0.0"
));



## include specific plot funtion
source(file="R/stsm_plotfn.R", chdir=TRUE);



## configuring variables
config = utils$newDict();
config$min_sequence_length_to_plot = 2;
config$max_sequence_length_to_plot = Inf;
config$plot_scale_preview = 1;
config$plot_scale = 5;
config$max_length_plot_limit = 100000;
config$block_area_start_count_from_y_frac = 0.1;   ## 10%
config$block_area_start_count_from_y = 0;  ## will be calculated
config$per_sequence_plot_block_requires_min_width_to_be_drawn = 5;
config$plot_only_ranges_that_contains_blocks = TRUE;
config$per_length_plot_image_type = "png";
config$per_sequence_plot_preview_image_type = "png";
config$per_sequence_plot_image_type = "svg";
config$preview_img_size = "800px";
config$full_img_size = "1440px";



## evaluating arguments
args = commandArgs(TRUE);
## cat("Arguments:\n");
## cat(args, "\n");
## examples:
## args = c();
## args[1] = "data/401_sax-10_original.csv";
## args[2] = "results/inline-401_orientation-original/sax-10/json/I401_Ooriginal_S10_FS80_FB20_MS0.json";
## args[3] = "results/inline-401_orientation-original/sax-10/img/spatial-80/block-20/stretch-0";
## args[4] = "data/inline_401_951x462.jpg";
## cat("    args:", args, "\n");

vars = utils$newDict();
vars$csv_database = args[1];
vars$input_file_json = args[2];
vars$output_img_dir = args[3];
vars$background_img_src = args[4];

vars$background_img = basename(vars$background_img_src);
vars$base_filename = utils$remove_extension(basename(vars$input_file_json));



## load original database to know it size
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


## loading json data
## cat("Loading json data", vars$input_file_json, "...");
json_data = utils$readJSON(vars$input_file_json);
## cat(" [DONE]\n")

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

## copy background image for html
system(paste("cp -f", vars$background_img_src, vars$output_img_dir));

## preparing html initial file
per_length_index_file = file(file.path(
    vars$output_img_dir, "index.html"));
per_length_index_lines = c(
    utils$html.getHTMLpreContentCode(title=vars$base_filename));

total_ranges = 0;
sum_of_range_size = 0;

## start the iterations, for each json data grouped by length
for(iteration in 1:length(solid_sequences)) {

    sequence_data_by_length = solid_sequences[[iteration]];

    if(
        is.null(sequence_data_by_length$length) ||
            is.null(sequence_data_by_length$sequences) ||
            length(sequence_data_by_length$sequences) < 1
    ) {
        cat("\nEmpty sequence data iteration", iteration,
            "of length", sequence_data_by_length$length, "\n");
        ## cat("Data:\n");
        ## dput(sequence_data_by_length);
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



## unloading dependences
utils$unloadLibs(loaded_libs);
