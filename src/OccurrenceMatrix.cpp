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

#include "OccurrenceMatrix.h"

#include <cstddef>
// #include <iostream>
#include <sstream>
#include <stdexcept>

OccurrenceMatrix::OccurrenceMatrix(
    const RangedSequence & _ranged_sequence,
    const ListPositions & _positions) :
    m_matrix(NULL)
{
    // std::cout << std::endl
    //           << _ranged_sequence.sequence().toStringOfItems() << std::endl;

    m_seq_size = _ranged_sequence.sequence().size();

    detectBorders(_positions);

    initMatrix();

    ListPositions::const_iterator it_pos;
    size_t idx_s;

    for(it_pos = _positions.begin(); it_pos != _positions.end(); ++it_pos)
    {
        for(idx_s = 0; idx_s < m_seq_size; ++idx_s)
        {
            m_matrix[it_pos->first - m_x_start][
                it_pos->second - m_y_start + idx_s] = true;
        }
    }

    // std::cout << toString();
}

OccurrenceMatrix::~OccurrenceMatrix()
{
    clearMatrix();
}

void OccurrenceMatrix::detectBorders(
    const ListPositions & _positions)
{
    ListPositions::const_iterator it_pos;

    it_pos = _positions.begin();

    if(it_pos == _positions.end())
    {
        std::stringstream msg;
        msg << "Positions are empty, OccurrenceMatrix cannot be calculated."
            << std::endl;
        throw std::runtime_error(msg.str());
    }

    size_t min_x, max_x, min_y, max_y;
    min_x = max_x = it_pos->first;
    min_y = max_y= it_pos->second;

    while(it_pos != _positions.end())
    {
        if(it_pos->first < min_x)
        {
            min_x = it_pos->first;
        }

        if(it_pos->first > max_x)
        {
            max_x = it_pos->first;
        }

        if(it_pos->second < min_y)
        {
            min_y = it_pos->second;
        }

        if(it_pos->second > max_y)
        {
            max_y = it_pos->second;
        }

        ++it_pos;
    }

    max_y += m_seq_size - 1;

    m_width = max_x - min_x + 1;
    m_height = max_y - min_y + 1;
    m_x_start = min_x;
    m_y_start = min_y;

    // std::cout << "w:" << m_width << " h:" << m_height
    //           << " xs:" << m_x_start << " ys:" << m_y_start << std::endl;
}

void OccurrenceMatrix::initMatrix()
{
    m_matrix = new bool * [m_width];

    for(size_t x = 0; x < m_width; ++x) {
        m_matrix[x] = new bool[m_height](); // () initialize all to false
    }
}

void OccurrenceMatrix::clearMatrix()
{
    if(m_matrix)
    {
        for(size_t x = 0; x < m_width; ++x) {
            delete [] m_matrix[x];
        }

        delete [] m_matrix;
        m_matrix = NULL;
    }
}

Support OccurrenceMatrix::getSupport(
    const Range & _range,
    const Interval & _interval) const
{
    size_t x_start = _range.start() - m_x_start;
    size_t x_end = x_start + _range.size();

    size_t y_start = _interval.start() - m_y_start;
    size_t y_end = y_start + _interval.size();

    size_t x, y;

    size_t items_count = 0;
    size_t support = 0;

    for(x = x_start; x != x_end; ++x)
    {
        items_count = 0;

        for(y = y_start; y != y_end; ++y)
        {
            if(m_matrix[x][y])
            {
                ++items_count;

                if(items_count == m_seq_size)
                {
                    ++support;
                    items_count = 0;
                }
            }
        }
    }

    // std::cout << "r:" << _range.start() << "," << _range.end()
    //           << "rb:" << x_start << "," << x_end
    //           << " i:" << _interval.start() << "," << _interval.end()
    //           << " ib:" << y_start << "," << y_end
    //           << " s:" << support * m_seq_size << std::endl;

    return support * m_seq_size;
}

std::string OccurrenceMatrix::toString() const
{
    std::stringstream ss;
    size_t x, y;

    for(x = 0; x != m_width; ++x)
    {
        for(y = 0; y != m_height; ++y)
        {
            ss << int(m_matrix[x][y]);
        }

        ss << std::endl;
    }

    return ss.str();
}
