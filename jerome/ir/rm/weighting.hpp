//
//  weighting.hpp
//
//  Created by Anton Leuski on 4/11/12.
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

#ifndef __jerome_ir_rm_weighting_hpp__
#define __jerome_ir_rm_weighting_hpp__

#include <vector>

#include <jerome/types.hpp>
#include <jerome/ir/collection.hpp>
#include <jerome/ir/evaluation.hpp>
#include <jerome/math/parameters/parameterized.hpp>

namespace jerome {
  namespace ir {
    namespace rm {
      namespace weighting {

        extern const double kAlmostZero;
        extern const double kAlmostOne;

        using namespace jerome::math::parameters;

#pragma mark - Weighting

        struct Weighting
        {

          static constexpr const char* NAME_KEY = "name";
          static constexpr const char* IDENTIFIER_KEY = "id";
          static constexpr const char* SUB_WEIGHTING_KEY = "weighting";

          Weighting(const String& inName = "") : mName(inName)
          {
          }

          const String& name() const
          {
            return mName;
          }

          void setName(const String& inName)
          {
            mName = inName;
          }

        private:
          String    mName;
        };

#pragma mark - Laplace

        struct Laplace
          : public Weighting
        {
          Laplace(const String& inName = "", double inEpsilon = 0.01)
            : Weighting(inName)
            , mEpsilon("\u03b5", inEpsilon, kAlmostZero, kAlmostOne)
          {
          }

          const Parameter&    epsilon() const
          {
            return mEpsilon;
          }

          Parameter&        epsilon()
          {
            return mEpsilon;
          }

        private:
          Parameter mEpsilon;
        };

#pragma mark - JelinekMercer

        struct JelinekMercer
          : public ParameterizedHelper<JelinekMercer, Laplace>
        {
          typedef ParameterizedHelper<JelinekMercer, Laplace> parent_type;

          JelinekMercer(const String& inName = "",
            double inLambda = 0.85,
            double inEpsilon = 0.01)
            : parent_type(inName, inEpsilon)
            , mLambda("\u03bb", inLambda, kAlmostZero, kAlmostOne)
          {
          }

          JelinekMercer(const char* inName,
            double inLambda = 0.85,
            double inEpsilon = 0.01)
            : parent_type(inName, inEpsilon)
            , mLambda("\u03bb", inLambda, kAlmostZero, kAlmostOne)
          {
          }

          const Parameter&    lambda() const
          {
            return mLambda;
          }

          Parameter&        lambda()
          {
            return mLambda;
          }

          template <class Index>
          double collection_weight(const typename Index::Term& term,
                                   const typename Index::Field& field) const
          {
            return (1.0 - lambda().value()) * (term.cf() + epsilon().value()) /
                   (field.cs() + epsilon().value() * field.vs());
          }

          template <class Index>
          auto document_weight(const typename Index::Term& term,
                               const typename Index::Field& field)
          const
          -> decltype(jerome::sparse_scale_div_by(matrix_cast<double>(term.tfs()), lambda().value(),
                                          matrix_cast<double>(field.documentLengths())))
          {
            return jerome::sparse_scale_div_by(matrix_cast<double>(term.tfs()), lambda().value(),
                                matrix_cast<double>(field.documentLengths()));
          }

          using parent_type::accept;

          void accept(GetTitle& visitor) const
          {
            GetTitle  prefixVisitor(&visitor, this->name() + " ");
            lambda().accept(prefixVisitor);
          }

        private:
          Parameter mLambda;

          friend parent_type;

          template <class Visitor, class Visitable>
          static void visit(Visitor& visitor, Visitable& visitable)
          {
            visitable.lambda().accept(visitor);
          }

        };

        //			class Dirichlet : public BaseWithLaplaceConstant {
        //
        //			private:
        //				Parameter	mMu;
        //			public:
        //				Dirichlet(const String& inFieldName, double inEpsilon = 0.01,
        // double inMu = 1)
        //				: BaseWithLaplaceConstant(inEpsilon), mMu("\u03bc", inMu, 1,
        // 100000) {}
        //				const Parameter&		mu() const { return mMu; }
        //				Parameter&				mu() { return mMu; }
        //			protected:
        //				void collectParameters(parameters_type& ioParameters) {
        //					BaseWithLaplaceConstant::collectParameters(ioParameters);
        //					ioParameters.push_back(&mMu);
        //				}
        //			};

        /*
           1. apply to
           void visit(Visitor inVisitor, Visitable inVisitable)

         */

#pragma mark - Collection

        template <class Element, class ... Args>
        struct Collection;

        namespace detail {

          template <int N, class this_type>
          struct visit_collection_helper
          {
            template <class Visitor>
            static void visit(Visitor& visitor, this_type& sup)
            {
              sup.alphas()[this_type::size - N].accept(visitor);
              std::get<this_type::size - N>(sup.weightings()).accept(visitor);
              visit_collection_helper<N - 1, this_type>::visit(visitor, sup);
            }

            static void visit(GetTitle& visitor, this_type& sup)
            {
              GetTitle  prefixVisitor(&visitor, std::get<this_type::size - N>
                                      (sup.weightings()).name() + " ");
              sup.alphas()[this_type::size - N].accept(prefixVisitor);
              std::get<this_type::size - N>(sup.weightings()).accept(visitor);
              visit_collection_helper<N - 1, this_type>::visit(visitor, sup);
            }

          };

