//
//  record_writer.cpp
//
//  Created by Anton Leuski on 8/31/15.
//  Copyright (c) 2015 ICT/USC. All rights reserved.
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
