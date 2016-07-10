#!/bin/bash

MEM_LIMIT=5248000;      # 5GB
#MEM_LIMIT=7340032;      # 7GB
#MEM_LIMIT=15728640;     # 15GB
ulimit -v $MEM_LIMIT;
ulimit -c unlimited;    # enable core dump

INPUT_FOLDER="data";
OUTPUT_FOLDER="results";
FILE_NAME="401_sax";

#ALPHABETS=("25" "20" "15" "10" "5");
ALPHABETS=("25");

#TYPES=("original" "transposed");
TYPES=("original");

MIN_SUPPORTS=("100" "90"  "80"  "80" "70" "70" "60" "60" "50" "50" "40" "40" "30" "30" "20");
MAX_SUPPORTS=("100" "100" "100" "90" "90" "80" "80" "70" "70" "60" "60" "50" "50" "40" "40");
#MIN_SUPPORTS=("100" "90" "80");
#MAX_SUPPORTS=("100" "100" "100");

MAX_TIME_WINDOWS=("0" "2" "5" "10");
#MAX_TIME_WINDOWS=("0");

PATH=./BUILD/release:$PATH;



for TYPE in "${TYPES[@]}"
do
    for ALPHABET in "${ALPHABETS[@]}"
    do
        INPUT_FILE=$INPUT_FOLDER"/"$FILE_NAME"-"$ALPHABET"_"$TYPE".csv";

        SUP_I=0
        while test "${SUP_I}" -lt "${#MIN_SUPPORTS[@]}"
        do
            MIN_SUPPORT="${MIN_SUPPORTS[SUP_I]}"
            MAX_SUPPORT="${MAX_SUPPORTS[SUP_I]}"
            SUP_I=$((SUP_I+1));

            for MAX_TIME_WINDOW in "${MAX_TIME_WINDOWS[@]}"
            do
                echo "--------------------------------------------------";
                SPEC_OUTPUT_FOLDER=$OUTPUT_FOLDER"/"$ALPHABET"_"$TYPE;
                mkdir -p $SPEC_OUTPUT_FOLDER"/json/";
                mkdir -p $SPEC_OUTPUT_FOLDER"/log/";
                mkdir -p $SPEC_OUTPUT_FOLDER"/png/";
                OUTPUT_FILE=$SPEC_OUTPUT_FOLDER"/json/"$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$MIN_SUPPORT"-"$MAX_SUPPORT"_g"$MAX_TIME_WINDOW".json";
                LOG_FILE=$SPEC_OUTPUT_FOLDER"/log/"$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$MIN_SUPPORT"-"$MAX_SUPPORT"_g"$MAX_TIME_WINDOW".log";
                PNG_FILE=$SPEC_OUTPUT_FOLDER"/png/"$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$MIN_SUPPORT"-"$MAX_SUPPORT"_g"$MAX_TIME_WINDOW"_-0.png";

                # Produce GSP results only if was not already done
                if test ! -f $OUTPUT_FILE.gz
                then
                    if test ! -f $OUTPUT_FILE
                    then
                        echo " * Running GSP $TYPE $ALPHABET $MIN_SUPPORT $MAX_SUPPORT $MAX_TIME_WINDOW [...]";
                        time gsp $INPUT_FILE $OUTPUT_FILE $LOG_FILE $MIN_SUPPORT $MAX_SUPPORT $MAX_TIME_WINDOW;
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
                        echo " * Plotting data $TYPE $ALPHABET $MIN_SUPPORT $MAX_SUPPORT $MAX_TIME_WINDOW [...]";
                        time R --vanilla --slave --file=R/display.r --args $INPUT_FILE $OUTPUT_FILE;
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