          template <class this_type>
          struct visit_collection_helper<0, this_type>
          {
            template <class Visitor>
            static void visit(Visitor& visitor, this_type& sup)
            {
            }

          };

          template <int N, class this_type>
          struct model_collection_helper
          {
            static void visit(Record& visitor, this_type& sup)
            {
              visitor.emplace(this_type::SUB_WEIGHTING_KEY,
                std::get<this_type::size - N>(sup.weightings()).model());
              model_collection_helper<N - 1, this_type>::visit(visitor, sup);
            }

          };

          template <class this_type>
          struct model_collection_helper<0, this_type>
          {
            static void visit(Record& visitor, this_type& sup)
            {
            }

          };

        }

        template <class Element, class Arg1, class Arg2, class ... Args>
        struct Collection<Element, Arg1, Arg2, Args ...>
          : public Parameterized<Collection<Element, Arg1, Arg2, Args ...>,
            Weighting>
          , public Element
        {

          typedef Parameterized<Collection<Element, Arg1, Arg2, Args ...>,
            Weighting>  parent_type;
          typedef std::tuple<Arg1, Arg2, Args ...>   weightings_type;
          typedef std::vector<Parameter>        alphas_type;
          typedef Collection<Element, Arg1, Arg2, Args ...> this_type;

          static const std::size_t  size  =
            std::tuple_size<weightings_type>::value;

          Collection()
            : mAlphas(size, Parameter("\u03b1",
                size == 0 ? 1 : 1.f / size,
                0,
                1))
          {
          }

          template <class S1, class S2, class ... Ss>
          Collection(const String& inName, S1&& arg1, S2&& arg2, Ss&& ... args)
            : parent_type(inName)
            , mWeightings(std::forward<S1>(arg1), std::forward<S2>(
                arg2), std::forward<Ss>(args) ...)
            , mAlphas(size, Parameter("\u03b1",
                size == 0 ? 1 : 1.f / size,
                0,
                1))
          {
          }

          weightings_type&    weightings()
          {
            return mWeightings;
          }

          const weightings_type&  weightings() const
          {
            return mWeightings;
          }

          alphas_type&      alphas()
          {
            return mAlphas;
          }

          const alphas_type&    alphas() const
          {
            return mAlphas;
          }

          void accept(SetValue& visitor)
          {
            detail::visit_collection_helper<size, this_type>::visit(visitor,
              *this);
            normalize_alphas();
          }

          template <class Visitor>
          void accept(Visitor& visitor)
          {
            detail::visit_collection_helper<size, this_type>::visit(visitor,
              *this);
          }

          void accept(GetTitle& visitor) const
          {
            GetTitle  prefixVisitor(&visitor, this->name() + " ");
            detail::visit_collection_helper<size, const this_type>::visit(
              prefixVisitor,
              *this);
          }

          template <class Visitor>
          void accept(Visitor& visitor) const
          {
            detail::visit_collection_helper<size, const this_type>::visit(
              visitor,
              *this);
          }

          Record model() const
          {
            Record record;
            detail::model_collection_helper<size, const this_type>::visit(
              record,
              *this);
            return record;
          }

        protected:
          double sumAlphas() const
          {
            double  sum = 0;
            for (const auto& x : alphas()) {
              sum += x.value();
            }
            return sum;
          }

          void normalize_alphas()
          {
            double  sum = sumAlphas();
            if (sum == 0) sum = 1;
            for (auto& i : alphas()) {
              i.value() /= sum;
            }
          }

        private:
          weightings_type     mWeightings;
          std::vector<Parameter>  mAlphas;

        };

        template <class Element, class Arg1>
        struct Collection<Element, Arg1>
          : public Weighting
          , public Element
        {
          typedef std::tuple<Arg1>    weightings_type;

          static const std::size_t  size  =
            std::tuple_size<weightings_type>::value;

          Collection() = default;

          template <class S1>
          Collection(const String& inName, S1&& arg1)
            : Weighting(inName)
            , mWeightings(std::forward<S1>(arg1))
          {
          }

          template <class Visitor>
          void accept(Visitor& visitor)
          {
            std::get<0>(mWeightings).accept(visitor);
          }

          void accept(GetTitle& visitor) const
          {
            GetTitle  prefixVisitor(&visitor, this->name() + " ");
            std::get<0>(mWeightings).accept(prefixVisitor);
          }

          template <class Visitor>
          void accept(Visitor& visitor) const
          {
            std::get<0>(mWeightings).accept(visitor);
          }

          weightings_type&    weightings()
          {
            return mWeightings;
          }

          const weightings_type&  weightings() const
          {
            return mWeightings;
          }

          Record model() const
          {
            Record record;
            record.emplace(SUB_WEIGHTING_KEY, std::get<0>(mWeightings).model());
            return record;
          }

        private:
          weightings_type     mWeightings;
        };

#pragma mark - Context

        template <class Index>
        struct Context
        {
          virtual ~Context()
          {
          }

          virtual const Index&  index() const = 0;
        };

      }
    }
  }
}    // namespace jerome::ir::rm

#endif // defined __jerome_ir_rm_weighting_hpp__
