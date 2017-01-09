#!/bin/bash

MEM_LIMIT=5248000;      # 5GB
#MEM_LIMIT=7340032;      # 7GB
#MEM_LIMIT=15728640;     # 15GB
ulimit -v $MEM_LIMIT;

PATH=./BUILD/release:$PATH;
#PATH=./BUILD/debug:$PATH;
#ulimit -c unlimited;    # enable core dump, useful only in debug mode



INPUT_INLINES=("100" "401")
#INPUT_INLINES=("100")

INPUT_FOLDER="data";
OUTPUT_FOLDER="results";

#TYPES=("original" "transposed");
TYPES=("original");

#ALPHABETS=("25" "20" "15" "10" "5");
ALPHABETS=("25" "20" "10");

#MIN_SPATIAL_FREQS=("100" "90" "75" "50" "25");
#MIN_BLOCK_FREQS=("100" "100" "100" "100" "100");
MIN_SPATIAL_FREQS=("100" "90" "75");
MIN_BLOCK_FREQS=("100" "100" "100");

#MAX_TIME_WINDOWS=("0" "2" "5" "10");
MAX_TIME_WINDOWS=("0");


for INPUT_INLINE in "${INPUT_INLINES[@]}"
do
    FILE_NAME=$INPUT_INLINE"_sax";

    for TYPE in "${TYPES[@]}"
    do
        for ALPHABET in "${ALPHABETS[@]}"
        do
            INPUT_FILE=$INPUT_FOLDER"/"$FILE_NAME"-"$ALPHABET"_"$TYPE".csv";
            SPEC_OUTPUT_FOLDER=$OUTPUT_FOLDER"/inline-"$INPUT_INLINE"/"$ALPHABET"_"$TYPE;

            SUP_I=0
            while test "${SUP_I}" -lt "${#MIN_SPATIAL_FREQS[@]}"
            do
                MIN_SPATIAL_FREQ="${MIN_SPATIAL_FREQS[SUP_I]}"
                MIN_BLOCK_FREQ="${MIN_BLOCK_FREQS[SUP_I]}"
                SUP_I=$((SUP_I+1));

                for MAX_TIME_WINDOW in "${MAX_TIME_WINDOWS[@]}"
                do
                    echo "--------------------------------------------------";
                    OUTPUT_FILE=$SPEC_OUTPUT_FOLDER"/json/"$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$MIN_SPATIAL_FREQ"-"$MIN_BLOCK_FREQ"_g"$MAX_TIME_WINDOW".json";
                    LOG_FILE=$SPEC_OUTPUT_FOLDER"/log/"$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$MIN_SPATIAL_FREQ"-"$MIN_BLOCK_FREQ"_g"$MAX_TIME_WINDOW".log";
                    IMG_DIR=$SPEC_OUTPUT_FOLDER"/img/"$FILE_NAME"-"$ALPHABET"_"$TYPE"_s"$MIN_SPATIAL_FREQ"-"$MIN_BLOCK_FREQ"_g"$MAX_TIME_WINDOW;

                    mkdir -p $SPEC_OUTPUT_FOLDER"/json/";
                    mkdir -p $SPEC_OUTPUT_FOLDER"/log/";
                    mkdir -p $SPEC_OUTPUT_FOLDER"/img/";


                    # Produce SIM results only if was not already done
                    if test ! -f $OUTPUT_FILE.gz
                    then
                        if test ! -f $OUTPUT_FILE
                        then
                            echo " * Running SIM $TYPE $ALPHABET $MIN_SPATIAL_FREQ $MIN_BLOCK_FREQ $MAX_TIME_WINDOW [...]";
                            time sim $INPUT_FILE $OUTPUT_FILE $LOG_FILE $MIN_SPATIAL_FREQ $MIN_BLOCK_FREQ
                            #$MAX_TIME_WINDOW;
                        fi;
                    fi;

                    # Produce Stacked Bar data and images only if was not already done
                    if test ! -d $IMG_DIR
                    then
                        if test -f $OUTPUT_FILE.gz
                        then
                            echo " * Decompressing previous output file [...]";
                            gzip -d ${OUTPUT_FILE}.gz;
                        fi;
                        if test -f $OUTPUT_FILE
                        then
                            #echo " * Producing Stacked Bar data"
                            #R --vanilla --slave --file=R/stacked_bar_data.r --args $OUTPUT_FILE $MIN_SPATIAL_FREQ $MIN_BLOCK_FREQ $MAX_TIME_WINDOW;

                            echo " * Plotting data $TYPE $ALPHABET $MIN_SPATIAL_FREQ $MIN_BLOCK_FREQ $MAX_TIME_WINDOW [...]";
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
                done;
            done;
        done;
    done;
done;



# # plotting stack bars
# echo " * Plotting stack bars [...]";
# for INPUT_INLINE in "${INPUT_INLINES[@]}"
# do
#     for TYPE in "${TYPES[@]}"
#     do
#         for ALPHABET in "${ALPHABETS[@]}"
#         do
#             SPEC_OUTPUT_FOLDER=$OUTPUT_FOLDER"/inline-"$INPUT_INLINE"/"$ALPHABET"_"$TYPE;

#             SUP_I=0
#             while test "${SUP_I}" -lt "${#MIN_SPATIAL_FREQS[@]}"
#             do
#                 MIN_SPATIAL_FREQ="${MIN_SPATIAL_FREQS[SUP_I]}"
#                 MIN_BLOCK_FREQ="${MIN_BLOCK_FREQS[SUP_I]}"
#                 SUP_I=$((SUP_I+1));

#                 SUP_RANGE=$MIN_SPATIAL_FREQ"-"$MIN_BLOCK_FREQ
#                 OUTPUT_FILE=$SPEC_OUTPUT_FOLDER"/stackedbar/support_range/"$SUP_RANGE;
#                 R --vanilla --slave --file=R/stacked_bar_data_plot_supports.r --args $OUTPUT_FILE $SUP_RANGE;
#             done;

#             for MAX_TIME_WINDOW in "${MAX_TIME_WINDOWS[@]}"
#             do
#                 OUTPUT_FILE=$SPEC_OUTPUT_FOLDER"/stackedbar/time_window/"$MAX_TIME_WINDOW;
#                 R --vanilla --slave --file=R/stacked_bar_data_plot_times.r --args $OUTPUT_FILE $MAX_TIME_WINDOW;
#             done;
#         done;
#     done;
# done;



# End
