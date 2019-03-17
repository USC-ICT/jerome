//
//  jelinek_mercer.hpp
//
//  Created by Anton Leuski on 7/28/14.
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

#ifndef __jerome_ir_rm_weighting_query_jelinek_mercer_hpp__
#define __jerome_ir_rm_weighting_query_jelinek_mercer_hpp__

#include <jerome/ir/rm/weighting/query/collection.hpp>

namespace jerome {
  namespace ir {
    namespace rm {
      namespace weighting {
        namespace query {

          struct JelinekMercer
            : public jerome::ir::rm::weighting::JelinekMercer
            , public Query
          {
            static constexpr const char* IDENTIFIER =
              "jerome.weighting.query.JelinekMercer ";

            Record model() const
            {
              return {
                       IDENTIFIER_KEY, IDENTIFIER
                       , NAME_KEY, name()
              };
            }

            using jerome::ir::rm::weighting::JelinekMercer::JelinekMercer;
            template <class Index>
            WeightMatrix computeQueryModelForQuery(
              const Context<Index>& inContext,
              const Index& inQuery) const
            {
              const Index&  index(inContext.index());
              const typename Index::Field&  field = index.findField(name());
              const typename Index::Field&  query = inQuery.findField(name());
              const typename Index::Field::size_type ndocs = field.documentLengths().size();
              MatrixSize size;
              size.rowCount = ndocs;
              size.columnCount = inQuery.documentCount();
              WeightMatrix  bw = WeightMatrixZero(size);

              typedef traits<WeightMatrix>::size_type IndexType;
              typedef traits<WeightMatrix>::value_type ValueType;

              // noinspection StringEquality
              for (const auto& te : query.terms()) {

                const typename Index::Term& ti      = field.findTerm(te.first);
                if (ti.cf() == 0)
                  continue;
                
                double    wIndep    = collection_weight<Index>(ti, field);
                if (wIndep == 0)
                  throw Exception(
                    std::string() + "Collection statistic for term " + te.first
                    //												+ " with Cf=" //+ (int)ti.cf()
                    //												+ " epsilon=" + epsilon
                    //												+ " ColSize=" + cs
                    //												+ " VocSize=" + vs
                    //												+ " Lambda=" + lambda
                    + " evaluates to zero. That is just plain wrong.");

                // note that we are assuming, we will be normalizing this value.
                // So we put document-independent part
                // into denominator. It is equivalent that we multiply the final
                // value by a constant:
                // \prod_j \b_k(q_j) = \exp (\sum_j log(x_jk+x_j)) = \exp
                // (\sum_j log(x_j)) \exp (\sum_j log(x_jk/x_j + 1))

                const typename Index::Term&       queryTermInfo(te.second);
                
                // we want a temporary before doing an outer product.
                // so we compute the values once. Ideally, the library
                // should understand that, but, e.g., eigen fails here anyway.
                SparseWeightVector  x = jerome::sparse_log_plus1(document_weight<Index>(ti, field) / wIndep);
                SparseWeightVector  y = jerome::matrix_cast<ValueType>(queryTermInfo.tfs());

                sparse_outer_prod_add_to(x, y, bw);

                //			for(Term::Frequencies::const_iterator termDocIterator =
                // ti.tfs().begin(), iend = ti.tfs().end(); termDocIterator !=
                // iend; ++termDocIterator) {
                //				//				std::cout << te->first << " " << cs << " "
                // << vs << " " << epsilon << " " << ti.df() <<  " "
                //				//							<< qf << " " << lambda << " " <<
                // (int)*i << " " << doclen[i.index()] << " " << wIndep <<
                // std::endl;
                //				double	theWeight = log(
                // document_weight(termDocIterator, field) / wIndep + 1);
                //				for(Term::Frequencies::const_iterator queryDocIterator
                // = queryTermInfo.tfs().begin(), qiend =
                // queryTermInfo.tfs().end(); queryDocIterator != qiend;
                // ++queryDocIterator) {
                //					bw(termDocIterator.index(),
                // queryDocIterator.index()) += (*queryDocIterator) * theWeight;
                //				}
                //			}
              }

              // here again I assume that we will normalize the values. To
              // minimize precision problems,
              // I shift the numbers so the maximum number is always 1.

              const double  min_exp =
                std::numeric_limits<double>::min_exponent * log(2) + 1;

              
              for (IndexType j = 0, n = size.columnCount; j < n; ++j) {
                auto theColumn = jerome::column(bw, j);

                double  max   = jerome::max_element(theColumn);
                
                //		std::cout << max << " " << min_exp << " " <<
                // std::numeric_limits<double>::min_exponent << " " << log(2) <<
                // std::endl;
                double sum = 0;
                for (IndexType k = 0, nk = theColumn.size(); k < nk; ++k) {
                  //			std::cout << i << " ";
                  double x = theColumn(k) - max;
                  x = x < min_exp ? 0 : exp(x);
                  theColumn(k) = x;
                  sum += x;
                }
                theColumn /= sum;
              }

              return bw;
            }

          };

        }
      }
    }
  }
}

#endif // defined __jerome_ir_rm_weighting_query_jelinek_mercer_hpp__
