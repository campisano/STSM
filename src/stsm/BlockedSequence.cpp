// Copyright (C) 2017 Riccardo Campisano <riccardo.campisano@gmail.com>
//               2017 Fabio Porto
//               2017 Fabio Perosi
//               2017 Esther Pacitti
//               2017 Florent Masseglia
//               2017 Eduardo Ogasawara
//
// This file is part of STSM (Spatio-Temporal Sequence Miner).
//
// STSM is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// STSM is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with STSM.  If not, see <http://www.gnu.org/licenses/>.

#include "BlockedSequence.h"

BlockedSequence::BlockedSequence(
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

bool BlockedSequence::operator==(const BlockedSequence & _other) const
{
    return (
               m_range == _other.m_range &&
               m_interval == _other.m_interval &&
               m_sequence == _other.m_sequence);
}

bool BlockedSequence::operator!=(const BlockedSequence & _other) const
{
    return !((*this) == _other);
}

const Sequence & BlockedSequence::sequence() const
{
    return m_sequence;
}

const Range & BlockedSequence::range() const
{
    return m_range;
}

const Interval & BlockedSequence::interval() const
{
    return m_interval;
}

Support BlockedSequence::support() const
{
    return m_support;
}

Frequency BlockedSequence::frequency() const
{
    return float(m_support) / (m_range.size() * m_interval.size());
}

Size BlockedSequence::area() const
{
    return m_range.size() * m_interval.size();
}

bool BlockedSequence::contains(const BlockedSequence & _other) const
{
    return (
               m_range.contains(_other.m_range) &&
               m_interval.contains(_other.m_interval));
}

bool BlockedSequence::contains(
    const Range & _other_range,
    const Interval & _other_interval) const
{
    return (
               m_range.contains(_other_range) &&
               m_interval.contains(_other_interval));
}

bool BlockedSequence::hasSameCoordinates(const BlockedSequence & _other) const
{
    return (
               m_range == _other.m_range &&
               m_interval == _other.m_interval);
}

bool BlockedSequence::LessThanComparer::operator()(
    const ListBlockedSequences::iterator & _left,
    const ListBlockedSequences::iterator & _right) const
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


BlockedSequence::PositionComparer::PositionComparer(
    size_t _x_start, size_t _y_start):
    m_x_start(_x_start), m_y_start(_y_start)
{
}

bool BlockedSequence::PositionComparer::operator()(
    const BlockedSequence & _left,
    const BlockedSequence & _right) const
{
    size_t left_distance =
        - m_x_start - m_y_start
        + _left.m_range.start() + _left.m_interval.start();
    size_t right_distance =
        - m_x_start - m_y_start
        + _right.m_range.start() + _right.m_interval.start();

    return left_distance < right_distance;
}
