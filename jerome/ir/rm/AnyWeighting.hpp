//
//  AnyWeighting.hpp
//
//  Created by Anton Leuski on 8/27/15.
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

#ifndef __jerome_ir_rm_AnyWeighting_hpp__
#define __jerome_ir_rm_AnyWeighting_hpp__

#include <jerome/types.hpp>
#include <jerome/type/matrix.hpp>
#include <jerome/ir/rm/weighting.hpp>
#include <jerome/math/parameters/parameterized.hpp>

namespace jerome {
  namespace ir {
    namespace rm {

      using namespace jerome::math;
      using namespace jerome::math::parameters;

      namespace detail {
        template <typename Index>
        struct AnyQueryWeightingImplementation;
        template <typename Index>
        struct AnyDocumentWeightingImplementation;
      }

      template <typename Derived, typename Impl>
      class AnyWeighting
        : public ParameterizedHelper<
          Derived,
          ReferenceClassInterface<Impl>
          >
      {
        typedef ParameterizedHelper<
          Derived,
          ReferenceClassInterface<Impl>
          > parent_type;

      public:
        using parent_type::parent_type;
				typedef AnyWeighting<Derived, Impl>	any_type;
				typedef Impl impl_type;

        template <template <typename ...> class W, typename ... Args>
        static Derived make(const String& inName, Args&& ... args)
        {
          return Derived(make_impl<W>(inName, std::forward<Args>(args) ...));
        }

        template <class Visitor, class Visitable>
        static void visit(Visitor& visitor, Visitable& visitable)
        {
          visitable.implementation().accept(visitor);
        }

        Record model() const
        {
          return this->implementation().model();
        }

			protected:
				template <template <typename ...> class W, typename ... Args>
				static shared_ptr<typename Impl::template Template<W<Args ...>>>
				make_impl(const String& inName, Args&& ... args)
				{
					return std::make_shared<typename Impl::template Template<W<
												 Args ...>>>(inName, std::forward<Args>(args) ...);
				}

      };

      template <typename Derived, typename Index>
      class AnyQueryWeighting
        : public AnyWeighting<
          Derived,
          detail::AnyQueryWeightingImplementation<Index>>
      {
        typedef AnyWeighting<
          Derived,
          detail::AnyQueryWeightingImplementation<Index>> parent_type;
        friend parent_type;

      public:
        using parent_type::parent_type;
				typedef typename parent_type::any_type any_type;
				typedef typename parent_type::impl_type impl_type;

        WeightMatrix computeQueryModelForQuery(
          const jerome::ir::rm::weighting::Context<Index>& inContext,
          const Index& inQuery) const
        {
          return this->implementation().computeQueryModelForQuery(inContext,
            inQuery);
        }

      };

      template <typename Derived, typename Index>
      class AnyDocumentWeighting
        : public AnyWeighting<
          Derived,
          detail::AnyDocumentWeightingImplementation<Index>>
      {
        typedef AnyWeighting<
          Derived,
          detail::AnyDocumentWeightingImplementation<Index>> parent_type;
        friend parent_type;

      public:
        using parent_type::parent_type;
				typedef typename parent_type::any_type any_type;
				typedef typename parent_type::impl_type impl_type;

        WeightMatrix computeAffinity(
          const jerome::ir::rm::weighting::Context<Index>& inContext) const
        {
          return this->implementation().computeAffinity(inContext);
        }

        WeightMatrix computeAffinityInitialWeight(
          const jerome::ir::rm::weighting::Context<Index>& inContext,
          const WeightMatrix& inModel) const
        {
          return this->implementation().computeAffinityInitialWeight(inContext,
            inModel);
        }

      };

      namespace detail {

        struct AnyWeightingImplementation
        {
          virtual ~AnyWeightingImplementation()
          {
          }

          virtual void accept(GetValue& visitor)  const   = 0;
          virtual void accept(GetTitle& visitor)  const   = 0;
          virtual void accept(GetRange& visitor)  const   = 0;
          virtual void accept(SetValue& visitor)      = 0;
          virtual Record model() const
          {
            return Record();
          }

        };

        template <typename Index>
        struct AnyQueryWeightingImplementation
          : public AnyWeightingImplementation
        {
          virtual WeightMatrix computeQueryModelForQuery(
            const jerome::ir::rm::weighting::Context<Index>& inContext,
            const Index& inQuery) const = 0;

          template <typename W>
          struct Template
            : public AnyQueryWeightingImplementation
            , W
          {
            template <typename ... Args>
            Template(Args&& ... args)
              : W(std::forward<Args>(args) ...)
            {
            }

            void accept(GetValue& visitor)  const override
            {
              W::accept(visitor);
            }

            void accept(GetTitle& visitor)  const override
            {
              W::accept(visitor);
            }

            void accept(GetRange& visitor)  const override
            {
              W::accept(visitor);
            }

            void accept(SetValue& visitor)        override
            {
              W::accept(visitor);
            }

            Record model()                  const override
            {
              return W::model();
            }

            WeightMatrix computeQueryModelForQuery(
              const jerome::ir::rm::weighting::Context<Index>& inContext,
              const Index& inQuery) const override
            {
              return W::computeQueryModelForQuery(inContext, inQuery);
            }

          };

        };

        template <typename Index>
        struct AnyDocumentWeightingImplementation
          : public AnyWeightingImplementation
        {

          virtual WeightMatrix computeAffinity(
            const jerome::ir::rm::weighting::Context<Index>& inContext) const = 0;
          virtual WeightMatrix computeAffinityInitialWeight(
            const jerome::ir::rm::weighting::Context<Index>& inContext,
            const WeightMatrix& inModel) const = 0;

          template <typename W>
          struct Template
            : public AnyDocumentWeightingImplementation
            , W
          {
            template <typename ... Args>
            Template(Args&& ... args)
              : W(std::forward<Args>(args) ...)
            {
            }

            void accept(GetValue& visitor)  const override
            {
              W::accept(visitor);
            }

            void accept(GetTitle& visitor)  const override
            {
              W::accept(visitor);
            }

            void accept(GetRange& visitor)  const override
            {
              W::accept(visitor);
            }

            void accept(SetValue& visitor)        override
            {
              W::accept(visitor);
            }

            Record model()                  const override
            {
              return W::model();
            }

            WeightMatrix computeAffinity(
              const jerome::ir::rm::weighting::Context<Index>& inContext) const
            override
            {
              return W::computeAffinity(inContext);
            }

            WeightMatrix computeAffinityInitialWeight(
              const jerome::ir::rm::weighting::Context<Index>& inContext,
              const WeightMatrix& inModel) const override
            {
              return W::computeAffinityInitialWeight(inContext, inModel);
            }

          };

        };

      }

    }
  }
}

#endif // defined __jerome_ir_rm_AnyWeighting_hpp__
