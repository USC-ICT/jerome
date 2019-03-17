//
//  matrix_eigen_sparse_vector_plugin.hpp
//  jerome
//
//  Created by Anton Leuski on 5/13/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//
//  This file is part of Jerome.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#if EIGEN_WORLD_VERSION <= 3 && EIGEN_MAJOR_VERSION < 3

// add this for the "older" version

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

#endif
