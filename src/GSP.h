#ifndef GSP__H__
#define GSP__H__

#include <fstream>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "Item.h"
#include "Sequence.h"

class GSP
{
    public:
        explicit GSP();
        virtual ~GSP();

        void run(
            std::string _input_filename,
            std::string _output_filename,
            std::string _log_filename,
            unsigned int _minimum_support,
            unsigned int _max_gap);

    protected:
        void setMinimumSupport(unsigned int _minimum_support);
        void setMaxGap(unsigned int _max_gap);
        void load(std::string &_input_filename);
        void detectFrequentItems(std::vector<Item>& _frequent_items);
        void join(
            std::vector<Sequence> &_candidates,
            unsigned int _seq_items,
            std::vector<Sequence> &_new_candidates
        );
        Sequence joinSubsequences(Sequence& _seq1, Sequence& _seq2);
        void prune(
            unsigned int _seq_items,
            std::vector<Sequence> &_new_candidates
        );
        void updateSupportCountPositions(
            std::vector<Sequence> &_new_candidates,
            unsigned int _seq_items
        );
        void print(std::vector<Sequence> &_sequences);
        void save(std::string &_output_filename);

    private:
        unsigned int m_minimum_support;
        unsigned int m_max_gap;
        std::vector< std::vector<Item> > m_input_dataset;
        std::map< unsigned int,    // mapping sequences per length (seq items)
            std::map< std::string, // mapping results per sequence (toString())
                std::pair < unsigned int,   // support count
                    std::list <             // list of positions
                        std::pair< unsigned int, unsigned int > // sensor, time
                    >
                >
            >
        > m_supported_sequences_positions;

        std::ofstream m_log_stream;
};

#endif
