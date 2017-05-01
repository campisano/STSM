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
#args[1] = "results_test/matches-by-sax.csv";
#args[2] = "results_test/blocks-by-sax.csv";
#args[3] = "results_test/sequences-by-sax.csv";
#args[4] = "results_test/sequences_with_2width_blocks-by-sax.csv";
#args[5] = "results_test/sequences_with_3width_blocks-by-sax.csv";
#args[6] = "results_test/sequences_with_4width_blocks-by-sax.csv";
#args[7] = "results_test/sequences_with_5width_blocks-by-sax.csv";
#args[8] = 401;
#args[9] = "original";
#cat("    args:", args, "\n");

vars = utils$newDict();
vars$matches_by_sax = utils$readCSV(args[1]);
vars$matches_by_sax_filename = utils$change_extension(args[1], "png");
vars$blocks_by_sax = utils$readCSV(args[2]);
vars$blocks_by_sax_filename = utils$change_extension(args[2], "png");
vars$sequences_by_sax = utils$readCSV(args[3]);
vars$sequences_by_sax_filename = utils$change_extension(args[3], "png");

vars$sequences_with_2width_blocks_by_sax = utils$readCSV(args[4]);
vars$sequences_with_2width_blocks_by_sax_filename =
    utils$change_extension(args[4], "png");
vars$sequences_with_3width_blocks_by_sax = utils$readCSV(args[5]);
vars$sequences_with_3width_blocks_by_sax_filename =
    utils$change_extension(args[5], "png");
vars$sequences_with_4width_blocks_by_sax = utils$readCSV(args[6]);
vars$sequences_with_4width_blocks_by_sax_filename =
    utils$change_extension(args[6], "png");
vars$sequences_with_5width_blocks_by_sax = utils$readCSV(args[7]);
vars$sequences_with_5width_blocks_by_sax_filename =
    utils$change_extension(args[7], "png");
vars$inline = args[8];
vars$orientation = args[9];

vars$width = 300;
vars$height = 300;
vars$dpi = 100;



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
    x_title="SAX alphabet size", y_title="Sequence pattern occurrences");
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
    x_title="SAX alphabet size", y_title="Solid block occurrencies");
utils$dev_off();
rm(df);



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
    x_title="SAX alphabet size",
    y_title="Seq. patterns with any solid blocks");
utils$dev_off();
rm(df);





df = vars$sequences_with_2width_blocks_by_sax;
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
    vars$sequences_with_2width_blocks_by_sax_filename,
    vars$width, vars$height, dpi=vars$dpi);
utils$bar_plot(
    data_frame=df,
    x_col="sax", y_col="sequences",
    x_title="SAX alphabet size",
    y_title="Results with blocks of 2 occurrences");
utils$dev_off();
rm(df);

df = vars$sequences_with_3width_blocks_by_sax;
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
    vars$sequences_with_3width_blocks_by_sax_filename,
    vars$width, vars$height, dpi=vars$dpi);
utils$bar_plot(
    data_frame=df,
    x_col="sax", y_col="sequences",
    x_title="SAX alphabet size",
    y_title="Results with blocks of 3 occurrences");
utils$dev_off();
rm(df);

df = vars$sequences_with_4width_blocks_by_sax;
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
    vars$sequences_with_4width_blocks_by_sax_filename,
    vars$width, vars$height, dpi=vars$dpi);
utils$bar_plot(
    data_frame=df,
    x_col="sax", y_col="sequences",
    x_title="SAX alphabet size",
    y_title="Results with blocks of 4 occurrences");
utils$dev_off();
rm(df);

df = vars$sequences_with_5width_blocks_by_sax;
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
    vars$sequences_with_5width_blocks_by_sax_filename,
    vars$width, vars$height, dpi=vars$dpi);
utils$bar_plot(
    data_frame=df,
    x_col="sax", y_col="sequences",
    x_title="SAX alphabet size",
    y_title="Results with blocks of 5 occurrences");
utils$dev_off();
rm(df);
