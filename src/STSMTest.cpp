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

#include "STSMTest.h"

#include <cxxtools/unit/registertest.h>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Candidate.h"
#include "Kernel.h"
#include "Sequence.h"

namespace
{
    double EPSILON = 0.0001;
    std::string TEST_FOLDER = "test_output";

    void prepareTestFolder(
        const std::string & _data,
        std::string & _input_file,
        std::string & _log_file)
    {
        _input_file = TEST_FOLDER + "/" + "input.csv";
        _log_file = TEST_FOLDER + "/" + "output.log";
        CXXTOOLS_UNIT_ASSERT_EQUALS(
            system(("mkdir -p " + TEST_FOLDER).c_str()), 0);

        std::ofstream ofs(_input_file.c_str());
        ofs << _data;
        ofs.close();
    }

    void cleanupTestFolder()
    {
        CXXTOOLS_UNIT_ASSERT_EQUALS(
            system(("rm -f " + TEST_FOLDER + "/*").c_str()), 0);
        CXXTOOLS_UNIT_ASSERT_EQUALS(
            system(("rmdir " + TEST_FOLDER + "/").c_str()),  0);
    }
}

STSMTest::STSMTest() : cxxtools::unit::TestSuite("STSMTest")
{
    std::cout << std::endl << "Test methods:" << std::endl;

    registerMethod(
        "test_sequenceEqualityOperator",
        *this,
        &STSMTest::test_sequenceEqualityOperator);

    registerMethod(
        "test_sequenceStringRepresentation",
        *this,
        &STSMTest::test_sequenceStringRepresentation);

    registerMethod(
        "test_mergeKernels_f100_size1_adjacent__does_joins",
        *this,
        &STSMTest::test_mergeKernels_f100_size1_adjacent__does_joins);

    registerMethod(
        "test_mergeKernels_f100_size2_adjacent__does_joins",
        *this,
        &STSMTest::test_mergeKernels_f100_size2_adjacent__does_joins);

    registerMethod(
        "test_mergeKernels_f100_size1_not_adjacent__does_not_joins",
        *this,
        &STSMTest::test_mergeKernels_f100_size1_not_adjacent__does_not_joins);

    registerMethod(
        "test_mergeKernels_f100_size2_not_adjacent__does_not_joins",
        *this,
        &STSMTest::test_mergeKernels_f100_size2_not_adjacent__does_not_joins);

    registerMethod(
        "test_mergeKernels_f75_nearby_2_1_2_that_should_joins__does_joins",
        *this,
        &STSMTest::test_mergeKernels_f75_nearby_2_1_2_that_should_joins__does_joins);

    registerMethod(
        "test_mergeKernels_f75_nearby_3_1_3_that_should_joins__does_joins",
        *this,
        &STSMTest::test_mergeKernels_f75_nearby_3_1_3_that_should_joins__does_joins);

    registerMethod(
        "test_mergeKernels_f50_nearby_half_3_1_3_that_should_joins__does_joins",
        *this,
        &STSMTest::test_mergeKernels_f50_nearby_half_3_1_3_that_should_joins__does_joins);

    registerMethod(
        "test_mergeKernels_f75_nearby_1_1_2_that_should_joins__does_joins",
        *this,
        &STSMTest::test_mergeKernels_f75_nearby_1_1_2_that_should_joins__does_joins);

    registerMethod(
        "test_mergeKernels_f90_nearby_1_1_2_that_should_not_joins__does_not_joins",
        *this,
        &STSMTest::test_mergeKernels_f90_nearby_1_1_2_that_should_not_joins__does_not_joins);

    registerMethod(
        "test_STSMRun_any_result",
        *this,
        &STSMTest::test_STSMRun_any_result);

    registerMethod(
        "test_STSMRun_f100_gets_only_single_ABCD100_solidSequence",
        *this,
        &STSMTest::test_STSMRun_f100_gets_only_single_ABCD100_solidSequence);

    registerMethod(
        "test_STSMRun_f100_testing_ABCD100_positions",
        *this,
        &STSMTest::test_STSMRun_f100_testing_ABCD100_positions);

    registerMethod(
        "test_STSMRun_f75_does_get_EFGH75_solidSequence",
        *this,
        &STSMTest::test_STSMRun_f75_does_get_EFGH75_solidSequence);

    registerMethod(
        "test_STSMRun_f90_does_not_get_EFGH75_solidSequence",
        *this,
        &STSMTest::test_STSMRun_f90_does_not_get_EFGH75_solidSequence);

    registerMethod(
        "test_STSMRun_f100_b100_does_get_EFGHI_solidBlock",
        *this,
        &STSMTest::test_STSMRun_f100_b100_does_get_EFGHI_solidBlock);

    registerMethod(
        "test_STSMRun_f100_b100_does_not_get_diagonal_EFGHI",
        *this,
        &STSMTest::test_STSMRun_f100_b100_does_not_get_diagonal_EFGHI);

    registerMethod(
        "test_STSMRun_f75_b75_does_get_EFGHI7575_solidBlock",
        *this,
        &STSMTest::test_STSMRun_f75_b75_does_get_EFGHI7575_solidBlock);

    registerMethod(
        "test_STSMRun_f100_b50_does_get_EFGHI10025_solidBlock",
        *this,
        &STSMTest::test_STSMRun_f100_b50_does_get_EFGHI10025_solidBlock);

    registerMethod(
        "test_STSMRun_f100_b75_same_line_does_get_EFGHI10034_solidBlock",
        *this,
        &STSMTest::test_STSMRun_f100_b75_same_line_does_get_EFGHI10034_solidBlock);

    registerMethod(
        "test_STSMRun_f100_b50_does_get_ABCD_big_solidBlock",
        *this,
        &STSMTest::test_STSMRun_f100_b50_does_get_ABCD_big_solidBlock);
}

