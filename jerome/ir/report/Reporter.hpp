//
//  Reporter.hpp
//
//  Created by Anton Leuski on 8/13/15.
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

#ifndef __jerome_ir_report_Reporter_hpp__
#define __jerome_ir_report_Reporter_hpp__

#include <jerome/types.hpp>

namespace jerome {
  namespace ir {
    namespace evaluation {

      namespace detail {
        template <typename Ranker, typename Report>
        class ReporterImplementation
        {
        public:
          typedef std::size_t rank_type;
          typedef Ranker ranker_type;
          typedef Report report_type;
          typedef typename ranker_type::query_type query_type;
          typedef typename ranker_type::document_type document_type;
          typedef typename report_type::sample_type sample_type;

          virtual ~ReporterImplementation()
          {
          }

          virtual void report(const ranker_type& inRanker,
                              const report_type& inReport) = 0;
        };

      }

      template <typename Ranker, typename Report>
      class Reporter
        : public ReferenceClassInterface<detail::ReporterImplementation<Ranker,
            Report>>
      {
        typedef ReferenceClassInterface<detail::ReporterImplementation<Ranker,
            Report>> parent_type;

      public:
        using parent_type::parent_type;

        void report(const Ranker& inRanker, const Report& inReport)
        {
          this->implementation().report(inRanker, inReport);
        }

        template <typename Impl, typename ... Args>
        static Reporter<Ranker, Report> make(Args&& ... args)
        {
          return Reporter<Ranker,
            Report>(make_impl<Impl>(std::forward<Args>(args) ...));
        }

			protected:
				template <typename Impl, typename ... Args>
				static shared_ptr<Impl> make_impl(Args&& ... args)
				{
					return std::make_shared<Impl>(std::forward<Args>(args) ...);
				}

      };

    }
  }
}

#endif // defined __jerome_ir_report_Reporter_hpp__
