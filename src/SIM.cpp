#include "SIM.h"

#include <ctime>
#include <cxxtools/csvdeserializer.h>
#include <cxxtools/decomposer.h>
#include <cxxtools/jsonformatter.h>
#include <cxxtools/utf8codec.h>
#include <sstream>
#include <stdexcept>
#include <string>

SIM::SIM()
{
    //TODO [CMP] the loadDatabase() can be performed there
    // or the run() can be this!
    m_min_spatial_freq = 0.0;
    m_min_block_freq = 0.0;
}

SIM::~SIM()
{
}

void SIM::run(
    const std::string & _input_filename,
    const std::string & _log_filename,
    const unsigned int & _min_spatial_freq_perc,
    const unsigned int & _min_block_freq_perc)
{
    m_database.clear();
    m_solid_sequences.clear();
    m_ranged_sequence_positions.clear();
    m_solid_sequence_blocks.clear();

    setMinSpatialFreq(float(_min_spatial_freq_perc) / 100.0);
    setMinBlockFreq(float(_min_block_freq_perc) / 100.0);

    clock_t start_timer = clock(), timer;

    // initialize the logger
    m_log_stream.open(_log_filename.c_str());

    // logging info
    m_log_stream << "Input: " << _input_filename << std::endl;
    m_log_stream << "Log: " << _log_filename << std::endl;
    m_log_stream << "Min Spatial Frequency value: "
                 << m_min_spatial_freq << std::endl;
    m_log_stream << "Min Block Frequency value: "
                 << m_min_block_freq << std::endl;
    m_log_stream << std::endl;

    m_log_stream << "Loading database...";
    timer = clock();
    loadDatabase(_input_filename);
    m_log_stream << " ends in "
                 << floor(double(clock() - timer)
                          / CLOCKS_PER_SEC * 1000) / 1000
                 << " seconds." << std::endl;

    m_log_stream << "Generating a set with all database items...";
    timer = clock();
    SetItems items;
    generateTheSetOfAllDatabaseItems(m_database, items);
    m_log_stream << " ends in "
                 << floor(double(clock() - timer)
                          / CLOCKS_PER_SEC * 1000) / 1000
                 << " seconds." << std::endl;

    Size seq_size = 1;
    ListCandidates candidates;
    m_log_stream << "Generating 1-size candidates...";
    timer = clock();
    generate1SizeCandidates(items, 0, m_database.size() - 1, candidates);
    m_log_stream << " ends in "
                 << floor(double(clock() - timer)
                          / CLOCKS_PER_SEC * 1000) / 1000
                 << " seconds." << std::endl;
    m_log_stream << "(Num of candidates: " <<
        candidates.size() << ")" << std::endl;

    Database::const_iterator db_it;
    ListCandidates::iterator cand_it;
    ListKernels::const_iterator kern_it;

    do
    {
        m_log_stream << "* Iteration for sequence of size: "
                     << seq_size << std::endl;

        ListRangedSequence & solid_sequences_k =
            m_solid_sequences[seq_size] = ListRangedSequence();

        ////////////////////////////////////////////////////////////////////////

        m_log_stream << " - Updating candidate kernels...";
        timer = clock();

        Point db_position = 0;

        // for each Database series
        for(db_it = m_database.begin(); db_it != m_database.end(); ++db_it)
        {
            // for each candidate...
            for(cand_it = candidates.begin();
                cand_it != candidates.end(); ++cand_it)
            {
                // ... having range containing the current series
                if(cand_it->range().contains(db_position))
                {
                    // update the current candidate kernels
                    // for the current series
                    cand_it->updateCandidateKernels(
                        *db_it, db_position, m_min_spatial_freq);
                }
            }

            ++db_position;
        }

        m_log_stream << " ends in "
                     << floor(double(clock() - timer)
                              / CLOCKS_PER_SEC * 1000) / 1000
                     << " seconds." << std::endl;

        ////////////////////////////////////////////////////////////////////////

        m_log_stream << " - Merging kernels and creating solid sequences...";
        timer = clock();

        // for each candidate
        for(
            cand_it = candidates.begin();
            cand_it != candidates.end(); ++cand_it)
        {
            // find optimal spatial ranges for the current candidate
            cand_it->mergeKernels(m_min_spatial_freq);

            // for each optimal range
            for(
                kern_it = cand_it->kernels().begin();
                kern_it != cand_it->kernels().end();
                ++ kern_it)
            {
                // defining a new Ranged Sequence
                // that is also a Solid Sequence for this kernel range
                solid_sequences_k.push_back(
                    RangedSequence(
                        cand_it->sequence(),
                        Range(kern_it->start(), kern_it->end()),
                        kern_it->support()));
            }
        }

        m_log_stream << " ends in "
                     << floor(double(clock() - timer)
                              / CLOCKS_PER_SEC * 1000) / 1000
                     << " seconds." << std::endl;
        m_log_stream << "   (Num of solid sequences (with range size > 1)"
                     << " for this iteration: "
                     << solid_sequences_k.size() << ")" << std::endl;

        ////////////////////////////////////////////////////////////////////////

        m_log_stream << " - Clean up solid sequences with small range size...";
        timer = clock();
        cleanupSolidSequencesWithSmallRangeSize(2, solid_sequences_k);
        m_log_stream << " ends in "
                     << floor(double(clock() - timer)
                              / CLOCKS_PER_SEC * 1000) / 1000
                     << " seconds." << std::endl;

        ////////////////////////////////////////////////////////////////////////

        m_log_stream << " - Detecting sequence positions in the database...";
        timer = clock();
        updateMatchingPositions(solid_sequences_k);
        m_log_stream << " ends in "
                     << floor(double(clock() - timer)
                              / CLOCKS_PER_SEC * 1000) / 1000
                     << " seconds." << std::endl;

        ////////////////////////////////////////////////////////////////////////

        m_log_stream << " - Clean up candidates...";
        timer = clock();
        candidates.clear();
        m_log_stream << " ends in "
                     << floor(double(clock() - timer)
                              / CLOCKS_PER_SEC * 1000) / 1000
                     << " seconds." << std::endl;

        ////////////////////////////////////////////////////////////////////////

        m_log_stream << " - Generating candidates...";
        timer = clock();
        generateCandidates(solid_sequences_k, candidates);
        m_log_stream << " ends in "
                     << floor(double(clock() - timer)
                              / CLOCKS_PER_SEC * 1000) / 1000
                     << " seconds." << std::endl;
        m_log_stream << "   (Num of candidates: " <<
            candidates.size() << ")" << std::endl;

        ////////////////////////////////////////////////////////////////////////

        ++seq_size;
    }
    while(candidates.size() > 0);

    // print solid sequences data and positions
    printSolidSequences();

    ////////////////////////////////////////////////////////////////////////

    m_log_stream << " - Detecting solid sequence blocks...";
    timer = clock();

    MapRangedSequencesByLength::const_iterator it_ss_by_len;
    ListRangedSequence::const_iterator it_ss;

    // for each length
    for(
        it_ss_by_len = m_solid_sequences.begin();
        it_ss_by_len != m_solid_sequences.end();
        ++it_ss_by_len
        )
    {
        const Size & size = it_ss_by_len->first;
        const ListRangedSequence & sequences = it_ss_by_len->second;

        // for each sequence of that length
        for(it_ss = sequences.begin(); it_ss != sequences.end(); ++it_ss)
        {
            ListSequenceBlocks & sequence_blocks =
                m_solid_sequence_blocks[size] = ListSequenceBlocks();
            detectSolidSequenceBlocksFromSolidSequence(*it_ss, sequence_blocks);
        }
    }

    m_log_stream << " ends in "
                 << floor(double(clock() - timer)
                          / CLOCKS_PER_SEC * 1000) / 1000
                 << " seconds." << std::endl;

    ////////////////////////////////////////////////////////////////////////

    m_log_stream << "* Total run time: "
                 << floor(double(clock() - start_timer)
                          / CLOCKS_PER_SEC * 1000) / 1000
                 << " seconds." << std::endl;

    // close the logger
    m_log_stream.close();
}

