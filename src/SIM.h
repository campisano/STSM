#ifndef SIM__H__
#define SIM__H__

#include <fstream>
#include <string>

#include "Candidate.h"
#include "Database.h"
#include "Frequency.h"
#include "Item.h"
#include "Point.h"
#include "RangedSequence.h"
#include <list>
#include <map>
#include <utility>

class SIM
{
public:
    explicit SIM();
    virtual ~SIM();

    void run(
        const std::string & _input_filename,
        const std::string & _log_filename,
        const Frequency & _min_spatial_freq,
        const Frequency & _min_block_freq);

    void saveJSON(const std::string & _output_filename) const;

protected:
    void setMinSpatialFreq(const Frequency & _min_spatial_freq);
    void setMinBlockFreq(const Frequency & _min_block_freq);

    void loadDatabase(const std::string & _input_filename);

    void generateTheSetOfAllDatabaseItems(
        const Database & _database,
        SetItems & _items) const;

    void generate1SizeCandidates(
        const SetItems & _items,
        const Point & _start,
        const Point & _end,
        ListCandidates & _candidates) const;

    void generateCandidates(
        const ListRangedSequence & _solid_sequences,
        ListCandidates & _candidates) const;

    void updateMatchingPositions(const ListRangedSequence & _solid_sequences);

    void printSS();
    void printSSP();

private:
    std::ofstream m_log_stream;

    Frequency m_min_spatial_freq;
    Frequency m_min_block_freq;

    Database m_database;

protected:
    ListListRangedSequence m_solid_sequences;

    typedef std::pair < Point, Point > Position;    // sensor, time
    typedef std::list < Position > ListPositions;   // matched positions
    typedef std::map <
        const RangedSequence *, ListPositions
        > MapPositionsBySeq;                        // positions by sequence
    typedef std::map <
        const Size, MapPositionsBySeq
        > MapSeqPositionsByLenght;                  // seq. positions by length

    MapSeqPositionsByLenght m_ranged_sequence_positions;
};

#endif
