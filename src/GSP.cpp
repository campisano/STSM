#include "GSP.h"

#include <cxxtools/csvdeserializer.h>
#include <iostream>
#include <fstream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Itemset.h"

GSP::GSP()
{
    this->m_minimum_support = 0;
}

GSP::~GSP()
{
}

void GSP::run(
    std::string _input_filename,
    unsigned int _minimum_support,
    unsigned int _max_gap)
{
    this->setMinimumSupport(_minimum_support);
    this->setMaxGap(_max_gap);

    load(_input_filename);
    std::vector<Item> frequent_items;

    std::cout << "Detecting frequent items:" << std::endl;
    detectFrequentItems(frequent_items);

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

    std::vector<Sequence> &curr_candidates = candidates;
    std::vector<Sequence> new_candidates;
    unsigned int seq_items = 0;

    // pag. 8
    // The algorithm terminates when there are no frequent sequences
    // at the end of a pass, or when there are no candidate sequences
    // generated.
    while(curr_candidates.size() > 0)
    {
        ++seq_items;

        join(curr_candidates, seq_items, new_candidates);
        std::cout << "Candidates after Join at pass " << seq_items
            << std::endl;
        print(new_candidates);

        prune(new_candidates);
        std::cout << "Candidates after Prune at pass " << seq_items
            << std::endl;
        print(new_candidates);

        curr_candidates = new_candidates;
        new_candidates.clear();
    }

    //TODO [CMP]:

    // merge of all candidates in the same list ?

    // support count and filter or prune already do this job?
    // Filter dependecies between k-candidates and k+1-candidades
    // is no needed?

    // what's next?
}

void GSP::setMinimumSupport(unsigned int _minimum_support)
{
    if(_minimum_support < 1)
    {
        throw std::runtime_error(
            "Minumum Support parameter cannot be less then 1.");
    }

    this->m_minimum_support = _minimum_support;
}

void GSP::setMaxGap(unsigned int _max_gap)
{
    if(_max_gap < 0)
    {
        throw std::runtime_error(
            "Max Gap parameter cannot be less then 0.");
    }

    this->m_max_gap = _max_gap;
}

void GSP::load(std::string &_input_filename)
{
    std::fstream file_stream;
    file_stream.open(_input_filename.c_str(), std::fstream::in);

    cxxtools::CsvDeserializer deserializer(file_stream);
    deserializer.delimiter(',');
    deserializer.deserialize(this->m_input_dataset);

    file_stream.close();
}

