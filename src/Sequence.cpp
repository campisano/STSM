#include "Sequence.h"

#include <cxxtools/regex.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

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

void Sequence::set(std::string _string_representation)
{
    cxxtools::Regex seq_regex("^<(\\([a-z]+\\))+>$");

    if(! seq_regex.match(_string_representation))
    {
        std::stringstream msg;
        msg << "Invalid sequence string representation: '"
            << _string_representation << "'.";
        std::runtime_error(msg.str());
    }

    cxxtools::Regex itemset_regex("(\\([a-z]+\\))");
    cxxtools::RegexSMatch itemsets;
    std::string substr = _string_representation.substr(1);

    while(itemset_regex.match(substr, itemsets))
    {
        Itemset is(itemsets.get(1));
        substr = substr.substr(itemsets.offsetEnd(1));
        this->append(is);
    }
}

bool Sequence::getAllFirstLevelContigousSubsequences(
    std::vector<Sequence> &_cont_subseq)
{
    // pag. 8
    // C is a contiguous subsequence of S if any of the following
    // conditions hold:
    //     1. C is derived from S by dropping an item from either
    //        S1 or Sn.
    {
        getAllContiguosSubsequencesDroppingAnItemFromFirstItemset(
            _cont_subseq);
        getAllContiguosSubsequencesDroppingAnItemFromLastItemset(
            _cont_subseq);
    }
    //     2. C is derived from S by dropping an item from an element
    //        Si which has at least 2 items.
    {
        getAllContiguosSubsequencesDroppingAnItemFromAnyMiddleItemset(
            _cont_subseq);
    }
    //     3. C is a contiguous subsequence of C', and C' is a
    //        contiguous subsequence of S.
    {
        //TODO [CMP] to implement,
        // out of scope of this function in this moment
    }
}

void Sequence::getAllContiguosSubsequencesDroppingAnItemFromFirstItemset(
    std::vector<Sequence> &_cont_subseq)
{
    if(
        // Sequence size must be at least 1
        this->m_itemsets.size() < 1 ||
        // If sequence has only 1 itemset, it size must be at least 2
        this->m_itemsets.size() < 2 && this->m_itemsets[0].size() < 2
    )
    {
        // no subsequences exists at all
        return;
    }

    unsigned int first_itemset_size = this->m_itemsets[0].size();

    // this can not happen
    if(first_itemset_size == 0)
    {
        throw std::runtime_error("Itemset with size 0 detected!");
    }
    // if first itemset has only one element
    // a single sequence without this itemset will be generated
    else if(first_itemset_size == 1)
    {
        Sequence seq;
        // copy skipping the first itemset
        seq.m_itemsets.assign(
            ++this->m_itemsets.begin(),
            this->m_itemsets.end());
        _cont_subseq.push_back(seq);
    }
    // if first itemset has more then one element
    // all combination of size-1 itemsets sequences will be generated
    else
    {
        // NOTE: using signed long to allow a check < 0
        for(signed long i = first_itemset_size - 1; i >= 0; --i)
        {
            Sequence seq;

            Itemset is;
            // copy all items except i
            this->m_itemsets[0].getCopyExceptPos(i, is);

            seq.m_itemsets.push_back(is);

            // append skipping the first itemset
            seq.m_itemsets.insert(
                seq.m_itemsets.begin() + 1,
                ++this->m_itemsets.begin(),
                this->m_itemsets.end());

            _cont_subseq.push_back(seq);
        }
    }
}

void Sequence::getAllContiguosSubsequencesDroppingAnItemFromLastItemset(
    std::vector<Sequence> &_cont_subseq)
{
    // Sequence size must be at least 2
    // to getAllContiguosSubsequencesDroppingAnItemFromFirstItemset
    // be different from
    // getAllContiguosSubsequencesDroppingAnItemFromLastItemset
    if(this->m_itemsets.size() < 2)
    {
        // no last subsequences exists
        return;
    }

    unsigned int last = this->m_itemsets.size() - 1;
    unsigned int last_itemset_size = this->m_itemsets[last].size();

    // this can not happen
    if(last_itemset_size == 0)
    {
        throw std::runtime_error("Itemset with size 0 detected!");
    }
    // if last itemset has only one element
    // a single sequence without this itemset will be generated
    else if(last_itemset_size == 1)
    {
        Sequence seq;

        // copy skipping the last itemset
        seq.m_itemsets.assign(
            this->m_itemsets.begin(),
            --this->m_itemsets.end());

        _cont_subseq.push_back(seq);
    }
    // if last itemset has more then one element
    // all combination of size-1 itemsets sequences will be generated
    else
    {
        // NOTE: using signed long to allow a check < 0
        for(signed long i = last_itemset_size - 1; i >= 0; --i)
        {
            Sequence seq;

            // copy skipping the last itemset
            seq.m_itemsets.assign(
                this->m_itemsets.begin(),
                --this->m_itemsets.end());

            Itemset is;
            // copy all items except i
            this->m_itemsets[last].getCopyExceptPos(i, is);
            seq.m_itemsets.push_back(is);

            _cont_subseq.push_back(seq);
        }
    }
}