STSMTest::~STSMTest()
{
}

void STSMTest::test_sequenceEqualityOperator()
{
    // Arrange
    Sequence seq1, seq2;

    // Act
    seq1.set("<abcdefghilm>");
    seq2.set("<abcdefghilm>");

    // Assert
    CXXTOOLS_UNIT_ASSERT(seq1 == seq2);
}

void STSMTest::test_sequenceStringRepresentation()
{
    // Arrange
    Sequence seq1;
    std::string str_items = "abcdefghilm";
    std::string str_seq = "<" + str_items + ">";

    // Act
    seq1.set(str_seq);

    // Assert
    CXXTOOLS_UNIT_ASSERT(seq1.toString() == str_seq);
    CXXTOOLS_UNIT_ASSERT(seq1.toStringOfItems() == str_items);
}

void STSMTest::test_mergeKernels_f100_size1_adjacent__does_joins()
{
    // Arrange
    Kernel k1(0, 0);
    k1.frequency(1.0);
    k1.support(1);
    Kernel k2(1, 1);
    k2.frequency(1.0);
    k2.support(1);

    ListKernels kernels;
    kernels.push_back(k1);
    kernels.push_back(k2);

    Candidate cand(
        Sequence("<aaa>"),
        Range(0, 1),
        kernels);

    // Act
    cand.mergeKernels(1.0);

    // Assert
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().size(), 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().start(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().end(), 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().support(), 2);
    CXXTOOLS_UNIT_ASSERT(
        std::abs(cand.kernels().back().frequency() - 1.0) < EPSILON);
}

void STSMTest::test_mergeKernels_f100_size2_adjacent__does_joins()
{
    // Arrange
    Kernel k1(0, 1);
    k1.frequency(1.0);
    k1.support(2);
    Kernel k2(2, 3);
    k2.frequency(1.0);
    k2.support(2);

    ListKernels kernels;
    kernels.push_back(k1);
    kernels.push_back(k2);

    Candidate cand(
        Sequence("<aaa>"),
        Range(0, 3),
        kernels);

    // Act
    cand.mergeKernels(1.0);

    // Assert
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().size(), 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().start(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().end(), 3);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().support(), 4);
    CXXTOOLS_UNIT_ASSERT(
        std::abs(cand.kernels().back().frequency() - 1.0) < EPSILON);
}

