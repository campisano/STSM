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

#include "Kernel.h"

#include <sstream>
#include <stdexcept>

Kernel::Kernel(
    const Range & _range):
    Segment(_range.start(), _range.end())
{
    m_support = 0;
    m_frequency = 0;
}

Kernel::Kernel(
    const Point & _start,
    const Point & _end):
    Segment(_start, _end)
{
    m_support = 0;
    m_frequency = 0;
}

const Support & Kernel::support() const
{
    return m_support;
}

void Kernel::support(const Support & _support)
{
    Size size = m_end - m_start + 1;

    if(_support > size)
    {
        std::stringstream msg;
        msg << "Support (" << _support
            << ") can not be greater then range size ("
            << size << ").";
        throw std::runtime_error(msg.str());
    }

    m_support = _support;
}

const Frequency & Kernel::frequency() const
{
    return m_frequency;
}

void Kernel::frequency(const Frequency & _frequency)
{
    if(_frequency > 1.0)
    {
        std::stringstream msg;
        msg << "Frequenc (" << _frequency
            << ") can not be greater then 1.0.";
        throw std::runtime_error(msg.str());
    }

    m_frequency = _frequency;
}
