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
              "jerome.weighting.document.CrossEntropyJelinekMercer";

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
              const typename Index::Field::size_type ndocs = field.documentLengths().size();

              MatrixSize size;
              size.rowCount = ndocs;
              size.columnCount = ndocs;
              WeightMatrix  ai_log_aj = WeightMatrixZero(size);

              WeightVector  each_column = WeightVectorZero(ndocs);
              WeightVector  each_row = WeightVectorZero(ndocs);
              double cf_log_cf = 0;

              for (const auto& te : field.terms()) {
                const typename Index::Term&   term(te.second);
                const double  cf    = collection_weight<Index>(term,
                  field);
                const double  log_cf  = log(cf);

                // I need to compute A[j][i] = a[i] * log(a[j])
                // = sum_t (a[i](t) * log(a[j](t))) // depend on terms
                // = sum_t (mle[i](t) * log(mle[j](t)+cf(t)) + cf(t) * log(mle[j](t)+cf(t)))
                // ~ mle[i] * log(mle[j]+cf) + cf * log(mle[j]+cf) //dropping index t and sum
                // = mle[i] * log(1+mle[j]/cf) + mle[i]*log(cf) + cf * log(1+mle[j]/cf) + cf*log(cf)
                // =>
                // A += outer_prod(log(1+mle/cf), mle)
                //    + (mle * log(cf)).TR
                //    + cf * log(1+mle/cf)
                //    + cf*log(cf)

                SparseWeightVector  mle     = document_weight<Index>(term, field);
                SparseWeightVector  log_mle = sparse_log_plus1(mle/cf);

                sparse_outer_prod_add_to(log_mle, mle, ai_log_aj);
                sparse_scale_add_to(log_mle, cf, each_column);
                sparse_scale_add_to(mle, log_cf, each_row);
                cf_log_cf += cf * log_cf;
              }

              ai_log_aj += cf_log_cf * WeightMatrixOnes(size)
                + jerome::outer_prod(each_column, WeightVectorOnes(ndocs))
                + jerome::outer_prod(WeightVectorOnes(ndocs), each_row);

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
              const typename Index::Field::size_type ndocs = field.documentLengths().size();
              MatrixSize size(inModel);
              size.rowCount = ndocs;
              return WeightMatrixZero(size);
            }

          };

        }
      }
    }
  }
}

#endif // __jerome_ir_rm_weigthing_document_cross_entropy_jelinek_mercer_hpp__