void GSP::detectFrequentItems(std::vector<Item>& _frequent_items)
{
    std::map<Item, unsigned int> map_count;

    // counting. This algorithm expect data-sequences as rows
    {
        Item item;
        unsigned int tot_rows = this->m_input_dataset.size();
        unsigned int tot_cols;
        unsigned int row, col;
        unsigned int sub_tot_cols;
        unsigned int sub_row, sub_col;

        for(row = 0; row < tot_rows; ++row)
        {
            tot_cols = this->m_input_dataset[row].size();

            for(col = 0; col < tot_cols; ++col)
            {
                // discovering all items, passing through every item
                // of every row
                item = this->m_input_dataset[row][col];

                // if the item was already counted, skip this item
                if (map_count.find(item) != map_count.end())
                {
                    continue;
                }

                // so, this is a new item, there is no item like it
                // in the previous rows and in the previous items of
                // current row. In the current data-sequence (row) the
                // current item exists, so the algorithm can count it
                map_count[item] = 1;

                // next, the algorithm will go in each sucessive
                // data-sequences to find if the item is contained

                for(sub_row = row + 1; sub_row < tot_rows; ++sub_row)
                {
                    sub_tot_cols = this->m_input_dataset[
                        sub_row].size();

                    for(sub_col = 0; sub_col < sub_tot_cols; ++sub_col)
                    {
                        // if the algorithm find the item in any
                        // position of current data-sequence (row)
                        if(item == this->m_input_dataset[sub_row][
                            sub_col])
                        {
                            // increment the counter of data-sequences
                            // that contains this item
                            map_count[item] = map_count[item] + 1;

                            // skip the entire current row
                            break;
                        }
                    }
                }
            }
        }
    }

    // filter
    {
        std::map<Item, unsigned int>::iterator it = map_count.begin();

        while(it != map_count.end())
        {
            if(it->second < this->m_minimum_support)
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
        std::map<Item, unsigned int>::iterator it;

        for(it = map_count.begin(); it != map_count.end(); ++it)
        {
            _frequent_items.push_back(it->first);
        }
    }

}

void GSP::join(
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
            it1 != (_candidates.end());
            ++it1)
        {
            // TODO [CMP] added and to be discussed
            // this block add sequences of repetitive items like
            // <(aa)>
            // <(a)(a)>
            {
                {
                    Sequence seq_both;
                    Itemset is_both;
                    is_both.append(it1->getFirst().getFirst());
                    is_both.append(it1->getFirst().getFirst());
                    seq_both.append(is_both);
                    _new_candidates.push_back(seq_both);
                }
                {
                    Sequence seq_separated;
                    Itemset is_separated_1;
                    is_separated_1.append(it1->getFirst().getFirst());
                    Itemset is_separated_2;
                    is_separated_2.append(it1->getFirst().getFirst());
                    seq_separated.append(is_separated_1);
                    seq_separated.append(is_separated_2);
                    _new_candidates.push_back(seq_separated);
                }
            }

            for(it2 = (it1 + 1); it2 != _candidates.end(); ++it2)
            {
                {
                    Sequence seq_both;
                    Itemset is_both;
                    is_both.append(it1->getFirst().getFirst());
                    is_both.append(it2->getFirst().getFirst());
                    seq_both.append(is_both);
                    _new_candidates.push_back(seq_both);
                }
                {
                    Sequence seq_separated;
                    Itemset is_separated_1;
                    is_separated_1.append(it1->getFirst().getFirst());
                    Itemset is_separated_2;
                    is_separated_2.append(it2->getFirst().getFirst());
                    seq_separated.append(is_separated_1);
                    seq_separated.append(is_separated_2);
                    _new_candidates.push_back(seq_separated);
                }
                {
                    Sequence seq_separated_inverted;
                    Itemset is_separated_1_inverted;
                    is_separated_1_inverted.append(
                        it2->getFirst().getFirst());
                    Itemset is_separated_2_inverted;
                    is_separated_2_inverted.append(
                        it1->getFirst().getFirst());
                    seq_separated_inverted.append(
                        is_separated_1_inverted);
                    seq_separated_inverted.append(
                        is_separated_2_inverted);
                    _new_candidates.push_back(seq_separated_inverted);
                }
            }
        }
    }
    else
    {
        std::vector<Sequence>::iterator it1;
        std::vector<Sequence>::iterator it2;

        for(
            it1 = _candidates.begin();
            it1 != (_candidates.end());
            ++it1)
        {
            for(it2 = _candidates.begin(); it2 != _candidates.end(); ++it2)
            {
                {
                    //std::vector<Sequence> sub_seqs_S1, sub_seqs_S2;
                    //it1->getAllContiguosSubsequencesDroppingAnItemFromFirstItemset(sub_seqs_S1);
                    //it2->getAllContiguosSubsequencesDroppingAnItemFromLastItemset(sub_seqs_S2);

                    //if(sub_seqs_S1.size() > 0 && sub_seqs_S2.size() > 0)
                    //{
                        //TODO [CMP] to be honest,
                        // I think that there is no a 'first item'
                        // or 'last item' in a sequence
                        // if the first (or last) element of a sequence
                        // is a itemset of size > 1, there is no first
                        // (or last) item in this itemset, because the
                        // itemset has no order, so we should consider all
                        // the combinations... is it correct??
                    //    Sequence sub_seq_S1 = sub_seqs_S1[0];
                    //    Sequence sub_seq_S2 = sub_seqs_S2[0];

                    Sequence sub_seq_S1, sub_seq_S2;
                    it1->getSubsequenceDroppingFirstItem(sub_seq_S1);
                    it2->getSubsequenceDroppingLastItem(sub_seq_S2);

                        if(sub_seq_S1 == sub_seq_S2)
                        {

                    //        std::stringstream a;
                    //        a << it1->toString();
                    //        a << it2->toString();
                    //        a << sub_seq_S1.toString();
                    //        a << sub_seq_S2.toString();
                    //        throw std::out_of_range(a.str());
        // The candidate sequence generated by joining S1 with S2 is the sequence
        // S1 extended with the last item in S2. The added item becomes a separate
        // element if it was a separate element in S2, and part of the last element of
        // S1 otherwise.
                            _new_candidates.push_back(
                                this->joinSubsequences(*it1, *it2));
                        }
                    //}
                }
                //TODO [CMP] check with Florent if is correct to join
                // S2 with S1 too
                {

                }
            }
        }
    }
}