void STSMTest::test_mergeKernels_f100_size1_not_adjacent__does_not_joins()
{
    // Arrange
    Kernel k1(0, 0);
    k1.frequency(1.0);
    k1.support(1);
    Kernel k2(2, 2);
    k2.frequency(1.0);
    k2.support(1);

    ListKernels kernels;
    kernels.push_back(k1);
    kernels.push_back(k2);

    Candidate cand(
        Sequence("<aaa>"),
        Range(0, 2),
        kernels);

    // Act
    cand.mergeKernels(1.0);

    // Assert
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().size(), 2);

    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().front().start(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().front().end(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().front().support(), 1);
    CXXTOOLS_UNIT_ASSERT(
        std::abs(cand.kernels().front().frequency() - 1.0) < EPSILON);

    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().start(), 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().end(), 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().support(), 1);
    CXXTOOLS_UNIT_ASSERT(
        std::abs(cand.kernels().back().frequency() - 1.0) < EPSILON);
}

void STSMTest::test_mergeKernels_f100_size2_not_adjacent__does_not_joins()
{
    // Arrange
    Kernel k1(0, 1);
    k1.frequency(1.0);
    k1.support(2);
    Kernel k2(3, 4);
    k2.frequency(1.0);
    k2.support(2);

    ListKernels kernels;
    kernels.push_back(k1);
    kernels.push_back(k2);

    Candidate cand(
        Sequence("<aaa>"),
        Range(0, 4),
        kernels);

    // Act
    cand.mergeKernels(1.0);

    // Assert
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().size(), 2);

    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().front().start(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().front().end(), 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().front().support(), 2);
    CXXTOOLS_UNIT_ASSERT(
        std::abs(cand.kernels().front().frequency() - 1.0) < EPSILON);

    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().start(), 3);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().end(), 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().support(), 2);
    CXXTOOLS_UNIT_ASSERT(
        std::abs(cand.kernels().back().frequency() - 1.0) < EPSILON);
}

void STSMTest::test_mergeKernels_f75_nearby_2_1_2_that_should_joins__does_joins()
{
    // Arrange
    Kernel k1(0, 1);
    k1.frequency(1.0);
    k1.support(2);
    Kernel k2(3, 4);
    k2.frequency(1.0);
    k2.support(2);

    ListKernels kernels;
    kernels.push_back(k1);
    kernels.push_back(k2);

    Candidate cand(
        Sequence("<aaa>"),
        Range(0, 4),
        kernels);

    // Act
    cand.mergeKernels(0.75);

    // Assert
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().size(), 1);

    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().start(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().end(), 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().support(), 4);
    CXXTOOLS_UNIT_ASSERT(
        std::abs(cand.kernels().back().frequency() - (4.0 / 5.0)) < EPSILON);
}

void STSMTest::test_mergeKernels_f75_nearby_3_1_3_that_should_joins__does_joins()
{
    // Arrange
    Kernel k1(0, 2);
    k1.frequency(1.0);
    k1.support(3);
    Kernel k2(4, 6);
    k2.frequency(1.0);
    k2.support(3);

    ListKernels kernels;
    kernels.push_back(k1);
    kernels.push_back(k2);

    Candidate cand(
        Sequence("<aaa>"),
        Range(0, 6),
        kernels);

    // Act
    cand.mergeKernels(0.75);

    // Assert
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().size(), 1);

    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().start(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().end(), 6);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().support(), 6);
    CXXTOOLS_UNIT_ASSERT(
        std::abs(cand.kernels().back().frequency() - (6.0 / 7.0)) < EPSILON);
}

void STSMTest::test_mergeKernels_f50_nearby_half_3_1_3_that_should_joins__does_joins()
{
    // Arrange
    Kernel k1(0, 2);
    k1.frequency(2.0 / 3.0);
    k1.support(2);
    Kernel k2(4, 6);
    k2.frequency(2.0 / 3.0);
    k2.support(2);

    ListKernels kernels;
    kernels.push_back(k1);
    kernels.push_back(k2);

    Candidate cand(
        Sequence("<aaa>"),
        Range(0, 6),
        kernels);

    // Act
    cand.mergeKernels(0.50);

    // Assert
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().size(), 1);

    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().start(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().end(), 6);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().support(), 4);
    CXXTOOLS_UNIT_ASSERT(
        std::abs(cand.kernels().back().frequency() - (4.0 / 7.0)) < EPSILON);
}

