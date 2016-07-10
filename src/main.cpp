#include "GSP.h"
#include <cmath>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

// make clean all && reset && ./BUILD/release/gsp > test_N.out

int main(int argc, char *argv[])
{
    if(argc != 7)
    {
        std::cerr << "Bad number of arguments." << std::endl;
        std::cerr << "Usage: "
            << argv[0]
            << " <input_data.csv> <result.json>"
            << " <output.log> <min_support> <max_support>"
            << " <max_time_window>" << std::endl;

        return 1;
    }

    std::string input_data_csv = argv[1];
    std::string result_json = argv[2];
    std::string output_log = argv[3];
    unsigned int min_support;
    std::stringstream(argv[4]) >> min_support;
    unsigned int max_support;
    std::stringstream(argv[5]) >> max_support;
    unsigned int max_time_window;
    std::stringstream(argv[6]) >> max_time_window;

    std::cout << "    running: "
        << input_data_csv
        << " " << result_json
        << " " << output_log
        << " " << min_support
        << " " << max_support
        << " " << max_time_window
        << std::endl;

    GSP gsp;

    clock_t begin;
    int elapsed_s;

    begin = clock();
    gsp.run(
        input_data_csv, output_log, min_support, max_support, max_time_window);
    elapsed_s = floor(double(clock() - begin) / CLOCKS_PER_SEC);
    std::cout << "        run clock time: " << elapsed_s << "s" << std::endl;

    std::cout << "    saving to " << result_json << std::endl;

    begin = clock();
    gsp.saveJSON(result_json);
    elapsed_s = floor(double(clock() - begin) / CLOCKS_PER_SEC);
    std::cout << "        save clock time: " << elapsed_s << "s" << std::endl;

    return 0;

    ////////////////////////////////////////////////////////////////////////////
    // Test 1:
    // If algorith find a frequent pattern, it's really frequent
    // (using a small dataset with manually defined data and frequent sequences)
    //
    // can find synthetic sequences without gap
    //{
        //GSP gsp;
        //gsp.run(
            //"data/100_sax-26_sample-26x10_synthetic",
            //"results/test_1.json", "results/test_1.log",
            //100, 100, 0);

        //length: 6
                //sequence: <(C)(C)(A)(R)(D)(O)>
                //count: 10
                //sequence: <(I)(C)(C)(A)(R)(D)>
                //count: 10
                //sequence: <(R)(I)(C)(C)(A)(R)>
                //count: 10
        //length: 7
                //sequence: <(I)(C)(C)(A)(R)(D)(O)>
                //count: 10
                //sequence: <(R)(I)(C)(C)(A)(R)(D)>
                //count: 10
        //length: 8
                //sequence: <(R)(I)(C)(C)(A)(R)(D)(O)>
                //count: 10
    //}

    ////////////////////////////////////////////////////////////////////////////
    // Test 2:
    // If there is a frequent pattern, the algorithm will find it
    // (using a modified dataset with injected frequent sequences)
    //
    // can find synthetic sequences without gap
    //{
        //GSP gsp;
        //gsp.run(
            //"data/100_sax-26_synthetic.csv",
            //"results/test_2.json", "results/test_2.log",
            //100, 100, 0);

        //length: 7
                //sequence: <(A)(M)(P)(I)(S)(A)(N)>
                //count: 951
                //sequence: <(C)(A)(M)(P)(I)(S)(A)>
                //count: 951
                //sequence: <(I)(C)(C)(A)(R)(D)(O)>
                //count: 951
                //sequence: <(M)(P)(I)(S)(A)(N)(O)>
                //count: 951
                //sequence: <(R)(I)(C)(C)(A)(R)(D)>
                //count: 951
        //length: 8
                //sequence: <(A)(M)(P)(I)(S)(A)(N)(O)>
                //count: 951
                //sequence: <(C)(A)(M)(P)(I)(S)(A)(N)>
                //count: 951
                //sequence: <(R)(I)(C)(C)(A)(R)(D)(O)>
                //count: 951
        //length: 9
                //sequence: <(C)(A)(M)(P)(I)(S)(A)(N)(O)>
                //count: 951
    //}

    ////////////////////////////////////////////////////////////////////////////
    // Test 3:
    // can find synthetic sequences with gap <= 0
    //{
        //GSP gsp;
        //gsp.run(
            //"data/100_sax-26_synthetic_manual_gap_0.csv",
            //"results/test_3.json", "results/test_3.log",
            //100, 100, 0);
        // injected M,I,N,z,G,A,P,z,z,z,A in all 951 lines
        //
        // results as espected:
        //length: 11
                //sequence: <(M)(I)(N)(z)(G)(A)(P)(z)(z)(z)(A)>
                //count: 951
    //}



    //// test 6: can find synthetic sequences with gap <= 1
    //{
        //GSP gsp;
        //gsp.run(
            //"data/100_sax-26_synthetic_manual_gap_1.csv",
            //"out/test_6.json", "out/test_6.log", 950, 1);
        //// injected M,I,N,z,G,A,P,z,z,z,A in all 951 lines
        //// changed the injected in the last line to M,I,N,z,G,A,P,z,Z,z,A
        //// results as espected:
        ////<(M)(I)(N)(z)(G)(A)(P)(z)(z)(A)>  951
        ////<(M)(I)(N)(z)(G)(A)(P)(z)(z)(z)(A)>   950
    //}

    //// test 7: can find synthetic sequences with gap <= 1
    //{
        //GSP gsp;
        //gsp.run(
            //"data/100_sax-26_synthetic_manual_gap_2a.csv",
            //"out/test_7.json", "out/test_7.log", 950, 1);
        //// injected M,I,N,z,G,A,P,z,z,z,A in all 951 lines
        //// changed the injected in the last line to M,I,N,z,G,A,P,Z,z,Z,A
        //// results as espected:
        ////<(M)(I)(N)(z)(G)(A)(P)(z)>    951
        ////<(M)(I)(N)(z)(G)(A)(P)(z)(z)(z)(A)>   950
    //}

    //// test 8: can find synthetic sequences with gap <= 2
    //{
        //GSP gsp;
        //gsp.run(
            //"data/100_sax-26_synthetic_manual_gap_2b.csv",
            //"out/test_8.json", "out/test_8.log", 950, 2);
        //// injected M,I,N,z,G,A,P,z,z,z,A in all 951 lines
        //// changed the injected in the last line to M,I,N,z,G,A,P,z,Z,Z,A
        //// results as espected:
        ////<(M)(I)(N)(z)(G)(A)(P)(z)(A)> 951
        ////<(M)(I)(N)(z)(G)(A)(P)(z)(z)(z)(A)>   950
    //}

    //// test 9: can find synthetic sequences with gap <= 2
    //{
        //GSP gsp;
        //gsp.run(
            //"data/100_sax-26_synthetic_manual_gap_2c.csv",
            //"out/test_9.json", "out/test_9.log", 950, 2);
        //// injected M,I,N,z,G,A,P,z,z,z,A in all 951 lines
        //// changed the injected in the last line to M,I,N,z,G,A,P,z,Z,Z,z,A
        //// results as espected:
        ////<(M)(I)(N)(z)(G)(A)(P)(z)(z)(A)>  951
        ////<(M)(I)(N)(z)(G)(A)(P)(z)(z)(z)(A)>   950
    //}

    //return 0;
}

