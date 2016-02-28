#ifndef SEQUENCE__H__
#define SEQUENCE__H__

#include <string>
#include <vector>

#include "Itemset.h"

class Sequence
{
    public:
        explicit Sequence();
        explicit Sequence(std::string _string_representation);
        virtual ~Sequence();

        void set(std::string _string_representation);
        void getAllFirstLevelContigousSubsequences(
            std::vector<Sequence> &_cont_subseq);
        void getAllContiguosSubsequencesDroppingAnItemFromFirstItemset(
            std::vector<Sequence> &_cont_subseq);
        void getAllContiguosSubsequencesDroppingAnItemFromLastItemset(
            std::vector<Sequence> &_cont_subseq);
        void getAllContiguosSubsequencesDroppingAnItemFromAnyMiddleItemset(
            std::vector<Sequence> &_cont_subseq);
        void getSubsequenceDroppingFirstItem(Sequence &_subseq);
        void getSubsequenceDroppingLastItem(Sequence &_subseq);
        bool operator==(const Sequence &other);
        Itemset & getFirst();
        Itemset & getLast();
        std::string toString();
        std::string toStringOfItems();
        bool hasItemsetWithSizeGreaterThenOne();

        inline void clear()
        {
            this->m_itemsets.clear();
        }

        inline unsigned int size()
        {
            return this->m_itemsets.size();
        }

        inline void append(Itemset& _itemset)
        {
            this->m_itemsets.push_back(_itemset);
        }

        inline std::vector<Itemset>::iterator begin()
        {
            return this->m_itemsets.begin();
        }

        inline std::vector<Itemset>::iterator end()
        {
            return this->m_itemsets.end();
        }

    private:
        std::vector<Itemset> m_itemsets;
};

#endif