void STSMTest::test_mergeKernels_f75_nearby_1_1_2_that_should_joins__does_joins()
{
    // Arrange
    Kernel k1(0, 0);
    k1.frequency(1.0);
    k1.support(1);
    Kernel k2(2, 3);
    k2.frequency(1.0);
    k2.support(2);

    ListKernels kernels;
    kernels.push_back(k1);
    kernels.push_back(k2);

    Candidate cand(
        Sequence("<aaa>"),
        Range(0, 3),
        kernels);

    // Act
    cand.mergeKernels(0.75);

    // Assert
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().size(), 1);

    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().start(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().end(), 3);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().support(), 3);
    CXXTOOLS_UNIT_ASSERT(
        std::abs(cand.kernels().back().frequency() - (3.0 / 4.0)) < EPSILON);
}

void STSMTest::test_mergeKernels_f90_nearby_1_1_2_that_should_not_joins__does_not_joins()
{
    // Arrange
    Kernel k1(0, 0);
    k1.frequency(1.0);
    k1.support(1);
    Kernel k2(2, 3);
    k2.frequency(1.0);
    k2.support(2);

    ListKernels kernels;
    kernels.push_back(k1);
    kernels.push_back(k2);

    Candidate cand(
        Sequence("<aaa>"),
        Range(0, 3),
        kernels);

    // Act
    cand.mergeKernels(0.90);

    // Assert
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().size(), 2);

    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().front().start(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().front().end(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().front().support(), 1);
    CXXTOOLS_UNIT_ASSERT(
        std::abs(cand.kernels().front().frequency() - 1.0) < EPSILON);

    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().start(), 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().end(), 3);
    CXXTOOLS_UNIT_ASSERT_EQUALS(cand.kernels().back().support(), 2);
    CXXTOOLS_UNIT_ASSERT(
        std::abs(cand.kernels().back().frequency() - 1.0) < EPSILON);
}

void STSMTest::test_STSMRun_any_result()
{
    // Arrange

    Frequency min_spatial = 1.0;
    Frequency min_block = 1.0;
    std::string input_file;
    std::string log_file;

    std::stringstream ss;
    ss  << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
        << "X16,X17,X18,X19,X20,X21,X22,X23,X24,X25" << std::endl
        << "a,a,A,B,C,D,a,a,a,a,a,a,a,a,a,a,a,a,a,a,L,M,N,a,a" << std::endl
        << "b,b,b,A,B,C,D,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b" << std::endl
        << "c,c,c,c,c,A,B,C,D,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c" << std::endl
        << "d,A,B,C,D,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,L,M,N,d,d" << std::endl
        << "e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e" << std::endl
        << "f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f" << std::endl
        << "g,g,E,F,G,H,I,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,J,K,g" << std::endl
        << "h,h,h,h,h,E,F,G,H,I,h,h,h,h,h,h,h,h,h,h,h,h,J,K,h" << std::endl
        << "i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i" << std::endl
        << "l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,E,F,G,H,I,l,l,l,l" << std::endl;

    prepareTestFolder(ss.str(), input_file, log_file);

    // Act

    STSM::run(input_file, log_file, min_spatial * 100, min_block * 100);

    // Assert

    CXXTOOLS_UNIT_ASSERT(m_solid_sequences.size() > 0);
    CXXTOOLS_UNIT_ASSERT(m_solid_sequences[2].size() > 0);
    CXXTOOLS_UNIT_ASSERT(m_solid_sequences[3].size() > 0);
    CXXTOOLS_UNIT_ASSERT(m_solid_sequences[4].size() > 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequences[5].size(), 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequences[6].size(), 0);

    // Cleanup

    cleanupTestFolder();
}

void STSMTest::test_STSMRun_f100_gets_only_single_ABCD100_solidSequence()
{
    // Arrange

    Frequency min_spatial = 1.0;
    Frequency min_block = 1.0;
    std::string input_file;
    std::string log_file;

    std::stringstream ss;
    ss  << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
        << "X16,X17,X18,X19,X20,X21,X22,X23,X24,X25" << std::endl
        << "a,a,A,B,C,D,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a" << std::endl
        << "b,b,b,A,B,C,D,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b" << std::endl
        << "c,c,c,c,c,A,B,C,D,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c" << std::endl
        << "d,A,B,C,D,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d" << std::endl
        << "e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e" << std::endl
        << "f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f" << std::endl
        << "g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g" << std::endl
        << "h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h" << std::endl
        << "i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i" << std::endl
        << "l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l" << std::endl;

    prepareTestFolder(ss.str(), input_file, log_file);

    // Act

    STSM::run(input_file, log_file, min_spatial * 100, min_block * 100);

    // Assert

    // testing number and last expected results
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequences[4].size(), 1);

    RangedSequence & rg = m_solid_sequences[4].back();

    // testing synthetic known data
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.sequence().toStringOfItems(), "ABCD");
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().start(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().end(), 3);
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.support(), 4);
    CXXTOOLS_UNIT_ASSERT((rg.frequency() - min_spatial) < EPSILON);

    // Cleanup

    cleanupTestFolder();
}

