#ifndef GSP_TEST__H__
#define GSP_TEST__H__

#include <cxxtools/unit/testsuite.h>
#include <list>

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
        void test_getAllFirstLevelContigousSubsequences();
        void test_getSubsequenceDroppingFirstItemFromSingleItemset();
        void test_getSubsequenceDroppingFirstItemFromMultipleItemset();
        void test_getSubsequenceDroppingLastItemFromSingleItemset();
        void test_getSubsequenceDroppingLastItemFromMultipleItemset();
        void test_GSP_join();
        void test_GSP_run();

        void prepare(Sequence &_seq);
        void prepareContSubseqFirsts(std::list<Sequence> &_cont_subseq);
        void prepareContSubseqLasts(std::list<Sequence> &_cont_subseq);
        void prepareContSubseqMiddles(std::list<Sequence> &_cont_subseq);
        void prepareContSubseqMix(std::list<Sequence> &_cont_subseq);
};

#endif
