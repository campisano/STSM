#!/bin/bash

MEM_LIMIT=5248000;      # 5GB
#MEM_LIMIT=7340032;      # 7GB
#MEM_LIMIT=15728640;     # 15GB
ulimit -v $MEM_LIMIT;
TIMECMD=(`which time` -f "real: %e, user: %U, sys: %S");



PATH=./BUILD/release:$PATH;
#PATH=./BUILD/debug:$PATH;
#ulimit -c unlimited;    # enable core dump, useful only in debug mode



INPUT_FOLDER="data";
OUTPUT_FOLDER="results";

#BASE_IMG_NAME="4755x2310.jpg";
BASE_IMG_NAME="951x462.jpg";



INLINES=("100" "401")
#INLINES=("100")

#ORIENTATIONS=("original" "transposed");
ORIENTATIONS=("original");

#SAXS=("25" "20" "15" "10" "5");
SAXS=("25" "10" "5");
#SAXS=("10");

#MIN_SPATIAL_FREQS=("100" "75" "50" "25");
MIN_SPATIAL_FREQS=("75" "50");
#MIN_SPATIAL_FREQS=("75");

#MIN_BLOCK_FREQS=("50" "25" "5");
MIN_BLOCK_FREQS=("25" "5");
#MIN_BLOCK_FREQS=("5");

#MAX_STRETCHS=("0" "2" "5" "10");
#MAX_STRETCHS=("0" "2" "5");
MAX_STRETCHS=("0");