void STSMTest::test_STSMRun_f100_testing_ABCD100_positions()
{
    // Arrange

    Frequency min_spatial = 1.0;
    Frequency min_block = 1.0;
    std::string input_file;
    std::string log_file;

    std::stringstream ss;
    ss  << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
        << "X16,X17,X18,X19,X20,X21,X22,X23,X24,X25" << std::endl
        << "a,a,A,B,C,D,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a" << std::endl
        << "b,b,b,A,B,C,D,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b" << std::endl
        << "c,c,c,c,c,A,B,C,D,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c" << std::endl
        << "d,A,B,C,D,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d" << std::endl
        << "e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e,e" << std::endl
        << "f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f" << std::endl
        << "g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g" << std::endl
        << "h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h" << std::endl
        << "i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i" << std::endl
        << "l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l" << std::endl;

    prepareTestFolder(ss.str(), input_file, log_file);

    // Act

    STSM::run(input_file, log_file, min_spatial * 100, min_block * 100);

    // Assert

    // testing number and last expected results
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequences[4].size(), 1);

    RangedSequence & rg = m_solid_sequences[4].back();
    ListPositions & positions = m_ranged_sequence_positions[&rg];
    std::vector < Position > v_pos (positions.begin(), positions.end());

    // testing expected positions
    CXXTOOLS_UNIT_ASSERT_EQUALS(positions.size(), 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(v_pos[0].first, 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(v_pos[0].second, 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(v_pos[1].first, 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(v_pos[1].second, 3);
    CXXTOOLS_UNIT_ASSERT_EQUALS(v_pos[2].first, 2);
    CXXTOOLS_UNIT_ASSERT_EQUALS(v_pos[2].second, 5);
    CXXTOOLS_UNIT_ASSERT_EQUALS(v_pos[3].first, 3);
    CXXTOOLS_UNIT_ASSERT_EQUALS(v_pos[3].second, 1);

    // Cleanup

    cleanupTestFolder();
}

void STSMTest::test_STSMRun_f75_does_get_EFGH75_solidSequence()
{
    // Arrange

    Frequency min_spatial = 0.75;
    Frequency min_block = 1.0;
    std::string input_file;
    std::string log_file;

    std::stringstream ss;
    ss  << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
        << "X16,X17,X18,X19,X20,X21,X22,X23,X24,X25" << std::endl
        << "a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a" << std::endl
        << "b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b" << std::endl
        << "c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c" << std::endl
        << "d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d" << std::endl
        << "e,e,e,e,e,e,e,e,e,e,e,e,e,E,F,G,H,e,e,e,e,e,e,e,e" << std::endl
        << "f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f" << std::endl
        << "g,g,g,g,g,g,g,g,g,g,g,g,g,E,F,G,H,g,g,g,g,g,g,g,g" << std::endl
        << "h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,E,F,G,H" << std::endl
        << "i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i" << std::endl
        << "l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l" << std::endl;

    prepareTestFolder(ss.str(), input_file, log_file);

    // Act

    STSM::run(input_file, log_file, min_spatial * 100, min_block * 100);

    // Assert

    // testing number and last expected results
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequences[4].size(), 1);

    RangedSequence & rg = m_solid_sequences[4].back();

    // testing synthetic known data
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.sequence().toStringOfItems(), "EFGH");
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().start(), 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().end(), 7);
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.support(), 3);
    CXXTOOLS_UNIT_ASSERT((rg.frequency() - min_spatial) < EPSILON);

    // Cleanup

    cleanupTestFolder();
}

