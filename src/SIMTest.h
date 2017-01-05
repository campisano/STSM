#ifndef SIM_TEST__H__
#define SIM_TEST__H__

#include <cxxtools/unit/testsuite.h>

#include "SIM.h"
#include "Sequence.h"

class SIMTest : public cxxtools::unit::TestSuite, public SIM
{
    public:
        explicit SIMTest();
        virtual ~SIMTest();

    private:
        void test_equality();
        void test_sequenceStringRepresentation();

        void prepare(Sequence &_seq);
};

#endif
