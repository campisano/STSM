#ifndef GSP__H__
#define GSP__H__

#include <string>
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
            unsigned int _minimum_support,
            unsigned int _max_gap);

    protected:
        void load(std::string &_input_filename);
        void detectFrequentItems(std::vector<Item>& _frequent_items);
        void join(
            std::vector<Sequence> &_candidates,
            unsigned int _seq_items,
            std::vector<Sequence> &_new_candidates);
        void prune(std::vector<Sequence> &_new_candidates);
        unsigned int getSupport(Sequence & _sequence);
        void print(std::vector<Sequence> &_sequences);

    private:
        unsigned int m_minimum_support;
        unsigned int m_max_gap;
        std::vector< std::vector<Item> > m_input_dataset;
};

#endif
