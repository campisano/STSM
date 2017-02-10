#include "SequenceBlock.h"

SequenceBlock::SequenceBlock(
    const Sequence & _sequence,
    const Range & _range,
    const Interval & _interval,
    const SetPositions & _positions):
    m_sequence(_sequence),
    m_range(_range),
    m_interval(_interval),
    m_positions(_positions)
{
}

bool SequenceBlock::operator==(const SequenceBlock & _other) const
{
    return (
        m_range == _other.m_range &&
        m_interval == _other.m_interval &&
        m_sequence == _other.m_sequence);
}

bool SequenceBlock::operator!=(const SequenceBlock & _other) const
{
    return ! ((*this) == _other);
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

Support SequenceBlock::support() const
{
    return m_positions.size() * m_sequence.size();
}

Frequency SequenceBlock::frequency() const
{
    return float(support()) / (m_range.size() * m_interval.size());
}

const SetPositions & SequenceBlock::positions() const
{
    return m_positions;
}
