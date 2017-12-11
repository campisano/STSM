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

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

namespace
{
    double FREQ_EPSILON = 0.0001;
    std::string TEST_FOLDER = "test_output";

    void prepareTestFolder(
        const std::string & _data,
        std::string & _input_file,
        std::string & _log_file)
    {
        _input_file = TEST_FOLDER + "/" + "input.csv";
        _log_file = TEST_FOLDER + "/" + "output.log";
        REQUIRE(system(("mkdir -p " + TEST_FOLDER).c_str()) == 0);

        std::ofstream ofs(_input_file.c_str());
        ofs << _data;
        ofs.close();
    }

    void cleanupTestFolder()
    {
        REQUIRE(system(("rm -f " + TEST_FOLDER + "/*").c_str()) == 0);
        REQUIRE(system(("rmdir " + TEST_FOLDER + "/").c_str()) == 0);
    }
}


TEST_CASE("Sequence equality operator", "[Sequence]")
{
    // Arrange
    Sequence seq1, seq2;
    seq1.set("<abcdefghilm>");
    seq2.set("<abcdefghilm>");

    // Assert
    CHECK(seq1 == seq2);
}

TEST_CASE("Sequence string representation", "[Sequence]")
{
    // Arrange
    Sequence seq1;
    std::string str_items = "abcdefghilm";
    std::string str_seq = "<" + str_items + ">";
    seq1.set(str_seq);

    // Assert
    CHECK(seq1.toString() == str_seq);
    CHECK(seq1.toStringOfItems() == str_items);
}

TEST_CASE("mergeKernels f100 size1 adjacent -> does joins", "[Kernel]")
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
    CHECK(cand.kernels().size() == 1);
    CHECK(cand.kernels().back().start() == 0);
    CHECK(cand.kernels().back().end() == 1);
    CHECK(cand.kernels().back().support() == 2);
    CHECK(std::abs(cand.kernels().back().frequency() - 1.0) < FREQ_EPSILON);
}

TEST_CASE("mergeKernels f100 size2 adjacent -> does joins", "[Kernel]")
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
    CHECK(cand.kernels().size() == 1);
    CHECK(cand.kernels().back().start() == 0);
    CHECK(cand.kernels().back().end() == 3);
    CHECK(cand.kernels().back().support() == 4);
    CHECK(std::abs(cand.kernels().back().frequency() - 1.0) < FREQ_EPSILON);
}

TEST_CASE("mergeKernels f100 size1 not adjacent -> does not joins", "[Kernel]")
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
    CHECK(cand.kernels().size() == 2);

    CHECK(cand.kernels().front().start() == 0);
    CHECK(cand.kernels().front().end() == 0);
    CHECK(cand.kernels().front().support() == 1);
    CHECK(std::abs(cand.kernels().front().frequency() - 1.0) < FREQ_EPSILON);

    CHECK(cand.kernels().back().start() == 2);
    CHECK(cand.kernels().back().end() == 2);
    CHECK(cand.kernels().back().support() == 1);
    CHECK(std::abs(cand.kernels().back().frequency() - 1.0) < FREQ_EPSILON);
}

TEST_CASE("mergeKernels f100 size2 not adjacent -> does not joins", "[Kernel]")
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
    CHECK(cand.kernels().size() == 2);

    CHECK(cand.kernels().front().start() == 0);
    CHECK(cand.kernels().front().end() == 1);
    CHECK(cand.kernels().front().support() == 2);
    CHECK(std::abs(cand.kernels().front().frequency() - 1.0) < FREQ_EPSILON);

    CHECK(cand.kernels().back().start() == 3);
    CHECK(cand.kernels().back().end() == 4);
    CHECK(cand.kernels().back().support() == 2);
    CHECK(std::abs(cand.kernels().back().frequency() - 1.0) < FREQ_EPSILON);
}

TEST_CASE(
    "mergeKernels f75 nearby 2 1 2 that should joins -> does joins",
    "[Kernel]")
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
    CHECK(cand.kernels().size() == 1);

    CHECK(cand.kernels().back().start() == 0);
    CHECK(cand.kernels().back().end() == 4);
    CHECK(cand.kernels().back().support() == 4);
    CHECK(
        std::abs(cand.kernels().back().frequency() - (4.0 / 5.0))
        < FREQ_EPSILON);
}

TEST_CASE(
    "mergeKernels f75 nearby 3 1 3 that should joins -> does joins",
    "[Kernel]")
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
    CHECK(cand.kernels().size() == 1);

    CHECK(cand.kernels().back().start() == 0);
    CHECK(cand.kernels().back().end() == 6);
    CHECK(cand.kernels().back().support() == 6);
    CHECK(
        std::abs(cand.kernels().back().frequency() - (6.0 / 7.0))
        < FREQ_EPSILON);
}

