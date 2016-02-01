#ifndef GSP_TEST__H__
#define GSP_TEST__H__

#include <cxxtools/unit/testsuite.h>
#include <vector>

#include "GSP.h"
#include "Sequence.h"

class GSPTest : public cxxtools::unit::TestSuite, public GSP
{
    public:
        explicit GSPTest();
        virtual ~GSPTest();

    private:
        void test_equality();
        void test_sequenceStringRepresentation();
        void test_getAllContiguosSubsequencesDroppingAnItemFromFirstItemset();
        void test_getAllContiguosSubsequencesDroppingAnItemFromLastItemset();
        void test_getAllContiguosSubsequencesDroppingAnItemFromAnyMiddleItemset();
        void test_getAllFirstLevelContigousSubsequences();
        void test_getSubsequenceDroppingFirstItemFromSingleItemset();
        void test_getSubsequenceDroppingFirstItemFromMultipleItemset();
        void test_getSubsequenceDroppingLastItemFromSingleItemset();
        void test_getSubsequenceDroppingLastItemFromMultipleItemset();
        void test_GSP_join();
        void test_GSP_run();

        void prepare(Sequence &_seq);
        void prepareContSubseqFirsts(std::vector<Sequence> &_cont_subseq);
        void prepareContSubseqLasts(std::vector<Sequence> &_cont_subseq);
        void prepareContSubseqMiddles(std::vector<Sequence> &_cont_subseq);
        void prepareContSubseqMix(std::vector<Sequence> &_cont_subseq);
};

#endif