void STSMTest::test_STSMRun_f90_does_not_get_EFGH75_solidSequence()
{
    // Arrange

    Frequency min_spatial = 0.9;
    Frequency min_block = 1.0;
    std::string input_file;
    std::string log_file;

    std::stringstream ss;
    ss  << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
        << "X16,X17,X18,X19,X20,X21,X22,X23,X24,X25" << std::endl
        << "a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a" << std::endl
        << "b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b" << std::endl
        << "c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c" << std::endl
        << "d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d" << std::endl
        << "e,e,e,e,e,e,e,e,e,e,e,e,e,E,F,G,H,e,e,e,e,e,e,e,e" << std::endl
        << "f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f,f" << std::endl
        << "g,g,g,g,g,g,g,g,g,g,g,g,g,E,F,G,H,g,g,g,g,g,g,g,g" << std::endl
        << "h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,E,F,G,H" << std::endl
        << "i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i" << std::endl
        << "l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l" << std::endl;

    prepareTestFolder(ss.str(), input_file, log_file);

    // Act

    STSM::run(input_file, log_file, min_spatial * 100, min_block * 100);

    // Assert

    // testing number and last expected results
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequences[4].size(), 1);

    RangedSequence & rg = m_solid_sequences[4].back();

    // testing defined min frequency
    CXXTOOLS_UNIT_ASSERT(rg.frequency() >= (min_spatial - EPSILON));

    // testing synthetic known data
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.sequence().toStringOfItems(), "EFGH");
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().start(), 6);
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().end(), 7);
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.support(), 2);
    CXXTOOLS_UNIT_ASSERT((rg.frequency() - 1.0) < EPSILON);

    // Cleanup

    cleanupTestFolder();
}

void STSMTest::test_STSMRun_f100_b100_does_get_EFGHI_solidBlock()
{
    // Arrange

    Frequency min_spatial = 1.0;
    Frequency min_block = 1.0;
    std::string input_file;
    std::string log_file;

    std::stringstream ss;
    ss  << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
        << "X16,X17,X18,X19,X20,X21,X22,X23,X24,X25" << std::endl
        << "a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a" << std::endl
        << "b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b" << std::endl
        << "c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c" << std::endl
        << "d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d" << std::endl
        << "e,e,e,e,e,e,e,e,e,e,e,e,e,E,F,G,H,I,e,e,e,e,e,e,e" << std::endl
        << "f,f,f,f,f,f,f,f,f,f,f,f,f,E,F,G,H,I,f,f,f,f,f,f,f" << std::endl
        << "g,g,g,g,g,g,g,g,g,g,g,g,g,E,F,G,H,I,g,g,g,g,g,g,g" << std::endl
        << "h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h" << std::endl
        << "i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i" << std::endl
        << "l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l" << std::endl;

    prepareTestFolder(ss.str(), input_file, log_file);

    // Act

    STSM::run(input_file, log_file, min_spatial * 100, min_block * 100);

    // Assert

    // testing number and last expected results
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequences[5].size(), 1);
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequence_blocks[5].size(), 1);

    RangedSequence & rg = m_solid_sequences[5].back();
    SequenceBlock & sb = m_solid_sequence_blocks[5].back();

    // testing synthetic known data
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.sequence().toStringOfItems(), "EFGHI");
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().start(), 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().end(), 6);
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.support(), 3);
    CXXTOOLS_UNIT_ASSERT((rg.frequency() - min_spatial) < EPSILON);

    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.sequence().toStringOfItems(), "EFGHI");
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.range().start(), 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.range().end(), 6);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.interval().start(), 13);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.interval().end(), 17);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.support(), 15);
    CXXTOOLS_UNIT_ASSERT((sb.frequency() - min_block) < EPSILON);

    // Cleanup

    cleanupTestFolder();
}