TEST_CASE(
    "mergeKernels f50 nearby half 3 1 3 that should joins -> does joins",
    "[Kernel]")
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
    CHECK(cand.kernels().size() == 1);

    CHECK(cand.kernels().back().start() == 0);
    CHECK(cand.kernels().back().end() == 6);
    CHECK(cand.kernels().back().support() == 4);
    CHECK(
        std::abs(cand.kernels().back().frequency() - (4.0 / 7.0))
        < FREQ_EPSILON);
}

TEST_CASE(
    "mergeKernels f75 nearby 1 1 2 that should joins -> does joins",
    "[Kernel]")
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
    CHECK(cand.kernels().size() == 1);

    CHECK(cand.kernels().back().start() == 0);
    CHECK(cand.kernels().back().end() == 3);
    CHECK(cand.kernels().back().support() == 3);
    CHECK(
        std::abs(cand.kernels().back().frequency() - (3.0 / 4.0))
        < FREQ_EPSILON);
}

TEST_CASE("mergeKernels f90 nearby 1 1 2 that should not joins -> does not joins",
    "[Kernel]")
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
    CHECK(cand.kernels().size() == 2);

    CHECK(cand.kernels().front().start() == 0);
    CHECK(cand.kernels().front().end() == 0);
    CHECK(cand.kernels().front().support() == 1);
    CHECK(std::abs(cand.kernels().front().frequency() - 1.0) < FREQ_EPSILON);

    CHECK(cand.kernels().back().start() == 2);
    CHECK(cand.kernels().back().end() == 3);
    CHECK(cand.kernels().back().support() == 2);
    CHECK(std::abs(cand.kernels().back().frequency() - 1.0) < FREQ_EPSILON);
}

TEST_CASE_METHOD(
    STSMTest,
    "Run obtaining single 5_solid_sequence result",
    "[Run]")
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

    CHECK(m_solid_sequences.size() > 0);
    CHECK(m_solid_sequences[2].size() > 0);
    CHECK(m_solid_sequences[3].size() > 0);
    CHECK(m_solid_sequences[4].size() > 0);
    CHECK(m_solid_sequences[5].size() == 1);
    CHECK(m_solid_sequences[6].size() == 0);

    // Cleanup

    cleanupTestFolder();
}

TEST_CASE_METHOD(
    STSMTest,
    "Run f100 gets only single ABCD100 solidSequence",
    "[Run]")
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
    CHECK(m_solid_sequences[4].size() == 1);

    RangedSequence & rg = m_solid_sequences[4].back();

    // testing synthetic known data
    CHECK(rg.sequence().toStringOfItems() == "ABCD");
    CHECK(rg.range().start() == 0);
    CHECK(rg.range().end() == 3);
    CHECK(rg.support() == 4);
    CHECK((rg.frequency() - min_spatial) < FREQ_EPSILON);

    // Cleanup

    cleanupTestFolder();
}

TEST_CASE_METHOD(
    STSMTest,
    "Run f100 testing ABCD100 positions",
    "[Run]")
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
    CHECK(m_solid_sequences[4].size() == 1);

    RangedSequence & rg = m_solid_sequences[4].back();
    ListPositions & positions = m_ranged_sequence_positions[&rg];
    std::vector < Position > v_pos (positions.begin(), positions.end());

    // testing expected positions
    CHECK(positions.size() == 4);
    CHECK(v_pos[0].first == 0);
    CHECK(v_pos[0].second == 2);
    CHECK(v_pos[1].first == 1);
    CHECK(v_pos[1].second == 3);
    CHECK(v_pos[2].first == 2);
    CHECK(v_pos[2].second == 5);
    CHECK(v_pos[3].first == 3);
    CHECK(v_pos[3].second == 1);

    // Cleanup

    cleanupTestFolder();
}

TEST_CASE_METHOD(
    STSMTest,
    "Run f75 does get EFGH75 solidSequence",
    "[Run]")
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
    CHECK(m_solid_sequences[4].size() == 1);

    RangedSequence & rg = m_solid_sequences[4].back();

    // testing synthetic known data
    CHECK(rg.sequence().toStringOfItems() == "EFGH");
    CHECK(rg.range().start() == 4);
    CHECK(rg.range().end() == 7);
    CHECK(rg.support() == 3);
    CHECK((rg.frequency() - min_spatial) < FREQ_EPSILON);

    // Cleanup

    cleanupTestFolder();
}

TEST_CASE_METHOD(
    STSMTest,
    "Run f90 does not get EFGH75 solidSequence",
    "[Run]")
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
    CHECK(m_solid_sequences[4].size() == 1);

    RangedSequence & rg = m_solid_sequences[4].back();

    // testing defined min frequency
    CHECK(rg.frequency() >= (min_spatial - FREQ_EPSILON));

    // testing synthetic known data
    CHECK(rg.sequence().toStringOfItems() == "EFGH");
    CHECK(rg.range().start() == 6);
    CHECK(rg.range().end() == 7);
    CHECK(rg.support() == 2);
    CHECK((rg.frequency() - 1.0) < FREQ_EPSILON);

    // Cleanup

    cleanupTestFolder();
}

