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

#ifndef SEGMENT__H__
#define SEGMENT__H__

#include "Point.h"
#include "Size.h"

class Segment
{
public:
    explicit Segment(
        const Point & _start,
        const Point & _end);

    explicit Segment(const Segment & _segment);

    bool operator==(const Segment & _other) const;
    bool operator!=(const Segment & _other) const;

    const Point & start() const;
    void start(const Point & _pos);

    const Point & end() const;
    void end(const Point & _end);

    bool contains(const Point & _point) const;
    bool contains(const Segment & _other) const;

    Size size() const;

    bool intersects(
        const Segment & _segment,
        Segment & _resulting_segment) const;

    void unify(
        const Segment & _segment,
        Segment & _resulting_segment) const;

protected:
    Point m_start;
    Point m_end;
};

#endif
