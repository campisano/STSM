// Require apt-get install libcxxtools-dev >= 2.2.1
//
// compile with -l cxxtools -l cxxtools-unit
// example:
// $ reset; rm -f test; g++ test.cpp -o test -l cxxtools -l cxxtools-unit && ./test
//

#include <cxxtools/csvdeserializer.h>
#include <iostream>
#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>



typedef char Item;



class Itemset
{
    public:
        explicit Itemset()
        {
        }
        /*
        explicit Itemset(const Itemset &_copy_itemset):
            m_items(_copy_itemset.m_items)
        {
        }
        */
        virtual ~Itemset()
        {
        }

        bool operator==(const Itemset &other)
        {
            if(this->m_items.size() != other.m_items.size())
            {
                return false;
            }

            unsigned int size = this->m_items.size();

            for(int i=0; i < size; ++i)
            {
                if(this->m_items[i] != other.m_items[i])
                {
                    return false;
                }
            }

            return true;
        }

        inline unsigned int size()
        {
            return this->m_items.size();
        }

        inline void append(Item _item)
        {
            this->m_items.push_back(_item);
        }

        //TODO [CMP] temporary
        inline Item getFirst()
        {
            if(this->m_items.size() < 1)
            {
                throw std::out_of_range("Itemset size must be at least 1.");
            }

            return this->m_items[0];
        }

        void print()
        {
            std::vector<Item>::iterator it;

            std::cout << '(';

            for(it = this->m_items.begin(); it != this->m_items.end(); ++it)
            {
                std::cout << (*it);
            }

            std::cout << ')';
        }

    private:
        std::vector<Item> m_items;
};



class Sequence
{
    public:
        explicit Sequence()
        {
        }
        /*
        explicit Sequence(const Sequence &_copy_sequence):
            m_itemsets(_copy_sequence.m_itemsets)
        {
        }
        */
        virtual ~Sequence()
        {
        }

        bool generateAllFirstLevelContigousSubsequences(std::vector<Sequence> &_cont_subseq)
        {
            // pag. 8
            // C is a contiguous subsequence of S if any of the following conditions hold:
            //     1. C is derived from S by dropping an item from either S1 or Sn.
            {
                Sequence seq_dropping_first, seq_dropping_last;
                this->getCopyDroppingFirstItem(seq_dropping_first);
                _cont_subseq.push_back(seq_dropping_first);
                this->getCopyDroppingLastItem(seq_dropping_last);
                _cont_subseq.push_back(seq_dropping_last);
            }
            //     2. C is derived from S by dropping an item from an element Si which has at least 2 items.
            //     3. C is a contiguous subsequence of C', and C' is a contiguous subsequence of S.
        }

        //bool isContiguousSubsequenceOf(Sequence &_super_sequence)
        //{
        //    //TODO [CMP] to implement
        //    throw std::runtime_error("Not implemented: isContiguousSubsequenceOf");
        //
        //    return false;
        //}

        inline void getCopyDroppingFirstItem(Sequence &_copy_sequence)
        {
            if(this->m_itemsets.size() < 1)
            {
                throw std::out_of_range("Sequence size must be at least 1.");
            }

            _copy_sequence.m_itemsets.assign(++this->m_itemsets.begin(), this->m_itemsets.end());
        }

        inline void getCopyDroppingLastItem(Sequence &_copy_sequence)
        {
            if(this->m_itemsets.size() < 1)
            {
                throw std::out_of_range("Sequence size must be at least 1.");
            }

            _copy_sequence.m_itemsets.assign(this->m_itemsets.begin(), --this->m_itemsets.end());
        }

        bool operator==(const Sequence &other)
        {
            if(this->m_itemsets.size() != other.m_itemsets.size())
            {
                return false;
            }

            unsigned int size = this->m_itemsets.size();

            for(int i=0; i < size; ++i)
            {
                if(! (this->m_itemsets[i] == other.m_itemsets[i]))
                {
                    return false;
                }
            }

            return true;
        }

        inline unsigned int size()
        {
            return this->m_itemsets.size();
        }

        inline void append(Itemset& _itemset)
        {
            this->m_itemsets.push_back(_itemset);
        }

        //TODO [CMP] temporary
        inline Itemset getFirst()
        {
            if(this->m_itemsets.size() < 1)
            {
                throw std::out_of_range("Sequence size must be at least 1.");
            }

            return this->m_itemsets[0];
        }

        void print()
        {
            std::vector<Itemset>::iterator it;

            std::cout << '<';

            for(it = this->m_itemsets.begin(); it != this->m_itemsets.end(); ++it)
            {
                (*it).print();
            }

            std::cout << '>';
        }

