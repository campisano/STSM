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

#include "database_loader.h"

#include <rapidcsv.h>
#include <string>

namespace
{
void loadNormal(const rapidcsv::Document & _doc, Database & _database)
{
    unsigned int i, cols = _doc.GetColumnCount();

    for(i = 0; i < cols; ++i)
    {
        _database.push_back(_doc.GetColumn<char>(i));
    }
}

void loadTransposed(const rapidcsv::Document & _doc, Database & _database)
{
    unsigned int i, rows = _doc.GetRowCount();

    for(i = 0; i < rows; ++i)
    {
        _database.push_back(_doc.GetRow<char>(i));
    }
}
}

void loadDatabase(
    const std::string & _input_filename,
    Database & _database,
    bool _has_header,
    bool _transpose
)
{
    int rows_name_idx;
    if(_has_header)
    {
        rows_name_idx = 0;
    }
    else
    {
        rows_name_idx = -1;
    }

    rapidcsv::Document doc(
        rapidcsv::Properties(_input_filename, -1, rows_name_idx));

    if(_transpose)
    {
        loadTransposed(doc, _database);
    }
    else
    {
        loadNormal(doc, _database);
    }
}