void SIM::loadDatabase(const std::string & _input_filename)
{
    std::ifstream file_stream;
    file_stream.open(_input_filename.c_str(), std::ifstream::in);

    cxxtools::CsvDeserializer deserializer(file_stream);
    deserializer.delimiter(',');
    deserializer.deserialize(m_database);

    file_stream.close();
}

void SIM::generateTheSetOfAllDatabaseItems(
    const Database & _database,
    SetItems & _items) const
{
    // from http://stackoverflow.com/a/1041939/846686
    // and http://stackoverflow.com/a/24477023/846686
    Database::const_iterator db_it;
    Serie::const_iterator sr_it;

    for(db_it = _database.begin(); db_it != _database.end(); ++db_it)
    {
        for(sr_it = db_it->begin(); sr_it != db_it->end(); ++sr_it)
        {
            _items.insert(*sr_it);
        }
    }
}

void SIM::generate1SizeCandidates(
    const SetItems & _items,
    const Point & _start,
    const Point & _end,
    ListCandidates & _candidates) const
{
    SetItems::const_iterator it;

    // for each item in the Database
    for(it = _items.begin(); it != _items.end(); ++it)
    {
        // generate a 1-size candidate associated to an empty set of kernels
        _candidates.push_back(
            Candidate(
                Sequence(*it),
                Range(_start, _end),
                ListKernels()
                ));
    }
}

