// Copyright (C) 2017 Riccardo Campisano <riccardo.campisano@gmail.com>
//               2017 Fabio Porto
//               2017 Fabio Perosi
//               2017 Esther Pacitti
//               2017 Florent Masseglia
//               2017 Eduardo Ogasawara
//
// This file is part of STSM (Spatio-Temporal Sequence Miner).
//
// STSM is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// STSM is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with STSM.  If not, see <http://www.gnu.org/licenses/>.

#include "patterns_saver.h"

#include <fstream>
#include <json.hpp>
#include <string>

void savePatterns(
    const std::string & _output_filename, const Patterns & _patterns)
{
    nlohmann::json root;

    nlohmann::json solid_ranged_sequences = nlohmann::json::array();

    MapRangedSequencesByLength::const_iterator it_ss_by_len;
    ListRangedSequence::const_iterator it_ss;
    Patterns::MapPositionsBySeq::const_iterator it_list_pos;
    ListPositions::const_iterator it_pos;

    // for each length in _patterns.m_solid_ranged_sequences
    for(
        it_ss_by_len = _patterns.m_solid_ranged_sequences.begin();
        it_ss_by_len != _patterns.m_solid_ranged_sequences.end();
        ++it_ss_by_len
    )
    {
        const Size & size = it_ss_by_len->first;
        const ListRangedSequence & lst_sequences = it_ss_by_len->second;

        nlohmann::json ss;
        ss["length"] = size;

        nlohmann::json sequences = nlohmann::json::array();

        // for each sequence of that length
        for(
            it_ss = lst_sequences.begin();
            it_ss != lst_sequences.end();
            ++it_ss
        )
        {
            nlohmann::json seq;
            seq["sequence"] =  it_ss->sequence().toStringOfItems();
            seq["support"] = it_ss->support();
            seq["start"] = it_ss->range().start();
            seq["end"] = it_ss->range().end();

            it_list_pos = _patterns.m_ranged_sequence_positions.find(
                              & (*it_ss));

            if(it_list_pos == _patterns.m_ranged_sequence_positions.end())
            {
                std::stringstream msg;
                msg << "Can not find positions for ranged sequence '"
                    << it_ss->sequence().toStringOfItems() << '('
                    << it_ss->range().start() << ','
                    << it_ss->range().end() << ")'.";
                throw std::runtime_error(msg.str());
            }

            const ListPositions & lst_positions = it_list_pos->second;

            if(lst_positions.size() == 0)
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
                nlohmann::json spaces = nlohmann::json::array();

                for(
                    it_pos = lst_positions.begin();
                    it_pos != lst_positions.end();
                    ++it_pos
                )
                {
                    spaces.push_back(it_pos->first);
                }

                seq["spaces"] = spaces;
            }
            {
                nlohmann::json times = nlohmann::json::array();

                for(
                    it_pos = lst_positions.begin();
                    it_pos != lst_positions.end();
                    ++it_pos
                )
                {
                    times.push_back(it_pos->second);
                }

                seq["times"] = times;
            }

            sequences.push_back(seq);
        }

        ss["sequences"] = sequences;

        solid_ranged_sequences.push_back(ss);
    }

    //TODO [CMP] rename solid_sequences to solid_ranged_sequences
    root["solid_sequences"] = solid_ranged_sequences;

    nlohmann::json solid_blocked_sequences = nlohmann::json::array();

    MapBlockedSequencesByLength::const_iterator it_sb_by_len;
    ListBlockedSequences::const_iterator it_sb;

    // for each length in m_solid_blocked_sequences
    for(
        it_sb_by_len = _patterns.m_solid_blocked_sequences.begin();
        it_sb_by_len != _patterns.m_solid_blocked_sequences.end();
        ++it_sb_by_len
    )
    {
        const Size & size = it_sb_by_len->first;
        const ListBlockedSequences & lst_blocks = it_sb_by_len->second;

        nlohmann::json sb;
        sb["length"] = size;

        nlohmann::json blocks = nlohmann::json::array();

        // for each sequence of that length
        for(it_sb = lst_blocks.begin(); it_sb != lst_blocks.end(); ++it_sb)
        {
            nlohmann::json blk;
            blk["sequence"] = it_sb->sequence().toStringOfItems();
            blk["support"] = it_sb->support();
            blk["r_start"] = it_sb->range().start();
            blk["r_end"] = it_sb->range().end();
            blk["i_start"] = it_sb->interval().start();
            blk["i_end"] = it_sb->interval().end();

            blocks.push_back(blk);
        }

        sb["blocks"] = blocks;

        solid_blocked_sequences.push_back(sb);
    }

    //TODO [CMP] rename solid_blocks to solid_blocked_sequences
    root["solid_blocks"] = solid_blocked_sequences;

    std::ofstream output_file(_output_filename);
    output_file << root << std::endl;
}
