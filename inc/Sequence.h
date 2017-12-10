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

#ifndef SEQUENCE__H__
#define SEQUENCE__H__

#include <list>
#include <string>
#include <vector>

#include "Item.h"
#include "Point.h"
#include "Serie.h"
#include "Size.h"

class Sequence
{
public:
    explicit Sequence();
    explicit Sequence(const Item & _item);
    explicit Sequence(const std::string & _string_representation);
    virtual ~Sequence();

    bool operator==(const Sequence & _other) const;
    bool operator!=(const Sequence & _other) const;

    void set(const std::string & _string_representation);

    void clear();
    void append(const Item & _item);

    Size size() const;

    const Item & getFirst() const;
    const Item & getLast() const;

    void getCopyExceptPos(const Point & _pos, Sequence & _copy_seq) const;
    void getSubsequenceDroppingFirstItem(Sequence & _subseq) const;
    void getSubsequenceDroppingLastItem(Sequence & _subseq) const;

    bool supportedBy(const Serie & _serie) const;

    std::string toString() const;
    std::string toStringOfItems() const;

private:
    std::vector < Item > m_items;
};

#endif
