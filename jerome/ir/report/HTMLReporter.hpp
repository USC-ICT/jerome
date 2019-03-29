//
//  HTMLReporter.hpp
//
//  Created by Anton Leuski on 8/13/15.
//  Copyright (c) 2015 Anton Leuski & ICT/USC. All rights reserved.
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
          static constexpr char const* IDENTIFIER = "jerome.reporter.html";
          
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
