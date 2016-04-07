//
//  accumulator_set.hpp
//
//  Created by Anton Leuski on 5/12/13.
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

#ifndef __jerome_ir_evaluation_accumulators_accumulator_set_hpp__
#define __jerome_ir_evaluation_accumulators_accumulator_set_hpp__

#ifdef nil
# define al_defined_nil
# pragma push_macro("nil")
# undef nil
# pragma push_macro("Nil")
# undef Nil
#endif

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#include <boost/accumulators/framework/accumulator_set.hpp>
#include <boost/accumulators/framework/extractor.hpp>
#include <boost/accumulators/framework/accumulator_base.hpp>
#include <boost/fusion/include/reverse_view.hpp>
#include <boost/accumulators/framework/features.hpp>
#pragma clang diagnostic pop

#ifdef al_defined_nil
# undef al_defined_nil
# pragma pop_macro("Nil")
# pragma pop_macro("nil")
#endif

// I extend the accumulator framework to include accumulators that
// support subtract operation.

namespace jerome {
  namespace ir {
    namespace evaluation {
      namespace accumulators {

				using namespace boost::accumulators;

        namespace detail {

          template <typename Args>
          struct accumulator_subtract_visitor
          {
            explicit accumulator_subtract_visitor(Args const& a) : args(a)
            {
            }

            template <typename Accumulator>
            void operator () (Accumulator& accumulator) const
            {
              accumulator.subtract(this->args);
            }

          private:
            accumulator_subtract_visitor& operator = (
              accumulator_subtract_visitor const&) = delete;
            Args const& args;
          };

          template <typename Args>
          inline accumulator_subtract_visitor<Args> const
          make_accumulator_subtract_visitor(Args const& args)
          {
            return accumulator_subtract_visitor<Args>(args);
          }

        }

        template <typename Sample, typename Features, typename Weight = void>
        struct subtractable_accumulator_set
          : accumulator_set<Sample, Features, Weight>
          , std::unary_function<Sample, void>
        {

          typedef subtractable_accumulator_set<Sample, Features,
            Weight>          this_type;
          typedef accumulator_set<Sample, Features,
            Weight>                parent_type;
          typedef typename parent_type::accumulators_type   accumulators_type;
          // The type of the samples that will be accumulated
          typedef typename parent_type::sample_type         sample_type;
          // An MPL sequence of the features that should be accumulated.
          typedef typename parent_type::features_type         features_type;
          // The type of the weight parameter. Must be
          typedef typename parent_type::weight_type         weight_type;

          subtractable_accumulator_set() : parent_type()
          {
          }

          template <typename A1>
          explicit subtractable_accumulator_set(A1 const& a1) : parent_type(a1)
          {
          }

          this_type& value()
          {
            return *this;
          }

          template <typename UnaryFunction>
          void reverse_visit(UnaryFunction const& func)
          {
            boost::fusion::reverse_view<accumulators_type> reversed_accs(
              this->accumulators);
            boost::fusion::for_each(reversed_accs, func);
          }

          void subtract()
          {
//				this->reverse_visit(
            this->visit(detail::make_accumulator_subtract_visitor(boost::
                 accumulators::detail::accumulator_params()(*this)));
          }

          template <typename A1>
          void subtract(A1 const& a1)
          {
//				this->reverse_visit(
            this->visit(detail::make_accumulator_subtract_visitor(boost::
                 accumulators::detail::accumulator_params()(*this, a1)));
          }

          template <typename A1>
          this_type&  operator += (A1 const& a1)
          {
            this->operator () (a1);
            return *this;
          }

          template <typename A1>
          this_type&  operator -= (A1 const& a1)
          {
            this->subtract(a1);
            return *this;
          }

        };
      }
    }
  }
}

#endif // __jerome_ir_evaluation_accumulators_accumulator_set_hpp__