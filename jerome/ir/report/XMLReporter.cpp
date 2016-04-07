//
//  XMLReporter.cpp
//
//  Created by Anton Leuski on 8/13/15.
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

#include <jerome/xml/writer.hpp>
#include <jerome/xml/writer_manip.hpp>

#include "XMLReporter.hpp"

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace detail {
        XMLReporterBase::XMLReporterBase(std::ostream& os)
          : mWriter(os)
        {
        }

        // for subclass to overwrite
        XMLReporterBase::XMLReporterBase(xml::document& doc)
          : mWriter(doc)
        {
        }

        XMLReporterBase::~XMLReporterBase()
        {
        }

        std::ostream& XMLReporterBase::stream()
        {
          return mWriter.stream();
        }

        void XMLReporterBase::beginReport()
        {
          stream() << xml::indent(" ");
          stream() << xml::startElement("report");
        }

        void XMLReporterBase::endReport()
        {
          stream() << xml::endElement;
          mWriter.close();
        }

        void XMLReporterBase::beginEntriesReport()
        {
          stream() << xml::startElement("experiments");
        }

        void XMLReporterBase::endEntriesReport()
        {
          stream() << xml::endElement;
        }

        void XMLReporterBase::beginEntryReport()
        {
          stream() << xml::startElement("retrieval");
        }

        void XMLReporterBase::endEntryReport()
        {
          stream() << xml::endElement; // "retrieval"
        }

        void XMLReporterBase::beginDocumentListReport()
        {
          stream() << xml::startElement("rankedList");
        }

        void XMLReporterBase::endDocumentListReport()
        {
          stream() << xml::endElement; // "rankedList"
        }

        void XMLReporterBase::beginDocumentReport(std::size_t inIndex,
                                 bool inRelevant,
                                 double inScore)
        {
          stream()
            << xml::startElement("utterance")
            << xml::attribute("rank", (inIndex + 1))
            << xml::attribute("score", inScore)
            << xml::attribute("relevant", inRelevant)
          ;
        }
        void XMLReporterBase::endDocumentReport()
				{
          stream() << xml::endElement; // "utterance"
				}

//        void XMLReporter::reportRanker(const RecordOfRecords& inRecords)
//        {
//          stream() << xml::startElement("classifierParameters");
//          reportRecords(inRecords);
//          stream() << xml::endElement;
//        }
//
//        void XMLReporter::reportStatistics(const Record& inRecords)
//        {
//          stream() << xml::startElement("result");
//          reportRecords(inRecords);
//          stream() << xml::endElement;
//        }
//
//        void XMLReporter::beginEntryReport(rank_type inID,
//                                           const Record& inQueryRecords,
//                                           const Record& inQueryResultRecords)
//        {
//          stream() << xml::startElement("retrieval");
//
//          stream()
//            << xml::startElement("query")
//            << xml::attribute("id", inID);
//          reportRecords(inQueryRecords);
//          stream() << xml::endElement;
//
//          stream() << xml::startElement("result");
//          reportRecords(inQueryResultRecords);
//          stream() << xml::endElement;
//
//          stream() << xml::startElement("rankedList");
//        }
//
//        static
//        void XMLReporter::reportRecords(const Record& inRecords)
//        {
//          for (auto& p : inRecords) {
//            stream()
//              << xml::startElement("field")
//              << xml::attribute("name", p.first)
//              << p.second
//              << xml::endElement;
//          }
//        }

      }
    }
  }
}