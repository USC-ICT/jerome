//
//  cross_entropy_jelinek_mercer.hpp
//
//  Created by Anton Leuski on 7/28/14.
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

#ifndef __jerome_ir_rm_weigthing_document_cross_entropy_jelinek_mercer_hpp__
#define __jerome_ir_rm_weigthing_document_cross_entropy_jelinek_mercer_hpp__

#include <jerome/ir/rm/weighting/document/collection.hpp>

namespace jerome {
  namespace ir {
    namespace rm {
      namespace weighting {
        namespace document {

          struct CrossEntropyJelinekMercer
            : public JelinekMercer
            , public Document
          {

            static constexpr const char* IDENTIFIER =
              "edu.usc.ict.jerome.weighting.document.CrossEntropyJelinekMercer";

            Record model() const
            {
              return {
                       IDENTIFIER_KEY, IDENTIFIER
                       , NAME_KEY, name()
              };
            }

            using JelinekMercer::JelinekMercer;
            template <class Index>
            WeightMatrix computeAffinity(const Context<Index>& inContext) const
            {
              const Index&  index(inContext.index());
              const typename Index::Field&  field = index.findField(name());
              const typename Index::Field::DocumentLengths::size_type ndocs =
                field.documentLengths().size();

              WeightMatrix  ai_log_aj(ndocs, ndocs, 0);

              //		WeightVector	af(ndocs, 0.0);
              WeightVector  cfs(ndocs, 0.0);

              for (const auto& te : field.terms()) {
                const typename Index::Term&   term(te.second);
                const double  col_stat_a    = collection_weight<Index>(term,
                  field);
                const double  col_stat_a_log  = log(col_stat_a);

                // I need to compute A[i][j] = a[j] * log(a[i]);
                // A[i][j] =
                // mle[j] * log(a[i]) + cf * log(a[i])

                // compute mles
                // af.clear();
                // lamda inside the expression is required! It serves to convert
                // integer tfs to double so
                // division gives the expected result
                // af = element_div(lambda * term.tfs(),
                // field.documentLengths());

                SparseWeightVector  af    = document_weight<Index>(term, field);
                //			SparseWeightVector	log_af(af.size());
                //			for(auto i = af.begin(), e = af.end(); i != e; ++i) {
                //				log_af[i.index()] = log(*i + col_stat_a) -
                // col_stat_a_log;
                //			}

                //			WeightScalarVector	col_stat_a_log_vector(ndocs,
                // col_stat_a_log);
                //
                //			cfs += (log_af + col_stat_a_log_vector) * col_stat_a;
                //
                //			ai_log_aj += outer_prod(log_af, af);
                //			ai_log_aj += outer_prod(col_stat_a_log_vector, af);

                for (uint32_t i = 0; i < ndocs; ++i) {
                  const double the_log = af(i) == 0 ? col_stat_a_log : log(af(
                      i) + col_stat_a);

                  // I'm storing the part which is j-independent in an array
                  // that way I need to only iterate thru documents that contain
                  // term te,
                  // and add the j-independent part at the end of the method

                  cfs(i) += the_log * col_stat_a;

                  //				WeightMatrixRow	row(ai_log_aj, i);
                  //				row += the_log * af;

                  for (auto de = af.begin(), end = af.end(); de != end; ++de) {
                    // TODO: This line is slow. 
                    ai_log_aj(i, de.index()) += the_log * af(de.index());
                  }
                }

              }

              ai_log_aj += outer_prod(cfs, WeightScalarVector(ndocs, 1));

              //		for(uint32_t i = 0; i < ndocs; ++i) {
              //			row(ai_log_aj, i) += WeightScalarVector(ndocs, cfs(i));
              //		}

              //		timeval endt;
              //		gettimeofday(&endt, NULL);
              //
              //		double	dt = (endt.tv_usec - startt.tv_usec) + 1000000 *
              // (endt.tv_sec - startt.tv_sec);
              //		std::cout << dt << std::endl;

              //		for(int i = 0, n = result->size1(); i < n; ++i) {
              //			for(int j = 0; j < n; ++j) {
              //				std::cout << (*result)(i, j) <<  " ";
              //			}
              //			std::cout << std::endl;
              //		}

              return ai_log_aj;
            }

            template <class Index>
            WeightMatrix computeAffinityInitialWeight(
              const Context<Index>& inContext,
              const WeightMatrix& inModel) const
            {
              const Index&  index(inContext.index());
              const typename Index::Field&  field = index.findField(name());
              const typename Index::Field::DocumentLengths::size_type ndocs =
                field.documentLengths().size();
              return WeightMatrix(ndocs, inModel.size2(), 0);
            }

          };

        }
      }
    }
  }
}

#endif // __jerome_ir_rm_weigthing_document_cross_entropy_jelinek_mercer_hpp__