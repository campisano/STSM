#include "SIMTest.h"

#include <cxxtools/unit/registertest.h>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Candidate.h"
#include "Kernel.h"
#include "Sequence.h"

#define EPSILON 0.0001

SIMTest::SIMTest() : cxxtools::unit::TestSuite("SIMTest")
{
    std::cout << std::endl << "Test methods:" << std::endl;

    registerMethod(
        "test_sequenceEqualityOperator",
        *this,
        &SIMTest::test_sequenceEqualityOperator);

    registerMethod(
        "test_sequenceStringRepresentation",
        *this,
        &SIMTest::test_sequenceStringRepresentation);

    registerMethod(
        "test_mergeKernels_f100_size1_adjacent__does_joins",
        *this,
        &SIMTest::test_mergeKernels_f100_size1_adjacent__does_joins);

    registerMethod(
        "test_mergeKernels_f100_size2_adjacent__does_joins",
        *this,
        &SIMTest::test_mergeKernels_f100_size2_adjacent__does_joins);

    registerMethod(
        "test_mergeKernels_f100_size1_not_adjacent__does_not_joins",
        *this,
        &SIMTest::test_mergeKernels_f100_size1_not_adjacent__does_not_joins);

    registerMethod(
        "test_mergeKernels_f100_size2_not_adjacent__does_not_joins",
        *this,
        &SIMTest::test_mergeKernels_f100_size2_not_adjacent__does_not_joins);

    registerMethod(
        "test_mergeKernels_f75_nearby_2_1_2_that_should_joins__does_joins",
        *this,
        &SIMTest::test_mergeKernels_f75_nearby_2_1_2_that_should_joins__does_joins);

    registerMethod(
        "test_mergeKernels_f75_nearby_3_1_3_that_should_joins__does_joins",
        *this,
        &SIMTest::test_mergeKernels_f75_nearby_3_1_3_that_should_joins__does_joins);

    registerMethod(
        "test_mergeKernels_f50_nearby_half_3_1_3_that_should_joins__does_joins",
        *this,
        &SIMTest::test_mergeKernels_f50_nearby_half_3_1_3_that_should_joins__does_joins);

    registerMethod(
        "test_mergeKernels_f75_nearby_1_1_2_that_should_joins__does_joins",
        *this,
        &SIMTest::test_mergeKernels_f75_nearby_1_1_2_that_should_joins__does_joins);

    registerMethod(
        "test_mergeKernels_f90_nearby_1_1_2_that_should_not_joins__does_not_joins",
        *this,
        &SIMTest::test_mergeKernels_f90_nearby_1_1_2_that_should_not_joins__does_not_joins);

    registerMethod(
        "test_SIMRun_any_result",
        *this,
        &SIMTest::test_SIMRun_any_result);

    registerMethod(
        "test_SIMRun_f100_gets_only_single_ABCD100_solidSequence",
        *this,
        &SIMTest::test_SIMRun_f100_gets_only_single_ABCD100_solidSequence);

    registerMethod(
        "test_SIMRun_f75_does_get_EFGH75_solidSequence",
        *this,
        &SIMTest::test_SIMRun_f75_does_get_EFGH75_solidSequence);

    registerMethod(
        "test_SIMRun_f90_does_not_get_EFGH75_solidSequence",
        *this,
        &SIMTest::test_SIMRun_f90_does_not_get_EFGH75_solidSequence);
}

SIMTest::~SIMTest()
{
}

void SIMTest::test_sequenceEqualityOperator()
{
    // Arrange
    Sequence seq1, seq2;

    // Act
    seq1.set("<abcdefghilm>");
    seq2.set("<abcdefghilm>");

    // Assert
    CXXTOOLS_UNIT_ASSERT(seq1 == seq2);
}

void SIMTest::test_sequenceStringRepresentation()
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

void SIMTest::test_mergeKernels_f100_size1_adjacent__does_joins()
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

void SIMTest::test_mergeKernels_f100_size2_adjacent__does_joins()
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

void SIMTest::test_mergeKernels_f100_size1_not_adjacent__does_not_joins()
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

void SIMTest::test_mergeKernels_f100_size2_not_adjacent__does_not_joins()
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

void SIMTest::test_mergeKernels_f75_nearby_2_1_2_that_should_joins__does_joins()
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

void SIMTest::test_mergeKernels_f75_nearby_3_1_3_that_should_joins__does_joins()
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

void SIMTest::test_mergeKernels_f50_nearby_half_3_1_3_that_should_joins__does_joins()
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

void SIMTest::test_mergeKernels_f75_nearby_1_1_2_that_should_joins__does_joins()
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

void SIMTest::test_mergeKernels_f90_nearby_1_1_2_that_should_not_joins__does_not_joins()
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