void Sequence::getAllContiguosSubsequencesDroppingAnItemFromAnyMiddleItemset(
    std::vector<Sequence> &_cont_subseq)
{
    // Sequence size must be at least 3 to have middle itemsets
    if(this->m_itemsets.size() < 3)
    {
        // no middle subsequences exists
        return;
    }

    unsigned int last_middle_itemset = this->m_itemsets.size() - 1;

    for(unsigned int i = 1; i < last_middle_itemset; ++i)
    {
        unsigned int middle_itemset_size = this->m_itemsets[i].size();

        // this can not happen
        if(middle_itemset_size == 0)
        {
            throw std::runtime_error("Itemset with size 0 detected!");
        }
        // if middle itemset has only one element
        // a single sequence without this itemset will be generated
        else if(middle_itemset_size == 1)
        {
            // by definition, no middele CONTIGUOS subequence can be
            // generated removing an item inside an itemset of size 1
            continue;
        }
        // if middle itemset has more then one element
        // all combination of size-1 itemsets sequences
        // will be generated
        else
        {
            // NOTE: using signed long to allow a check < 0
            for(signed long j = middle_itemset_size - 1; j >= 0; --j)
            {
                Sequence seq;

                // copy initial itemsets
                // skipping current middle itemset
                seq.m_itemsets.insert(
                    seq.m_itemsets.end(),
                    this->m_itemsets.begin(),
                    this->m_itemsets.begin() + i);

                Itemset is;
                // copy all items except j
                this->m_itemsets[i].getCopyExceptPos(j, is);
                seq.m_itemsets.push_back(is);

                // copy final itemsets skipping current middle itemset
                seq.m_itemsets.insert(
                    seq.m_itemsets.end(),
                    this->m_itemsets.begin() + i +1,
                    this->m_itemsets.end());

                _cont_subseq.push_back(seq);
            }
        }
    }
}

void Sequence::getSubsequenceDroppingFirstItem(Sequence &_subseq)
{
    if(
        // Sequence size must be at least 1
        this->m_itemsets.size() < 1 ||
        // If sequence has only 1 itemset, it size must be at least 2
        this->m_itemsets.size() < 2 && this->m_itemsets[0].size() < 2
    )
    {
        // no subsequences exists at all
        // TODO [CMP] for all returns to do nothing,
        // check if an exception is not better
        return;
    }

    unsigned int first_itemset_size = this->m_itemsets[0].size();

    // this can not happen
    if(first_itemset_size == 0)
    {
        throw std::runtime_error("Itemset with size 0 detected!");
    }
    // if first itemset has only one element
    // a single sequence without this itemset will be generated
    else if(first_itemset_size == 1)
    {
        // copy skipping the first itemset
        _subseq.m_itemsets.assign(
            ++this->m_itemsets.begin(),
            this->m_itemsets.end());
    }
    // if first itemset has more then one element
    // a single sequence dropping the first element will be generated
    else
    {
        Itemset is;
        // copy all items except the first
        this->m_itemsets[0].getCopyExceptPos(0, is);

        _subseq.m_itemsets.push_back(is);

        // append skipping the first itemset
        _subseq.m_itemsets.insert(
            _subseq.m_itemsets.begin() + 1,
            ++this->m_itemsets.begin(),
            this->m_itemsets.end());
    }
}

void Sequence::getSubsequenceDroppingLastItem(Sequence &_subseq)
{
    // Sequence size must be at least 2
    // to getAllContiguosSubsequencesDroppingAnItemFromFirstItemset
    // be different from
    // getAllContiguosSubsequencesDroppingAnItemFromLastItemset
    if(this->m_itemsets.size() < 2)
    {
        // no last subsequences exists
        return;
    }

    unsigned int last = this->m_itemsets.size() - 1;
    unsigned int last_itemset_size = this->m_itemsets[last].size();

    // this can not happen
    if(last_itemset_size == 0)
    {
        throw std::runtime_error("Itemset with size 0 detected!");
    }
    // if last itemset has only one element
    // a single sequence without this itemset will be generated
    else if(last_itemset_size == 1)
    {
        // copy skipping the last itemset
        _subseq.m_itemsets.assign(
            this->m_itemsets.begin(),
            --this->m_itemsets.end());
    }
    // if last itemset has more then one element
    // a single sequence dropping the last element will be generated
    else
    {
        // copy skipping the last itemset
        _subseq.m_itemsets.assign(
            this->m_itemsets.begin(),
            --this->m_itemsets.end());

        Itemset is;
        // copy all items except last_itemset_size - 1
        this->m_itemsets[last].getCopyExceptPos(
            last_itemset_size - 1, is);
        _subseq.m_itemsets.push_back(is);
    }
}

bool Sequence::operator==(const Sequence &other)
{
    if(this->m_itemsets.size() != other.m_itemsets.size())
    {
        return false;
    }

    unsigned int size = this->m_itemsets.size();

    for(int i=0; i < size; ++i)
    {
        if(! (this->m_itemsets[i] == other.m_itemsets[i]))
        {
            return false;
        }
    }

    return true;
}

Itemset & Sequence::getFirst()
{
    if(this->m_itemsets.size() < 1)
    {
        throw std::out_of_range("Sequence size must be at least 1.");
    }

    return this->m_itemsets[0];
}

Itemset & Sequence::getLast()
{
    unsigned int size = this->m_itemsets.size();

    if(size < 1)
    {
        throw std::out_of_range("Sequence size must be at least 1.");
    }

    return this->m_itemsets[size - 1];
}

std::string Sequence::toString()
{
    std::stringstream output;

    std::vector<Itemset>::iterator it;

    output << '<';

    for(
        it = this->m_itemsets.begin();
        it != this->m_itemsets.end();
        ++it)
    {
        output << it->toString();
    }

    output << '>';

    return output.str();
}
