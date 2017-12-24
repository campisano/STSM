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

#include <catch.hpp>
#include <cmath>

#include "Candidate.h"
#include "Kernel.h"
#include "Sequence.h"

namespace
{
double FREQ_EPSILON = 0.0001;
}

TEST_CASE(
    "mergeKernels f100 size1 adjacent -> does joins",
    "[Kernel]")
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

TEST_CASE(
    "mergeKernels f100 size2 adjacent -> does joins",
    "[Kernel]")
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

TEST_CASE(
    "mergeKernels f100 size1 not adjacent -> does not joins",
    "[Kernel]")
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

TEST_CASE(
    "mergeKernels f100 size2 not adjacent -> does not joins",
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

TEST_CASE(
    "mergeKernels f90 nearby 1 1 2 that should not joins -> does not joins",
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
