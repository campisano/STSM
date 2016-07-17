#include "GSP.h"

#include <cxxtools/csvdeserializer.h>
#include <cxxtools/decomposer.h>
#include <cxxtools/jsonformatter.h>
//#include <cxxtools/jsonserializer.h>
#include <cxxtools/utf8codec.h>
#include <fstream>
#include <limits>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Itemset.h"

GSP::GSP()
{
    //TODO [CMP] the load can be performed there
    this->m_min_support = 0;
    this->m_max_support = std::numeric_limits<double>::max();
}

GSP::~GSP()
{
}

void GSP::run( //TODO [CMP] rename to findFrequentSequences
    std::string _input_filename,
    std::string _log_filename,
    unsigned int _min_support,
    unsigned int _max_support,
    unsigned int _max_time_window)
{
    this->m_input_dataset.clear();
    this->m_supported_sequences_positions.clear();
    this->m_log_stream.open(_log_filename.c_str());

    this->load(_input_filename);

    this->setMinimumSupportPerc(_min_support);
    this->setMaximumSupportPerc(_max_support);
    this->setMaxTimeWindow(_max_time_window);

    // logging info
    this->m_log_stream << "Input: " << _input_filename << std::endl;
    this->m_log_stream << "Log: " << _log_filename << std::endl;
    this->m_log_stream << "Num of datasources: " << this->getNumDatasources()
        << std::endl;
    this->m_log_stream << "Min support: " << _min_support
        << "% = " << this->m_min_support << std::endl;
    this->m_log_stream << "Max support: " << _max_support
        << "% = " << this->m_max_support << std::endl;
    this->m_log_stream << "Max time window: " << _max_time_window << std::endl;
    this->m_log_stream << std::endl;

    std::list<Item> frequent_items;

    this->m_log_stream << "Detecting frequent items:" << std::endl;
    this->detectFrequentItems(frequent_items);

    std::list<Sequence> candidates;

    //TODO [CMP]
    // frequent_items = detectFrequentItems(database, min_support)
    // frequent_1sequences = convertToSequences(frequent_items))

    // temporary code, probably detectFrequentItems can return
    // directly a list of Sequences
    {
        std::list<Item>::iterator it;

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

    //TODO [CMP] tehy aren't candidades, they are frequent 1-sequences
    this->m_log_stream << "First candidates:" << std::endl;
    this->print(candidates);

    std::list<Sequence> &curr_candidates = candidates;
    std::list<Sequence> new_candidates;
    unsigned int seq_items = 0;

    // pag. 8
    // The algorithm terminates when there are no frequent sequences
    // at the end of a pass, or when there are no candidate sequences
    // generated.
    while(curr_candidates.size() > 0)
    {
        ++seq_items;

        // TODO[CMP] rename to joinSequences
        this->join(curr_candidates, seq_items, new_candidates);
        this->m_log_stream << "Candidates after Join at pass " << seq_items
            << std::endl;
        print(new_candidates);

        // TODO[CMP] rename to pruneCandidates
        this->prune(seq_items + 1, new_candidates);
        this->m_log_stream << "Candidates after Prune at pass " << seq_items
            << std::endl;
        print(new_candidates);

        curr_candidates = new_candidates;
        new_candidates.clear();
    }

    // filter following max-support constraint
    {
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

        it_seq_map = this->m_supported_sequences_positions.begin();

        // for each length group
        while(it_seq_map != this->m_supported_sequences_positions.end())
        {
            it_results_map = it_seq_map->second.begin();

            // for each sequence
            while(it_results_map != it_seq_map->second.end())
            {
                if(it_results_map->second.first > this->m_max_support)
                {
                    it_seq_map->second.erase(it_results_map++);
                }
                else
                {
                    ++it_results_map;
                }
            }

            if(it_seq_map->second.size() == 0)
            {
                this->m_supported_sequences_positions.erase(it_seq_map++);
            }
            else
            {
                ++it_seq_map;
            }
        }
    }

    // print supported sequences
    {
        this->m_log_stream
            << std::endl << "Printing supported sequences:" << std::endl;

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
            this->m_log_stream <<
                '\t' << "length: " << it_seq_map->first << std::endl;

            for(
                it_results_map = it_seq_map->second.begin();
                it_results_map != it_seq_map->second.end();
                ++it_results_map
            )
            {
                if(it_results_map->second.first > 0)
                {
                    this->m_log_stream << "\t\t" << "sequence: "
                        << it_results_map->first << std::endl;

                    this->m_log_stream << "\t\t" << "count: "
                        << it_results_map->second.first << std::endl;

                    /*
                    this->m_log_stream << "\t\t" << "position: ";

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
                        this->m_log_stream << '('
                            << it_positions_vect->first << ','
                            << it_positions_vect->second << ')';
                    }

                    this->m_log_stream << std::endl;
                    */
                }
            }
        }
    }

    this->m_log_stream.close();
}

