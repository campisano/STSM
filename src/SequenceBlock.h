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

#ifndef SEQUENCE_BLOCK__H__
#define SEQUENCE_BLOCK__H__

#include <list>
#include <map>

#include "Frequency.h"
#include "Interval.h"
#include "Range.h"
#include "Sequence.h"
#include "Size.h"
#include "Support.h"

class SequenceBlock;
typedef std::list < SequenceBlock > ListSequenceBlocks;
typedef std::map < Size, ListSequenceBlocks > MapSequenceBlocksByLength;

class SequenceBlock
{
public:
    explicit SequenceBlock(
        const Sequence & _sequence,
        const Range & _range,
        const Interval & _interval,
        const Support & _support);

    bool operator==(const SequenceBlock & _other) const;
    bool operator!=(const SequenceBlock & _other) const;

    const Sequence & sequence() const;
    const Range & range() const;
    const Interval & interval() const;

    Support support() const;
    Frequency frequency() const;
    Size area() const;

    bool contains(const SequenceBlock & _other) const;
    bool contains(
        const Range & _other_range,
        const Interval & _other_interval) const;

    bool hasSameCoordinates(const SequenceBlock & _other) const;

    struct LessThanComparer
    {
        bool operator() (
            const ListSequenceBlocks::iterator & _left,
            const ListSequenceBlocks::iterator & _right) const;
    };

    struct PositionComparer
    {
        explicit PositionComparer(size_t _x_start, size_t _y_start);

        bool operator() (
            const SequenceBlock & _left,
            const SequenceBlock & _right) const;

        size_t m_x_start;
        size_t m_y_start;
    };

private:
    Sequence m_sequence;
    Range m_range;
    Interval m_interval;
    Support m_support;
};

#endif
