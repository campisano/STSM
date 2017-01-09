# configure detailed error outputs
options(warn=2, keep.source=TRUE);



# defining functions
loadLibs = function(lib_names) {
    repos = "http://cran.r-project.org";
    lib = paste(Sys.getenv("HOME"), "R", "library", sep="/");
    dir.create(lib, showWarnings=FALSE, recursive=TRUE, mode="0777");
    .libPaths(c(lib, .libPaths()));

    for(lib_name in lib_names) {
        if (!require(lib_name, character.only=TRUE, quietly=TRUE)) {
            install.packages(lib_name, repos=repos, lib=lib,
                             quiet=FALSE, dependencies = TRUE);
            library(lib_name, character.only=TRUE, quietly=TRUE);
        }
    }

    return(lib_names);
}



plotRangedSequences = function(
    x_points, y_points, xmin_ranges=c(), xmax_ranges=c(),
    lim_x_min=NA, lim_x_max=NA, lim_y_min=-NA, lim_y_max=NA,
    clean=TRUE) {
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
            size=0.5, color=alpha("red", 0.3), fill="red", alpha=0.1);
    }

    # then print the points
    gg = gg + geom_point(
        data=df_points,
        aes(x, y, inherit.ae=FALSE),
        size=2, color="darkblue", alpha=0.3);

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



# loading dependences
loaded_libs = loadLibs(c("rjson", "TSMining", "ggplot2", "grid", "scales"));



# test plot funtion
# plotRangedSequences(
#     rnorm(100,2), rnorm(100,2),
#     c(1, 2, 3), c(1.5, 2.5, 4),
#     lim_x_min=0, lim_x_max=10, lim_y_min=0, lim_y_max=10,
#     clean=FALSE);



# evaluating arguments
args = commandArgs(TRUE);
#cat("Arguments:\n");
#cat(args, "\n");
# examples:
#args = c();
#args[1] = "data/100_sax-25_original.csv";
#args[2] = "results/25_original/json/100_sax-25_original_s90_g2.json";
#args[2] = "results/inline-100/25_original/json/100_sax-25_original_s100-100_g0.json.gz";



# configuring variables
csv_database = args[1];
input_file_json = args[2];
base_json_path_dir = dirname(input_file_json);
base_path_dir = dirname(base_json_path_dir);
base_filename = sub(file.path(base_json_path_dir, ""), "", input_file_json);
base_filename = sub("[.][^.]*$", "", base_filename, perl=TRUE);



# load original database
database = read.table(
    file=csv_database, header=TRUE, fill=TRUE, as.is=TRUE,
    stringsAsFactors=FALSE, sep=",", quote=""
);
lim_database_x_min = 0;
lim_database_x_max = nrow(database);
database_x_size = lim_database_x_max - lim_database_x_min;
lim_database_y_min = 0;
lim_database_y_max = ncol(database);
database_y_size = lim_database_y_max - lim_database_y_min;
rm(database);



# loading json data
cat("Loading json data", input_file_json, "...");
json_data = fromJSON(file=input_file_json, method="C");
cat("[done]\n")



