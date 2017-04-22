#setwd("/home/shared/develop/projects/CEFET/mestrado/SIM");

# include utility file
source(file="R/utils.r", chdir=TRUE);

utils$setVerbose();

# loading dependences
loaded_libs = utils$loadLibs(c(
    "rjson:0.2.15",
    "ggplot2:1.0.0",
    "grid:3.1.1",
    "scales:3.1.1",
    "sqldf:0.4-10"));



# evaluating arguments
args = commandArgs(TRUE);
#cat("Arguments:\n");
#cat(args, "\n");
# examples:
#args = c();
#args[1] = "results/sequences-by-sax.csv";
#args[2] = "results/matches-by-sax.csv";
#args[3] = "results/blocks-by-sax.csv";
#args[4] = 401;
#args[5] = "original";
#args[6] = 80;
#args[7] = 20;
#args[8] = 0;
#cat("    args:", args, "\n");

vars = utils$newDict();
vars$sequences_by_sax = utils$readCSV(args[1]);
vars$sequences_by_sax_filename = utils$change_extension(args[1], "png");
vars$matches_by_sax = utils$readCSV(args[2]);
vars$matches_by_sax_filename = utils$change_extension(args[2], "png");
vars$blocks_by_sax = utils$readCSV(args[3]);
vars$blocks_by_sax_filename = utils$change_extension(args[3], "png");
vars$inline = args[4];
vars$orientation = args[5];
vars$min_spatial = args[6];
vars$min_block = args[7];
vars$max_stretch = args[8];



df = vars$sequences_by_sax;
df = sqldf(paste(
    "SELECT sax, SUM(sequences) AS sequences",
    "FROM df",
    paste("WHERE inline = ", vars$inline, sep=""),
    paste("AND orientation = '", vars$orientation, "'", sep=""),
    paste("AND spatial = ", vars$min_spatial, sep=""),
    paste("AND block = ", vars$min_block, sep=""),
    paste("AND stretch = ", vars$max_stretch, sep=""),
    "GROUP BY sax;"));
utils$dev_open_file(vars$sequences_by_sax_filename, 600, 600);
utils$bar_plot(
    data_frame=df,
    x_col="sax", y_col="sequences", title=paste(
        "Sequences by SAX alphabet size (inline ", vars$inline,
        ", min seq. freq. ", vars$min_spatial,
        ", min blk. freq. ", vars$min_block, ")", sep=""),
    x_title="SAX alphabet size", y_title="Sequences");
utils$dev_off();
rm(df);



df = vars$matches_by_sax;
df = sqldf(paste(
    "SELECT sax, SUM(num_pos) AS matches",
    "FROM df",
    paste("WHERE inline = ", vars$inline, sep=""),
    paste("AND orientation = '", vars$orientation, "'", sep=""),
    paste("AND spatial = ", vars$min_spatial, sep=""),
    paste("AND block = ", vars$min_block, sep=""),
    paste("AND stretch = ", vars$max_stretch, sep=""),
    "GROUP BY sax;"));
utils$dev_open_file(vars$matches_by_sax_filename, 600, 600);
utils$bar_plot(
    data_frame=df,
    x_col="sax", y_col="matches", title=paste(
        "Matches by SAX alphabet size (inline ", vars$inline,
        ", min seq. freq. ", vars$min_spatial,
        ", min blk. freq. ", vars$min_block, ")", sep=""),
    x_title="SAX alphabet size", y_title="Matches");
utils$dev_off();
rm(df);



df = vars$blocks_by_sax;
df = sqldf(paste(
    "SELECT sax, SUM(num_blocks) AS blocks",
    "FROM df",
    paste("WHERE inline = ", vars$inline, sep=""),
    paste("AND orientation = '", vars$orientation, "'", sep=""),
    paste("AND spatial = ", vars$min_spatial, sep=""),
    paste("AND block = ", vars$min_block, sep=""),
    paste("AND stretch = ", vars$max_stretch, sep=""),
    "GROUP BY sax;"));
utils$dev_open_file(vars$blocks_by_sax_filename, 600, 600);
utils$bar_plot(
    data_frame=df,
    x_col="sax", y_col="blocks", title=paste(
        "Blocks by SAX alphabet size (inline ", vars$inline,
        ", min seq. freq. ", vars$min_spatial,
        ", min blk. freq. ", vars$min_block, ")", sep=""),
    x_title="SAX alphabet size", y_title="Blocks");
utils$dev_off();
rm(df);
