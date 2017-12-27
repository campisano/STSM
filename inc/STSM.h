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

#include <list>
#include <ostream>
#include <string>

#include "BlockedSequence.h"
#include "Candidate.h"
#include "Database.h"
#include "Frequency.h"
#include "Item.h"
#include "Patterns.h"
#include "Point.h"
#include "Position.h"
#include "RangedSequence.h"

class STSM
{
public:
    explicit STSM();
    virtual ~STSM();

    void run(
        const Database & _database,
        const unsigned int & _min_spatial_freq_perc,
        const unsigned int & _min_block_freq_perc,
        std::ostream & _log_stream);

    void run(
        const Database & _database,
        const unsigned int & _min_spatial_freq_perc,
        const unsigned int & _min_block_freq_perc);

    const Patterns & getPatterns() const;

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
        ListRangedSequence & _solid_ranged_sequences_k);

    void updateMatchingPositions(
        const Database & _database,
        const ListRangedSequence & _solid_ranged_sequences);

    void generateCandidates(
        const ListRangedSequence & _solid_ranged_sequences,
        ListCandidates & _candidates);

    void cleanupSolidRangedSequencesWithSmallRangeSize(
        const Size & _min_size, ListRangedSequence & _solid_ranged_sequences);

    void detectBlocksOfAllSolidRangedSequences();

    void detectSolidBlockedSequencesFromSolidRangedSequence(
        const RangedSequence & _solid_ranged_sequence,
        const Frequency & _min_block_freq,
        ListBlockedSequences & _blocked_sequences);

    void generate1SizeBlockCandidatesForEachSequenceOccurrence(
        const ListPositions & _positions,
        const RangedSequence & _solid_ranged_sequence,
        ListBlockedSequences & _sb_candidates) const;

    void printSolidRangedSequences();
    void printSolidBlockedSequences();

protected:
    Patterns m_patterns;                // resulting patterns

private:
    std::ostream * m_log_stream;

    Frequency m_min_spatial_freq;
    Frequency m_min_block_freq;
};

#endif
