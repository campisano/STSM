#include "SIMTest.h"

#include <cxxtools/unit/registertest.h>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>


// local utility functions
namespace {
    void prepare(Sequence &_seq)
    {
        _seq.set("<abcdefghilm>");
    }
}



SIMTest::SIMTest() : cxxtools::unit::TestSuite("SIMTest")
{
    std::cout << std::endl << "Test methods:" << std::endl;

    registerMethod(
        "test_sequenceStringRepresentation",
        *this,
        &SIMTest::test_sequenceStringRepresentation);

    registerMethod(
        "test_equality",
        *this,
        &SIMTest::test_equality);
}

SIMTest::~SIMTest()
{
}

void SIMTest::test_equality()
{
    // Arrange
    Sequence seq1, seq2;

    // Act
    prepare(seq1);
    prepare(seq2);

    // Assert
    std::cout << std::endl;
    std::cout << seq1.toString();
    std::cout << seq2.toString();
    std::cout << std::endl;
    CXXTOOLS_UNIT_ASSERT(seq1 == seq2);

    std::cout << std::endl
              << "__________________________________________________";
}

void SIMTest::test_sequenceStringRepresentation()
{
    // Arrange
    Sequence seq1;

    // Act
    prepare(seq1);
    Sequence seq2("<abcdefghilm>");

    // Assert
    std::cout << std::endl;
    std::cout << seq1.toString();
    std::cout << seq2.toString();
    std::cout << std::endl;
    CXXTOOLS_UNIT_ASSERT(seq1 == seq2);

    std::cout << std::endl
              << "__________________________________________________";
}



cxxtools::unit::RegisterTest<SIMTest> register_SIMTest;

#include "cxxtools/unit/testmain.h"
