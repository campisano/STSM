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
