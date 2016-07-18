#!/bin/bash

MEM_LIMIT=5248000;      # 5GB
#MEM_LIMIT=7340032;      # 7GB
#MEM_LIMIT=15728640;     # 15GB
ulimit -v $MEM_LIMIT;

PATH=./BUILD/release:$PATH;
#PATH=./BUILD/debug:$PATH;
#ulimit -c unlimited;    # enable core dump, useful only in debug mode



INPUT_FOLDER="data";

#OUTPUT_FOLDER="results_inline-100";
OUTPUT_FOLDER="results_inline-401";

#FILE_NAME="100_sax";
FILE_NAME="401_sax";



#ALPHABETS=("25" "20" "15" "10" "5");
ALPHABETS=("25");

#TYPES=("original" "transposed");
TYPES=("original");

#MIN_SUPPORTS=("100" "90"  "80"  "80" "70" "70" "60" "60" "50" "50" "40" "40" "30" "30" "20");
#MAX_SUPPORTS=("100" "100" "100" "90" "90" "80" "80" "70" "70" "60" "60" "50" "50" "40" "40");
#MIN_SUPPORTS=("90" "80" "70" "60" "50" "40" "30");
#MAX_SUPPORTS=("100" "90" "80" "70" "60" "50" "40");
MIN_SUPPORTS=("80" "70" "60" "50" "40" "30");
MAX_SUPPORTS=("100" "90" "80" "70" "60" "50");
#MIN_SUPPORTS=("90" "80");
#MAX_SUPPORTS=("100" "90");

MAX_TIME_WINDOWS=("0" "2" "5" "10");
#MAX_TIME_WINDOWS=("0");



for TYPE in "${TYPES[@]}"
do
    for ALPHABET in "${ALPHABETS[@]}"
    do
        INPUT_FILE=$INPUT_FOLDER"/"$FILE_NAME"-"$ALPHABET"_"$TYPE".csv";
        SPEC_OUTPUT_FOLDER=$OUTPUT_FOLDER"/"$ALPHABET"_"$TYPE;

        SUP_I=0
        while test "${SUP_I}" -lt "${#MIN_SUPPORTS[@]}"
        do
            MIN_SUPPORT="${MIN_SUPPORTS[SUP_I]}"
            MAX_SUPPORT="${MAX_SUPPORTS[SUP_I]}"
            SUP_I=$((SUP_I+1));

            for MAX_TIME_WINDOW in "${MAX_TIME_WINDOWS[@]}"
            do
                echo "--------------------------------------------------";
                OUTPUT_FILE=$SPEC_OUTPUT_FOLDER"/json/"$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$MIN_SUPPORT"-"$MAX_SUPPORT"_g"$MAX_TIME_WINDOW".json";
                LOG_FILE=$SPEC_OUTPUT_FOLDER"/log/"$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$MIN_SUPPORT"-"$MAX_SUPPORT"_g"$MAX_TIME_WINDOW".log";
                PNG_DIR=$SPEC_OUTPUT_FOLDER"/png/"$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$MIN_SUPPORT"-"$MAX_SUPPORT"_g"$MAX_TIME_WINDOW;

                mkdir -p $SPEC_OUTPUT_FOLDER"/json/";
                mkdir -p $SPEC_OUTPUT_FOLDER"/log/";
                mkdir -p $SPEC_OUTPUT_FOLDER"/png/";


                # Produce GSP results only if was not already done
                if test ! -f $OUTPUT_FILE.gz
                then
                    if test ! -f $OUTPUT_FILE
                    then
                        echo " * Running GSP $TYPE $ALPHABET $MIN_SUPPORT $MAX_SUPPORT $MAX_TIME_WINDOW [...]";
                        time gsp $INPUT_FILE $OUTPUT_FILE $LOG_FILE $MIN_SUPPORT $MAX_SUPPORT $MAX_TIME_WINDOW;
                    fi;
                fi;

                # Produce Stacked Bar data and PNG images only if was not already done
                if test ! -d $PNG_DIR
                then
                    if test -f $OUTPUT_FILE.gz
                    then
                        echo " * Decompressing previous output file [...]";
                        gzip -d ${OUTPUT_FILE}.gz;
                    fi;
                    if test -f $OUTPUT_FILE
                    then
                        echo " * Producing Stacked Bar data"
                        R --vanilla --slave --file=R/stacked_bar_data.r --args $OUTPUT_FILE $MIN_SUPPORT $MAX_SUPPORT $MAX_TIME_WINDOW;

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



# plotting stack bars
echo " * Plotting stack bars [...]";
for TYPE in "${TYPES[@]}"
do
    for ALPHABET in "${ALPHABETS[@]}"
    do
        SPEC_OUTPUT_FOLDER=$OUTPUT_FOLDER"/"$ALPHABET"_"$TYPE;

        SUP_I=0
        while test "${SUP_I}" -lt "${#MIN_SUPPORTS[@]}"
        do
            MIN_SUPPORT="${MIN_SUPPORTS[SUP_I]}"
            MAX_SUPPORT="${MAX_SUPPORTS[SUP_I]}"
            SUP_I=$((SUP_I+1));

            SUP_RANGE=$MIN_SUPPORT"-"$MAX_SUPPORT
            OUTPUT_FILE=$SPEC_OUTPUT_FOLDER"/stackedbar/support_range/"$SUP_RANGE;
            R --vanilla --slave --file=R/stacked_bar_data_plot_supports.r --args $OUTPUT_FILE $SUP_RANGE;
        done;

        for MAX_TIME_WINDOW in "${MAX_TIME_WINDOWS[@]}"
        do
            OUTPUT_FILE=$SPEC_OUTPUT_FOLDER"/stackedbar/time_window/"$MAX_TIME_WINDOW;
            R --vanilla --slave --file=R/stacked_bar_data_plot_times.r --args $OUTPUT_FILE $MAX_TIME_WINDOW;
        done;
    done;
done;



# End
