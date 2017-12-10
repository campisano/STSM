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

#ifndef STSM_TEST__H__
#define STSM_TEST__H__

#include <cxxtools/unit/testsuite.h>

#include "STSM.h"

class STSMTest : public cxxtools::unit::TestSuite, public STSM
{
public:
    explicit STSMTest();
    virtual ~STSMTest();

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
    void test_STSMRun_any_result();
    void test_STSMRun_f100_gets_only_single_ABCD100_solidSequence();
    void test_STSMRun_f100_testing_ABCD100_positions();
    void test_STSMRun_f75_does_get_EFGH75_solidSequence();
    void test_STSMRun_f90_does_not_get_EFGH75_solidSequence();
    void test_STSMRun_f100_b100_does_get_EFGHI_solidBlock();
    void test_STSMRun_f100_b100_does_not_get_diagonal_EFGHI();
    void test_STSMRun_f75_b75_does_get_EFGHI7575_solidBlock();
    void test_STSMRun_f100_b50_does_get_EFGHI10025_solidBlock();
    void test_STSMRun_f100_b75_same_line_does_get_EFGHI10034_solidBlock();
    void test_STSMRun_f100_b50_does_get_ABCD_big_solidBlock();
};

#endif
