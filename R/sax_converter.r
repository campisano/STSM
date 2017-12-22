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

# include utility file
source(file="R/utils.r", chdir=TRUE);

utils$setVerbose();

# loading dependences
loaded_libs = utils$loadLibs(c("TSMining"));

# read arguments
args = commandArgs(TRUE);

if(length(args) != 3) {
    cat("Usage: R --vanilla --slave --file=sax_conveter.r --args floatvalues.csv sax.csv alphabet_size\n");
    quit(status=1);
}

input = args[1];
output = args[2];
alphabet = as.numeric(args[3]);

# reading input data
input_data = utils$readCSV(input, header=FALSE, sep=",");


# sampling columns, for speedup test only
# input_data = t(input_data);
# input_data = input_data[
#     sample(nrow(input_data),replace=F,size=0.01*nrow(input_data)),];
# input_data = t(input_data);

# merging dataframe rows in a vector
# the transpose here is needed to join the matrix as vector
# and to convert it back with the same orientation, without transpose
vector_data = as.vector(t(input_data));

# applying SAX
sax_data = Func.SAX(
    x=vector_data, w=length(vector_data), a=alphabet, eps=.1, norm=TRUE
);

# test data for debugging
#input_data = data.frame(
#    x = 1, y = 1:10, fac = sample(91:93, 10, replace = TRUE));
#output_data = as.vector(t(input_data));
#input_data == output_data;

# splitting the vector back in a dataframe
# it need a transposed vector, you can test that using the code above
output_data = data.frame(
    do.call(rbind, split(sax_data, ceiling(
        seq_along(sax_data) / ncol(input_data)
    )))
);

# writing to disk
utils$writeCSV(output_data, output, header=FALSE, sep=",");

# filtred_output_data = output_data[13:ncol(output_data)];
#
# writing to disk
# utils$writeCSV(filtred_output_data, output, header=FALSE, sep=",");
