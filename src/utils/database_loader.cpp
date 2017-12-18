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

void loadDatabase(const std::string & _input_filename, Database & _database)
{
    rapidcsv::Document doc(rapidcsv::Properties(_input_filename, 0, -1));

    //TODO [CMP] now we should rotate the csv to the expected order
    // unsigned int i, cols = doc.GetColumnCount();

    // for(i = 0; i < cols; ++i)
    // {
    //     _database.push_back(doc.GetColumn<char>(i));
    // }

    unsigned int i, rows = doc.GetRowCount();

    for(i = 0; i < rows; ++i)
    {
        _database.push_back(doc.GetRow<char>(i));
    }
}
