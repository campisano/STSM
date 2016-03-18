#!/bin/bash

MEM_LIMIT=7340032;     # 7GB
ulimit -v $MEM_LIMIT;

INPUT_FOLDER="data/";
OUTPUT_FOLDER="results/";
FILE_NAME="100_sax";

ALPHABETS=("5" "10" "15" "20" "25");
TYPES=("original" "transposed");
SUPPORTS=("100" "90" "80");
GAPS=("0" "1" "2" "3");

PATH=./BUILD/release:$PATH



for TYPE in "${TYPES[@]}"
do
    for ALPHABET in "${ALPHABETS[@]}"
    do
        INPUT_FILE=$INPUT_FOLDER$FILE_NAME"-"$ALPHABET"_"$TYPE".csv";

        for SUPPORT in "${SUPPORTS[@]}"
        do
            for GAP in "${GAPS[@]}"
            do
                echo "--------------------------------------------------"
                SPEC_OUTPUT_FOLDER=$OUTPUT_FOLDER$ALPHABET"_"$TYPE"/";
                OUTPUT_FILE=$SPEC_OUTPUT_FOLDER$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$SUPPORT"_g"$GAP".json";
                LOG_FILE=$SPEC_OUTPUT_FOLDER$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$SUPPORT"_g"$GAP".log";
                LOG_FILE_R=$SPEC_OUTPUT_FOLDER$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$SUPPORT"_g"$GAP"_R.log";
                LOG_FILE_GSP=$SPEC_OUTPUT_FOLDER$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$SUPPORT"_g"$GAP"_GSP.log";
                mkdir -p $SPEC_OUTPUT_FOLDER;
                echo " * Running GSP $TYPE $ALPHABET $SUPPORT $GAP [...]";
                gsp $INPUT_FILE $OUTPUT_FILE $LOG_FILE $SUPPORT $GAP &> $LOG_FILE_GSP;
                echo " * Plotting data [...]";
                Rscript R/display.r $INPUT_FILE $OUTPUT_FILE &> $LOG_FILE_R;
            done;
        done;
    done;
done;



# End
