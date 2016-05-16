//
//  model_impl.hpp
//
//  Created by Anton Leuski on 8/21/15.
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

#ifndef __jerome_npc_detail_model_impl_hpp__
#define __jerome_npc_detail_model_impl_hpp__

#include <jerome/npc/model.hpp>

namespace jerome {
  namespace npc {
    namespace detail {

      template <class T>
      struct Envelope;

      struct ModelObjectImpl
      {
        typedef ModelObject interface_type;
        virtual ~ModelObjectImpl();
        virtual std::size_t hash() const
        {
          std::size_t x = (std::size_t)dynamic_cast<const ModelObjectImpl*>(this);
					return x;
        }

        virtual bool equal_to(const ModelObjectImpl& x) const
        {
          return baseThis() == x.baseThis();
        }

      private:
        template <class T>
        friend struct Envelope;

        virtual const ModelObjectImpl * baseThis() const
        {
          return dynamic_cast<const ModelObjectImpl*>(this);
        }
      };

      struct NamedModelObjectImpl
        : public ModelObjectImpl
      {
        typedef NamedModelObject interface_type;
        virtual String name() const = 0;
        virtual void setName(const String& inName) = 0;
      };

      struct UtteranceImpl
        : public ModelObjectImpl
      {
        typedef Utterance interface_type;
        typedef Utterance::field_names_type field_names_type;
        typedef Utterance::field_not_found_exception
          field_not_found_exception;

        virtual String  get(const String& key) const = 0;
        virtual String  get(const String& key,
                            const String& inDefault) const = 0;
        virtual bool  has(const String& key) const = 0;
        virtual void  put(const String& key, const String& value) = 0;
        virtual void  remove(const String& key) = 0;

        virtual const field_names_type& fieldNames() const = 0;

				virtual double count() const = 0;
				virtual void setCount(double inCount) = 0;
      };

      struct LinkImpl
        : public ModelObjectImpl
      {
        typedef Link interface_type;
        typedef Link::value_type       value_type;
        typedef Link::utterances_type  utterances_type;

        virtual utterances_type&    utterances() = 0;
        virtual const utterances_type&  utterances() const = 0;

        virtual value_type value() const = 0;
        virtual void setValue(value_type inValue) = 0;
      };

      struct LatticeImpl
        : public NamedModelObjectImpl
      {
        typedef Lattice interface_type;
        typedef Lattice::links_type      links_type;

        virtual links_type& links() = 0;
        virtual const links_type& links() const = 0;
      };

      struct DomainImpl
        : public NamedModelObjectImpl
      {
        typedef Domain interface_type;
        typedef Domain::utterances_type  utterances_type;

        virtual utterances_type&  utterances() = 0;
        virtual const utterances_type&  utterances() const = 0;
      };

      struct StateImpl
        : public NamedModelObjectImpl
      {
        typedef State interface_type;
        virtual Domain   answers() const = 0;
        virtual void setAnswers(const Domain&  inAnswers) = 0;
        virtual Domain   questions() const = 0;
        virtual void setQuestions(const Domain& inQuestions) = 0;
        virtual Lattice    links() const = 0;
        virtual void setLinks(const Lattice& inLinks) = 0;
        virtual State::ranker_model_type  rankerModel() const = 0;
        virtual void setRankerModel(const State::ranker_model_type& inClassifier) = 0;
      };

      struct CollectionImpl
        : public ModelObjectImpl
      {
        typedef Collection interface_type;
        typedef Collection::domains_type    domains_type;
        typedef Collection::states_type   states_type;
        typedef Collection::lattices_type   lattices_type;
        typedef Collection::links_type    links_type;
        typedef Collection::utterances_type utterances_type;
        typedef Collection::utterance_index_type utterance_index_type;

        virtual domains_type&     domains() = 0;
        virtual const domains_type&   domains() const = 0;

        virtual states_type&      states() = 0;
        virtual const states_type&    states() const = 0;

        virtual lattices_type&      lattices() = 0;
        virtual const lattices_type&  lattices() const  = 0;

        virtual links_type&       links() = 0;
        virtual const links_type&   links() const = 0;

        virtual utterances_type&    utterances() = 0;
        virtual const utterances_type&  utterances() const = 0;

        virtual utterance_index_type  utterance_index() const = 0;

      protected:
        template <typename T, typename ... Args>
        static utterance_index_type makeUtteranceIndex(Args&& ... args)
        {
          return utterance_index_type(std::make_shared<T>(std::forward<Args>(
                args) ...));
        }

      };

      struct UtteranceIndexImpl
      {
        virtual ~UtteranceIndexImpl();
        virtual OptionalUtterance  utteranceWithFieldValue(const String& field,
                                                           const String& value)
          = 0;
      };

      template <class T>
      struct Envelope
        : public T
      {
        typedef T opbject_type;
        typedef shared_ptr<T> pointer_type;
        typedef typename T::interface_type interface_type;

        explicit Envelope(const interface_type& inSource)
          : mSource(std::static_pointer_cast<T>(inSource.mImplementation))
        {
        }

        explicit Envelope(const pointer_type& inSource)
          : mSource(inSource)
        {
        }

        virtual ~Envelope()
        {
        }

      protected:
        template <typename Impl, typename ... Args>
        static interface_type envelop(Args&& ... args)
        {
          return ObjectFactory::makeWithImpl<Impl>(std::forward<Args>(args) ...);
        }

        const opbject_type& source() const
        {
          return *mSource;
        }

        opbject_type& source()
        {
          return *mSource;
        }

      private:
        pointer_type mSource;

        virtual const ModelObjectImpl * baseThis() const
        {
          return mSource->baseThis();
        }

      };

      struct UtteranceEnvelope
        : public Envelope<UtteranceImpl>
      {
        typedef Envelope<UtteranceImpl> parent_type;

        explicit UtteranceEnvelope(const pointer_type& inSource);
        explicit UtteranceEnvelope(const Utterance& inSource);
        std::size_t hash() const override;
        bool equal_to(const ModelObjectImpl& x) const override;
        String get(const String& key) const override;
        String get(const String& key, const String& inDefault) const override;
        bool has(const String& key) const override;
        void put(const String& key, const String& value) override;
        void remove(const String& key) override;
        const field_names_type& fieldNames() const  override;
        double count() const override;
        void setCount(double inCount) override;
      };
    }
  }
}
#endif // defined __jerome_npc_detail_model_impl_hpp__
