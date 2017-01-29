#include "RangedSequence.h"

RangedSequence::RangedSequence(
    const Sequence & _sequence,
    const Range & _range,
    const Support & _support):
    m_sequence(_sequence),
    m_range(_range),
    m_support(_support)
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

const Support & RangedSequence::support() const
{
    return m_support;
}

Frequency RangedSequence::frequency() const
{
    return float(m_support) / m_range.size();
}
