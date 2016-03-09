dep = "jsonlite"; if (!require(dep, character.only = TRUE, quietly = TRUE)) { install.packages(dep); library(dep, character.only = TRUE); }; rm(dep);

csv_dataset <- "../data/100.csv"
filename <- "../results/100g0"
filename_json <- paste(filename, "json", sep=".")
filename_rdata <- paste(filename, "RData", sep=".")
filename_png <- paste(filename, "png", sep=".")

json_data <- fromJSON(file(filename_json))



# for(i in 1:nrow(json_data))
# {
#     sequence_data_length <- json_data[i,]
# }

sequence_data_length <- json_data[nrow(json_data),] # getting only the last ones
sequence_length <- sequence_data_length$first



sequence_data <- sequence_data_length$second[[1]]  # c++ pair

my_stmotifs <- list()

for(j in 1:nrow(sequence_data))
{
    sequence_data_item <- sequence_data[j,]
    sequence <- sequence_data_item$first
    sequence <- gsub("[()<>]", "", sequence)
    support <- sequence_data_item$second[1,]$first
    match_data <- sequence_data_item$second[1,]$second[[1]] # c++ pair
    
    my_item <- list()
    #my_item[['saxcod']] <- data.frame(
    #    t(strsplit(sequence, split="")), check.names = FALSE)
    my_item[['saxcod']] <- data.frame(
        t(data.frame(strsplit(sequence, split=""), row.names = NULL)),
        row.names = NULL)
    my_item[['isaxcod']] <- sequence
    my_item[['recmatrix']] <- ''
    my_item[['vecst']] <- data.frame(
        's'=match_data$second, 't'=match_data$first)
    
    my_stmotifs[[sequence]] <- my_item
}

# cleanup
rm(
    my_item, match_data, support, sequence, sequence_data_item,
    sequence_data, j, sequence_length, sequence_data_length, json_data
)



save(my_stmotifs, file=filename_rdata)
load(filename_rdata)



source("motif_lib.R")
load_lib()
#setwd("/home/t1t0/Desktop/mestrado/GSP")
#load("20_10_4_5_C.RData") # candidates
#load("t100.RData") # dataset 40x100
dataset <- read.table(
    file = csv_dataset,
    header = TRUE,
    fill = TRUE,
    as.is = TRUE,
    stringsAsFactors = FALSE,
    sep = " ",
    quote = "");
#stmotifs <- STSIdentifySTMotifs(candidates, 5, 5)
#sttightmotifs <- STSIdentifyTightSTMotifs(stmotifs, candidates$rectangles)
#ranksttightmotifs <- STSRankTightSTMotifs(sttightmotifs)

png(filename_png)
ds = PlotSTMotifs(my_stmotifs)
dev.off()
