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
            install.packages(lib_name, repos=repos, lib=lib);
            library(lib_name, character.only=TRUE, quietly=TRUE);
        }
    }
    rm(lib_name, lib_names);
}



# loading dependences
loaded_libs = loadLibs(c("rjson", "TSMining", "ggplot2"));



# evaluating arguments
args = commandArgs(TRUE);
#print("Arguments:");
#print(args);
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
base_filename =  sub("[.][^.]*$", "", base_filename, perl=TRUE);



# load original database
database = read.table(
    file=csv_database, header=TRUE, fill=TRUE, as.is=TRUE,
    stringsAsFactors=FALSE, sep=",", quote=""
);



# loading json data
#print(paste("Loading json data", input_file_json, "..."));
json_data = fromJSON(file=input_file_json, method="C");
#print("Load complete")



if(is.null(json_data) || length(json_data) < 1)
{
    print("Empty json data");
} else {
    for(len in 1:length(json_data))
    {
        cat("\rlen =", len);
        sequence_data_by_length = json_data[[len]];

        if(
            is.null(sequence_data_by_length$length) ||
                is.null(sequence_data_by_length$sequences) ||
                length(sequence_data_by_length$sequences) < 1
        )
        {
            cat(paste("\rEmpty sequence data of len", len, "\n"));
            #print("Data:");
            #dput(sequence_data_by_length);
            next;
        }

        if(length(sequence_data_by_length$sequences) > 10000)
        {
            cat(paste("\n", length(sequence_data_by_length$sequences),
                      "are too much sequences to plot for len", len, "\n"));
            next;
        }

        sequence_length = sequence_data_by_length$length;
        sequence_data = sequence_data_by_length$sequences;

        my_stmotifs = list();

        for(j in 1:length(sequence_data))
        {
            sequence_data_item = sequence_data[[j]];
            #sequence = gsub("[()<>]", "", sequence_data_item$sequence);
            sequence = sequence_data_item$sequence;
            #support = sequence_data_item$support;
            #frequency = sequence_data_item$frequency;
            #start = sequence_data_item$start;
            #end = sequence_data_item$end;
            #match_data = sequence_data_item$positions;

            #times = unlist(lapply(match_data, function(item){item$x}));
            #spaces = unlist(lapply(match_data, function(item){item$y}));
            times = sequence_data_item$times;
            spaces = sequence_data_item$spaces;

            my_item = list();
            my_item[["saxcod"]] = data.frame(
                t(data.frame(strsplit(sequence, split=""), row.names=NULL)),
                row.names=NULL
            );
            my_item[["isaxcod"]] = sequence;
            my_item[["recmatrix"]] = "";
            my_item[["vecst"]] = data.frame("s"=spaces, "t"=times);

            # multiple sequences png
            my_stmotifs[[sequence]] = my_item;
        }

        # cleanup
        rm(
            my_item, spaces, times, sequence, sequence_data_item,
            sequence_data, j, sequence_data_by_length
        );

        #setwd("/home/t1t0/Desktop/mestrado/GSP")
        #load("20_10_4_5_C.RData") # candidates
        #load("t100.RData") # database 40x100
        #stmotifs = STSIdentifySTMotifs(candidates, 5, 5)
        #sttightmotifs = STSIdentifyTightSTMotifs(stmotifs, candidates$rectangles)
        #ranksttightmotifs = STSRankTightSTMotifs(sttightmotifs)

        filename_by_len_png = file.path(
            base_path_dir, "png",
            paste(base_filename, "_", len, ".png", sep="")
        );

        png(filename_by_len_png, bg="transparent");
        ds = PlotSTMotifs(my_stmotifs, database);
        dev.off();

        # cleanup
        rm(
            ds, filename_by_len_png
        );

        # print for each sequence

        for(w in 1:length(my_stmotifs))
        {
            # single sequence png
            sequence = names(my_stmotifs)[w];
            my_stmotifs_tmp = list();
            my_stmotifs_tmp[[sequence]] = my_stmotifs[[w]];

            dir.create(
                file.path(
                    base_path_dir, "png", base_filename, sequence_length
                ),
                showWarnings=FALSE, recursive=TRUE, mode="0777"
            );
            filename_by_seq_png = file.path(
                base_path_dir, "png", base_filename, sequence_length,
                paste(sequence, ".png", sep="")
            );

            png(filename_by_seq_png, bg="transparent");
            PlotSTMotifs(my_stmotifs_tmp, database);
            dev.off();
        }


        # cleanup
        rm(
            sequence_length, my_stmotifs, filename_by_seq_png, my_stmotifs_tmp, sequence
        );
    }
    cat("\n");
}
