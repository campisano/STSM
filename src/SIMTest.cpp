#include "GSPTest.h"

#include <cxxtools/unit/registertest.h>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

GSPTest::GSPTest() : cxxtools::unit::TestSuite("GSPTest")
{
    std::cout << std::endl << "Test methods:" << std::endl;

    registerMethod(
        "test_sequenceStringRepresentation",
        *this,
        &GSPTest::test_sequenceStringRepresentation);
    registerMethod(
        "test_equality",
        *this,
        &GSPTest::test_equality);

    registerMethod("test_GSP_join", *this, &GSPTest::test_GSP_join);
    registerMethod("test_GSP_run", *this, &GSPTest::test_GSP_run);
}

GSPTest::~GSPTest()
{
}

void GSPTest::test_equality()
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

void GSPTest::test_sequenceStringRepresentation()
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

void GSPTest::test_GSP_join()
{
    // Arrange
    std::list<Sequence> candidates;
    std::list<Sequence> prepared_candidates;
    std::list<Sequence> generated_candidates;


    Sequence seq1("<abc>");     // <1, 2) (3>
    Sequence seq2("<abd>");     // <1, 2) (4>
    Sequence seq3("<acd>");     // <1) (3, 4>
    Sequence seq4("<ace>");     // <1, 3) (5>
    Sequence seq5("<bcd>");     // <2) (3, 4>
    Sequence seq6("<bce>");   // <2) (3) (5>
    candidates.push_back(seq1);
    candidates.push_back(seq2);
    candidates.push_back(seq3);
    candidates.push_back(seq4);
    candidates.push_back(seq5);
    candidates.push_back(seq6);
    Sequence sub_seq_prepared1("<abcd>");       // <1, 2) (3, 4>
    Sequence sub_seq_prepared2("<abce>");     // <1, 2) (3) (5>
    prepared_candidates.push_back(sub_seq_prepared1);
    prepared_candidates.push_back(sub_seq_prepared2);

    // Act
    this->join(candidates, 3, generated_candidates);

    // Assert
    std::cout << std::endl;

    std::list<Sequence>::iterator it_cand;

    for(it_cand  = candidates.begin(); it_cand != candidates.end(); ++it_cand)
    {
        std::cout << it_cand->toString() << std::endl;
    }

    CXXTOOLS_UNIT_ASSERT_EQUALS(
        prepared_candidates.size(), generated_candidates.size());

    std::list<Sequence>::iterator it1 = prepared_candidates.begin();
    std::list<Sequence>::iterator it2 = generated_candidates.begin();
    unsigned int i = 0;

    while(it1 != prepared_candidates.end())
    {
        std::cout << std::endl << '\t' << i << ' ';
        std::cout << it1->toString();
        std::cout << it2->toString();
        CXXTOOLS_UNIT_ASSERT((*it1) == (*it2));

        ++it1;
        ++it2;
        ++i;
    }

    std::cout << std::endl
        << "__________________________________________________";
}

void GSPTest::test_GSP_run()
{
    GSP gsp;
    gsp.run(
        "data/100_sax-26_sample-46x951.csv", "out/test_GSP_run.log",
        90, 100, 0);

    std::cout << std::endl
        << "__________________________________________________";
}

void GSPTest::prepare(Sequence &_seq)
{
    _seq.set("<abcdefghilm>");
}

void GSPTest::prepareContSubseqFirsts(std::list<Sequence> &_cont_subseq)
{
    {
        Sequence seq("<abdefghilm>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<acdefghilm>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<bcdefghilm>");
        _cont_subseq.push_back(seq);
    }
}

void GSPTest::prepareContSubseqLasts(std::list<Sequence> &_cont_subseq)
{
    {
        Sequence seq("<abcdefghil>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<abcdefghim>");
        _cont_subseq.push_back(seq);
    }
}

void GSPTest::prepareContSubseqMiddles(std::list<Sequence> &_cont_subseq)
{
    {
        Sequence seq("<abcdfghilm>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<abcefghilm>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<abcdefghlm>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<abcdefgilm>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<abcdefhilm>");
        _cont_subseq.push_back(seq);
    }
}

void GSPTest::prepareContSubseqMix(std::list<Sequence> &_cont_subseq)
{
    // <abcdefghilm>
    // became
    // 1. C is derived from S by dropping an item from either
    //    S1 or Sn .
    // <abdefghilm>
    // <acdefghilm>
    // <bcdefghilm>
    prepareContSubseqFirsts(_cont_subseq);

    // <abcdefghil>
    // <abcdefghim>
    prepareContSubseqLasts(_cont_subseq);

    //  2. C is derived from S by dropping an item from an element
    //     Si which has at least 2 items.
    // <abcdflm>
    // <abceflm>
    // <abcdefghm>
    // <abcdefgim>
    // <abcdefhim>
    prepareContSubseqMiddles(_cont_subseq);

    // 3. out of scope for this test
}

cxxtools::unit::RegisterTest<GSPTest> register_GSPTest;

#include "cxxtools/unit/testmain.h"
