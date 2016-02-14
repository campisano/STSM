#include "Itemset.h"

#include <cxxtools/regex.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

Itemset::Itemset()
{
}

Itemset::Itemset(std::string _string_representation)
{
    this->clear();
    this->set(_string_representation);
}

Itemset::~Itemset()
{
}

void Itemset::set(std::string _string_representation)
{
    cxxtools::Regex seq_regex("^(\\([a-zA-Z]+\\))$");

    if(! seq_regex.match(_string_representation))
    {
        std::stringstream msg;
        msg << "Invalid itemset string representation: '"
            << _string_representation << "'.";
        std::runtime_error(msg.str());
    }

    cxxtools::Regex itemset_regex("([a-zA-Z])");
    cxxtools::RegexSMatch itemsets;
    std::string substr = _string_representation.substr(1);

    while(itemset_regex.match(substr, itemsets))
    {
        Item is = itemsets.get(1).c_str()[0];
        substr = substr.substr(itemsets.offsetEnd(1));
        this->append(is);
    }

    // this can not happen
    if(this->m_items.size() == 0)
    {
        throw std::runtime_error("Itemset with 0 Items detected!");
    }
}

bool Itemset::operator==(const Itemset &other)
{
    if(this->m_items.size() != other.m_items.size())
    {
        return false;
    }

    unsigned int size = this->m_items.size();

    for(unsigned int i=0; i < size; ++i)
    {
        if(this->m_items[i] != other.m_items[i])
        {
            return false;
        }
    }

    return true;
}

Item & Itemset::getFirst()
{
    if(this->m_items.size() < 1)
    {
        throw std::out_of_range("Itemset size must be at least 1.");
    }

    return this->m_items[0];
}

Item & Itemset::getLast()
{
    unsigned int size = this->m_items.size();

    if(size < 1)
    {
        throw std::out_of_range("Itemset size must be at least 1.");
    }

    return this->m_items[size - 1];
}

void Itemset::getCopyExceptPos(unsigned int i, Itemset & _copy_itemset)
{
    unsigned int size = this->m_items.size();

    if(i > (size - 1))
    {
        std::stringstream message;
        message << "Itemset position cannot be greater then"
            << " it size - 1\n size: " << size << ", i: " << i << '.';
        throw std::out_of_range(message.str());
    }

    for(unsigned int j = 0; j < size; ++j)
    {
        if(j != i)
        {
            _copy_itemset.append(this->m_items[j]);
        }
    }
}

std::string Itemset::toString()
{
    std::stringstream output;

    std::vector<Item>::iterator it;

    output << '(';

    for(it = this->m_items.begin(); it != this->m_items.end(); ++it)
    {
        output << (*it);
    }

    output << ')';

    return output.str();
}
