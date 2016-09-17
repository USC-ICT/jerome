//
//  XMLReporter.hpp
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

#ifndef __jerome_ir_report_XMLReporter_hpp__
#define __jerome_ir_report_XMLReporter_hpp__

#include <jerome/ir/evaluation/accumulators/accumulator_set.hpp>
#include <jerome/ir/evaluation/accumulators/statistics_fwd.hpp>
#include <jerome/ir/evaluation/accumulators/statistics_names.hpp>
#include <jerome/ir/evaluation/accumulators/to_record.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/query.hpp>
#include <jerome/ir/evaluation/accumulators/statistics/vector.hpp>

#include <jerome/ir/report/Reporter.hpp>
#include <jerome/xml/xml_fwd.hpp>
#include <jerome/xml/writer_manip.hpp>
#include <jerome/xml/writer.hpp>

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace detail {

        class XMLReporterBase
        {
        public:
          static constexpr char const* IDENTIFIER
            = "jerome.reporter.xml";

          XMLReporterBase(std::ostream& os);
          virtual ~XMLReporterBase();

        protected:
          std::ostream& stream();
          XMLReporterBase(xml::document& doc);

        private:
          xml::writer mWriter;

        protected:
          virtual void beginReport();
          virtual void endReport();

          virtual void beginEntriesReport();
          virtual void endEntriesReport();

          virtual void beginEntryReport();
          virtual void endEntryReport();

          virtual void beginDocumentListReport();
          virtual void endDocumentListReport();

          virtual void beginDocumentReport(std::size_t inIndex,
                                           bool inRelevant,
                                           double inScore);
          virtual void endDocumentReport();

        };

        struct ReporterResultVisitor;

        template <typename Ranker, typename Report>
        class XMLReporter
          : public ReporterImplementation<Ranker, Report>
          , public XMLReporterBase
        {
        public:

          typedef ReporterImplementation<Ranker, Report> parent_type;
          typedef typename parent_type::rank_type rank_type;
          typedef typename parent_type::ranker_type ranker_type;
          typedef typename parent_type::report_type report_type;
          typedef typename parent_type::query_type query_type;
          typedef typename parent_type::document_type document_type;
          typedef typename parent_type::sample_type sample_type;

          XMLReporter(std::ostream& os)
            : XMLReporterBase(os)
          {
          }

          XMLReporter(xml::document& doc)
            : XMLReporterBase(doc)
          {
          }

          template <typename T>
          void reportResult(const T& result);

          void report(const ranker_type& inRanker, const report_type& inReport) override
          {
            auto rls = accumulators::vector(inReport);

            // the entries are in random order, lets sort them by query.id
            // our list is rather big and we do not want to sort the list,
            // we will build the sorted index map:

            struct index2id
            {
              std::size_t index;
              std::size_t id;
            };

            std::vector<index2id> index2idMap;

            std::size_t i = 0;
            for (const auto& rl : rls) {
              index2idMap.push_back({ i++, accumulators::query<query_type>(
                                        rl).identifier });
            }

            std::sort(index2idMap.begin(), index2idMap.end(),
              [] (const index2id& lhs, const index2id& rhs) -> bool {
              return lhs.id < rhs.id;
            });

            // iterate over entries in the right order:

            beginReport();

						stream() << xml::startElement("classifier");
						reportRanker(inRanker);
						stream() << xml::endElement;

            reportResult(inReport);

            beginEntriesReport();

            for (const auto& i2i : index2idMap) {
              const auto& entry(rls[i2i.index]);
              const auto& query(accumulators::query<query_type>(entry).value);
              const auto& id(accumulators::query<query_type>(entry).identifier);
              const auto& rl(accumulators::vector(entry));
              const auto& rel(accumulators::map_of<accumulators::tag::
                   is_relevant>(
                  entry));

              beginEntryReport();

              stream() << xml::startElement("query") << xml::attribute("id", id);
              reportQuery(query);
              stream() << xml::endElement;

              reportResult(entry);

              beginDocumentListReport();
              std::size_t index = 0;
              for (const auto& rle : rl) {
                if (index <= 2 || rel[index]) {
                  beginDocumentReport(index, rel[index], rle.score());
                  reportDocument(rle.object());
                  endDocumentReport();
                }
                index++;
              }
              endDocumentListReport();

              endEntryReport();
            }

            endEntriesReport();

            endReport();
          }

        protected:
					friend ReporterResultVisitor;

					virtual void reportDocument(const document_type& inDocument)
					{
						stream() << "do not know how to print the document";
					}

					virtual void reportQuery(const query_type& inQuery)
					{
						stream() << "do not know how to print the query";
					}

					virtual void reportRanker(const ranker_type& inRanker)
					{
						stream() << "do not know how to print the ranker";
					}
        };

        struct ReporterResultVisitor
        {
					ReporterResultVisitor(std::ostream& os)
					: mStream(os)
					{
					}
					
					template <typename T>
					void operator() (const String& inName, const T& inValue) const
					{
						mStream
							<< xml::startElement("field")
							<< xml::attribute("name", inName)
							<< inValue
							<< xml::endElement
							;
					}
					
					std::ostream& mStream;
        };

        template <typename Ranker, typename Report>
        template <typename T>
        void XMLReporter<Ranker, Report>::reportResult(const T& result)
        {
          stream() << xml::startElement("result");
					namespace acc = jerome::ir::evaluation::accumulators;

          accumulators::visitAccumulatorsExcluding<
            acc::features<
              acc::tag::vector
              , acc::tag::map_of<acc::tag::is_relevant>
              , acc::tag::query<query_type>
              , acc::tag::is_relevant
              , acc::tag::relevant_set
              >
            >(result, ReporterResultVisitor(this->stream()));

          stream() << xml::endElement;
        }

      }
    }
  }
}

#endif // defined __jerome_ir_report_XMLReporter_hpp__
