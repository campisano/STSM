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

#ifndef OCCURRENCE_MATRIX__H__
#define OCCURRENCE_MATRIX__H__

#include <string>

#include "Interval.h"
#include "Position.h"
#include "Range.h"
#include "RangedSequence.h"
#include "Support.h"

class OccurrenceMatrix
{
public:
    explicit OccurrenceMatrix(
        const RangedSequence & _ranged_sequence,
        const ListPositions & _positions);
    virtual ~OccurrenceMatrix();

    Support getSupport(
        const Range & _range,
        const Interval & _interval) const;

    std::string toString() const;

private:
    void detectBorders(const ListPositions & _positions);
    void initMatrix();
    void clearMatrix();

    bool ** m_matrix;
    size_t m_width, m_height;

    size_t m_x_start, m_y_start;
    size_t m_seq_size;
};

#endif