void SIM::generateCandidates(
    const ListRangedSequence & _solid_sequences,
    ListCandidates & _candidates) const
{
    ListRangedSequence::const_iterator x_it, y_it;
    Sequence seq1_without_first_item;
    Sequence seq2_without_last_item;
    Sequence seq_joined;
    Range rg_intersect(0, 0);

    if(_candidates.size() > 0)
    {
        std::stringstream msg;
        msg << "generateCandidates expects"
            << " an empty list of candidates as input."
            << std::endl;
        throw std::runtime_error(msg.str());
    }

    // for each (x, y) permutation of SSset k
    // where all x sequence items excluding the first
    // are equal to all y sequence items excluding the last
    // and skipping generation of candidates
    // with ranges containing just one time series
    for(
        x_it = _solid_sequences.begin();
        x_it != _solid_sequences.end();
        ++x_it
        )
    {
        for(
            y_it = _solid_sequences.begin();
            y_it != _solid_sequences.end();
            ++y_it
            )
        {
            //TODO [CMP] to test
            if(x_it->range().intersect(y_it->range(), rg_intersect) &&
               rg_intersect.size() > 1
                )
            {
                seq1_without_first_item.clear();
                x_it->sequence().getSubsequenceDroppingFirstItem(
                    seq1_without_first_item);

                seq2_without_last_item.clear();
                y_it->sequence().getSubsequenceDroppingLastItem(
                    seq2_without_last_item);

                if(seq1_without_first_item == seq2_without_last_item)
                {
                    seq_joined = x_it->sequence();
                    seq_joined.append(y_it->sequence().getLast());
                    _candidates.push_back(
                        Candidate(
                            seq_joined,
                            rg_intersect,
                            ListKernels())
                        );
                }
            }
        }
    }
}

void SIM::setMinSpatialFreq(const Frequency & _min_spatial_freq)
{
    if(_min_spatial_freq <=0 )
    {
        std::stringstream msg;
        msg << "Minumum Spatial Frequency parameter value ("
            << _min_spatial_freq << ")"
            << " cannot be less then or equal to 0.";
        throw std::runtime_error(msg.str());
    }

    if(_min_spatial_freq > 1.0)
    {
        std::stringstream msg;
        msg << "Minumum Spatial Frequency parameter value ("
            << _min_spatial_freq << ")"
            << " cannot be greater then 1.0.";
        throw std::runtime_error(msg.str());
    }

    m_min_spatial_freq = _min_spatial_freq;
}

void SIM::setMinBlockFreq(const Frequency & _min_block_freq)
{
    if(_min_block_freq <= 0)
    {
        std::stringstream msg;
        msg << "Minumum Block Frequency parameter value ("
            << _min_block_freq << ")"
            << " cannot be less then or equal to 0.";
        throw std::runtime_error(msg.str());
    }

    if(_min_block_freq > 1.0)
    {
        std::stringstream msg;
        msg << "Minumum Block Frequency parameter value ("
            << _min_block_freq << ")"
            << " cannot be greater then 1.0.";
        throw std::runtime_error(msg.str());
    }

    m_min_block_freq = _min_block_freq;
}