    private:
        std::vector<Itemset> m_itemsets;
};



class GSP
{
    public:
        explicit GSP(std::string _input_filename)
        {
            this->m_max_gap = 0;

            load(_input_filename);
            std::vector<Item> frequent_items;

            std::cout << "Detecting frequent items:" << std::endl;
            detectFrequentItems(13, 100, frequent_items);

            std::vector<Sequence> candidates;

            // temporary code, probably detectFrequentItems can return
            // directly a vector of Sequences
            {
                std::vector<Item>::iterator it;

                for(it = frequent_items.begin(); it != frequent_items.end(); ++it)
                {
                    Itemset is;
                    is.append(*it);
                    Sequence seq;
                    seq.append(is);
                    candidates.push_back(seq);
                }
            }

            std::cout << "First candidates:" << std::endl;
            print(candidates);

            std::vector<Sequence> new_candidates;
            unsigned int seq_items = 0;

            do
            {
                ++seq_items;
                new_candidates.clear();

                join(candidates, seq_items, new_candidates);
                std::cout << "Joined candidates at pass " << seq_items << std::endl;
                print(new_candidates);

                prune(new_candidates);
                std::cout << "Pruned candidates at pass " << seq_items << std::endl;
                print(new_candidates);
            }
            while(new_candidates.size() > 0);

            //TODO [CMP]:

            // merge of all candidates in the same list ?

            // support count and filter or prune already do this job? Filter dependecies between k-candidates and k+1-candidades is no needed?

            // what's next?
        }

        virtual ~GSP()
        {
        }

    protected:
        void load(std::string &_input_filename)
        {
            std::fstream file_stream;
            file_stream.open(_input_filename.c_str(), std::fstream::in);

            cxxtools::CsvDeserializer deserializer(file_stream);
            deserializer.delimiter(',');
            //deserializer.readTitle(false);
            deserializer.deserialize(this->m_input_dataset);

            file_stream.close();
        }

        void detectFrequentItems(unsigned int _column_num, unsigned int _minimum_support, std::vector<Item>& _frequent_items)
        {
            std::map<Item,int> map_count;

            // counting
            {
                Item item;
                unsigned int size = this->m_input_dataset.size();

                for(int i = 0; i< size; ++i)
                {
                    item = this->m_input_dataset[i][_column_num];

                    if (map_count.find(item) == map_count.end())
                    {
                        map_count[item] = 1;
                    }
                    else
                    {
                        map_count[item] = map_count[item] + 1;
                    }
                }
            }

            // filter
            {
                std::map<Item,int>::iterator it = map_count.begin();

                while(it != map_count.end())
                {
                    if(it->second < _minimum_support)
                    {
                        map_count.erase(it++);
                    }
                    else
                    {
                        std::cout << it->first << '\t' << it->second << std::endl;
                        ++it;
                    }
                }
            }

            // return
            {
                std::map<Item,int>::iterator it;

                for(it = map_count.begin(); it != map_count.end(); ++it)
                {
                    _frequent_items.push_back(it->first);
                }
            }

        }

        void join(std::vector<Sequence> &_candidates, unsigned int _seq_items, std::vector<Sequence> &_new_candidates)
        {
            // pag. 9
            // A sequence S1 joins with S2 if the subsequence obtained by dropping the first
            // item of S1 is the same as the subsequence obtained by dropping the last item
            // of S2. The candidate sequence generated by joining S1 with S2 is the sequence
            // S1 extended with the last item in S2. The added item becomes a separate
            // element if it was a separate element in S2, and part of the last element of
            // S1 otherwise. When joining L1 with L1, we need to add the item in S2 both
            // as part of an itemset and as a separate element

            if(_candidates.size() < 2)
            {
                return;
            }

            if(_seq_items == 1)
            {
                std::vector<Sequence>::iterator it1;
                std::vector<Sequence>::iterator it2;

                for(it1 = _candidates.begin(); it1 != (_candidates.end() - 1); ++it1)
                {
                    for(it2 = (it1 + 1); it2 != _candidates.end(); ++it2)
                    {
                        Sequence seq_both;
                        Itemset is_both;
                        is_both.append((*it1).getFirst().getFirst());
                        is_both.append((*it2).getFirst().getFirst());
                        seq_both.append(is_both);
                        _new_candidates.push_back(seq_both);

                        Sequence seq_separated;
                        Itemset is_separated1;
                        is_separated1.append((*it1).getFirst().getFirst());
                        Itemset is_separated2;
                        is_separated2.append((*it2).getFirst().getFirst());
                        seq_separated.append(is_separated1);
                        seq_separated.append(is_separated2);
                        _new_candidates.push_back(seq_separated);
                    }
                }
            }
            else
            {
                //TODO [CMP] to implement
                throw std::runtime_error("Not implemented: join for k-sequence for k > 1.");
            }
        }

