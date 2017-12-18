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

#ifndef STSM__H__
#define STSM__H__

#include <fstream>
#include <list>
#include <map>
#include <string>
#include <utility>

#include "Candidate.h"
#include "Database.h"
#include "Frequency.h"
#include "Item.h"
#include "Point.h"
#include "Position.h"
#include "RangedSequence.h"
#include "SequenceBlock.h"

class STSM
{
public:
    explicit STSM();
    virtual ~STSM();

    void run(
        const Database & _database,
        const std::string & _log_filename,
        const unsigned int & _min_spatial_freq_perc,
        const unsigned int & _min_block_freq_perc);

    void saveJSON(const std::string & _output_filename);

protected:
    void setMinSpatialFreq(const Frequency & _min_spatial_freq);
    void setMinBlockFreq(const Frequency & _min_block_freq);

    void generateTheSetOfAllDatabaseItems(
        const Database & _database,
        SetItems & _items);

    void generate1SizeCandidates(
        const SetItems & _items,
        const Point & _start,
        const Point & _end,
        ListCandidates & _candidates);

    void updateKernelsOfAllCandidates(
        const Database & _database,
        ListCandidates & _candidates);
    void mergeKernelsOfAllCandidates(
        ListCandidates & _candidates,
        ListRangedSequence & _solid_sequences_k);

    void updateMatchingPositions(
        const Database & _database,
        const ListRangedSequence & _solid_sequences);

    void generateCandidates(
        const ListRangedSequence & _solid_sequences,
        ListCandidates & _candidates);

    void cleanupSolidSequencesWithSmallRangeSize(
        const Size & _min_size, ListRangedSequence & _solid_sequences);

    void detectBlocksOfAllSolidSequences();

    void detectSolidSequenceBlocksFromSolidSequence(
        const RangedSequence & _solid_sequence,
        const Frequency & _min_block_freq,
        ListSequenceBlocks & _sequence_blocks);

    void generate1SizeBlockCandidatesForEachSequenceOccurrence(
        const ListPositions & _positions,
        const RangedSequence & _solid_sequence,
        ListSequenceBlocks & _sb_candidates) const;

    void printSolidSequences();
    void printSolidBlocks();

private:
    std::ofstream m_log_stream;

    Frequency m_min_spatial_freq;
    Frequency m_min_block_freq;

protected:
    MapRangedSequencesByLength m_solid_sequences;
    typedef std::map <
        const RangedSequence *, ListPositions
        > MapPositionsBySeq;                        // positions by sequence
    MapPositionsBySeq m_ranged_sequence_positions;

    MapSequenceBlocksByLength m_solid_sequence_blocks;
};

#endif
