#include "Sequence.h"

#include <algorithm>
#include <cxxtools/regex.h>
#include <sstream>
#include <stdexcept>
#include <string>

Sequence::Sequence()
{
}

Sequence::Sequence(std::string _string_representation)
{
    this->clear();
    this->set(_string_representation);
}

Sequence::~Sequence()
{
}

bool Sequence::operator==(const Sequence &other)
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

void Sequence::set(std::string _string_representation)
{
    cxxtools::Regex seq_regex("^<[a-zA-Z]+>$");

    if(! seq_regex.match(_string_representation))
    {
        std::stringstream msg;
        msg << "Invalid sequence string representation: '"
            << _string_representation << "'.";
        std::runtime_error(msg.str());
    }

    cxxtools::Regex items_regex("([a-zA-Z])");
    cxxtools::RegexSMatch items;
    std::string substr = _string_representation.substr(1);

    while(items_regex.match(substr, items))
    {
        Item is = items.get(1).c_str()[0];
        substr = substr.substr(items.offsetEnd(1));
        this->append(is);
    }

    // this can not happen
    if(this->m_items.size() == 0)
    {
        throw std::runtime_error("Sequence with 0 Items detected!");
    }
}

Item & Sequence::getFirst()
{
    if(this->m_items.size() < 1)
    {
        throw std::out_of_range("Sequence size must be at least 1.");
    }

    return this->m_items[0];
}

Item & Sequence::getLast()
{
    unsigned int size = this->m_items.size();

    if(size < 1)
    {
        throw std::out_of_range("Sequence size must be at least 1.");
    }

    return this->m_items[size - 1];
}

void Sequence::getCopyExceptPos(unsigned int i, Sequence & _copy_seq)
{
    unsigned int size = this->m_items.size();

    if(i > (size - 1))
    {
        std::stringstream message;
        message << "Sequence position cannot be greater then"
            << " it size - 1\n size: " << size << ", i: " << i << '.';
        throw std::out_of_range(message.str());
    }

    _copy_seq.clear();

    for(unsigned int j = 0; j < size; ++j)
    {
        if(j != i)
        {
            _copy_seq.append(this->m_items[j]);
        }
    }
}

void Sequence::getSubsequenceDroppingFirstItem(Sequence &_subseq)
{
    // Sequence size must be at least 1
    if(this->m_items.size() < 1)
    {
        throw std::runtime_error("Sequence with size < 1 detected!");
    }

    // copy all items except the first
    this->getCopyExceptPos(0, _subseq);
}

void Sequence::getSubsequenceDroppingLastItem(Sequence &_subseq)
{
    // Sequence size must be at least 2
    if(this->m_items.size() < 2)
    {
        throw std::runtime_error("Sequence with size < 2 detected!");
    }

    // copy all items except the last
    this->getCopyExceptPos(this->m_items.size() - 1, _subseq);
}

std::string Sequence::toString()
{
    std::stringstream output;
    output << '<' << this->toStringOfItems() << '>';

    return output.str();
}

std::string Sequence::toStringOfItems()
{
    return std::string(this->m_items.begin(), this->m_items.end());
}
