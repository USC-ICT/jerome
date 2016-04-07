//
//  record_writer.cpp
//
//  Created by Anton Leuski on 8/31/15.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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

#include "writer_manip.hpp"
#include "record_writer.hpp"

namespace jerome {

//  namespace xml {
//    namespace detail {
//      template <typename T>
//      static std::ostream& writeRecord(
//        std::ostream& os,
//        const RecordTags& inRecordTags,
//        const ordered_multimap<String,T>& inRecord)
//      {
//        os
//          << xml::startElement(inRecordTags.element);
//
//        os << record(RecordTags());
//
//        for(const auto& p : inRecord) {
//        os
//          << xml::startElement(inRecordTags.item)
//          << xml::attribute(inRecordTags.nameAttribute, p.first, inRecordTags.defaultName)
//          << p.second
//          << xml::endElement;
//        }
//
//        return os
//          << xml::endElement;
//      }
//      
//      struct RecordTagsStorage {
//        static const int xindex;
//        static RecordTagsStorage& fromStream(std::ios_base& os);
//        static void callback(std::ios_base::event evt, std::ios_base& str, int idx);
//        static RecordTagsStorage* pointer(std::ios_base& os) {
//          return static_cast<RecordTagsStorage*>(os.pword(xindex));
//        }
//        
//        RecordTags  recordTags;
//        RecordTags  recordOfRecordTags;
//      };
//      
//      const int RecordTagsStorage::xindex = std::ios_base::xalloc();
//
//      RecordTagsStorage& RecordTagsStorage::fromStream(std::ios_base& os)
//      {
//        RecordTagsStorage*  p = pointer(os);
//        if (!p) {
//          p = new RecordTagsStorage;
//          os.pword(xindex) = p;
//          os.register_callback(callback, xindex);
//        }
//        return *p;
//      }
//     
//      void RecordTagsStorage::callback(std::ios_base::event evt, std::ios_base& os, int idx)
//      {
//        if (evt == std::ios_base::erase_event) {
//          RecordTagsStorage*  p = pointer(os);
//          delete p;
//          os.pword(xindex) = nullptr;
//        } else if (evt == std::ios_base::copyfmt_event) {
//          RecordTagsStorage*  p = pointer(os);
//          if (p) p = new RecordTagsStorage(*p);
//          os.pword(xindex) = p;
//        }
//      }
//
//    }
//    
//    std::ostream& operator << (std::ostream& os, const record& inRecord)
//    {
//      detail::RecordTagsStorage s(detail::RecordTagsStorage::fromStream(os));
//      s.recordTags = inRecord.mTags;
//      if (inRecord.mOtherTags) s.recordOfRecordTags = *inRecord.mOtherTags;
//      return os;
//    }
//
//  }
  
//  using namespace jerome::xml::detail;
//  
//  std::ostream& operator << (std::ostream& os, const Record& inRecord)
//  {
//    return writeRecord(os, RecordTagsStorage::fromStream(os).recordTags,
//      inRecord);
//  }
//  
//  std::ostream& operator << (std::ostream& os, const RecordOfRecords& inRecord)
//  {
//    return writeRecord(os, RecordTagsStorage::fromStream(os).recordOfRecordTags,
//      inRecord);
//  }
  

}
