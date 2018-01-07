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
#include <fstream>
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
        ("l,log", "Log file", cxxopts::value<std::string>(output_log))
        ("s,spatial", "Minimum spatial frequency",
         cxxopts::value<float>(min_spatial_frequency))
        ("b,block", "Minimum block frequency",
         cxxopts::value<float>(min_block_frequency))
        ("v,verbose", "Verbose output");
        cxxopts::ParseResult parsed = options.parse(_argn, _argv);

        if(parsed.count("h") != 0)
        {
            std::cout << options.help({"", "Group"}) << std::endl;
            return 0;
        }

        if(
            parsed.count("i") == 0 ||
            parsed.count("s") == 0 ||
            parsed.count("b") == 0
        )
        {
            std::cerr << "Error: 'input', 'spatial' and 'block' arguments"
                      << " are mandatory." << std::endl;
            return 1;
        }

        if(parsed.count("v") != 0)
        {
            std::cout << "    args:"
                      << " " << input_data_csv
                      << " " << result_json
                      << " " << output_log
                      << " " << min_spatial_frequency
                      << " " << min_block_frequency
                      << std::endl;
        }

        Database database;
        STSM stsm;

        clock_t begin = clock();
        int elapsed_s;

        if(parsed.count("v") != 0)
        {
            std::cout << "Loading database from "
                      << input_data_csv << "..." << std::endl;
        }

        loadDatabase(input_data_csv, database, false, false);

        if(parsed.count("v") != 0)
        {
            elapsed_s = floor(double(clock() - begin) / CLOCKS_PER_SEC);
            std::cout << "        load clock time: "
                      << elapsed_s << "s" << std::endl;

            std::cout << "Runnin STSM ..." << std::endl;
            begin = clock();
        }

        if(parsed.count("l") == 0)
        {
            if(parsed.count("v") == 0)
            {
                stsm.run(database, min_spatial_frequency, min_block_frequency);
            }
            else
            {
                stsm.run(
                    database,
                    min_spatial_frequency, min_block_frequency,
                    std::cout);
            }
        }
        else
        {
            std::ofstream log_stream;
            log_stream.open(output_log.c_str());
            stsm.run(
                database,
                min_spatial_frequency, min_block_frequency,
                log_stream);
            log_stream.close();
        }

        if(parsed.count("v") != 0)
        {
            elapsed_s = floor(double(clock() - begin) / CLOCKS_PER_SEC);
            std::cout << "        run clock time: "
                      << elapsed_s << "s" << std::endl;

            std::cout << "Saving results to "
                      << result_json << "..." << std::endl;
            begin = clock();
        }

        savePatterns(result_json, stsm.getPatterns());

        if(parsed.count("v") != 0)
        {
            elapsed_s = floor(double(clock() - begin) / CLOCKS_PER_SEC);
            std::cout << "        save clock time: "
                      << elapsed_s << "s" << std::endl;
        }

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
