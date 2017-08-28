#include "Sequence.h"

#include <algorithm>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>

Sequence::Sequence()
{
}

Sequence::Sequence(const Item & _item)
{
    clear();
    append(_item);
}

Sequence::Sequence(const std::string & _string_representation)
{
    clear();
    set(_string_representation);
}

Sequence::~Sequence()
{
}

bool Sequence::operator==(const Sequence & _other) const
{
    if(m_items.size() != _other.m_items.size())
    {
        return false;
    }

    Size size = m_items.size();

    for(Size i=0; i < size; ++i)
    {
        if(m_items[i] != _other.m_items[i])
        {
            return false;
        }
    }

    return true;
}

bool Sequence::operator!=(const Sequence & _other) const
{
    return ! ((*this) == _other);
}

void Sequence::set(const std::string & _string_representation)
{
    std::regex seq_regex("^<[a-zA-Z]+>$");

    if(! std::regex_match(_string_representation, seq_regex))
    {
        std::stringstream msg;
        msg << "Invalid sequence string representation: '"
            << _string_representation << "'.";
        std::runtime_error(msg.str());
    }

    std::regex items_regex("([a-zA-Z])");
    std::smatch items;
    std::string substr = _string_representation;

    while(std::regex_search(substr, items, items_regex))
    {
        Item is = items[0].str().c_str()[0];
        substr = items.suffix();
        append(is);
    }

    // this can not happen
    if(m_items.size() == 0)
    {
        throw std::runtime_error("Sequence with 0 Items detected!");
    }
}

Size Sequence::size() const
{
    return m_items.size();
}

void Sequence::clear()
{
    m_items.clear();
}

void Sequence::append(const Item & _item)
{
    m_items.push_back(_item);
}

const Item & Sequence::getFirst() const
{
    if(m_items.size() < 1)
    {
        throw std::out_of_range("Sequence size must be at least 1.");
    }

    return m_items[0];
}

const Item & Sequence::getLast() const
{
    Size size = m_items.size();

    if(size < 1)
    {
        throw std::out_of_range("Sequence size must be at least 1.");
    }

    return m_items[size - 1];
}

void Sequence::getCopyExceptPos(const Point & _pos, Sequence & _copy_seq) const
{
    Size size = m_items.size();

    if(_pos > (size - 1))
    {
        std::stringstream message;
        message << "Sequence position cannot be greater then"
                << " it size - 1" << std::endl
                << "size: " << size << ", i: " << _pos << '.';
        throw std::out_of_range(message.str());
    }

    if(_copy_seq.size() > 0)
    {
        std::stringstream msg;
        msg << "getCopyExceptPos expects"
            << " an empty sequence as input."
            << std::endl;
        throw std::runtime_error(msg.str());
    }

    for(Size j = 0; j < size; ++j)
    {
        if(j != _pos)
        {
            _copy_seq.append(m_items[j]);
        }
    }
}

void Sequence::getSubsequenceDroppingFirstItem(Sequence & _subseq) const
{
    // Sequence size must be at least 1
    if(m_items.size() == 0)
    {
        throw std::runtime_error("There are no items to drop in the sequence.");
    }

    // copy all items except the first
    getCopyExceptPos(0, _subseq);
}

void Sequence::getSubsequenceDroppingLastItem(Sequence & _subseq) const
{
    // Sequence size must be at least 1
    if(m_items.size() < 1)
    {
        throw std::runtime_error("There are no items to drop in the sequence.");
    }

    // copy all items except the last
    getCopyExceptPos(m_items.size() - 1, _subseq);
}

bool Sequence::supportedBy(const Serie & _serie) const
{
    Serie::const_iterator match = std::search(
        _serie.begin(), _serie.end(),
        m_items.begin(), m_items.end());

    return match != _serie.end();

    //TODO [CMP] check the utility (and the possibility,
    // i.e. without break any logic) to implement max_gap
    // or max_stretch constraints

    // obtaining a string representation of the sequence
    // to easy loop inside it's elements
    // std::string str_seq = toStringOfItems();
    // Size str_seq_size = str_seq.size();
    // std::string::iterator str_it = str_seq.begin();

    // Serie::iterator it;

    // for(it = _serie.begin(); it != _serie.end(); ++it)
    // {

    // }

    // return false;
}

std::string Sequence::toString() const
{
    std::stringstream output;
    output << '<' << toStringOfItems() << '>';

    return output.str();
}

std::string Sequence::toStringOfItems() const
{
    return std::string(m_items.begin(), m_items.end());
}
