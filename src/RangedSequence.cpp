#include "RangedSequence.h"

#include <sstream>
#include <stdexcept>

RangedSequence::RangedSequence(
    const Sequence & _sequence,
    const Range & _range,
    const Frequency & _frequency):
    m_sequence(_sequence),
    m_range(_range),
    m_frequency(_frequency)
{
}

const Sequence & RangedSequence::sequence() const
{
    return m_sequence;
}

const Range & RangedSequence::range() const
{
    return m_range;
}

const Frequency & RangedSequence::frequency() const
{
    return m_frequency;
}