/* TODOs

- fix

 * Running GSP original 5 95 3 [...]
running: data/100_sax-5_original.csv results/5_original/json/100_sax-5_original_s95_g3.json results/5_original/log/100_sax-5_original_s95_g3.log 95 3
terminate called after throwing an instance of 'std::runtime_error'
  what():  Current support count function doesn't handle sequence with less then two items.
./run.sh: line 33: 24198 Aborted                 gsp $INPUT_FILE $OUTPUT_FILE $LOG_FILE $SUPPORT $GAP



- better performance

1) use prefix tree
    a
   /
  b
 / \
c   d
: abcgf abcgh abdgf abdgh
: leaf of c : abcgf abcgh
: leaf of d : abdgf abdgh
: in a vector of vectors for instance
: associated to leaf "c"
: and leaf "d"

2) read from the CSV file one input data-sequence at time and check all candidate sequences in this data-sequence before to go at the next data-sequence:
: candidates : <(a)(a)(b)> and <(a)(b)(c)>
: data sequences : <(a)(b)(a)(b)> and <(a)(b)(c)(d)>
: is C1 included in D1? yes
: support of C1++
: is C2 included in D1? no
: read D2 and compare to the candidates
: is C1 included in D2? no
: is C2 included in D2? yes
: support of C2++
: say that abde is not frequent
: you have created the candidate abcde
: for each frequent sequence given by the algorithm : check it is actually frequent in the dataset
: for each actual frequent sequence in the database : check it is given by the algorithm

*/