void SIM::updateMatchingPositions(const ListRangedSequence & _solid_sequences)
{
    ListRangedSequence::const_iterator it_seq;
    unsigned int str_seq_size;
    std::string str_seq_rep;
    std::stringstream ss_tmp;
    std::string::iterator str_it;
    unsigned int tot_rows = m_database.size();
    unsigned int tot_cols;
    unsigned int col;
    unsigned int start_match_col;
    bool match_started;
    //unsigned int last_match_col; // [CMP] useful only for max-gap

    // for each input data-seqeunce, check if contain the sequence
    for(unsigned int row = 0; row < tot_rows; ++row)
    {
        // [CMP] for debug
        // m_log_stream << std::string(
        //     m_database[row].begin(), m_database[row].end()) << std::endl;

        tot_cols = m_database[row].size();

        // for each sequence candidate
        for(
            it_seq = _solid_sequences.begin();
            it_seq != _solid_sequences.end();
            ++it_seq
            )
        {
            if(! it_seq->range().contains(row))
            {
                continue;
            }

            // obtaining a string representation of the sequence
            // to easy loop inside it's elements
            str_seq_rep = it_seq->sequence().toStringOfItems();
            str_seq_size = it_seq->sequence().size();

            // if(str_seq_size < 2)
            // {
            //     std::stringstream msg;
            //     msg << "Current support count function doesn't handle"
            //         << " sequence with less then two items." << std::endl
            //         << "Current sequence: '" << str_seq_rep << "'.";
            //     throw std::runtime_error(msg.str());
            // }

            if(m_ranged_sequence_positions.find(& (*it_seq))
               == m_ranged_sequence_positions.end())
            {
                m_ranged_sequence_positions[& (*it_seq)] =
                    ListPositions();
            }

            str_it = str_seq_rep.begin();

            col = 0;
            match_started = false;
            //last_match_col = 0; // [CMP] useful only for max-gap

            // passing through every item of the row
            while(col < tot_cols)
            {
                // check the max_time window constrant
                // this check must be an early check
                if(
                    // [CMP] this is useful only for max-gap
                    //last_match_col != 0 &&
                    //(col - last_match_col) > (m_max_gap + 1)

                    // [CMP] imagine a sequence of length 5
                    // max_time_window < 5 make no sense
                    match_started &&
                    // [CMP]
                    // (col - start_match_col) >= (
                    //     m_max_time_window + str_seq_size)
                    (col - start_match_col >= str_seq_size)
                    )
                {
                    // rewind to start_match_col + 1
                    col = start_match_col + 1;
                    match_started = false;
                    //last_match_col = 0; // [CMP] useful only for max-gap
                    str_it = str_seq_rep.begin();
                    continue;
                }

                Item & item = m_database[row][col];

                // if match, go to next item of the sequence
                if(item == *str_it)
                {
                    // take note for the start of the match
                    if(! match_started)
                    {
                        start_match_col = col;
                        match_started = true;
                    }

                    //last_match_col = col; // [CMP] useful only for max-gap
                    ++str_it;

                    // if the sequence iterator reach the end,
                    // then the input data-sequence contain the sequence
                    if(str_it == str_seq_rep.end())
                    {
                        m_ranged_sequence_positions[& (*it_seq)]
                            .push_back(Position(row, col - str_seq_size + 1));

                        // rewind to start_match_col + 1
                        col = start_match_col + 1;
                        match_started = false;
                        //last_match_col = 0; // [CMP] useful only for max-gap
                        str_it = str_seq_rep.begin();
                        continue;
                    }
                }

                // in any other case
                // go to the next item of the input data-sequence
                ++col;
            }
        }
    }
}