void STSMTest::test_STSMRun_f100_b100_does_not_get_diagonal_EFGHI()
{
    // Arrange

    Frequency min_spatial = 1.0;
    Frequency min_block = 1.0;
    std::string input_file;
    std::string log_file;

    std::stringstream ss;
    ss  << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
        << "X16,X17,X18,X19,X20,X21,X22,X23,X24,X25" << std::endl
        << "a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a" << std::endl
        << "b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b" << std::endl
        << "c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c" << std::endl
        << "d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d" << std::endl
        << "e,e,e,e,e,e,e,e,e,e,e,e,e,E,F,G,H,I,e,e,e,e,e,e,e" << std::endl
        << "f,f,f,f,f,f,f,f,f,f,f,f,f,f,E,F,G,H,I,f,f,f,f,f,f" << std::endl
        << "g,g,g,g,g,g,g,g,g,g,g,g,g,E,F,G,H,I,g,g,g,g,g,g,g" << std::endl
        << "h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h" << std::endl
        << "i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i" << std::endl
        << "l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l" << std::endl;

    prepareTestFolder(ss.str(), input_file, log_file);

    // Act

    STSM::run(input_file, log_file, min_spatial * 100, min_block * 100);

    // Assert

    // testing number and last expected results
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequence_blocks[5].size(), 3);

    ListSequenceBlocks::const_iterator it;

    for(
        it = m_solid_sequence_blocks[5].begin();
        it != m_solid_sequence_blocks[5].end();
        ++it)
    {
        CXXTOOLS_UNIT_ASSERT_EQUALS(it->sequence().toStringOfItems(), "EFGHI");
        CXXTOOLS_UNIT_ASSERT_EQUALS(it->range().size(), 1);
    }

    // Cleanup

    cleanupTestFolder();
}

void STSMTest::test_STSMRun_f75_b75_does_get_EFGHI7575_solidBlock()
{
    // Arrange

    Frequency min_spatial = 0.75;
    Frequency min_block = 0.75;
    std::string input_file;
    std::string log_file;

    std::stringstream ss;
    ss  << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
        << "X16,X17,X18,X19,X20,X21,X22,X23,X24,X25" << std::endl
        << "a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a" << std::endl
        << "b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b" << std::endl
        << "c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c" << std::endl
        << "d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d" << std::endl
        << "e,e,e,e,e,e,e,e,e,e,e,e,e,E,F,G,H,I,e,e,e,e,e,e,e" << std::endl
        << "f,f,f,f,f,f,f,f,f,f,f,f,f,E,F,G,H,I,f,f,f,f,f,f,f" << std::endl
        << "g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g" << std::endl
        << "h,h,h,h,h,h,h,h,h,h,h,h,h,E,F,G,H,I,h,h,h,h,h,h,h" << std::endl
        << "i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i" << std::endl
        << "l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l" << std::endl;

    prepareTestFolder(ss.str(), input_file, log_file);

    // Act

    STSM::run(input_file, log_file, min_spatial * 100, min_block * 100);

    // Assert

    // testing number and last expected results
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequence_blocks[5].size(), 1);

    SequenceBlock & sb = m_solid_sequence_blocks[5].back();

    // testing synthetic known data
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.sequence().toStringOfItems(), "EFGHI");
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.range().start(), 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.range().end(), 7);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.interval().start(), 13);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.interval().end(), 17);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.support(), 15);
    CXXTOOLS_UNIT_ASSERT((sb.frequency() - min_block) < EPSILON);

    // Cleanup

    cleanupTestFolder();
}

void STSMTest::test_STSMRun_f100_b50_does_get_EFGHI10025_solidBlock()
{
    // Arrange

    Frequency min_spatial = 1.0;
    Frequency min_block = 0.50;
    std::string input_file;
    std::string log_file;

    std::stringstream ss;
    ss  << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
        << "X16,X17,X18,X19,X20,X21,X22,X23,X24,X25" << std::endl
        << "a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a" << std::endl
        << "b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b" << std::endl
        << "c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c" << std::endl
        << "d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d" << std::endl
        << "e,e,e,e,e,e,e,e,e,e,e,e,e,E,F,G,H,I,e,e,e,e,e,e,e" << std::endl
        << "f,f,f,f,f,f,f,f,f,f,f,f,f,E,F,G,H,I,f,f,f,f,f,f,f" << std::endl
        << "g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,E,F,G,H,I,g,g" << std::endl
        << "h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,E,F,G,H,I,h,h" << std::endl
        << "i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i" << std::endl
        << "l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l" << std::endl;

    prepareTestFolder(ss.str(), input_file, log_file);

    // Act

    STSM::run(input_file, log_file, min_spatial * 100, min_block * 100);

    // Assert

    // testing number and last expected results
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequence_blocks[5].size(), 1);

    SequenceBlock & sb = m_solid_sequence_blocks[5].back();

    // testing synthetic known data
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.sequence().toStringOfItems(), "EFGHI");
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.range().start(), 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.range().end(), 7);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.interval().start(), 13);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.interval().end(), 22);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.support(), 20);
    CXXTOOLS_UNIT_ASSERT((sb.frequency() - min_block) < EPSILON);

    // Cleanup

    cleanupTestFolder();
}

