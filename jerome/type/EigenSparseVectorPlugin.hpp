//
//  EigenSparseVectorPlugin.hpp
//  jerome
//
//  Created by Anton Leuski on 5/13/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//
//  This file is part of Jerome.
//
//  Jerome is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Jerome is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with Jerome.  If not, see <http://www.gnu.org/licenses/>.
//

void conservativeResize(Index newSize)
{
  if (newSize < m_size)
  {
    Index i = 0;
    while (i<m_data.size() && m_data.index(i)<newSize) ++i;
    m_data.resize(i);
  }
  m_size = newSize;
}

