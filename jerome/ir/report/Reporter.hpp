//
//  Reporter.hpp
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