void STSMTest::test_STSMRun_f100_b75_same_line_does_get_EFGHI10034_solidBlock()
{
    // Arrange

    Frequency min_spatial = 1.0;
    Frequency min_block = 0.75;
    std::string input_file;
    std::string log_file;

    std::stringstream ss;
    ss  << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
        << "X16,X17,X18,X19,X20,X21,X22,X23,X24,X25" << std::endl
        << "a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,a" << std::endl
        << "b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,b" << std::endl
        << "c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c,c" << std::endl
        << "d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d,d" << std::endl
        << "e,e,e,e,e,e,e,e,e,e,e,e,e,E,F,G,H,I,e,e,e,e,e,e,e" << std::endl
        << "f,f,f,f,f,f,f,f,f,f,f,f,f,E,F,G,H,I,E,F,G,H,I,f,f" << std::endl
        << "g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g,g" << std::endl
        << "h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h" << std::endl
        << "i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i" << std::endl
        << "l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l" << std::endl;

    prepareTestFolder(ss.str(), input_file, log_file);

    // Act

    STSM::run(input_file, log_file, min_spatial * 100, min_block * 100);

    // Assert

    // testing number and last expected results
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequence_blocks[5].size(), 1);

    SequenceBlock & sb = m_solid_sequence_blocks[5].back();

    // testing synthetic known data
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.sequence().toStringOfItems(), "EFGHI");
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.range().start(), 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.range().end(), 5);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.interval().start(), 13);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.interval().end(), 22);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.support(), 15);
    CXXTOOLS_UNIT_ASSERT((sb.frequency() - min_block) < EPSILON);

    // Cleanup

    cleanupTestFolder();
}

void STSMTest::test_STSMRun_f100_b50_does_get_ABCD_big_solidBlock()
{
    // Arrange

    Frequency min_spatial = 1.0;
    Frequency min_block = 0.5;
    std::string input_file;
    std::string log_file;

    std::stringstream ss;
    ss  << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
        << "X16,X17,X18,X19,X20,X21,X22,X23,X24,X25" << std::endl
        << "a,a,a,a,a,a,a,a,a,a,a,a,a,a,a,A,B,C,D,a,a,a,a,a,a" << std::endl
        << "b,b,b,b,b,b,b,b,b,b,b,b,b,b,b,A,B,C,D,b,b,b,b,b,b" << std::endl
        << "c,c,c,c,c,c,c,c,c,c,c,c,A,B,C,D,c,c,c,c,c,c,c,c,c" << std::endl
        << "d,d,d,d,d,d,d,d,d,d,A,B,C,D,d,A,B,C,D,d,d,d,d,d,d" << std::endl
        << "e,e,e,e,e,e,e,e,e,e,A,B,C,D,e,A,B,C,D,e,e,e,e,e,e" << std::endl
        << "f,f,f,f,f,f,f,f,f,f,A,B,C,D,f,f,f,f,f,f,f,f,f,f,f" << std::endl
        << "g,g,g,g,g,g,g,g,g,A,B,C,D,g,g,g,g,g,g,g,g,g,g,g,g" << std::endl
        << "h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h,h" << std::endl
        << "i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i,i" << std::endl
        << "l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l,l" << std::endl;

    prepareTestFolder(ss.str(), input_file, log_file);

    // Act

    STSM::run(input_file, log_file, min_spatial * 100, min_block * 100);

    // Assert

    // testing number and last expected results
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequence_blocks[4].size(), 1);

    SequenceBlock & sb = m_solid_sequence_blocks[4].back();

    // testing synthetic known data
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.sequence().toStringOfItems(), "ABCD");
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.range().start(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.range().end(), 6);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.interval().start(), 9);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.interval().end(), 18);
    CXXTOOLS_UNIT_ASSERT_EQUALS(sb.support(), 36);
    CXXTOOLS_UNIT_ASSERT((sb.frequency() - (36.0/70.0)) < EPSILON);

    // Cleanup

    cleanupTestFolder();
}

cxxtools::unit::RegisterTest<STSMTest> register_STSMTest;

#include "cxxtools/unit/testmain.h"
