#include "GSP.h"

#include <cxxtools/csvdeserializer.h>
#include <cxxtools/jsonserializer.h>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
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
    std::string _output_filename,
    unsigned int _minimum_support,
    unsigned int _max_gap)
{
    this->setMinimumSupport(_minimum_support);
    this->setMaxGap(_max_gap);

    this->load(_input_filename);
    std::vector<Item> frequent_items;

    std::cout << "Detecting frequent items:" << std::endl;
    this->detectFrequentItems(frequent_items);

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
    this->print(candidates);

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

        this->join(curr_candidates, seq_items, new_candidates);
        std::cout << "Candidates after Join at pass " << seq_items
            << std::endl;
        print(new_candidates);

        this->prune(seq_items, new_candidates);
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

    // print supported sequences
    {
        std::cout << std::endl << "Printing supported sequences:" << std::endl;
        std::map< unsigned int,    // mapping sequences per length (seq items)
            std::map< std::string, // mapping results per sequence (toString())
                std::pair < unsigned int,           // support count
                    std::list <                     // list of positions
                        std::pair< unsigned int, unsigned int > // sensor, time
                    >
                >
            >
        >::iterator it_seq_map;

        std::map< std::string, // mapping results per sequence (toString())
            std::pair < unsigned int,               // support count
                std::list <                         // list of positions
                    std::pair< unsigned int, unsigned int > // sensor, time
                >
            >
        >::iterator it_results_map;

        std::list <                                 // list of positions
            std::pair< unsigned int, unsigned int > // sensor, time
        >::iterator it_positions_vect;

        for(
            it_seq_map = this->m_supported_sequences_positions.begin();
            it_seq_map != this->m_supported_sequences_positions.end();
            ++it_seq_map
        )
        {
            std::cout << '\t' << "length: " << it_seq_map->first << std::endl;

            for(
                it_results_map = it_seq_map->second.begin();
                it_results_map != it_seq_map->second.end();
                ++it_results_map
            )
            {
                if(it_results_map->second.first > 0)
                {
                    std::cout << "\t\t" << "sequence: "
                        << it_results_map->first << std::endl;

                    std::cout << "\t\t" << "count: "
                        << it_results_map->second.first << std::endl;
                    for(
                        it_positions_vect = (
                            it_results_map->second.second.begin()
                        );
                        it_positions_vect != (
                            it_results_map->second.second.end()
                        );
                        ++it_positions_vect
                    )
                    {
                        std::cout << "\t\t\t" << "position: "
                            << it_positions_vect->first << " "
                            << it_positions_vect->second << std::endl;
                    }
                }
            }
        }
    }

    this->save(_output_filename);
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
    std::ifstream file_stream;
    file_stream.open(_input_filename.c_str(), std::ifstream::in);

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

void GSP::prune(
    unsigned int _seq_items,
    std::vector<Sequence> &_new_candidates
)
{
    // TODO [CMP] there is a temporary simple support count
    // to determine if the candidate should be removed. It is no part
    // of the GSP paper

    this->updateSupportCountPositions(_new_candidates, _seq_items);

    unsigned int support;
    std::vector<Sequence>::iterator it = _new_candidates.begin();

    // for each candidate
    while(it != _new_candidates.end())
    {
        //support = this->getSupport(*it);
        support = this->m_supported_sequences_positions[
            _seq_items][it->toString()].first;

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

void GSP::updateSupportCountPositions(
    std::vector<Sequence> &_new_candidates,
    unsigned int _seq_items
)
{
    unsigned int tot_rows = this->m_input_dataset.size();
    std::vector<Sequence>::iterator it_seq;
    std::string str_seq;
    std::string::iterator str_it;

    // initialize support vector map
    // for each sequence candidate
    for(
        it_seq = _new_candidates.begin();
        it_seq != _new_candidates.end();
        ++it_seq
    )
    {
        std::list< std::pair<unsigned int, unsigned int> > list;
        std::pair <
            unsigned int, std::list <
                std::pair<unsigned int, unsigned int>
            >
        > pair(0, list);
        this->m_supported_sequences_positions[_seq_items][
            it_seq->toString()] = pair;
    }

    // use a temporary way to store support count per sequence
    std::map<std::string, std::set<unsigned int> > support;

    // for each input data-seqeunce, check if contain the sequence
    for(unsigned int row = 0; row < tot_rows; ++row)
    {
        unsigned int tot_cols = this->m_input_dataset[row].size();

        // for each sequence candidate
        for(
            it_seq = _new_candidates.begin();
            it_seq != _new_candidates.end();
            ++it_seq
        )
        {
            // considering the input data-sequences as itemsets of only one
            // item, e.g. <(a)(c)(a)(d)(n)(z)(x)(f)>
            // so a sequence with an itemset.size() > 1 cannot exists in any
            // of the input data-sequences.
            if(it_seq->hasItemsetWithSizeGreaterThenOne())
            {
                continue;
            }

            // obtaining a string representation of the sequence
            // to easy loop inside it's elements
            str_seq = it_seq->toStringOfItems();
            str_it = str_seq.begin();

            if(str_seq.size() < 2)
            {
                throw std::runtime_error(
                    "Current support count function doesn't handle"
                    " sequence with less then two items.");
            }

            unsigned int col = 0;
            unsigned int start_match_col = 0;
            unsigned int last_match_col = 0;

            // passing through every item of the row
            while(col < tot_cols)
            {
                // check the max_gap constrant
                // this check must be an early check
                if(
                    last_match_col != 0 &&
                    (col - last_match_col) > (this->m_max_gap + 1)
                )
                {
                    // rewind to start_match_col + 1
                    col = start_match_col + 1;
                    start_match_col = 0;
                    last_match_col = 0;
                    str_it = str_seq.begin();
                    continue;
                }

                Item &item = this->m_input_dataset[row][col];

                // if match, go to next item of the sequence
                if(item == *str_it)
                {
                    // take note for the start of the match
                    if(start_match_col == 0)
                    {
                        start_match_col = col;
                    }

                    last_match_col = col;
                    ++str_it;

                    // if the sequence iterator reach the end,
                    // then the input data-sequence contain the sequence
                    if(str_it == str_seq.end())
                    {
                        std::pair<unsigned int, unsigned int>
                            position(row, col - str_seq.size() + 1);
                        this->m_supported_sequences_positions[_seq_items][
                            it_seq->toString()].second.push_back(position);

                        support[it_seq->toString()].insert(row);

                        // rewind to start_match_col + 1
                        col = start_match_col + 1;
                        start_match_col = 0;
                        last_match_col = 0;
                        str_it = str_seq.begin();
                        continue;
                    }
                }

                // in any other case
                // go to the next item of the input data-sequence
                 ++col;
            }
        }
    }

    std::map<std::string, std::set<unsigned int> >::iterator support_it;
    for(
        support_it = support.begin();
        support_it != support.end();
        ++support_it
    )
    {
        // update support count
        this->m_supported_sequences_positions[_seq_items][
            support_it->first].first = support_it->second.size();
    }
}

void GSP::print(std::vector<Sequence> &_sequences)
{
    std::vector<Sequence>::iterator it;

    for(it = _sequences.begin(); it != _sequences.end(); ++it)
    {
        std::cout << it->toString() << std::endl;
    }
}

void GSP::save(std::string &_output_filename)
{
    //cxxtools::SerializationInfo si;
    //si.addMember("encoding") <<= "UTF-8";
    //cxxtools::SerializationInfo& psi = si.addMember("plug-ins");
    //psi.addMember() <<= "python";
    //psi.addMember() <<= "c++";
    //psi.addMember() <<= "ruby";
    //cxxtools::SerializationInfo& isi = si.addMember("indent");
    //isi.addMember("length") <<= 3;
    //isi.addMember("use_space") <<= true;

    //std::ofstream output(_output_filename);
    //output << cxxtools::Json(si);  // this prints unformatted and very compact json without any white space
    //output << cxxtools::Json(si, true);


    // Writing JSON needs a cxxtools::JsonSerializer:

    //std::ofstream output(_output_filename.c_str());
    //cxxtools::JsonSerializer serializer(output);
    //serializer.beautify(true);
    //output.serialize(serializer);

    std::ofstream file;
    //char buffer[8192];
    //file.rdbuf()->pubsetbuf(buffer, sizeof buffer);
    file.open(_output_filename.c_str());
    cxxtools::JsonSerializer serializer(file);
    serializer.beautify(true);
    serializer.serialize(m_supported_sequences_positions).finish();
}
