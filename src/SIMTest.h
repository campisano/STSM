#ifndef SIM_TEST__H__
#define SIM_TEST__H__

#include <cxxtools/unit/testsuite.h>

#include "SIM.h"

class SIMTest : public cxxtools::unit::TestSuite, public SIM
{
public:
    explicit SIMTest();
    virtual ~SIMTest();

private:
    void test_sequenceEqualityOperator();
    void test_sequenceStringRepresentation();
    void test_SIMRun_anyResult();
    void test_SIMRun_f100_gets_only_single_ABCD100_solidSequence();
    void test_SIMRun_f75_does_get_EFGH75_solidSequence();
    void test_SIMRun_f90_does_not_get_EFGH75_solidSequence();
};

#endif