        void prune(std::vector<Sequence> &_new_candidates)
        {
            // pag. 9
            // We delete candidate sequences that have a contiguous
            // (k-1)-subsequence whose support count is less than the
            // minimum support. If there is no max-gap constraint, we
            // also delete candidate sequences that have any subsequence
            // without minimum support.

            if(this->m_max_gap == 0)
            {
                std::vector<Sequence> contiguous_subseq;

                std::vector<Sequence>::iterator it;

                for(it = _new_candidates.begin(); it != _new_candidates.end(); ++it)
                {
                    contiguous_subseq.clear();
                    it->generateAllFirstLevelContigousSubsequences(contiguous_subseq);
                }
            }
            else
            {
                throw std::runtime_error("Not implemented: prune for max_gap != 0.");
            }
        }

        void print(std::vector<Sequence> &_sequences)
        {
            std::vector<Sequence>::iterator it;

            for(it = _sequences.begin(); it != _sequences.end(); ++it)
            {
                (*it).print();
                std::cout << std::endl;
            }
        }

    private:
        unsigned int m_max_gap;
        std::vector< std::vector<Item> > m_input_dataset;
};



#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class GSPTest : public cxxtools::unit::TestSuite
{
    public:
        GSPTest() : cxxtools::unit::TestSuite("GSPTest")
        {
            std::cout << std::endl << "Test methods:" << std::endl;
            registerMethod("test_equality", *this, &GSPTest::test_equality);
            registerMethod("test_getCopyDroppingFirstItem", *this, &GSPTest::test_getCopyDroppingFirstItem);
            registerMethod("test_getCopyDroppingLastItem", *this, &GSPTest::test_getCopyDroppingLastItem);
            registerMethod("test_generateAllFirstLevelContigousSubsequences", *this, &GSPTest::test_generateAllFirstLevelContigousSubsequences);

            registerMethod("test_GSP", *this, &GSPTest::test_GSP);
        }

    private:
        void test_equality()
        {
            // Arrange
            Sequence seq1, seq2;

            // Act
            prepare(seq1);
            prepare(seq2);

            // Assert
            CXXTOOLS_UNIT_ASSERT(seq1 == seq2);
        }

        void test_getCopyDroppingFirstItem()
        {
            // Arrange
            Sequence seq1, seq2, seq1DF, seq2DF;
            prepare(seq1);
            prepare(seq2);

            // Act
            seq1.getCopyDroppingFirstItem(seq1DF);
            seq2.getCopyDroppingFirstItem(seq2DF);

            // Assert
            CXXTOOLS_UNIT_ASSERT(! (seq1 == seq1DF));
            if(seq1 == seq1DF)
            {
                std::cout << "seq1 == seq1DF ";
            }
            else
            {
                std::cout << "seq1 != seq1DF ";
            }

            CXXTOOLS_UNIT_ASSERT(seq1DF == seq2DF);
            if(seq1DF == seq2DF)
            {
                std::cout << "seq1DF == seq2DF ";
            }
            else
            {
                std::cout << "seq1DF != seq2DF ";
            }
        }

        void test_getCopyDroppingLastItem()
        {
            // Arrange
            Sequence seq1, seq2, seq1DL, seq2DL;
            prepare(seq1);
            prepare(seq2);

            // Act
            seq1.getCopyDroppingLastItem(seq1DL);
            seq2.getCopyDroppingLastItem(seq2DL);

            // Assert
            CXXTOOLS_UNIT_ASSERT(! (seq1 == seq1DL));
            if(seq1 == seq1DL)
            {
                std::cout << "seq1 == seq1DL ";
            }
            else
            {
                std::cout << "seq1 != seq1DL ";
            }

            CXXTOOLS_UNIT_ASSERT(seq1DL == seq2DL);
            if(seq1DL == seq2DL)
            {
                std::cout << "seq1DL == seq2DL ";
            }
            else
            {
                std::cout << "seq1DL != seq2DL ";
            }
        }

        void test_generateAllFirstLevelContigousSubsequences()
        {
            // Arrange
            Sequence seq;
            prepare(seq);

            std::vector<Sequence> cont_subseq;
            prepareContSubseqMix(cont_subseq);

            // Act
            std::vector<Sequence> gsp_cont_subseq;
            seq.generateAllFirstLevelContigousSubsequences(gsp_cont_subseq);

            // Assert
            CXXTOOLS_UNIT_ASSERT(cont_subseq.size() == gsp_cont_subseq.size());

            unsigned int size = cont_subseq.size();
            for(unsigned int i = 0; i < size; ++i)
            {
                CXXTOOLS_UNIT_ASSERT(cont_subseq[i] == gsp_cont_subseq[i]);
            }
        }

        void test_GSP()
        {
            GSP gsp("reduced_data_sax.csv");
        }

    private:
        void prepare(Sequence &_seq)
        {
            // <(abc)(de)(e)(fg)>
            Itemset is1, is2, is3, is4;

            is1.append('a');
            is1.append('b');
            is1.append('c');
            _seq.append(is1);

            is2.append('d');
            is2.append('e');
            _seq.append(is2);

            is3.append('f');
            _seq.append(is3);

            is4.append('g');
            is4.append('h');
            _seq.append(is4);
        }

        void prepareContSubseqMix(std::vector<Sequence> _cont_subseq)
        {
            // <(abc)(de)(f)(gh)>
            // became
            // 1. C is derived from S by dropping an item from either S1 or Sn .
            // <(ab)(de)(f)(gh)>
            // <(bc)(de)(f)(gh)>
            // <(ac)(de)(f)(gh)>
            // <(abc)(de)(f)(g)>
            // <(abc)(de)(f)(h)>
            //  2. C is derived from S by dropping an item from an element Si which has at least 2 items.
            // <(abc)(d)(f)(gh)>
            // <(abc)(e)(f)(gh)>
            // 3. out of scope for this test

            {
                Itemset is1, is2, is3, is4;
                is1.append('a');
                is1.append('b');
                is2.append('d');
                is2.append('e');
                is3.append('f');
                is4.append('g');
                is4.append('h');
                Sequence seq;
                seq.append(is1);
                seq.append(is2);
                seq.append(is3);
                seq.append(is4);
                _cont_subseq.push_back(seq);
            }
            {
                Itemset is1, is2, is3, is4;
                is1.append('b');
                is1.append('c');
                is2.append('d');
                is2.append('e');
                is3.append('f');
                is4.append('g');
                is4.append('h');
                Sequence seq;
                seq.append(is1);
                seq.append(is2);
                seq.append(is3);
                seq.append(is4);
                _cont_subseq.push_back(seq);
            }
            {
                Itemset is1, is2, is3, is4;
                is1.append('a');
                is1.append('c');
                is2.append('d');
                is2.append('e');
                is3.append('f');
                is4.append('g');
                is4.append('h');
                Sequence seq;
                seq.append(is1);
                seq.append(is2);
                seq.append(is3);
                seq.append(is4);
                _cont_subseq.push_back(seq);
            }
            {
                Itemset is1, is2, is3, is4;
                is1.append('a');
                is1.append('b');
                is1.append('c');
                is2.append('d');
                is2.append('e');
                is3.append('f');
                is4.append('g');
                Sequence seq;
                seq.append(is1);
                seq.append(is2);
                seq.append(is3);
                seq.append(is4);
                _cont_subseq.push_back(seq);
            }
            {
                Itemset is1, is2, is3, is4;
                is1.append('a');
                is1.append('b');
                is1.append('c');
                is2.append('d');
                is2.append('e');
                is3.append('f');
                is4.append('h');
                Sequence seq;
                seq.append(is1);
                seq.append(is2);
                seq.append(is3);
                seq.append(is4);
                _cont_subseq.push_back(seq);
            }
            {
                Itemset is1, is2, is3, is4;
                is1.append('a');
                is1.append('b');
                is1.append('c');
                is2.append('d');
                is3.append('f');
                is4.append('g');
                is4.append('h');
                Sequence seq;
                seq.append(is1);
                seq.append(is2);
                seq.append(is3);
                seq.append(is4);
                _cont_subseq.push_back(seq);
            }
            {
                Itemset is1, is2, is3, is4;
                is1.append('a');
                is1.append('b');
                is1.append('c');
                is2.append('e');
                is3.append('f');
                is4.append('g');
                is4.append('h');
                Sequence seq;
                seq.append(is1);
                seq.append(is2);
                seq.append(is3);
                seq.append(is4);
                _cont_subseq.push_back(seq);
            }
        }
};

cxxtools::unit::RegisterTest<GSPTest> register_GSPTest;



#include "cxxtools/unit/testmain.h"
