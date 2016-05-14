//
//  weighting.hpp
//
//  Created by Anton Leuski on 4/11/12.
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

          template <class Idx, typename INDEX_TYPE, typename VALUE_TYPE>
          struct document_weight_functor {
            document_weight_functor(double inLambda, const typename Idx::Field::DocumentLengths& inDocumentLength)
            : mLambda(inLambda)
            , mDocumentLength(inDocumentLength)
            {}
            
            double operator() (const INDEX_TYPE& inIndex, const VALUE_TYPE& inValue)
            {
              return inValue * mLambda / mDocumentLength[inIndex];
            }
            
          private:
            double mLambda;
            const typename Idx::Field::DocumentLengths& mDocumentLength;
          };
          
          template <class Index>
          SparseWeightVector document_weight(const typename Index::Term& term,
                                             const typename Index::Field& field)
          const
          {
            SparseWeightVector  result((traits<SparseWeightVector>::size_type)field.documentCount());
            document_weight_functor<Index,
              typename traits<typename Index::Term::Frequencies>::size_type,
              typename traits<typename Index::Term::Frequencies>::value_type
            >  functor(lambda().value(), field.documentLengths());
            
            JEROME_FOR_EACH_ELEMENT_OF_SPARSE_VECTOR(i, term.tfs(), typename Index::Term::Frequencies) {
              JEROME_MATRIX_SET_VECTOR_ELEMENT_AT_INDEX_TO(result,
                                                           (JEROME_SPARSE_VECTOR_ELEMENT_INDEX(i)), functor(JEROME_SPARSE_VECTOR_ELEMENT_INDEX(i),
                                                                                                            JEROME_SPARSE_VECTOR_ELEMENT_VALUE(i)));
            }
            return result;
            // it causes divide by zero. I guess, it tries to compute 1/|Dl| for
            // each document even for those that tf = 0.
            // return element_div(term.tfs() * lambda().value(),
            // field.documentLengths());
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
