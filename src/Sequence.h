#ifndef SEQUENCE__H__
#define SEQUENCE__H__

#include <string>
#include <list>
#include <vector>

#include "Item.h"

class Sequence
{
    public:
        explicit Sequence();
        explicit Sequence(std::string _string_representation);
        virtual ~Sequence();

        bool operator==(const Sequence &other);

        void set(std::string _string_representation);

        Item & getFirst();
        Item & getLast();

        void getCopyExceptPos(unsigned int i, Sequence & _copy_seq);
        void getSubsequenceDroppingFirstItem(Sequence &_subseq);
        void getSubsequenceDroppingLastItem(Sequence &_subseq);

        std::string toString();
        std::string toStringOfItems();

        inline void clear()
        {
            this->m_items.clear();
        }

        inline void append(Item _item)
        {
            this->m_items.push_back(_item);
        }

    private:
        std::vector<Item> m_items;
};

#endif
