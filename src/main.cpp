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

#include <cmath>
#include <ctime>
#include <cxxopts.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

#include "database_loader.h"
#include "patterns_saver.h"
#include "Database.h"
#include "STSM.h"

int main(int _argn, char * _argv[])
{
    try
    {
        std::string input_data_csv;
        std::string result_json;
        std::string output_log;
        float min_spatial_frequency;
        float min_block_frequency;

        cxxopts::Options options(_argv[0], "Spatio-Temporal Sequence Miner");
        options.add_options()
        ("h,help", "Print help")
        ("i,input", "Input", cxxopts::value<std::string>(input_data_csv))
        ("o,output", "Output file",
         cxxopts::value<std::string>(result_json)->default_value("result.json"))
        ("l,log", "Log file", cxxopts::value<std::string>
         (output_log)->default_value("stsm.log"))
        ("s,spatial", "Minimum spatial frequency",
         cxxopts::value<float>(min_spatial_frequency))
        ("b,block", "Minimum block frequency",
         cxxopts::value<float>(min_block_frequency));
        cxxopts::ParseResult parsed = options.parse(_argn, _argv);

        if(parsed.count("h"))
        {
            std::cout << options.help({"", "Group"}) << std::endl;
            return 0;
        }

        if(parsed.count("i") != 1 || parsed.count("s") != 1 || parsed.count("b") != 1)
        {
            std::cout << "Error: 'input', 'spatial' and 'block' arguments"
                      << " are mandatory." << std::endl;
            return 1;
        }

        std::cout << "    args:"
                  << " " << input_data_csv
                  << " " << result_json
                  << " " << output_log
                  << " " << min_spatial_frequency
                  << " " << min_block_frequency
                  << std::endl;

        Database database;
        STSM stsm;

        clock_t begin;
        int elapsed_s;

        std::cout << "Loading database from "
                  << input_data_csv << "..." << std::endl;
        begin = clock();
        loadDatabase(input_data_csv, database);
        elapsed_s = floor(double(clock() - begin) / CLOCKS_PER_SEC);
        std::cout << "        load clock time: "
                  << elapsed_s << "s" << std::endl;

        std::cout << "Runnin STSM ..." << std::endl;
        begin = clock();
        stsm.run(
            database, output_log,
            min_spatial_frequency, min_block_frequency);
        elapsed_s = floor(double(clock() - begin) / CLOCKS_PER_SEC);
        std::cout << "        run clock time: "
                  << elapsed_s << "s" << std::endl;

        std::cout << "Saving results to "
                  << result_json << "..." << std::endl;
        begin = clock();
        savePatterns(result_json, stsm.getPatterns());
        elapsed_s = floor(double(clock() - begin) / CLOCKS_PER_SEC);
        std::cout << "        save clock time: "
                  << elapsed_s << "s" << std::endl;

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
