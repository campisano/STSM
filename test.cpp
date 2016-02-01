// Require apt-get install libcxxtools-dev >= 2.2.1
//
// compile with -l cxxtools -l cxxtools-unit
// example:
// $ reset; rm -f test; ulimit -c unlimited; g++ -g test.cpp -o test -l cxxtools -l cxxtools-unit && ./test; ulimit -c 0;
//

#include <cxxtools/csvdeserializer.h>
#include <cxxtools/regex.h>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
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

        explicit Itemset(std::string _string_representation)
        {
            cxxtools::Regex seq_regex("^(\\([a-z]+\\))$");

            if(! seq_regex.match(_string_representation))
            {
                std::stringstream msg;
                msg << "Invalid itemset string representation: '"
                    << _string_representation << "'.";
                std::runtime_error(msg.str());
            }

            cxxtools::Regex itemset_regex("([a-z])");
            cxxtools::RegexSMatch itemsets;
            std::string substr = _string_representation.substr(1);

            while(itemset_regex.match(substr, itemsets))
            {
                Item is = itemsets.get(1).c_str()[0];
                substr = substr.substr(itemsets.offsetEnd(1));
                this->append(is);
            }
        }

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

        Item getFirst()
        {
            if(this->m_items.size() < 1)
            {
                throw std::out_of_range("Itemset size must be at least 1.");
            }

            return this->m_items[0];
        }

        void getCopyExceptPos(unsigned int i, Itemset & _copy_itemset)
        {
            unsigned int size = this->m_items.size();

            if(i > (size - 1))
            {
                std::stringstream message;
                message << "Itemset position cannot be greater then"
                    << " it size - 1\n size: " << size << ", i: " << i << '.';
                throw std::out_of_range(message.str());
            }

            for(unsigned int j = 0; j < size; ++j)
            {
                if(j != i)
                {
                    _copy_itemset.append(this->m_items[j]);
                }
            }
        }

        std::string toString()
        {
            std::stringstream output;

            std::vector<Item>::iterator it;

            output << '(';

            for(it = this->m_items.begin(); it != this->m_items.end(); ++it)
            {
                output << (*it);
            }

            output << ')';

            return output.str();
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

        explicit Sequence(std::string _string_representation)
        {
            cxxtools::Regex seq_regex("^<(\\([a-z]+\\))+>$");

            if(! seq_regex.match(_string_representation))
            {
                std::stringstream msg;
                msg << "Invalid sequence string representation: '"
                    << _string_representation << "'.";
                std::runtime_error(msg.str());
            }

            cxxtools::Regex itemset_regex("(\\([a-z]+\\))");
            cxxtools::RegexSMatch itemsets;
            std::string substr = _string_representation.substr(1);

            while(itemset_regex.match(substr, itemsets))
            {
                Itemset is(itemsets.get(1));
                substr = substr.substr(itemsets.offsetEnd(1));
                this->append(is);
            }
        }

        virtual ~Sequence()
        {
        }

        bool getAllFirstLevelContigousSubsequences(
            std::vector<Sequence> &_cont_subseq)
        {
            // pag. 8
            // C is a contiguous subsequence of S if any of the following
            // conditions hold:
            //     1. C is derived from S by dropping an item from either
            //        S1 or Sn.
            {
                getAllContiguosSubsequencesDroppingAnItemFromFirstItemset(
                    _cont_subseq);
                getAllContiguosSubsequencesDroppingAnItemFromLastItemset(
                    _cont_subseq);
            }
            //     2. C is derived from S by dropping an item from an element
            //        Si which has at least 2 items.
            {
                getAllContiguosSubsequencesDroppingAnItemFromAnyMiddleItemset(
                    _cont_subseq);
            }
            //     3. C is a contiguous subsequence of C', and C' is a
            //        contiguous subsequence of S.
            {
                //TODO [CMP] to implement,
                // out of scope of this function in this moment
            }
        }

        void getAllContiguosSubsequencesDroppingAnItemFromFirstItemset(
            std::vector<Sequence> &_cont_subseq)
        {
            // Sequence size must be at least 1
            if(this->m_itemsets.size() < 1)
            {
                // no subsequences exists at all
                return;
            }

            unsigned int first_itemset_size = this->m_itemsets[0].size();

            // this can not happen
            if(first_itemset_size == 0)
            {
                throw std::runtime_error("Itemset with size 0 detected!");
            }
            // if first itemset has only one element
            // a single sequence without this itemset will be generated
            else if(first_itemset_size == 1)
            {
                Sequence seq;
                // copy skipping the first itemset
                seq.m_itemsets.assign(
                    ++this->m_itemsets.begin(),
                    this->m_itemsets.end());
                _cont_subseq.push_back(seq);
            }
            // if first itemset has more then one element
            // all combination of size-1 itemsets sequences will be generated
            else
            {
                // NOTE: using signed long to allow a check < 0
                for(signed long i = first_itemset_size - 1; i >= 0; --i)
                {
                    Sequence seq;

                    Itemset is;
                    // copy all items except i
                    this->m_itemsets[0].getCopyExceptPos(i, is);

                    seq.m_itemsets.push_back(is);

                    // append skipping the first itemset
                    seq.m_itemsets.insert(
                        seq.m_itemsets.begin() + 1,
                        ++this->m_itemsets.begin(),
                        this->m_itemsets.end());

                    _cont_subseq.push_back(seq);
                }
            }
        }

        void getAllContiguosSubsequencesDroppingAnItemFromLastItemset(
            std::vector<Sequence> &_cont_subseq)
        {
            // Sequence size must be at least 2
            // to last itemset be different from first itemset
            if(this->m_itemsets.size() < 2)
            {
                // no last subsequences exists
                return;
            }

            unsigned int last = this->m_itemsets.size() - 1;
            unsigned int last_itemset_size = this->m_itemsets[last].size();

            // this can not happen
            if(last_itemset_size == 0)
            {
                throw std::runtime_error("Itemset with size 0 detected!");
            }
            // if last itemset has only one element
            // a single sequence without this itemset will be generated
            else if(last_itemset_size == 1)
            {
                Sequence seq;

                // copy skipping the last itemset
                seq.m_itemsets.assign(
                    this->m_itemsets.begin(),
                    --this->m_itemsets.end());

                _cont_subseq.push_back(seq);
            }
            // if last itemset has more then one element
            // all combination of size-1 itemsets sequences will be generated
            else
            {
                // NOTE: using signed long to allow a check < 0
                for(signed long i = last_itemset_size - 1; i >= 0; --i)
                {
                    Sequence seq;

                    // copy skipping the last itemset
                    seq.m_itemsets.assign(
                        this->m_itemsets.begin(),
                        --this->m_itemsets.end());

                    Itemset is;
                    // copy all items except i
                    this->m_itemsets[last].getCopyExceptPos(i, is);
                    seq.m_itemsets.push_back(is);

                    _cont_subseq.push_back(seq);
                }
            }
        }

        void getAllContiguosSubsequencesDroppingAnItemFromAnyMiddleItemset(
            std::vector<Sequence> &_cont_subseq)
        {
            // Sequence size must be at least 3 to have middle itemsets
            if(this->m_itemsets.size() < 3)
            {
                // no middle subsequences exists
                return;
            }

            unsigned int last_middle_itemset = this->m_itemsets.size() - 1;

            for(unsigned int i = 1; i < last_middle_itemset; ++i)
            {
                unsigned int middle_itemset_size = this->m_itemsets[i].size();

                // this can not happen
                if(middle_itemset_size == 0)
                {
                    throw std::runtime_error("Itemset with size 0 detected!");
                }
                // if middle itemset has only one element
                // a single sequence without this itemset will be generated
                else if(middle_itemset_size == 1)
                {
                    // by definition, no middele CONTIGUOS subequence can be
                    // generated removing an item inside an itemset of size 1
                    continue;
                }
                // if middle itemset has more then one element
                // all combination of size-1 itemsets sequences
                // will be generated
                else
                {
                    // NOTE: using signed long to allow a check < 0
                    for(signed long j = middle_itemset_size - 1; j >= 0; --j)
                    {
                        Sequence seq;

                        // copy initial itemsets
                        // skipping current middle itemset
                        seq.m_itemsets.insert(
                            seq.m_itemsets.end(),
                            this->m_itemsets.begin(),
                            this->m_itemsets.begin() + i);

                        Itemset is;
                        // copy all items except j
                        this->m_itemsets[i].getCopyExceptPos(j, is);
                        seq.m_itemsets.push_back(is);

                        // copy final itemsets skipping current middle itemset
                        seq.m_itemsets.insert(
                            seq.m_itemsets.end(),
                            this->m_itemsets.begin() + i +1,
                            this->m_itemsets.end());

                        _cont_subseq.push_back(seq);
                    }
                }
            }
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

        Itemset getFirst()
        {
            if(this->m_itemsets.size() < 1)
            {
                throw std::out_of_range("Sequence size must be at least 1.");
            }

            return this->m_itemsets[0];
        }

        std::string toString()
        {
            std::stringstream output;

            std::vector<Itemset>::iterator it;

            output << '<';

            for(
                it = this->m_itemsets.begin();
                it != this->m_itemsets.end();
                ++it)
            {
                output << it->toString();
            }

            output << '>';

            return output.str();
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

                for(
                    it = frequent_items.begin();
                    it != frequent_items.end();
                    ++it)
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
                std::cout << "Joined candidates at pass " << seq_items
                    << std::endl;
                print(new_candidates);

                prune(new_candidates);
                std::cout << "Pruned candidates at pass " << seq_items
                    << std::endl;
                print(new_candidates);
            }
            while(new_candidates.size() > 0);

            //TODO [CMP]:

            // merge of all candidates in the same list ?

            // support count and filter or prune already do this job?
            // Filter dependecies between k-candidates and k+1-candidades
            // is no needed?

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
            deserializer.deserialize(this->m_input_dataset);

            file_stream.close();
        }

        void detectFrequentItems(
            unsigned int _column_num,
            unsigned int _minimum_support,
            std::vector<Item>& _frequent_items)
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
                        std::cout << it->first << '\t' << it->second
                            << std::endl;
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

        void join(
            std::vector<Sequence> &_candidates,
            unsigned int _seq_items,
            std::vector<Sequence> &_new_candidates)
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

                for(
                    it1 = _candidates.begin();
                    it1 != (_candidates.end() - 1);
                    ++it1)
                {
                    for(it2 = (it1 + 1); it2 != _candidates.end(); ++it2)
                    {
                        Sequence seq_both;
                        Itemset is_both;
                        is_both.append(it1->getFirst().getFirst());
                        is_both.append(it2->getFirst().getFirst());
                        seq_both.append(is_both);
                        _new_candidates.push_back(seq_both);

                        Sequence seq_separated;
                        Itemset is_separated1;
                        is_separated1.append(it1->getFirst().getFirst());
                        Itemset is_separated2;
                        is_separated2.append(it2->getFirst().getFirst());
                        seq_separated.append(is_separated1);
                        seq_separated.append(is_separated2);
                        _new_candidates.push_back(seq_separated);
                    }
                }
            }
            else
            {
                //TODO [CMP] to implement
                throw std::runtime_error(
                    "Not implemented: join for k-sequence for k > 1.");
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
                std::vector<Sequence> filtered_candidates;
                bool sequence_to_keep;
                std::vector<Sequence>::iterator it, sub_it;

                // obtaining all continuous subsequences
                for(
                    it = _new_candidates.begin();
                    it != _new_candidates.end();
                    ++it)
                {
                    contiguous_subseq.clear();
                    it->getAllFirstLevelContigousSubsequences(
                        contiguous_subseq);

                    sequence_to_keep = true;

                    for(
                        sub_it = contiguous_subseq.begin();
                        sub_it != contiguous_subseq.end();
                        ++sub_it)
                    {
                        // TODO [CMP]
                        // mark candidate to remove if a subseq is not
                        // contained
                        /*
                        if(
                            std::find(vector.begin(),
                            vector.end(),
                            (*sub_it)) == vector.end()
                        )
                        {
                        * TO TEST:
                            sequence_to_keep = false;
                            break;
                        }
                        */
                    }

                    /*
                     * * TO TEST:
                    if(sequence_to_keep)
                    {
                        filtered_candidates.push_back(*it);
                    }
                    * */
                }

                std::cout << "printing all continuous subsequences"
                    << " for candidates" << std::endl;
                GSP::print(contiguous_subseq);
            }
            else
            {
                throw std::runtime_error(
                    "Not implemented: prune for max_gap != 0.");
            }
        }

        void print(std::vector<Sequence> &_sequences)
        {
            std::vector<Sequence>::iterator it;

            for(it = _sequences.begin(); it != _sequences.end(); ++it)
            {
                std::cout << it->toString() << std::endl;
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
            registerMethod(
                "test_sequenceStringRepresentation",
                *this,
                &GSPTest::test_sequenceStringRepresentation);
            registerMethod("test_equality", *this, &GSPTest::test_equality);
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

        void test_sequenceStringRepresentation()
        {
            // Arrange
            Sequence seq1;

            // Act
            prepare(seq1);
            Sequence seq2("<(abc)(de)(f)(ghi)(lm)>");

            // Assert
            CXXTOOLS_UNIT_ASSERT(seq1 == seq2);
        }

        void test_getAllContiguosSubsequencesDroppingAnItemFromFirstItemset()
        {
            // Arrange
            Sequence seq1;
            prepare(seq1);

            std::vector<Sequence> vect_sub_seq_prepared;
            prepareContSubseqFirsts(vect_sub_seq_prepared);
            std::vector<Sequence> vect_sub_seq_generated;

            // Act
            seq1.getAllContiguosSubsequencesDroppingAnItemFromFirstItemset(
                vect_sub_seq_generated);

            // Assert
            CXXTOOLS_UNIT_ASSERT_EQUALS(
                vect_sub_seq_prepared.size(), vect_sub_seq_generated.size());

            unsigned int size = vect_sub_seq_prepared.size();

            for(unsigned int i = 0; i < size; ++i)
            {
                std::cout << std::endl << '\t' << i << ' ';
                std::cout << vect_sub_seq_prepared[i].toString();
                std::cout << vect_sub_seq_generated[i].toString();
                CXXTOOLS_UNIT_ASSERT(
                    vect_sub_seq_prepared[i] == vect_sub_seq_generated[i]);
            }

            std::cout << std::endl;
        }

        void test_getAllContiguosSubsequencesDroppingAnItemFromLastItemset()
        {
            // Arrange
            Sequence seq1;
            prepare(seq1);

            std::vector<Sequence> vect_sub_seq_prepared;
            prepareContSubseqLasts(vect_sub_seq_prepared);
            std::vector<Sequence> vect_sub_seq_generated;

            // Act
            seq1.getAllContiguosSubsequencesDroppingAnItemFromLastItemset(
                vect_sub_seq_generated);

            // Assert
            CXXTOOLS_UNIT_ASSERT_EQUALS(
                vect_sub_seq_prepared.size(), vect_sub_seq_generated.size());

            unsigned int size = vect_sub_seq_prepared.size();

            for(unsigned int i = 0; i < size; ++i)
            {
                std::cout << std::endl << '\t' << i << ' ';
                std::cout << vect_sub_seq_prepared[i].toString();
                std::cout << vect_sub_seq_generated[i].toString();
                CXXTOOLS_UNIT_ASSERT(
                    vect_sub_seq_prepared[i] == vect_sub_seq_generated[i]);
            }

            std::cout << std::endl;
        }

        void test_getAllContiguosSubsequencesDroppingAnItemFromAnyMiddleItemset()
        {
            // Arrange
            Sequence seq1;
            prepare(seq1);

            std::vector<Sequence> vect_sub_seq_prepared;
            prepareContSubseqMiddles(vect_sub_seq_prepared);
            std::vector<Sequence> vect_sub_seq_generated;

            // Act
            seq1.getAllContiguosSubsequencesDroppingAnItemFromAnyMiddleItemset(
                vect_sub_seq_generated);

            // Assert
            CXXTOOLS_UNIT_ASSERT_EQUALS(
                vect_sub_seq_prepared.size(), vect_sub_seq_generated.size());

            unsigned int size = vect_sub_seq_prepared.size();

            for(unsigned int i = 0; i < size; ++i)
            {
                std::cout << std::endl << '\t' << i << ' ';
                std::cout << vect_sub_seq_prepared[i].toString();
                std::cout << vect_sub_seq_generated[i].toString();
                CXXTOOLS_UNIT_ASSERT(
                    vect_sub_seq_prepared[i] == vect_sub_seq_generated[i]);
            }

            std::cout << std::endl;
        }

        void test_getAllFirstLevelContigousSubsequences()
        {
            // Arrange
            Sequence seq;
            prepare(seq);

            std::vector<Sequence> cont_subseq;
            prepareContSubseqMix(cont_subseq);

            // Act
            std::vector<Sequence> gsp_cont_subseq;
            seq.getAllFirstLevelContigousSubsequences(gsp_cont_subseq);

            // Assert
            CXXTOOLS_UNIT_ASSERT_EQUALS(
                cont_subseq.size(), gsp_cont_subseq.size());

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
            // <(abc)(de)(f)(ghi)(lm)>
            Itemset
                is1("(abc)"),
                is2("(de)"),
                is3("(f)"),
                is4("(ghi)"),
                is5("(lm)");
            _seq.append(is1);
            _seq.append(is2);
            _seq.append(is3);
            _seq.append(is4);
            _seq.append(is5);
        }

        void prepareContSubseqFirsts(std::vector<Sequence> &_cont_subseq)
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

        void prepareContSubseqLasts(std::vector<Sequence> &_cont_subseq)
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

        void prepareContSubseqMiddles(std::vector<Sequence> &_cont_subseq)
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

        void prepareContSubseqMix(std::vector<Sequence> &_cont_subseq)
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
};

cxxtools::unit::RegisterTest<GSPTest> register_GSPTest;



#include "cxxtools/unit/testmain.h"