void SIM::cleanupSolidSequencesWithSmallRangeSize(
    const Size & _min_size, ListRangedSequence & _solid_sequences) const
{
    ListRangedSequence::iterator it = _solid_sequences.begin();

    while(it != _solid_sequences.end())
    {
        if(it->range().size() < _min_size)
        {
            it = _solid_sequences.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void SIM::detectSolidSequenceBlocksFromSolidSequence(
    const RangedSequence & _solid_sequence,
    ListSequenceBlocks & _sequence_blocks) const
{
    // generate a candidate Sequence Block for each sequence match
    // in the Database
    ListPositions::const_iterator it_pos;
    const ListPositions & positions =
        // m_ranged_sequence_positions.find(& (*it_ss))->second;
        m_ranged_sequence_positions.find(& _solid_sequence)->second;

    // for each position
    for(it_pos = positions.begin(); it_pos != positions.end(); ++it_pos)
    {
        _sequence_blocks.push_back(
            SequenceBlock(
                _solid_sequence.sequence(),
                Range(it_pos->second, it_pos->second),
                Interval(it_pos->first,
                         it_pos->first + _solid_sequence.sequence().size() - 1),
                _solid_sequence.sequence().size()));
    }
}

void SIM::printSolidSequences()
{
    // print solid sequences
    m_log_stream
        << std::endl << "Printing solid sequences:" << std::endl;

    MapRangedSequencesByLength::const_iterator it_ss_by_len;
    ListRangedSequence::const_iterator it_ss;

    for(
        it_ss_by_len = m_solid_sequences.begin();
        it_ss_by_len != m_solid_sequences.end();
        ++it_ss_by_len
        )
    {
        const ListRangedSequence & sequences = it_ss_by_len->second;

        for(it_ss = sequences.begin(); it_ss != sequences.end(); ++it_ss)
        {
            m_log_stream
                << '\t' << it_ss->sequence().toStringOfItems()
                << '\t' << "len: " << it_ss->sequence().size()
                << '\t' << "sup: " << it_ss->support()
                << '\t' << "start: " << it_ss->range().start()
                << '\t' << "end: " << it_ss->range().end()
                << std::endl;
        }
    }
}

void SIM::saveJSON(const std::string & _output_filename) const
{
    std::ofstream output_file;
    output_file.open(_output_filename.c_str());
    cxxtools::TextOStream ts(output_file, new cxxtools::Utf8Codec());

    cxxtools::JsonFormatter formatter(ts);
    formatter.beautify(true);

    MapRangedSequencesByLength::const_iterator it_ss_by_len;
    ListRangedSequence::const_iterator it_ss;
    MapPositionsBySeq::const_iterator it_list_pos;
    ListPositions::const_iterator it_pos;

    formatter.beginArray("", "");

    // for each length
    for(
        it_ss_by_len = m_solid_sequences.begin();
        it_ss_by_len != m_solid_sequences.end();
        ++it_ss_by_len
        )
    {
        const Size & size = it_ss_by_len->first;
        const ListRangedSequence & sequences = it_ss_by_len->second;

        formatter.beginObject("", "");
        formatter.addValueInt("length", "", size);
        formatter.beginArray("sequences", "");

        // for each sequence of that length
        for(it_ss = sequences.begin(); it_ss != sequences.end(); ++it_ss)
        {
            formatter.beginObject("", "");

            formatter.addValueStdString(
                "sequence", "", it_ss->sequence()
                .toStringOfItems());
            formatter.addValueInt(
                "support", "", it_ss->support());
            formatter.addValueInt(
                "start", "", it_ss->range().start());
            formatter.addValueInt(
                "end", "", it_ss->range().end());

            it_list_pos = m_ranged_sequence_positions.find(& (*it_ss));

            if(it_list_pos == m_ranged_sequence_positions.end())
            {
                std::stringstream msg;
                msg << "Can not find positions for ranged sequence '"
                    << it_ss->sequence().toStringOfItems() << '('
                    << it_ss->range().start() << ','
                    << it_ss->range().end() << ")'.";
                throw std::runtime_error(msg.str());
            }

            const ListPositions & positions = it_list_pos->second;

            if(positions.size() == 0)
            {
                std::stringstream msg;
                msg << "We find 0 positions for ranged sequence '"
                    << it_ss->sequence().toStringOfItems() << '('
                    << it_ss->range().start() << ','
                    << it_ss->range().end() << ")': this is a bug.";
                throw std::runtime_error(msg.str());
            }

            // the follow strange way to structure the data
            // is needed by R language
            {
                formatter.beginArray("times", "");

                for(
                    it_pos = positions.begin();
                    it_pos != positions.end();
                    ++it_pos
                    )
                {
                    formatter.addValueInt(
                        "", "", it_pos->first);
                }

                formatter.finishArray();

                formatter.beginArray("spaces", "");

                for(
                    it_pos = positions.begin();
                    it_pos != positions.end();
                    ++it_pos
                    )
                {
                    formatter.addValueInt(
                        "", "", it_pos->second);
                }

                formatter.finishArray();
            }

            formatter.finishObject();
        }

        formatter.finishArray();
        formatter.finishObject();
    }

    formatter.finishArray();
}