void GSP::setMinimumSupportPerc(unsigned int _min_support)
{
    if(_min_support < 1)
    {
        throw std::runtime_error(
            "Minumum Support parameter cannot be less then 1%.");
    }

    double min_support = _min_support * this->getNumDatasources() / 100.0;

    if(min_support > this->m_max_support)
    {
        std::stringstream text;
        text << "Minimum Support (" << min_support
            << ") cannot be greater than then Maximum Support ("
            << this->m_max_support << ").";
        throw std::runtime_error(text.str());
    }

    this->m_min_support = min_support;
}

void GSP::setMaximumSupportPerc(unsigned int _max_support)
{
    if(_max_support < 1)
    {
        throw std::runtime_error(
            "Maximum Support parameter cannot be less then 1%.");
    }

    double max_support = _max_support * this->getNumDatasources() / 100.0;

    if(max_support < this->m_min_support)
    {
        std::stringstream text;
        text << "Maximum Support (" << max_support
            << ") cannot be greater than then Minimum Support ("
            << this->m_min_support << ").";
        throw std::runtime_error(text.str());
    }

    this->m_max_support = max_support;
}

void GSP::setMaxTimeWindow(unsigned int _max_time_window)
{
    this->m_max_time_window = _max_time_window;
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

void GSP::detectFrequentItems(std::list<Item>& _frequent_items)
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
            if(it->second < this->m_min_support)
            {
                map_count.erase(it++);
            }
            else
            {
                this->m_log_stream << it->first << '\t' << it->second
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
    std::list<Sequence> &_candidates,
    unsigned int _seq_items,
    std::list<Sequence> &_new_candidates)
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
        std::list<Sequence>::iterator it1;
        std::list<Sequence>::iterator it1_next;
        std::list<Sequence>::iterator it2;

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

            // [CMP] a strange way to get next element
            // without increment the original iterator
            it1_next = it1;
            ++it1_next;

            for(it2 = it1_next; it2 != _candidates.end(); ++it2)
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
        std::list<Sequence>::iterator it1;
        std::list<Sequence>::iterator it2;

        for(
            it1 = _candidates.begin();
            it1 != (_candidates.end());
            ++it1)
        {
            for(it2 = _candidates.begin(); it2 != _candidates.end(); ++it2)
            {
                {
                    //std::list<Sequence> sub_seqs_S1, sub_seqs_S2;
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
    std::list<Sequence> &_new_candidates
)
{
    // TODO [CMP] there is a temporary simple support count
    // to determine if the candidate should be removed. It is no part
    // of the GSP paper

    this->updateSupportCountPositions(_new_candidates, _seq_items);

    unsigned int support;
    std::list<Sequence>::iterator it = _new_candidates.begin();

    // for each candidate
    while(it != _new_candidates.end())
    {
        //support = this->getSupport(*it);
        support = this->m_supported_sequences_positions[
            _seq_items][it->toString()].first;

        if(support < this->m_min_support)
        {
            this->pruneSequence(_seq_items, *it);
            it = _new_candidates.erase(it);
        }
        else
        {
            this->m_log_stream <<
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

        std::list<Sequence> contiguous_subseq;
        std::list<Sequence> filtered_candidates;
        //bool sequence_to_keep;
        std::list<Sequence>::iterator it, sub_it;

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

        this->m_log_stream << "printing all continuous subsequences"
            << " for candidates" << std::endl;
        GSP::print(contiguous_subseq);
    }
}

void GSP::pruneSequence(unsigned int _seq_items, Sequence& _sequence)
{
    this->m_supported_sequences_positions[_seq_items].erase(
        _sequence.toString());
    if(this->m_supported_sequences_positions[_seq_items].size()==0)
    {
        this->m_supported_sequences_positions.erase(_seq_items);
    }
}

void GSP::updateSupportCountPositions(
    std::list<Sequence> &_new_candidates,
    unsigned int _seq_items
)
{
    // use a temporary way to store support count per sequence
    // for each sequence, map the data-sequences that support it
    //       sequence              data-sequences
    std::map<std::string, std::set<unsigned int> > supports;

    // use a tempory way to store positions
    //       sequence
    std::map<std::string,
    //  positions           x             y
        std::list<std::pair<unsigned int, unsigned int> > > positions;

    std::list<Sequence>::iterator it_seq;
    std::string str_seq;
    unsigned int str_seq_size;
    std::string::iterator str_it;
    unsigned int tot_rows = this->m_input_dataset.size();
    unsigned int tot_cols;
    unsigned int col;
    unsigned int start_match_col;
    //unsigned int last_match_col;                  // [CMP] this is useful only for max-gap

    // for each input data-seqeunce, check if contain the sequence
    for(unsigned int row = 0; row < tot_rows; ++row)
    {
        tot_cols = this->m_input_dataset[row].size();

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
            str_seq_size = str_seq.size();
            str_it = str_seq.begin();

            if(str_seq_size < 2)
            {
                std::stringstream msg;
                msg << "Current support count function doesn't handle"
                    << " sequence with less then two items." << std::endl
                    << "Current sequence: '" << str_seq << "'.";
                throw std::runtime_error(msg.str());
            }

            col = 0;
            start_match_col = 0;
            //last_match_col = 0;                   // [CMP] this is useful only for max-gap

            // passing through every item of the row
            while(col < tot_cols)
            {
                // check the max_time window constrant
                // this check must be an early check
                if(
                    // [CMP] this is useful only for max-gap
                    //last_match_col != 0 &&
                    //(col - last_match_col) > (this->m_max_gap + 1)

                    // [CMP] imagine a sequence of length 5
                    // max_time_window < 5 make no sense
                    start_match_col != 0 &&
                    (col - start_match_col) >= (
                        this->m_max_time_window + str_seq_size)
                )
                {
                    // rewind to start_match_col + 1
                    col = start_match_col + 1;
                    start_match_col = 0;
                    //last_match_col = 0;           // [CMP] this is useful only for max-gap
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

                    //last_match_col = col;         // [CMP] this is useful only for max-gap
                    ++str_it;

                    // if the sequence iterator reach the end,
                    // then the input data-sequence contain the sequence
                    if(str_it == str_seq.end())
                    {
                        std::pair<unsigned int, unsigned int>position(
                            row, col - str_seq_size + 1);

                        supports[it_seq->toString()].insert(row);
                        positions[it_seq->toString()].push_back(position);

                        // rewind to start_match_col + 1
                        col = start_match_col + 1;
                        start_match_col = 0;
                        //last_match_col = 0;       // [CMP] this is useful only for max-gap
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

    // update support count and position list
    std::map<std::string, std::set<unsigned int> >::iterator supports_it;
    for(
        supports_it = supports.begin();
        supports_it != supports.end();
        ++supports_it
    )
    {
        std::pair <unsigned int,
            std::list <std::pair<unsigned int, unsigned int> >
        > pair(
            supports_it->second.size(),     // set support count
            positions[supports_it->first]   // set position list
        );

        // update support count and position list
        this->m_supported_sequences_positions[_seq_items][
            supports_it->first] = pair;
    }
}

void GSP::print(std::list<Sequence> &_sequences)
{
    std::list<Sequence>::iterator it;

    for(it = _sequences.begin(); it != _sequences.end(); ++it)
    {
        this->m_log_stream << it->toString() << std::endl;
    }
}

void GSP::saveJSON(std::string &_output_filename)
{
    std::ofstream output_file;
    output_file.open(_output_filename.c_str());
    cxxtools::TextOStream ts(output_file, new cxxtools::Utf8Codec());

    cxxtools::JsonFormatter formatter(ts);
    formatter.beautify(false);

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

    formatter.beginArray("", "");

    // for each length group of detected sequence
    for(
        it_seq_map = this->m_supported_sequences_positions.begin();
        it_seq_map != this->m_supported_sequences_positions.end();
        ++it_seq_map
    )
    {
        formatter.beginObject("", "");
        formatter.addValueInt("length", "", it_seq_map->first);
        formatter.beginArray("sequences", "");

        // for each sequence of that length
        for(
            it_results_map = it_seq_map->second.begin();
            it_results_map != it_seq_map->second.end();
            ++it_results_map
        )
        {
            if(it_results_map->second.first > 0)
            {
                formatter.beginObject("", "");

                formatter.addValueStdString(
                    "sequence", "", it_results_map->first);
                formatter.addValueInt(
                    "support", "", it_results_map->second.first);

                formatter.beginArray("times", "");

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
                    formatter.addValueInt(
                        "", "", it_positions_vect->first);
                }

                formatter.finishArray();
                formatter.beginArray("spaces", "");

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
                    formatter.addValueInt(
                        "", "", it_positions_vect->second);
                }

                formatter.finishArray();
                formatter.finishObject();
            }
        }

        formatter.finishArray();
        formatter.finishObject();
    }

    formatter.finishArray();
}
