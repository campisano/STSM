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
    void test_mergeKernels_f100_size1_adjacent__does_joins();
    void test_mergeKernels_f100_size2_adjacent__does_joins();
    void test_mergeKernels_f100_size1_not_adjacent__does_not_joins();
    void test_mergeKernels_f100_size2_not_adjacent__does_not_joins();
    void test_mergeKernels_f75_nearby_2_1_2_that_should_joins__does_joins();
    void test_mergeKernels_f75_nearby_3_1_3_that_should_joins__does_joins();
    void test_mergeKernels_f50_nearby_half_3_1_3_that_should_joins__does_joins();
    void test_mergeKernels_f75_nearby_1_1_2_that_should_joins__does_joins();
    void test_mergeKernels_f90_nearby_1_1_2_that_should_not_joins__does_not_joins();
    void test_SIMRun_any_result();
    void test_SIMRun_f100_gets_only_single_ABCD100_solidSequence();
    void test_SIMRun_f75_does_get_EFGH75_solidSequence();
    void test_SIMRun_f90_does_not_get_EFGH75_solidSequence();
};

#endif
