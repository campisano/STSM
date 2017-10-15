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

#include "Segment.h"

#include <algorithm>

Segment::Segment(
    const Point & _start,
    const Point & _end):
    m_start(_start),
    m_end(_end)
{
}

Segment::Segment(const Segment & _segment):
    m_start(_segment.start()),
    m_end(_segment.end())
{
}

bool Segment::operator==(const Segment & _other) const
{
    return m_start == _other.m_start && m_end == _other.m_end;
}

bool Segment::operator!=(const Segment & _other) const
{
    return ! ((*this) == _other);
}

const Point & Segment::start() const
{
    return m_start;
}

void Segment::start(const Point & _start)
{
    m_start = _start;
}

const Point & Segment::end() const
{
    return m_end;
}

void Segment::end(const Point & _end)
{
    m_end = _end;
}

bool Segment::contains(const Point & _point) const
{
    return _point >= m_start && _point <= m_end;
}

bool Segment::contains(const Segment & _other) const
{
    return (
        m_start <= _other.m_start &&
        m_end >= _other.m_end
        );
}

Size Segment::size() const
{
    return m_end - m_start + 1;
}

bool Segment::intersects(
    const Segment & _segment,
    Segment & _resulting_segment) const
{
    Point max_start = std::max(m_start, _segment.m_start);
    Point min_end = std::min(m_end, _segment.m_end);

    if(max_start > min_end)
    {
        return false;
    }

    _resulting_segment.m_start = max_start;
    _resulting_segment.m_end = min_end;

    return true;
}

void Segment::unify(
    const Segment & _segment,
    Segment & _resulting_segment) const
{
    _resulting_segment.m_start = std::min(m_start, _segment.m_start);
    _resulting_segment.m_end = std::max(m_end, _segment.m_end);
}