for INLINE in "${INLINES[@]}"
do
    FILE_NAME=$INLINE"_sax";

    for ORIENTATION in "${ORIENTATIONS[@]}"
    do
        for SAX in "${SAXS[@]}"
        do
            INPUT_FILE=$INPUT_FOLDER"/"$FILE_NAME"-"$SAX"_"$ORIENTATION".csv";

            for MIN_SPATIAL_FREQ in "${MIN_SPATIAL_FREQS[@]}"
            do
                for MIN_BLOCK_FREQ in "${MIN_BLOCK_FREQS[@]}"
                do
                    for MAX_STRETCH in "${MAX_STRETCHS[@]}"
                    do
                        SPEC_OUTPUT_FOLDER=$OUTPUT_FOLDER"/inline-"$INLINE"_orientation-"$ORIENTATION"/sax-"$SAX;
                        BASE_FILENAME="I"$INLINE"_O"$ORIENTATION"_S"$SAX"_FS"$MIN_SPATIAL_FREQ"_FB"$MIN_BLOCK_FREQ"_MS"$MAX_STRETCH;

                        mkdir -p $SPEC_OUTPUT_FOLDER;

                        RUN_LOG=$SPEC_OUTPUT_FOLDER"/run_"$BASE_FILENAME".out";

                        echo "------------------------------------------------------------" 2>&1 | tee -a $RUN_LOG;
                        echo -e "I:"$INLINE"\tO:"$ORIENTATION"\tS:"$SAX"\tSF:"$MIN_SPATIAL_FREQ"\tBF:"$MIN_BLOCK_FREQ"\tMS:"$MAX_STRETCH 2>&1 | tee -a $RUN_LOG;

                        JSON_OUTPUT_FOLDER=$SPEC_OUTPUT_FOLDER"/json";
                        LOG_OUTPUT_FOLDER=$SPEC_OUTPUT_FOLDER"/log";
                        STATS_OUTPUT_FOLDER=$SPEC_OUTPUT_FOLDER"/stats/spatial-"$MIN_SPATIAL_FREQ"/block-"$MIN_BLOCK_FREQ"/stretch-"$MAX_STRETCH;
                        IMG_OUTPUT_FOLDER=$SPEC_OUTPUT_FOLDER"/img/spatial-"$MIN_SPATIAL_FREQ"/block-"$MIN_BLOCK_FREQ"/stretch-"$MAX_STRETCH;
                        OUTPUT_FILE=$JSON_OUTPUT_FOLDER"/"$BASE_FILENAME".json";
                        LOG_FILE=$LOG_OUTPUT_FOLDER"/"$BASE_FILENAME".log";

                        mkdir -p $JSON_OUTPUT_FOLDER;
                        mkdir -p $LOG_OUTPUT_FOLDER;
                        mkdir -p $STATS_OUTPUT_FOLDER;
                        mkdir -p $IMG_OUTPUT_FOLDER;

                        # Produce SIM results only if was not already done
                        if test ! -f $OUTPUT_FILE.gz
                        then
                            if test ! -f $OUTPUT_FILE
                            then
                                echo " * Running SIM $ORIENTATION $SAX $MIN_SPATIAL_FREQ $MIN_BLOCK_FREQ $MAX_STRETCH [...]" 2>&1 | tee -a $RUN_LOG;
                                "${TIMECMD[@]}" sim $INPUT_FILE $OUTPUT_FILE $LOG_FILE $MIN_SPATIAL_FREQ $MIN_BLOCK_FREQ
                                #$MAX_STRETCH;
                            fi;
                        fi;

                        # Produce Stacked Bar data and images only if was not already done
                        if test ! -f $STATS_OUTPUT_FOLDER"/complete" -o ! -f  $IMG_OUTPUT_FOLDER"/complete"
                        then
                            if test -f $OUTPUT_FILE.gz
                            then
                                echo " * Decompressing previous output file [...]" 2>&1 | tee -a $RUN_LOG;
                                gzip -d ${OUTPUT_FILE}.gz;
                            fi;
                            if test -f $OUTPUT_FILE
                            then
                                if test ! -f $STATS_OUTPUT_FOLDER"/complete"
                                then
                                    echo " * Producing Stacked Bar data" 2>&1 | tee -a $RUN_LOG;
                                    "${TIMECMD[@]}" R --vanilla --slave --file=R/stacked_bar_data.r --args $OUTPUT_FILE $STATS_OUTPUT_FOLDER $MIN_SPATIAL_FREQ $MIN_BLOCK_FREQ $MAX_STRETCH;
                                fi;

                                if test ! -f $IMG_OUTPUT_FOLDER"/complete"
                                then
                                    echo " * Plotting data $ORIENTATION $SAX $MIN_SPATIAL_FREQ $MIN_BLOCK_FREQ $MAX_STRETCH [...]" 2>&1 | tee -a $RUN_LOG;
                                    "${TIMECMD[@]}" R --vanilla --slave --file=R/display.r --args $INPUT_FILE $OUTPUT_FILE $IMG_OUTPUT_FOLDER $INPUT_FOLDER"/inline_"$INLINE"_"$BASE_IMG_NAME;
                                fi;
                            fi;
                        fi;

                        # Compress outputs
                        if test -f $OUTPUT_FILE
                        then
                            echo " * Compressing ouput file [...]" 2>&1 | tee -a $RUN_LOG;
                            rm -f $OUTPUT_FILE.gz;
                            gzip --fast $OUTPUT_FILE;
                        fi;
                    done;
                done;
            done;
        done;
    done;
done;

echo "------------------------------------------------------------";



# # plotting stack bars
# echo " * Plotting stack bars [...]";
# for INLINE in "${INLINES[@]}"
# do
#     for ORIENTATION in "${ORIENTATIONS[@]}"
#     do
#         for SAX in "${SAXS[@]}"
#         do
#             SPEC_OUTPUT_FOLDER=$OUTPUT_FOLDER"/inline-"$INLINE"/"$SAX"_"$ORIENTATION;

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

#             for MAX_STRETCH in "${MAX_STRETCHS[@]}"
#             do
#                 OUTPUT_FILE=$SPEC_OUTPUT_FOLDER"/stackedbar/time_window/"$MAX_STRETCH;
#                 R --vanilla --slave --file=R/stacked_bar_data_plot_times.r --args $OUTPUT_FILE $MAX_STRETCH;
#             done;
#         done;
#     done;
# done;



# End
