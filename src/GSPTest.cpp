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
    registerMethod(
        "test_getAllContiguosSubsequencesDroppingAnItemFromFirstItemset",
        *this,
        &GSPTest::test_getAllContiguosSubsequencesDroppingAnItemFromFirstItemset);
    registerMethod(
        "test_getAllContiguosSubsequencesDroppingAnItemFromLastItemset",
        *this,
        &GSPTest::test_getAllContiguosSubsequencesDroppingAnItemFromLastItemset);
    registerMethod(
        "test_getAllContiguosSubsequencesDroppingAnItemFromAnyMiddleItemset",
        *this,
        &GSPTest::test_getAllContiguosSubsequencesDroppingAnItemFromAnyMiddleItemset);
    registerMethod(
        "test_getAllFirstLevelContigousSubsequences",
        *this,
        &GSPTest::test_getAllFirstLevelContigousSubsequences);
    registerMethod(
        "test_getSubsequenceDroppingFirstItemFromSingleItemset",
        *this,
        &GSPTest::test_getSubsequenceDroppingFirstItemFromSingleItemset);
    registerMethod(
        "test_getSubsequenceDroppingFirstItemFromMultipleItemset",
        *this,
        &GSPTest::test_getSubsequenceDroppingFirstItemFromMultipleItemset);
    registerMethod(
        "test_getSubsequenceDroppingLastItemFromSingleItemset",
        *this,
        &GSPTest::test_getSubsequenceDroppingLastItemFromSingleItemset);
    registerMethod(
        "test_getSubsequenceDroppingLastItemFromMultipleItemset",
        *this,
        &GSPTest::test_getSubsequenceDroppingLastItemFromMultipleItemset);

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
    Sequence seq2("<(abc)(de)(f)(ghi)(lm)>");

    // Assert
    std::cout << std::endl;
    std::cout << seq1.toString();
    std::cout << seq2.toString();
    std::cout << std::endl;
    CXXTOOLS_UNIT_ASSERT(seq1 == seq2);

    std::cout << std::endl
        << "__________________________________________________";
}

