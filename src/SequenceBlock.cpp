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
    return m_support;
}

Frequency SequenceBlock::frequency() const
{
    return float(m_support) / (m_range.size() * m_interval.size());
}

Size SequenceBlock::area() const
{
    return m_range.size() * m_interval.size();
}

bool SequenceBlock::contains(const SequenceBlock & _other) const
{
    return (
        m_range.contains(_other.m_range) &&
        m_interval.contains(_other.m_interval));
}

bool SequenceBlock::contains(
    const Range & _other_range,
    const Interval & _other_interval) const
{
    return (
        m_range.contains(_other_range) &&
        m_interval.contains(_other_interval));
}

bool SequenceBlock::hasSameCoordinates(const SequenceBlock & _other) const
{
    return (
        m_range == _other.m_range &&
        m_interval == _other.m_interval);
}

bool SequenceBlock::LessThanComparer::operator() (
    const ListSequenceBlocks::iterator & _left,
    const ListSequenceBlocks::iterator & _right) const
{
    if(_left->m_range.start() == _right->m_range.start())
    {
        if(_left->m_interval.start() == _right->m_interval.start())
        {
            if(_left->m_range.end() == _right->m_range.end())
            {
                return (
                    _left->m_interval.end()
                    < _right->m_interval.end());
            }
            else
            {
                return (
                    _left->m_range.end()
                    < _right->m_range.end());
            }
        }
        else
        {
            return (
                _left->m_interval.start()
                < _right->m_interval.start());
        }
    }
    else
    {
        return _left->m_range.start() < _right->m_range.start();
    }
}


SequenceBlock::PositionComparer::PositionComparer(
    size_t _x_start, size_t _y_start):
    m_x_start(_x_start), m_y_start(_y_start)
{
}

bool SequenceBlock::PositionComparer::operator() (
    const SequenceBlock & _left,
    const SequenceBlock & _right) const
{
    size_t left_distance =
        - m_x_start - m_y_start
        + _left.m_range.start() + _left.m_interval.start();
    size_t right_distance =
        - m_x_start - m_y_start
        + _right.m_range.start() + _right.m_interval.start();

    return left_distance < right_distance;
}
