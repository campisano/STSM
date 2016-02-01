// Require apt-get install libcxxtools-dev >= 2.2.1

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

        bool isContiguousSubsequenceOf(Sequence &_super_sequence)
        {
            // pag. 8

            // 1. C is derived from S by dropping an item from either S1 or Sn.
            // 2. C is derived from S by dropping an item from an element Si which has at least 2 items.
            // 3. C is a contiguous subsequence of C', and C' is a contiguous subsequence of S.

            //TODO [CMP] to implement
            throw std::runtime_error("Not implemented.");

            return false;
        }

        inline void getCopyDroppingFirst(Sequence &_copy_sequence)
        {
            if(this->m_itemsets.size() < 1)
            {
                throw std::out_of_range("Sequence size must be at least 1.");
            }

            _copy_sequence.m_itemsets.assign(++this->m_itemsets.begin(), this->m_itemsets.end());
        }

        inline void getCopyDroppingLast(Sequence &_copy_sequence)
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
                std::cout << "Joined candidates at " << seq_items << std::endl;
                print(new_candidates);
                prune(new_candidates);
                std::cout << "Pruned candidates at " << seq_items << std::endl;
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
                std::cout << "TODO implement join k-sequence for k > 1." << std::endl;
            }
        }

        void prune(std::vector<Sequence> &_new_candidates)
        {
            // We delete candidate sequences that have a contiguous
            // (k-1)-subsequence whose support count is less than the
            // minimum support. If there is no max-gap constraint, we
            // also delete candidate sequences that have any subsequence
            // without minimum support.
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
        std::vector< std::vector<Item> > m_input_dataset;
};



int main()
{
    GSP gsp("reduced_data_sax.csv");

    std::cout << std::endl << "Test methods:" << std::endl;

    Sequence seq1, seq2;

    Itemset is1, is2, is3, is4;

    is1.append('a');
    is1.append('b');
    is1.append('c');
    is2.append('d');
    is2.append('e');
    is2.append('f');
    seq1.append(is1);
    seq1.append(is2);

    is3.append('a');
    is3.append('b');
    is3.append('c');
    is4.append('d');
    is4.append('e');
    is4.append('f');
    seq2.append(is3);
    seq2.append(is4);

    //test
    if(seq1 == seq2)
    {
        std::cout << "seq1 == seq2" << std::endl;
    }
    else
    {
        std::cout << "seq1 != seq2" << std::endl;
    }

    Sequence seq1DF;
    seq1.getCopyDroppingFirst(seq1DF);

    //test
    if(seq1 == seq1DF)
    {
        std::cout << "seq1 == seq1DF" << std::endl;
    }
    else
    {
        std::cout << "seq1 != seq1DF" << std::endl;
    }

    Sequence seq1DL;
    seq1.getCopyDroppingLast(seq1DL);

    //test
    if(seq1 == seq1DL)
    {
        std::cout << "seq1 == seq1DL" << std::endl;
    }
    else
    {
        std::cout << "seq1 != seq1DL" << std::endl;
    }

    Sequence seq2DF;
    seq2.getCopyDroppingFirst(seq2DF);

    //test
    if(seq1DF == seq2DF)
    {
        std::cout << "seq1DF == seq2DF" << std::endl;
    }
    else
    {
        std::cout << "seq1DF != seq2DF" << std::endl;
    }

    Sequence seq2DL;
    seq2.getCopyDroppingLast(seq2DL);

    //test
    if(seq1DL == seq2DL)
    {
        std::cout << "seq1DL == seq2DL" << std::endl;
    }
    else
    {
        std::cout << "seq1DL != seq2DL" << std::endl;
    }

    return 0;
}