void SIMTest::test_SIMRun_any_result()
{
    // Arrange

    std::string test_folder = "test_output";
    std::string input_file = test_folder + "/" + "input.csv";
    std::string log_file = test_folder + "/" + "output.log";
    CXXTOOLS_UNIT_ASSERT_EQUALS(system(("mkdir -p " + test_folder).c_str()), 0);

    std::ofstream ofs(input_file.c_str());
    ofs
        << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
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
    ofs.close();

    // Act

    SIM::run(input_file, log_file, 100, 100);

    // Assert

    CXXTOOLS_UNIT_ASSERT(m_solid_sequences.size() > 0);
    CXXTOOLS_UNIT_ASSERT(m_solid_sequences.back().size() > 0);

    // Cleanup

    CXXTOOLS_UNIT_ASSERT_EQUALS(
        system(("rm -f " + test_folder + "/*").c_str()), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(
        system(("rmdir " + test_folder + "/").c_str()), 0);
}

void SIMTest::test_SIMRun_f100_gets_only_single_ABCD100_solidSequence()
{
    // Arrange

    std::string test_folder = "test_output";
    std::string input_file = test_folder + "/" + "input.csv";
    std::string log_file = test_folder + "/" + "output.log";
    CXXTOOLS_UNIT_ASSERT_EQUALS(system(("mkdir -p " + test_folder).c_str()), 0);

    std::ofstream ofs(input_file.c_str());
    ofs
        << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
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
    ofs.close();

    // Act

    SIM::run(input_file, log_file, 100, 100);

    // Assert

    // testing number and last expected results
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequences.back().size(), 1);

    RangedSequence rg = m_solid_sequences.back().back();

    // testing defined min frequency
    CXXTOOLS_UNIT_ASSERT(rg.frequency() >= (1 - EPSILON));

    // testing synthetic known data
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.sequence().toStringOfItems(), "ABCD");
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().start(), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().end(), 3);
    CXXTOOLS_UNIT_ASSERT(std::abs(rg.frequency() - 1.0) < EPSILON);

    // Cleanup

    CXXTOOLS_UNIT_ASSERT_EQUALS(
        system(("rm -f " + test_folder + "/*").c_str()), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(
        system(("rmdir " + test_folder + "/").c_str()), 0);
}

void SIMTest::test_SIMRun_f75_does_get_EFGH75_solidSequence()
{
    // Arrange

    std::string test_folder = "test_output";
    std::string input_file = test_folder + "/" + "input.csv";
    std::string log_file = test_folder + "/" + "output.log";
    CXXTOOLS_UNIT_ASSERT_EQUALS(system(("mkdir -p " + test_folder).c_str()), 0);

    std::ofstream ofs(input_file.c_str());
    ofs
        << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
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
    ofs.close();

    // Act

    SIM::run(input_file, log_file, 75, 100);

    // Assert

    // testing number and last expected results
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequences.back().size(), 1);

    RangedSequence rg = m_solid_sequences.back().back();

    // testing defined min frequency
    CXXTOOLS_UNIT_ASSERT(rg.frequency() >= (0.75 - EPSILON));

    // testing synthetic known data
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.sequence().toStringOfItems(), "EFGH");
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().start(), 4);
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().end(), 7);
    CXXTOOLS_UNIT_ASSERT(std::abs(rg.frequency() - 0.75) < EPSILON);

    // Cleanup

    CXXTOOLS_UNIT_ASSERT_EQUALS(
        system(("rm -f " + test_folder + "/*").c_str()), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(
        system(("rmdir " + test_folder + "/").c_str()), 0);
}

void SIMTest::test_SIMRun_f90_does_not_get_EFGH75_solidSequence()
{
    // Arrange

    std::string test_folder = "test_output";
    std::string input_file = test_folder + "/" + "input.csv";
    std::string log_file = test_folder + "/" + "output.log";
    CXXTOOLS_UNIT_ASSERT_EQUALS(system(("mkdir -p " + test_folder).c_str()), 0);

    std::ofstream ofs(input_file.c_str());
    ofs
        << "X1,X2,X3,X4,X5,X6,X7,X8,X9,X10,X11,X12,X13,X14,X15,"
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
    ofs.close();

    // Act

    SIM::run(input_file, log_file, 90, 100);

    // Assert

    // testing number and last expected results
    CXXTOOLS_UNIT_ASSERT_EQUALS(m_solid_sequences.back().size(), 1);

    RangedSequence rg = m_solid_sequences.back().back();

    // testing defined min frequency
    CXXTOOLS_UNIT_ASSERT(rg.frequency() >= (0.9 - EPSILON));

    // testing synthetic known data
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.sequence().toStringOfItems(), "EFGH");
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().start(), 6);
    CXXTOOLS_UNIT_ASSERT_EQUALS(rg.range().end(), 7);
    CXXTOOLS_UNIT_ASSERT(std::abs(rg.frequency() - 1.0) < EPSILON);

    // Cleanup

    CXXTOOLS_UNIT_ASSERT_EQUALS(
        system(("rm -f " + test_folder + "/*").c_str()), 0);
    CXXTOOLS_UNIT_ASSERT_EQUALS(
        system(("rmdir " + test_folder + "/").c_str()), 0);
}



cxxtools::unit::RegisterTest<SIMTest> register_SIMTest;

#include "cxxtools/unit/testmain.h"
