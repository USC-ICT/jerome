//
//  HTMLReporter.hpp
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

#ifndef __jerome_ir_report_HTMLReporter_hpp__
#define __jerome_ir_report_HTMLReporter_hpp__

#include <fstream>
#include <jerome/ir/report/XMLReporter.hpp>
#include <jerome/xml/document.hpp>
#include <jerome/xml/stylesheet.hpp>

namespace jerome {
  namespace ir {
    namespace evaluation {

      namespace detail {

        class HTMLReporterBase
        {
        public:
          static constexpr char const* IDENTIFIER
            = "jerome.reporter.html";
          
          HTMLReporterBase();
          ~HTMLReporterBase();

        protected:
          xml::stylesheet& stylesheet();
        };

        template <typename XMLRep>
        class HTMLReporter
          : public ReporterImplementation<typename XMLRep::ranker_type,
            typename XMLRep::report_type>
          , public HTMLReporterBase
        {
        public:
          typedef ReporterImplementation<
            typename XMLRep::ranker_type,
            typename XMLRep::report_type> parent_type;
          typedef typename parent_type::rank_type rank_type;
          typedef typename parent_type::ranker_type ranker_type;
          typedef typename parent_type::report_type report_type;
          typedef typename parent_type::query_type query_type;
          typedef typename parent_type::document_type document_type;
          typedef typename parent_type::sample_type sample_type;

          HTMLReporter(std::ostream& os)
            : mStream(os)
            , mDocument()
            , mReporter(mDocument)
          {
          }

          void report(const ranker_type& inRanker,
                      const report_type& inReport) override
          {
            mReporter.report(inRanker, inReport);
            const auto result = stylesheet()(mDocument);
						
						std::ofstream test("test.xml");
						mDocument.save(test);
						
            if (result) {
              result.value().save(mStream, true);
              mStream.flush();
            }
          }

        private:
          std::ostream& mStream;
          xml::document mDocument;
          XMLRep  mReporter;
        };

      }
    }
  }
}

#endif // defined __jerome_ir_report_HTMLReporter_hpp__
