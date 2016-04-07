//
//  ReporterFactory.hpp
//
//  Created by Anton Leuski on 9/1/15.
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

#ifndef __jerome_npc_factories_ReporterFactory_hpp__
#define __jerome_npc_factories_ReporterFactory_hpp__

#include <iosfwd>
#include <jerome/type/Factory.hpp>
#include <jerome/type/Record.hpp>
#include <jerome/npc/detail/types_fwd.hpp>
#include <jerome/ir/report/HTMLReporter.hpp>

#include <jerome/npc/detail/ModelWriterXML.hpp>
#include <jerome/npc/detail/Ranker.hpp>

namespace jerome {

  class Record;

  namespace ir {
    namespace evaluation {
      template <typename Ranker, typename Report>
      class Reporter;
    }
  }

  namespace npc {

    template <typename Report>
    using Reporter = ir::evaluation::Reporter<detail::Ranker, Report>;

    template <typename R>
    class ReporterFactory
      : public Factory<
        ReporterFactory<R>,
        Reporter<R>,
        const Record&,
        std::ostream&>
    {
    public:
      typedef Factory<
        ReporterFactory,
        Reporter<R>,
        const Record&,
        std::ostream&> parent_type;

      typedef typename parent_type::object_type object_type;
      typedef typename parent_type::provider_pointer provider_pointer;

      ReporterFactory();

      Result<object_type> make(const Record& inRecord, std::ostream& os)
      {
        return parent_type::make(inRecord, inRecord, os);
      }

      template <typename ReporterImplementation>
      void registerProvider();
    };

    namespace detail {
      template <typename Impl, typename Report>
      struct DefaultReporterProvider
        : public ReporterFactory<Report>::Provider
      {
				static constexpr const char* IDENTIFIER = Impl::IDENTIFIER;
			
        Result<typename ReporterFactory<Report>::object_type> operator () (
          const Record& inRecord,
          std::ostream& os) override
        {
          return Reporter<Report>::template make<Providable<Impl>>(String(IDENTIFIER), os);
        }

      };

			template <typename R>
			class XMLReporter : public ir::evaluation::detail::XMLReporter<detail::Ranker, R>
			{
				typedef ir::evaluation::detail::XMLReporter<detail::Ranker, R> parent_type;
			public:
				using parent_type::parent_type;

				typedef typename parent_type::rank_type rank_type;
				typedef typename parent_type::ranker_type ranker_type;
				typedef typename parent_type::report_type report_type;
				typedef typename parent_type::query_type query_type;
				typedef typename parent_type::document_type document_type;
				typedef typename parent_type::sample_type sample_type;
			
			protected:
				void reportDocument(const document_type& inDocument) override
				{
					ModelWriterXML::write(this->stream(), inDocument);
				}

				void reportQuery(const query_type& inQuery) override
				{
					ModelWriterXML::write(this->stream(), inQuery);
				}

				void reportRanker(const ranker_type& inRanker) override
				{
					ModelWriterXML::write(this->stream(), inRanker.state().rankerModel());
				}
				
			};

    }

    template <typename R>
    template <typename ReporterImplementation>
    void ReporterFactory<R>::registerProvider()
    {
      this->template registerProviderClass<detail::DefaultReporterProvider<ReporterImplementation, R>>();
    }

    template <typename R>
    ReporterFactory<R>::ReporterFactory()
    {
      registerProvider<ir::evaluation::detail::HTMLReporter<
          detail::XMLReporter<R>>>();
      registerProvider<detail::XMLReporter<R>>();
    }

  }
}

#endif // defined __jerome_npc_factories_ReporterFactory_hpp__