void GSPTest::test_getAllContiguosSubsequencesDroppingAnItemFromFirstItemset()
{
    // Arrange
    Sequence seq1;
    prepare(seq1);

    std::list<Sequence> sub_seq_prepared;
    prepareContSubseqFirsts(sub_seq_prepared);
    std::list<Sequence> sub_seq_generated;

    // Act
    seq1.getAllContiguosSubsequencesDroppingAnItemFromFirstItemset(
        sub_seq_generated);

    // Assert
    std::cout << std::endl;
    std::cout << seq1.toString();
    CXXTOOLS_UNIT_ASSERT_EQUALS(
        sub_seq_prepared.size(), sub_seq_generated.size());

    std::list<Sequence>::iterator it1 = sub_seq_prepared.begin();
    std::list<Sequence>::iterator it2 = sub_seq_generated.begin();
    unsigned int i = 0;

    while(it1 != sub_seq_prepared.end())
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

void GSPTest::test_getAllContiguosSubsequencesDroppingAnItemFromLastItemset()
{
    // Arrange
    Sequence seq1;
    prepare(seq1);

    std::list<Sequence> sub_seq_prepared;
    prepareContSubseqLasts(sub_seq_prepared);
    std::list<Sequence> sub_seq_generated;

    // Act
    seq1.getAllContiguosSubsequencesDroppingAnItemFromLastItemset(
        sub_seq_generated);

    // Assert
    std::cout << std::endl;
    std::cout << seq1.toString();
    CXXTOOLS_UNIT_ASSERT_EQUALS(
        sub_seq_prepared.size(), sub_seq_generated.size());

    std::list<Sequence>::iterator it1 = sub_seq_prepared.begin();
    std::list<Sequence>::iterator it2 = sub_seq_generated.begin();
    unsigned int i = 0;

    while(it1 != sub_seq_prepared.end())
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

void GSPTest::test_getAllContiguosSubsequencesDroppingAnItemFromAnyMiddleItemset()
{
    // Arrange
    Sequence seq1;
    prepare(seq1);

    std::list<Sequence> sub_seq_prepared;
    prepareContSubseqMiddles(sub_seq_prepared);
    std::list<Sequence> sub_seq_generated;

    // Act
    seq1.getAllContiguosSubsequencesDroppingAnItemFromAnyMiddleItemset(
        sub_seq_generated);

    // Assert
    std::cout << std::endl;
    std::cout << seq1.toString();
    CXXTOOLS_UNIT_ASSERT_EQUALS(
        sub_seq_prepared.size(), sub_seq_generated.size());

    std::list<Sequence>::iterator it1 = sub_seq_prepared.begin();
    std::list<Sequence>::iterator it2 = sub_seq_generated.begin();
    unsigned int i = 0;

    while(it1 != sub_seq_prepared.end())
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

void GSPTest::test_getAllFirstLevelContigousSubsequences()
{
    // Arrange
    Sequence seq;
    prepare(seq);

    std::list<Sequence> cont_subseq;
    prepareContSubseqMix(cont_subseq);

    // Act
    std::list<Sequence> gsp_cont_subseq;
    seq.getAllFirstLevelContigousSubsequences(gsp_cont_subseq);

    // Assert
    std::cout << std::endl;
    CXXTOOLS_UNIT_ASSERT_EQUALS(cont_subseq.size(), gsp_cont_subseq.size());

    std::list<Sequence>::iterator it1 = cont_subseq.begin();
    std::list<Sequence>::iterator it2 = gsp_cont_subseq.begin();

    while(it1 != cont_subseq.end())
    {
        CXXTOOLS_UNIT_ASSERT((*it1) == (*it2));

        ++it1;
        ++it2;
    }

    std::cout << std::endl
        << "__________________________________________________";
}

void GSPTest::test_getSubsequenceDroppingFirstItemFromSingleItemset()
{
    // Arrange
    Sequence seq1("<(a)(bc)(de)(f)(ghi)(lm)>");
    Sequence sub_seq_prepared("<(bc)(de)(f)(ghi)(lm)>");
    Sequence sub_seq_generated;

    // Act
    seq1.getSubsequenceDroppingFirstItem(sub_seq_generated);

    // Assert
    std::cout << std::endl;
    std::cout << seq1.toString();
    std::cout << sub_seq_prepared.toString();
    std::cout << sub_seq_generated.toString();
    std::cout << std::endl;
    CXXTOOLS_UNIT_ASSERT_EQUALS(
        sub_seq_prepared.size(), sub_seq_generated.size());
    CXXTOOLS_UNIT_ASSERT(sub_seq_prepared == sub_seq_generated);

    std::cout << std::endl
        << "__________________________________________________";
}

void GSPTest::test_getSubsequenceDroppingFirstItemFromMultipleItemset()
{
    // Arrange
    Sequence seq1("<(abc)(de)(f)(ghi)(lm)>");
    Sequence sub_seq_prepared("<(bc)(de)(f)(ghi)(lm)>");
    Sequence sub_seq_generated;

    // Act
    seq1.getSubsequenceDroppingFirstItem(sub_seq_generated);

    // Assert
    std::cout << std::endl;
    std::cout << seq1.toString();
    std::cout << sub_seq_prepared.toString();
    std::cout << sub_seq_generated.toString();
    std::cout << std::endl;
    CXXTOOLS_UNIT_ASSERT_EQUALS(
        sub_seq_prepared.size(), sub_seq_generated.size());
    CXXTOOLS_UNIT_ASSERT(sub_seq_prepared == sub_seq_generated);

    std::cout << std::endl
        << "__________________________________________________";
}

void GSPTest::test_getSubsequenceDroppingLastItemFromSingleItemset()
{
    // Arrange
    Sequence seq1("<(abc)(de)(f)(ghi)(l)(m)>");
    Sequence sub_seq_prepared("<(abc)(de)(f)(ghi)(l)>");
    Sequence sub_seq_generated;

    // Act
    seq1.getSubsequenceDroppingLastItem(sub_seq_generated);

    // Assert
    std::cout << std::endl;
    std::cout << seq1.toString();
    std::cout << sub_seq_prepared.toString();
    std::cout << sub_seq_generated.toString();
    std::cout << std::endl;
    CXXTOOLS_UNIT_ASSERT_EQUALS(
        sub_seq_prepared.size(), sub_seq_generated.size());
    CXXTOOLS_UNIT_ASSERT(sub_seq_prepared == sub_seq_generated);

    std::cout << std::endl
        << "__________________________________________________";
}

void GSPTest::test_getSubsequenceDroppingLastItemFromMultipleItemset()
{
    // Arrange
    Sequence seq1("<(abc)(de)(f)(ghi)(lm)>");
    Sequence sub_seq_prepared("<(abc)(de)(f)(ghi)(l)>");
    Sequence sub_seq_generated;

    // Act
    seq1.getSubsequenceDroppingLastItem(sub_seq_generated);

    // Assert
    std::cout << std::endl;
    std::cout << seq1.toString();
    std::cout << sub_seq_prepared.toString();
    std::cout << sub_seq_generated.toString();
    std::cout << std::endl;
    CXXTOOLS_UNIT_ASSERT_EQUALS(
        sub_seq_prepared.size(), sub_seq_generated.size());
    CXXTOOLS_UNIT_ASSERT(sub_seq_prepared == sub_seq_generated);

    std::cout << std::endl
        << "__________________________________________________";
}

void GSPTest::test_GSP_join()
{
    // Arrange
    std::list<Sequence> candidates;
    std::list<Sequence> prepared_candidates;
    std::list<Sequence> generated_candidates;


    Sequence seq1("<(ab)(c)>");     // <(1, 2) (3)>
    Sequence seq2("<(ab)(d)>");     // <(1, 2) (4)>
    Sequence seq3("<(a)(cd)>");     // <(1) (3, 4)>
    Sequence seq4("<(ac)(e)>");     // <(1, 3) (5)>
    Sequence seq5("<(b)(cd)>");     // <(2) (3, 4)>
    Sequence seq6("<(b)(c)(e)>");   // <(2) (3) (5)>
    candidates.push_back(seq1);
    candidates.push_back(seq2);
    candidates.push_back(seq3);
    candidates.push_back(seq4);
    candidates.push_back(seq5);
    candidates.push_back(seq6);
    Sequence sub_seq_prepared1("<(ab)(cd)>");       // <(1, 2) (3, 4)>
    Sequence sub_seq_prepared2("<(ab)(c)(e)>");     // <(1, 2) (3) (5)>
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
        "data/100_sax-26_sample-46x951.csv", "out/test_GSP_run.log", 300, 0);

    std::cout << std::endl
        << "__________________________________________________";
}

void GSPTest::prepare(Sequence &_seq)
{
    _seq.set("<(abc)(de)(f)(ghi)(lm)>");
}

void GSPTest::prepareContSubseqFirsts(std::list<Sequence> &_cont_subseq)
{
    {
        Sequence seq("<(ab)(de)(f)(ghi)(lm)>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<(ac)(de)(f)(ghi)(lm)>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<(bc)(de)(f)(ghi)(lm)>");
        _cont_subseq.push_back(seq);
    }
}

void GSPTest::prepareContSubseqLasts(std::list<Sequence> &_cont_subseq)
{
    {
        Sequence seq("<(abc)(de)(f)(ghi)(l)>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<(abc)(de)(f)(ghi)(m)>");
        _cont_subseq.push_back(seq);
    }
}

void GSPTest::prepareContSubseqMiddles(std::list<Sequence> &_cont_subseq)
{
    {
        Sequence seq("<(abc)(d)(f)(ghi)(lm)>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<(abc)(e)(f)(ghi)(lm)>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<(abc)(de)(f)(gh)(lm)>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<(abc)(de)(f)(gi)(lm)>");
        _cont_subseq.push_back(seq);
    }
    {
        Sequence seq("<(abc)(de)(f)(hi)(lm)>");
        _cont_subseq.push_back(seq);
    }
}

void GSPTest::prepareContSubseqMix(std::list<Sequence> &_cont_subseq)
{
    // <(abc)(de)(f)(ghi)(lm)>
    // became
    // 1. C is derived from S by dropping an item from either
    //    S1 or Sn .
    // <(ab)(de)(f)(ghi)(lm)>
    // <(ac)(de)(f)(ghi)(lm)>
    // <(bc)(de)(f)(ghi)(lm)>
    prepareContSubseqFirsts(_cont_subseq);

    // <(abc)(de)(f)(ghi)(l)>
    // <(abc)(de)(f)(ghi)(m)>
    prepareContSubseqLasts(_cont_subseq);

    //  2. C is derived from S by dropping an item from an element
    //     Si which has at least 2 items.
    // <(abc)(d)(f)(lm)>
    // <(abc)(e)(f)(lm)>
    // <(abc)(de)(f)(gh)(m)>
    // <(abc)(de)(f)(gi)(m)>
    // <(abc)(de)(f)(hi)(m)>
    prepareContSubseqMiddles(_cont_subseq);

    // 3. out of scope for this test
}

cxxtools::unit::RegisterTest<GSPTest> register_GSPTest;

#include "cxxtools/unit/testmain.h"