TEST_CASE_METHOD(
    STSMTest,
    "Run f100 b100 does get EFGHI solidBlock",
    "[Run]")
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
    CHECK(m_solid_sequences[5].size() == 1);
    CHECK(m_solid_sequence_blocks[5].size() == 1);

    RangedSequence & rg = m_solid_sequences[5].back();
    SequenceBlock & sb = m_solid_sequence_blocks[5].back();

    // testing synthetic known data
    CHECK(rg.sequence().toStringOfItems() == "EFGHI");
    CHECK(rg.range().start() == 4);
    CHECK(rg.range().end() == 6);
    CHECK(rg.support() == 3);
    CHECK((rg.frequency() - min_spatial) < FREQ_EPSILON);

    CHECK(sb.sequence().toStringOfItems() == "EFGHI");
    CHECK(sb.range().start() == 4);
    CHECK(sb.range().end() == 6);
    CHECK(sb.interval().start() == 13);
    CHECK(sb.interval().end() == 17);
    CHECK(sb.support() == 15);
    CHECK((sb.frequency() - min_block) < FREQ_EPSILON);

    // Cleanup

    cleanupTestFolder();
}

TEST_CASE_METHOD(
    STSMTest,
    "Run f100 b100 does not get diagonal EFGHI",
    "[Run]")
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
    CHECK(m_solid_sequence_blocks[5].size() == 3);

    ListSequenceBlocks::const_iterator it;

    for(
        it = m_solid_sequence_blocks[5].begin();
        it != m_solid_sequence_blocks[5].end();
        ++it)
    {
        CHECK(it->sequence().toStringOfItems() == "EFGHI");
        CHECK(it->range().size() == 1);
    }

    // Cleanup

    cleanupTestFolder();
}

TEST_CASE_METHOD(
    STSMTest,
    "Run f75 b75 does get EFGHI7575 solidBlock",
    "[Run]")
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
    CHECK(m_solid_sequence_blocks[5].size() == 1);

    SequenceBlock & sb = m_solid_sequence_blocks[5].back();

    // testing synthetic known data
    CHECK(sb.sequence().toStringOfItems() == "EFGHI");
    CHECK(sb.range().start() == 4);
    CHECK(sb.range().end() == 7);
    CHECK(sb.interval().start() == 13);
    CHECK(sb.interval().end() == 17);
    CHECK(sb.support() == 15);
    CHECK((sb.frequency() - min_block) < FREQ_EPSILON);

    // Cleanup

    cleanupTestFolder();
}

TEST_CASE_METHOD(
    STSMTest,
    "Run f100 b50 does get EFGHI10025 solidBlock",
    "[Run]")
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
    CHECK(m_solid_sequence_blocks[5].size() == 1);

    SequenceBlock & sb = m_solid_sequence_blocks[5].back();

    // testing synthetic known data
    CHECK(sb.sequence().toStringOfItems() == "EFGHI");
    CHECK(sb.range().start() == 4);
    CHECK(sb.range().end() == 7);
    CHECK(sb.interval().start() == 13);
    CHECK(sb.interval().end() == 22);
    CHECK(sb.support() == 20);
    CHECK((sb.frequency() - min_block) < FREQ_EPSILON);

    // Cleanup

    cleanupTestFolder();
}

TEST_CASE_METHOD(
    STSMTest,
    "Run f100 b75 same line does get EFGHI10034 solidBlock",
    "[Run]")
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
    CHECK(m_solid_sequence_blocks[5].size() == 1);

    SequenceBlock & sb = m_solid_sequence_blocks[5].back();

    // testing synthetic known data
    CHECK(sb.sequence().toStringOfItems() == "EFGHI");
    CHECK(sb.range().start() == 4);
    CHECK(sb.range().end() == 5);
    CHECK(sb.interval().start() == 13);
    CHECK(sb.interval().end() == 22);
    CHECK(sb.support() == 15);
    CHECK((sb.frequency() - min_block) < FREQ_EPSILON);

    // Cleanup

    cleanupTestFolder();
}

TEST_CASE_METHOD(
    STSMTest,
    "Run f100 b50 does get ABCD big solidBlock",
    "[Run]")
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
    CHECK(m_solid_sequence_blocks[4].size() == 1);

    SequenceBlock & sb = m_solid_sequence_blocks[4].back();

    // testing synthetic known data
    CHECK(sb.sequence().toStringOfItems() == "ABCD");
    CHECK(sb.range().start() == 0);
    CHECK(sb.range().end() == 6);
    CHECK(sb.interval().start() == 9);
    CHECK(sb.interval().end() == 18);
    CHECK(sb.support() == 36);
    CHECK((sb.frequency() - (36.0/70.0)) < FREQ_EPSILON);

    // Cleanup

    cleanupTestFolder();
}
