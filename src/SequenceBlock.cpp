#include "SequenceBlock.h"

SequenceBlock::SequenceBlock(
    const Sequence & _sequence,
    const Range & _range,
    const Interval & _interval,
    const Support & _support):
    m_sequence(_sequence),
    m_range(_range),
    m_interval(_interval),
    m_support(_support)
{
}

const Sequence & SequenceBlock::sequence() const
{
    return m_sequence;
}

const Range & SequenceBlock::range() const
{
    return m_range;
}

const Interval & SequenceBlock::interval() const
{
    return m_interval;
}

const Support & SequenceBlock::support() const
{
    return m_support;
}

Frequency SequenceBlock::frequency() const
{
    return float(m_support) / (m_range.size() * m_interval.size());
}
