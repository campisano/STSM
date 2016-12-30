#ifndef SIM__H__
#define SIM__H__

#include <fstream>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "Candidate.h"
#include "Database.h"
#include "Frequency.h"
#include "Item.h"
#include "Point.h"
#include "RangedSequence.h"
#include "Sequence.h"
#include "Size.h"

class SIM
{
public:
    explicit SIM();
    virtual ~SIM();

    void run(
        const std::string _input_filename,
        const std::string _log_filename,
        const Frequency _min_spatial_frequency,
        const Frequency _min_block_frequency);

    void saveJSON(const std::string & _output_filename) const;

protected:
    void setMinSpatialFreq(const Frequency _min_spatial_frequency);
    void setMinBlockFreq(const Frequency _min_block_frequency);

    void loadDatabase(const std::string & _input_filename);

    void generateTheSetOfAllDatabaseItems(
        const Database & _database,
        SetItems & _items) const;

    void generate1SizeCandidates(
        const SetItems & _items,
        const Point _start,
        const Point _end,
        ListCandidates & _candidates) const;

    void generateCandidates(
        const ListRangedSequence & _solid_sequences,
        ListCandidates & _candidates) const;

    void updateMatchingPositions(
        const ListRangedSequence _solid_sequences,
        Size _seq_size);

private:
    Frequency m_min_spatial_freq;
    Frequency m_min_block_freq;

    Database m_database;

    std::map<Size,    // mapping sequences per length (seq items)
             std::map<std::string, // mapping results per seq. (toString())
                      std::pair<unsigned int,   // support count
                                std::list<             // list of positions
                                    std::pair<Point, Point> // sensor, time
                                    >
                                >
                      >
             > m_supported_sequences_positions;

    std::ofstream m_log_stream;
};

#endif
