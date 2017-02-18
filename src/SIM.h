#ifndef SIM__H__
#define SIM__H__

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

class SIM
{
public:
    explicit SIM();
    virtual ~SIM();

    void clear();

    void run(
        const std::string & _input_filename,
        const std::string & _log_filename,
        const unsigned int & _min_spatial_freq_perc,
        const unsigned int & _min_block_freq_perc);

    void saveJSON(const std::string & _output_filename) const;

protected:
    void setMinSpatialFreq(const Frequency & _min_spatial_freq);
    void setMinBlockFreq(const Frequency & _min_block_freq);

    void loadDatabase(const std::string & _input_filename);

    void generateTheSetOfAllDatabaseItems(
        const Database & _database,
        SetItems & _items);

    void generate1SizeCandidates(
        const SetItems & _items,
        const Point & _start,
        const Point & _end,
        ListCandidates & _candidates);

    void updateKernelsOfAllCandidates(ListCandidates & _candidates);
    void mergeKernelsOfAllCandidates(
        ListCandidates & _candidates,
        ListRangedSequence & _solid_sequences_k);

    void updateMatchingPositions(const ListRangedSequence & _solid_sequences);

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

    Database m_database;

protected:
    MapRangedSequencesByLength m_solid_sequences;
    typedef std::map <
        const RangedSequence *, ListPositions
        > MapPositionsBySeq;                        // positions by sequence
    MapPositionsBySeq m_ranged_sequence_positions;

    MapSequenceBlocksByLength m_solid_sequence_blocks;
};

#endif
