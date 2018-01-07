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
source(file="R/utils.R", chdir=TRUE);

## set verbose mode
utils$setVerbose();



## read arguments
args = commandArgs(TRUE);

if(length(args) != 2) {
    cat("Usage: R --vanilla --slave --file=dat2csv_converter.R --args input.dat output.csv\n");
    quit(status=1);
}

input = args[1];
output = args[2];

## reading input data
data = utils$readCSV(input, header=FALSE, sep="\t");

cleaned_data = data[,-c(1,2,3)];
transposed_data = t(cleaned_data);

## writing output data
utils$writeCSV(transposed_data, output, header=FALSE, sep=",");
