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

#ifndef CANDIDATE__H__
#define CANDIDATE__H__

#include <list>

#include "Kernel.h"
#include "Range.h"
#include "Sequence.h"
#include "Serie.h"

class Candidate
{
public:
    explicit Candidate(
        const Sequence & _sequence,
        const Range & _range,
        const ListKernels & _kernels);

    const Sequence & sequence() const;
    const Range & range() const;
    const ListKernels & kernels() const;

    void updateCandidateKernels(
        const Serie & _serie,
        const Point & _position,
        const Frequency & _min_spatial_freq);

    void mergeKernels(
        const Frequency & _min_spatial_freq);

private:
    Sequence m_sequence;
    Range m_range;
    ListKernels m_kernels;

    bool m_kernel_closed;
    Kernel * m_current;
};

typedef std::list < Candidate > ListCandidates;

#endif
