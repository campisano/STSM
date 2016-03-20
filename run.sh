#!/bin/bash

MEM_LIMIT=7340032;      # 7GB
#MEM_LIMIT=15728640;     # 15GB
ulimit -v $MEM_LIMIT;

INPUT_FOLDER="data";
OUTPUT_FOLDER="results";
FILE_NAME="100_sax";

ALPHABETS=("5" "10" "15" "20" "25");
TYPES=("original" "transposed");
SUPPORTS=("100" "95" "90" "85" "80");
GAPS=("0" "1" "2" "3");

PATH=./BUILD/release:$PATH;



for TYPE in "${TYPES[@]}"
do
    for ALPHABET in "${ALPHABETS[@]}"
    do
        INPUT_FILE=$INPUT_FOLDER"/"$FILE_NAME"-"$ALPHABET"_"$TYPE".csv";

        for SUPPORT in "${SUPPORTS[@]}"
        do
            for GAP in "${GAPS[@]}"
            do
                echo "--------------------------------------------------";
                SPEC_OUTPUT_FOLDER=$OUTPUT_FOLDER"/"$ALPHABET"_"$TYPE;
                mkdir -p $SPEC_OUTPUT_FOLDER"/json/";
                mkdir -p $SPEC_OUTPUT_FOLDER"/log/";
                mkdir -p $SPEC_OUTPUT_FOLDER"/png/";
                OUTPUT_FILE=$SPEC_OUTPUT_FOLDER"/json/"$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$SUPPORT"_g"$GAP".json";
                LOG_FILE=$SPEC_OUTPUT_FOLDER"/log/"$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$SUPPORT"_g"$GAP".log";
                PNG_FILE=$SPEC_OUTPUT_FOLDER"/png/"$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$SUPPORT"_g"$GAP"_-0.png";

                # Produce GSP results only if was not already done
                if test ! -f $OUTPUT_FILE.gz
                then
                    if test ! -f $OUTPUT_FILE
                    then
                        echo " * Running GSP $TYPE $ALPHABET $SUPPORT $GAP [...]";
                        gsp $INPUT_FILE $OUTPUT_FILE $LOG_FILE $SUPPORT $GAP;
                    fi;
                fi;

                # Produce PNG image oly if was not already done
                if test ! -f $PNG_FILE
                then
                    if test -f $OUTPUT_FILE.gz
                    then
                        echo " * Decompressing previous output file [...]";
                        gzip -d ${OUTPUT_FILE}.gz;
                    fi;
                    if test -f $OUTPUT_FILE
                    then
                        echo " * Plotting data $TYPE $ALPHABET $SUPPORT $GAP [...]";
                        R --vanilla --slave --file=R/display.r --args $INPUT_FILE $OUTPUT_FILE;
                    fi;
                fi;

                # Compress outputs
                if test -f $OUTPUT_FILE
                then
                    echo " * Compressing ouput file [...]";
                    rm -f $OUTPUT_FILE.gz;
                    gzip --fast $OUTPUT_FILE;
                fi;

                if test -f $LOG_FILE
                then
                    echo " * Compressing log file [...]";
                    rm -f $LOG_FILE.gz;
                    gzip --fast $LOG_FILE;
                fi;
            done;
        done;
    done;
done;



# End