if(is.null(json_data) || length(json_data) < 1) {
    cat("Empty json data\n");
} else {
    for(iteration in 1:length(json_data)) {
        cat("\nIteration:", iteration);

        sequence_data_by_length = json_data[[iteration]];

        if(
            is.null(sequence_data_by_length$length) ||
                is.null(sequence_data_by_length$sequences) ||
                length(sequence_data_by_length$sequences) < 1
        ) {
            cat("\rEmpty sequence data iteration", iteration,
                "of length", sequence_data_by_length$length, "\n");
            #cat("Data:\n");
            #dput(sequence_data_by_length);
            next;
        }

        cat("\tsequence length:", sequence_data_by_length$length,
            "\tnum of ranged sequences:",
            length(sequence_data_by_length$sequences), "...");

        if(length(sequence_data_by_length$sequences) > 10000) {
            cat("\n", length(sequence_data_by_length$sequences),
                "are too much sequences to plot just only a single image",
                "with all of them\n");
            next;
        }

        sequence_length = sequence_data_by_length$length;
        sequence_data = sequence_data_by_length$sequences;

        ###
        # plotting all the sequequences of same length
        ###

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
            base_path_dir, "img",
            paste(base_filename, "_", sequence_length, ".svg", sep="")
        );

        svg(filename_by_len, bg="transparent", antialias="none",
            width=database_x_size / 50, height=database_y_size / 50);
        # png(filename_by_len, bg="transparent", antialias="none",
        #     res=200, units="px",
        #     width=database_x_size * 3, height=database_y_size * 3);
        plot(plotRangedSequences(
            x_points, y_points, xmin_ranges, xmax_ranges,
            lim_x_min=lim_database_x_min, lim_x_max=lim_database_x_max,
            lim_y_min=lim_database_y_min, lim_y_max=lim_database_y_max));
        dev.off();

        # cleanup
        rm(
            filename_by_len, spaces, times, sequence, sequence_data_item,
            j, y_points, x_points, xmax_ranges, xmin_ranges
        );

        ####
        # plot an image with all ranges for each sequence
        ####

        if(length(sequence_data_by_length$sequences) > 1000) {
            cat("\n", length(sequence_data_by_length$sequences),
                "are too much sequences to plot one image per sequence\n");
            next;
        }

        # organize the data by sequence

        seq_plotd = list();

        for(j in 1:length(sequence_data)) {
            sequence_data_item = sequence_data[[j]];
            sequence = sequence_data_item$sequence;

            if(is.null(seq_plotd[[sequence]])) {
                seq_plotd[[sequence]] = list()
                seq_plotd[[sequence]][["xmin_ranges"]] = c();
                seq_plotd[[sequence]][["xmax_ranges"]] = c();
                seq_plotd[[sequence]][["x_points"]] = c();
                seq_plotd[[sequence]][["y_points"]] = c();
            }

            #frequency = sequence_data_item$frequency;
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

        # plot all sequence data

        dir.create(
            file.path(
                base_path_dir, "img", base_filename, sequence_length
            ),
            showWarnings=FALSE, recursive=TRUE, mode="0777"
        );

        filename_by_seq = file.path(
            base_path_dir, "img", base_filename, sequence_length,
            paste("%d", ".svg", sep="")
        );

        # producing the entire bunch of plots at one time,
        # a img with all the points and ranges for each sequence

        svg(filename_by_seq, bg="transparent", antialias="none",
            width=database_x_size / 50, height=database_y_size / 50);
        # png(filename_by_seq, bg="transparent", antialias="none",
        #     res=200, units="px",
        #     width=database_x_size * 3, height=database_y_size * 3);

        for(j in 1:length(seq_plotd)) {
            plot(plotRangedSequences(
                seq_plotd[[j]][["x_points"]],
                seq_plotd[[j]][["y_points"]],
                seq_plotd[[j]][["xmin_ranges"]],
                seq_plotd[[j]][["xmax_ranges"]],
                lim_x_min=lim_database_x_min,
                lim_x_max=lim_database_x_max,
                lim_y_min=lim_database_y_min,
                lim_y_max=lim_database_y_max));
        }

        dev.off();

        # renaming to have the sequence name

        for(j in 1:length(seq_plotd)) {
            file.rename(
                file.path(
                    base_path_dir, "img", base_filename, sequence_length,
                    paste(j, ".svg", sep="")),
                file.path(
                    base_path_dir, "img", base_filename, sequence_length,
                    paste(names(seq_plotd)[j], ".svg", sep="")));
        }

        # cleanup
        rm(
            filename_by_seq, sequence, j, spaces, times, end, start,
            sequence_data_item, seq_plotd,
            sequence_data, sequence_length, sequence_data_by_length, iteration
        );
    }
}
