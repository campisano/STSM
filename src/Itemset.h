#ifndef ITEMSET__H__
#define ITEMSET__H__

#include <string>
#include <vector>

#include "Item.h"

class Itemset
{
    public:
        explicit Itemset();
        explicit Itemset(std::string _string_representation);
        virtual ~Itemset();

        void set(std::string _string_representation);
        bool operator==(const Itemset &other);
        Item & getFirst();
        Item & getLast();
        void getCopyExceptPos(unsigned int i, Itemset & _copy_itemset);
        std::string toString();
        std::string toStringOfItems();

        inline void clear()
        {
            this->m_items.clear();
        }

        inline unsigned int size()
        {
            return this->m_items.size();
        }

        inline void append(Item _item)
        {
            this->m_items.push_back(_item);
        }

        inline std::vector<Item>::iterator begin()
        {
            return this->m_items.begin();
        }

        inline std::vector<Item>::iterator end()
        {
            return this->m_items.end();
        }

    private:
        std::vector<Item> m_items;
};

#endif
