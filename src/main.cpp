// Copyright (C) 2017 Riccardo Campisano <riccardo.campisano@gmail.com>
//               2017 Fabio Porto
//               2017 Fabio Perosi
//               2017 Esther Pacitti
//               2017 Florent Masseglia
//               2017 Eduardo Ogasawara
//
// This file is part of STSM (Spatio-Temporal Sequence Miner).
//
// STSM is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// STSM is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with STSM.  If not, see <http://www.gnu.org/licenses/>.

#include "STSM.h"
#include <cmath>
#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

// make clean all && reset && ./BUILD/release/stsm > test_N.out

int main(int _argn, char * _argv[])
{
    unsigned int expected_arguments = 6;
    std::vector < std::string > parameters;

    if(_argn != 1)
    {
        // read from command line arguments
        for(int i = 0; i < _argn; ++i)
        {
            parameters.push_back(_argv[i]);
        }
    }
    else
    {
        parameters.push_back((_argv[0]));

        // read from standard input
        for(std::string line; std::getline(std::cin, line);)
        {
            parameters.push_back(line);
        }
    }

    if(parameters.size() != expected_arguments)
    {
        std::cerr << "Bad number of parameters: expected " << expected_arguments
                  << ", obtained " << parameters.size() << "." << std::endl;
        std::cerr << "Usage: "
                  << parameters[0]
                  << " <input_data.csv> <result.json>"
                  << " <output.log> <min_spatial_frequency>"
                  << " <min_block_frequency>"
                  << std::endl;

        std::cerr << "Alternative usage (you can mix them): "
                  << parameters[0] << " < filename_with_arguments.txt"
                  << std::endl;

        return 1;
    }

    std::string input_data_csv = parameters[1];
    std::string result_json = parameters[2];
    std::string output_log = parameters[3];
    unsigned int min_spatial_frequency;
    std::stringstream(parameters[4]) >> min_spatial_frequency;
    unsigned int min_block_frequency;
    std::stringstream(parameters[5]) >> min_block_frequency;

    std::cout << "    args: "
              << input_data_csv
              << " " << result_json
              << " " << output_log
              << " " << min_spatial_frequency
              << " " << min_block_frequency
              << std::endl;

    try
    {
        STSM stsm;

        clock_t begin;
        int elapsed_s;

        begin = clock();
        stsm.run(
            input_data_csv, output_log,
            min_spatial_frequency, min_block_frequency);
        elapsed_s = floor(double(clock() - begin) / CLOCKS_PER_SEC);
        std::cout
            << "        run clock time: " << elapsed_s << "s" << std::endl;

        std::cout << "    saving to " << result_json << std::endl;

        begin = clock();
        stsm.saveJSON(result_json);
        elapsed_s = floor(double(clock() - begin) / CLOCKS_PER_SEC);
        std::cout
            << "        save clock time: " << elapsed_s << "s" << std::endl;

        return 0;
    }
    catch(const std::exception & _exc)
    {
        std::cerr << "std::exception: " << _exc.what() << std::endl;
        return 1;
    }
    catch(...)
    {
        std::cerr << "unknown exception!" << std::endl;
        return 1;
    }
}
