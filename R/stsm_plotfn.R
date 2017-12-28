## Copyright (C) 2017 Riccardo Campisano <riccardo.campisano@gmail.com>
##               2017 Fabio Porto
##               2017 Fabio Perosi
##               2017 Esther Pacitti
##               2017 Florent Masseglia
##               2017 Eduardo Ogasawara
##
## This file is part of STSM (Spatio-Temporal Sequence Miner).
##
## STSM is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## STSM is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with STSM.  If not, see <http://www.gnu.org/licenses/>.



## include utility file
## MUST BE INCLUDED BY THE CALLING SOURCE
## source(file="utils.R", chdir=TRUE);



####
## defining a specific plot function
plotSequencePositionsRangesAndBlocks_src = function(
    x_points, y_points,
    xmin_ranges=c(), xmax_ranges=c(),
    xmin_blocks=c(), xmax_blocks=c(),
    ymin_blocks=c(), ymax_blocks=c(),
    lim_x_min=NA, lim_x_max=NA, lim_y_min=-NA, lim_y_max=NA,
    clean=TRUE, scale=1) {

    df_points = data.frame(x=x_points, y=y_points);
    df_rectangles = data.frame(xmin=xmin_ranges, xmax=xmax_ranges);
    df_blocks = data.frame(
        xmin=xmin_blocks, xmax=xmax_blocks,
        ymin=ymin_blocks, ymax=ymax_blocks
        );

    ####
    ## define limits, if not defined
    if(is.na(lim_x_min)) {
        lim_x_min = min(c(df_points$x, df_rectangles$xmin, df_blocks$xmin));
    }

    if(is.na(lim_x_max)) {
        lim_x_max = max(c(df_points$x, df_rectangles$xmax, df_blocks$xmax));
    }

    if(is.na(lim_y_min)) {
        lim_y_min = min(c(df_points$y, df_rectangles$ymin, df_blocks$ymin));
    }

    if(is.na(lim_y_max)) {
        lim_y_max = max(c(df_points$y, df_rectangles$ymax, df_blocks$ymax));
    }

    gg = ggplot();

    ####
    ## if defined, print the ranges
    if(length(xmin_ranges) > 0 && length(xmax_ranges) > 0) {
        gg = gg + geom_rect(
            data=df_rectangles,
            aes_string(
                xmin="xmin", xmax="xmax",
                ymin=-Inf, ymax=Inf),
            size=0.5 * scale,
            color=alpha("green", 0.5), fill="darkgreen", alpha=0.1);
    }

    ####
    ## then print the points
    if(length(x_points) > 0 && length(y_points) > 0) {
        gg = gg + geom_point(
            ## shape=15,  # square
            shape=17,  # triangle
            ## shape=18,  # diamond
            data=df_points,
            aes_string("x", "y"),
            size=2 * scale, color="darkblue");
    }

    ####
    ## finally, if defined, print the blocks
    if(
        length(xmin_blocks) > 0 && length(xmax_blocks) > 0 &&
            length(ymin_blocks) > 0 && length(ymax_blocks) > 0
    ) {
        gg = gg + geom_rect(
            data=df_blocks,
            aes_string(
                xmin="xmin", xmax="xmax",
                ymin="ymin", ymax="ymax"),
            size=0.5 * scale,
            color=alpha("red", 1), fill="darkred", alpha=0.5);
    }

    ####
    ## defines the limites
    gg = gg + coord_cartesian(
        xlim=c(lim_x_min, lim_x_max), ylim=c(lim_y_min, lim_y_max));

    if(clean) {
        gg = gg + scale_x_continuous(expand=c(0,0));
        ####
        ## start from the top to the bottom
        gg = gg + scale_y_continuous(expand=c(0,0), trans="reverse");

        gg = gg + labs(x=NULL, y=NULL, title=NULL);
        gg = gg + theme(panel.background=element_rect(
            fill="transparent", colour=NA));
        gg = gg + theme(plot.background=element_rect(
            fill="transparent", colour=NA));
        gg = gg + theme(panel.grid=element_blank());
        gg = gg + theme(panel.border=element_blank());
        gg = gg + theme(plot.margin=unit(c(0,0,0,0), "null"));
        ## gg = gg + theme(panel.margin=unit(c(0,0,0,0), "null"));
        gg = gg + theme(axis.ticks=element_blank());
        gg = gg + theme(axis.text=element_blank());
        gg = gg + theme(axis.title=element_blank());
        gg = gg + theme(axis.line=element_blank());
        gg = gg + theme(legend.position="none");
        gg = gg + theme(axis.ticks.length=unit(0, "null"));
        ## gg = gg + theme(legend.margin=unit(0, "null"));
    } else {
        ####
        ## start from the top to the bottom
        gg = gg + scale_y_continuous(trans="reverse");
    }

    return(gg);
}

## MUST BE LOADED BY THE CALLING SOURCE
## loaded_libs = utils$loadLibs(c("compiler:3.1.1"));
plotSequencePositionsRangesAndBlocks = cmpfun(
    plotSequencePositionsRangesAndBlocks_src);
## utils$unloadLibs(loaded_libs);



####
## test plot function
## utils$dev_open_file("stsm_plot_test.pdf");
## ## MUST BE LOADED BY THE CALLING SOURCE
## ## loaded_libs = utils$loadLibs(c("ggplot2:1.0.0"));
## plotSequencePositionsRangesAndBlocks(
##     rnorm(100,2), rnorm(100,2),
##     xmin_ranges=c(1, 2, 3), xmax_ranges=c(1.5, 2.5, 4),
##     xmin_blocks=c(3, 6), xmax_blocks=c(4, 8),
##     ymin_blocks=c(2, 4), ymax_blocks=c(5, 10),
##     lim_x_min=0, lim_x_max=10, lim_y_min=0, lim_y_max=10,
##     clean=FALSE, scale=5);
## ## utils$unloadLibs(loaded_libs);
## utils$dev_off();
