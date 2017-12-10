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

#ifndef KERNEL__H__
#define KERNEL__H__

#include <list>

#include "Frequency.h"
#include "Point.h"
#include "Range.h"
#include "Segment.h"
#include "Support.h"

class Kernel : public Segment
{
public:
    explicit Kernel(
        const Range & _range);

    explicit Kernel(
        const Point & _start,
        const Point & _end);

    const Support & support() const;
    void support(const Support & _support);

    const Frequency & frequency() const;
    void frequency(const Frequency & _frequency);

private:
    Support m_support;
    Frequency m_frequency;
};

typedef std::list < Kernel > ListKernels;

#endif
