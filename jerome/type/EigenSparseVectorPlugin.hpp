//
//  EigenSparseVectorPlugin.hpp
//  jerome
//
//  Created by Anton Leuski on 5/13/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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

