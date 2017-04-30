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
#args[4] = "results/sequences_with_big_blocks-by-sax.csv";
#args[5] = 401;
#args[6] = "original";
#cat("    args:", args, "\n");

vars = utils$newDict();
vars$sequences_by_sax = utils$readCSV(args[1]);
vars$sequences_by_sax_filename = utils$change_extension(args[1], "png");
vars$matches_by_sax = utils$readCSV(args[2]);
vars$matches_by_sax_filename = utils$change_extension(args[2], "png");
vars$blocks_by_sax = utils$readCSV(args[3]);
vars$blocks_by_sax_filename = utils$change_extension(args[3], "png");
vars$sequences_with_big_blocks_by_sax = utils$readCSV(args[4]);
vars$sequences_with_big_blocks_by_sax_filename =
    utils$change_extension(args[4], "png");
vars$inline = args[5];
vars$orientation = args[6];

vars$width = 300;
vars$height = 300;
vars$dpi = 100;



df = vars$sequences_by_sax;
df = sqldf(paste(
    "SELECT sax, SUM(sequences) AS sequences, spatial, block, stretch",
    "FROM df",
    paste("WHERE inline = ", vars$inline, sep=""),
    paste("AND orientation = '", vars$orientation, "'", sep=""),
    "GROUP BY sax, spatial, block, stretch;"));
df = sqldf(paste(
    "SELECT sax, SUM(sequences) / COUNT(*) AS sequences",
    "FROM df",
    "GROUP BY sax;"));
utils$dev_open_file(
    vars$sequences_by_sax_filename,
    vars$width, vars$height, dpi=vars$dpi);
utils$bar_plot(
    data_frame=df,
    x_col="sax", y_col="sequences",
    x_title="SAX alphabet size", y_title="Sequence patterns");
utils$dev_off();
rm(df);



df = vars$matches_by_sax;
df = sqldf(paste(
    "SELECT sax, SUM(num_pos) AS occurrences, spatial, block, stretch",
    "FROM df",
    paste("WHERE inline = ", vars$inline, sep=""),
    paste("AND orientation = '", vars$orientation, "'", sep=""),
    "GROUP BY sax, spatial, block, stretch;"));
df = sqldf(paste(
    "SELECT sax, SUM(occurrences) / COUNT(*) AS occurrences",
    "FROM df",
    "GROUP BY sax;"));
utils$dev_open_file(
    vars$matches_by_sax_filename,
    vars$width, vars$height, dpi=vars$dpi);
utils$bar_plot(
    data_frame=df,
    x_col="sax", y_col="occurrences",
    x_title="SAX alphabet size", y_title="Pattern occurrences");
utils$dev_off();
rm(df);



df = vars$blocks_by_sax;
df = sqldf(paste(
    "SELECT sax, SUM(num_blocks) AS blocks, spatial, block, stretch",
    "FROM df",
    paste("WHERE inline = ", vars$inline, sep=""),
    paste("AND orientation = '", vars$orientation, "'", sep=""),
    "GROUP BY sax, spatial, block, stretch;"));
df = sqldf(paste(
    "SELECT sax, SUM(blocks) / COUNT(*) AS blocks",
    "FROM df",
    "GROUP BY sax;"));
utils$dev_open_file(
    vars$blocks_by_sax_filename,
    vars$width, vars$height, dpi=vars$dpi);
utils$bar_plot(
    data_frame=df,
    x_col="sax", y_col="blocks",
    x_title="SAX alphabet size", y_title="Solid blocks");
utils$dev_off();
rm(df);




df = vars$sequences_with_big_blocks_by_sax;
df = sqldf(paste(
    "SELECT sax, SUM(sequences) AS sequences, spatial, block, stretch",
    "FROM df",
    paste("WHERE inline = ", vars$inline, sep=""),
    paste("AND orientation = '", vars$orientation, "'", sep=""),
    "GROUP BY sax, spatial, block, stretch;"));
df = sqldf(paste(
    "SELECT sax, SUM(sequences) / COUNT(*) AS sequences",
    "FROM df",
    "GROUP BY sax;"));
utils$dev_open_file(
    vars$sequences_with_big_blocks_by_sax_filename,
    vars$width, vars$height, dpi=vars$dpi);
utils$bar_plot(
    data_frame=df,
    x_col="sax", y_col="sequences",
    x_title="SAX alphabet size", y_title="Sequence patterns with big blocks");
utils$dev_off();
rm(df);