Sequence GSP::joinSubsequences(
    Sequence& _seq1,
    Sequence& _seq2)
{
    Sequence seq_S1_ext(_seq1);

    // if is a separate element
    if(_seq2.getLast().size() == 1)
    {
        Itemset is_last;
        is_last.append(_seq2.getLast().getLast());
        seq_S1_ext.append(is_last);
    }
    else
    {
        //TODO [CMP] itemset.getFirst() is
        // nosense if itemset is
        // a unordered set !!!
        seq_S1_ext.getLast().append(_seq2.getLast().getLast());
    }

    return seq_S1_ext;
}

void GSP::prune(std::vector<Sequence> &_new_candidates)
{
    // TODO [CMP] there is a temporary simple support count
    // to determine if the candidate should be removed. It is no part
    // of the GSP paper

    unsigned int support;
    std::vector<Sequence>::iterator it = _new_candidates.begin();

    // for each candidate
    while(it != _new_candidates.end())
    {
        support = this->getSupport(*it);

        if(support < this->m_minimum_support)
        {
            it = _new_candidates.erase(it);
        }
        else
        {
            std::cout <<
                it->toString() << '\t' << support << std::endl;
            ++it;
        }
    }

    ///////////////////////////////////////////////////////////////
    if(false)
    {
        // pag. 9
        // We delete candidate sequences that have a contiguous
        // (k-1)-subsequence whose support count is less than the
        // minimum support. If there is no max-gap constraint, we
        // also delete candidate sequences that have any subsequence
        // without minimum support.

        std::vector<Sequence> contiguous_subseq;
        std::vector<Sequence> filtered_candidates;
        //bool sequence_to_keep;
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

            //sequence_to_keep = true;

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
}

// counting the number of input data-sequences
// that contain the sequence
unsigned int GSP::getSupport(Sequence & _sequence)
{
    // considering the input data-sequences as itemsets of only one
    // item, e.g. <(a)(c)(a)(d)(n)(z)(x)(f)>
    // so a sequence with an itemset.size() > 1 cannot exists in any
    // of the input data-sequences.
    if(_sequence.hasItemsetWithSizeGreaterThenOne())
    {
        return 0;
    }

    // obtaining a string representation of the sequence
    // to easy loop inside it's elements
    std::string seq = _sequence.toStringOfItems();

    unsigned int support = 0;

    Item item;
    unsigned int tot_rows = this->m_input_dataset.size();
    unsigned int tot_cols;
    unsigned int row, col;
    unsigned int start_match_col;
    unsigned int last_match_col;
    std::string::iterator it;

    // for each input data-seqeunce, check if contain the sequence
    for(row = 0; row < tot_rows; ++row)
    {
        tot_cols = this->m_input_dataset[row].size();

        // passing through every item of every row
        // and every item of the sequence
        // the candidate sequence pointer only advance
        // if the current item of the input data-sequence
        // match match with the current item
        // of the candidate sequence
        col = 0;
        it = seq.begin();
        start_match_col = 0;
        last_match_col = 0;

        // for all items in the sequence
        // and for all items in the input data-sequence
        while(it != seq.end() && col < tot_cols)
        {
            // check the max_gap constrant
            if(last_match_col != 0 &&
                (col - last_match_col) > (this->m_max_gap + 1))
            {
                // rewind to start_match_col + 1
                col = start_match_col + 1;
                it = seq.begin();
                start_match_col = 0;
                last_match_col = 0;
                continue;
            }

            item = this->m_input_dataset[row][col];

            // if match, go to next item of the sequence
            if(item == *it)
            {
                // take note for the start of the match
                if(start_match_col == 0)
                {
                    start_match_col = col;
                }

                last_match_col = col;
                ++it;
            }

            // in any case
            // go to the next item of the input data-sequence
             ++col;
        }

        // if the sequence iterator reach the end,
        // then the input data-sequence contain the sequence
        if(it == seq.end())
        {
            ++support;
        }
    }

    return support;
}

void GSP::print(std::vector<Sequence> &_sequences)
{
    std::vector<Sequence>::iterator it;

    for(it = _sequences.begin(); it != _sequences.end(); ++it)
    {
        std::cout << it->toString() << std::endl;
    }
}
