//
//  model_cpp.cpp
//
//  Created by Anton Leuski on 9/16/12.
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

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <jerome/npc/detail/model_impl.hpp>
#include <jerome/npc/model_cpp.hpp>

namespace jerome {
	
		// for iterating over map keys.
	template <typename Iter>
	boost::transform_iterator<KeyGetter<Iter>, Iter,
	const typename Iter::value_type::first_type&> KeyIterator(Iter iterator)
	{
		return boost::transform_iterator<
		KeyGetter<Iter>,
		Iter,
		const typename Iter::value_type::first_type&>
		(iterator, KeyGetter<Iter>());
	}
	

  namespace npc {
    namespace cpp {
      
      template <class T, typename Base = jerome::npc::detail::Set<T>>
      struct Set
      : public Base
      {
        typedef typename jerome::npc::detail::Set<T>    parent_type;
        typedef std::unordered_set<T>                 storage_type;
        typedef typename parent_type::iterator        iterator;
        typedef typename parent_type::const_iterator  const_iterator;
        typedef typename parent_type::size_type       size_type;
        
        Set() = default;
        
        void insert(const T& v) override
        {
          mStorage.insert(v);
        }
        
        void remove(const T& v) override
        {
          mStorage.erase(v);
        }
        
        iterator    begin() override
        {
          return iterator(mStorage.begin());
        }
        
        iterator    end() override
        {
          return iterator(mStorage.end());
        }
        
        const_iterator  begin() const override
        {
          return const_iterator(mStorage.begin());
        }
        
        const_iterator  end() const override
        {
          return const_iterator(mStorage.end());
        }
        
        size_type   size()  const override
        {
          return mStorage.size();
        }
        
        bool contains(const T& v) const override
        {
          return mStorage.find(v) != mStorage.end();
        }
        
      private:
        storage_type  mStorage;
      };
      
      template <class T, typename Base = jerome::npc::detail::SetOfNamed<T>>
      struct SetOfNamed
      : public Set<T, Base>
      {
        optional<T> optionalObjectWithName(const String& inName) const override
        {
          auto i = boost::find_if(*this,
                                  [inName] (const T& x) {
                                    return x.name() == inName;
                                  });
          return (i == this->end())
          ? optional<T>()
          : optional<T>(*i);
        }
      };
      
      template <class T>
      struct OrderedSet
      : jerome::npc::detail::OrderedSet<T>
      {
      private:
        typedef typename jerome::npc::detail::OrderedSet<T>     parent_type;
        typedef boost::multi_index_container<
        T,
        boost::multi_index::indexed_by<
        boost::multi_index::random_access<>, // vector-like index
        boost::multi_index::hashed_unique<boost::multi_index::identity<T>>
        >
        > storage_type;
        typedef typename storage_type::template nth_index<1>::type hashed_values;
        
      public:
        typedef typename parent_type::iterator        iterator;
        typedef typename parent_type::const_iterator  const_iterator;
        typedef typename parent_type::size_type       size_type;
        
        OrderedSet()
        : mStorage()
        , mHashedValues(mStorage.template get<1>())
        {
        }
        
        void insert(const T& v) override
        {
          if (contains(v)) return;
          mStorage.push_back(v);
        }
        
        void remove(const T& v) override
        {
          mHashedValues.erase(v);
        }
        
        void remove(size_type index) override
        {
          mStorage.erase(mStorage.begin() + index);
        }
        
        iterator    begin() override
        {
          return iterator(mStorage.begin());
        }
        
        iterator    end() override
        {
          return iterator(mStorage.end());
        }
        
        const_iterator  begin() const override
        {
          return const_iterator(mStorage.begin());
        }
        
        const_iterator  end() const override
        {
          return const_iterator(mStorage.end());
        }
        
        size_type   size()  const override
        {
          return mStorage.size();
        }
        
        T       at(size_type index) const override
        {
          return mStorage[index];
        }
        
        bool contains(const T& v) const override
        {
          return mHashedValues.find(v) != mHashedValues.end();
        }
        
      private:
        storage_type    mStorage;
        hashed_values&  mHashedValues;
      };
      
      template <typename Base = detail::ModelObjectImpl>
      struct ModelObjectImpl
      : public Base
      {
        //	const String& identifier() const { return mId; }
        //	void setIdentifier(const String& inID) { mId = inID; }
      };
      
      template <typename Base>
      struct NamedModelObjectImpl
      : public ModelObjectImpl<Base>
      {
        String name() const override
        {
          return mName;
        }
        
        void setName(const String& inName) override
        {
          mName = inName;
        }
        
      private:
        String  mName;
      };
      
      struct SetOfKeys
      : jerome::npc::detail::ImmutableSet<String>
      {
        typedef typename jerome::npc::detail::ImmutableSet<String> parent_type;
        typedef typename parent_type::const_iterator  const_iterator;
        typedef StringMap<String> map_type;
        
        SetOfKeys(const map_type& map)
        : mMap(map)
        {
        }
        
        const_iterator  begin() const
        {
          return const_iterator(KeyIterator(mMap.cbegin()));
        }
        
        const_iterator  end() const
        {
          return const_iterator(KeyIterator(mMap.cend()));
        }
        
        std::size_t   size()  const
        {
          return mMap.size();
        }
        
        bool contains(const String& s) const
        {
          return mMap.find(s) != mMap.end();
        }
        
      private:
        const map_type& mMap;
      };
      
      struct UtteranceImpl
      : ModelObjectImpl<detail::UtteranceImpl>
      {
        UtteranceImpl()
        : mFields()
        , mFieldNames(mFields)
        {
        }
        
        String get(const String& key) const
        {
          StringStringMap::const_iterator i = mFields.find(key);
          if (i == mFields.end())
            throw field_not_found_exception(key);
          return i->second;
        }
        
        String get(const String& key, const String& inDefault) const
        {
          StringStringMap::const_iterator i = mFields.find(key);
          return (i == mFields.end()) ? inDefault : i->second;
        }
        
        bool has(const String& key) const
        {
          return mFields.find(key) != mFields.end();
        }
        
        void put(const String& key, const String& value)
        {
          if (has(key)) {
            mFields.erase(key);
          }
          mFields.emplace(key, value);
        }
        
        void remove(const String& key)
        {
          if (has(key)) {
            mFields.erase(key);
          }
        }
        
        const detail::UtteranceImpl::field_names_type& fieldNames() const
        {
          return mFieldNames;
        }
        
        double count() const
        {
          return mCount;
        }
        
        void setCount(double inCount)
        {
          mCount = inCount;
        }
        
      private:
        typedef StringMap<String>   StringStringMap;
        StringStringMap mFields;
        SetOfKeys mFieldNames;
        double mCount;
      };
      
      struct LinkImpl
      : ModelObjectImpl<detail::LinkImpl>
      {
        detail::LinkImpl::utterances_type&    utterances()
        {
          return mUtterances;
        }
        
        const utterances_type&  utterances() const
        {
          return mUtterances;
        }
        
        detail::LinkImpl::value_type value() const
        {
          return mValue;
        }
        
        void setValue(value_type inValue)
        {
          mValue = inValue;
        }
        
      private:
        Set<Utterance>      mUtterances;
        value_type          mValue;
      };
      
      struct LatticeImpl
      : NamedModelObjectImpl<detail::LatticeImpl>
      {
        links_type& links()
        {
          return mLinks;
        }
        
        const links_type& links() const
        {
          return mLinks;
        }
        
      private:
        Set<Link>        mLinks;
      };
      
      struct DomainImpl
      : NamedModelObjectImpl<detail::DomainImpl>
      {
        utterances_type&  utterances()
        {
          return mUtterances;
        }
        
        const utterances_type&  utterances() const
        {
          return mUtterances;
        }
        
      private:
        OrderedSet<Utterance>    mUtterances;
      };
      
      struct StateImpl
      : NamedModelObjectImpl<detail::StateImpl>
      {
        StateImpl(const Domain& inQuestions,
                  const Domain& inAnswers,
                  const Lattice& inLinks)
        : mQuestions(inQuestions)
        , mAnswers(inAnswers)
        , mLinks(inLinks)
        {
        }
        
        Domain   answers() const override
        {
          return mAnswers;
        }
        
        void          setAnswers(const Domain& inAnswers) override
        {
          mAnswers = inAnswers;
        }
        
        Domain   questions() const override
        {
          return mQuestions;
        }
        
        void          setQuestions(const Domain& inQuestions) override
        {
          mQuestions = inQuestions;
        }
        
        Lattice    links() const override
        {
          return mLinks;
        }
        
        void          setLinks(const Lattice& inLinks) override
        {
          mLinks = inLinks;
        }
        
        State::ranker_model_type  rankerModel() const override
        {
          return mClassifier;
        }
        
        void setRankerModel(const State::ranker_model_type& inClassifier) override
        {
          mClassifier = inClassifier;
        }
        
      private:
        Domain   mAnswers;
        Domain   mQuestions;
        Lattice  mLinks;
        State::ranker_model_type   mClassifier;
      };
      
      struct CollectionImpl
      : ModelObjectImpl<detail::CollectionImpl>
      , std::enable_shared_from_this<CollectionImpl>
      {
        domains_type&     domains()
        {
          return mDomains;
        }
        
        const domains_type&   domains() const
        {
          return mDomains;
        }
        
        states_type&      states()
        {
          return mStates;
        }
        
        const states_type&    states() const
        {
          return mStates;
        }
        
        lattices_type&      lattices()
        {
          return mLattices;
        }
        
        const lattices_type&  lattices() const
        {
          return mLattices;
        }
        
        links_type&       links()
        {
          return mLinks;
        }
        
        const links_type&   links() const
        {
          return mLinks;
        }
        
        utterances_type&    utterances()
        {
          return mUtterances;
        }
        
        const utterances_type&  utterances() const
        {
          return mUtterances;
        }
        
        utterance_index_type  utterance_index() const;
        
      private:
        SetOfNamed<Domain>     mDomains;
        SetOfNamed<State>      mStates;
        SetOfNamed<Lattice>    mLattices;
        Set<Link>       mLinks;
        Set<Utterance>  mUtterances;
      };
      
      struct UtteranceIndexImpl
      : public detail::UtteranceIndexImpl
      {
        UtteranceIndexImpl(const shared_ptr<const CollectionImpl>& inCollection)
        :
        mCollection(inCollection)
        {
        }
        
        OptionalUtterance utteranceWithFieldValue(const String& field,
                                                  const String& value);
        
      private:
        shared_ptr<const CollectionImpl>  mCollection;
        
        typedef StringMap<Utterance> StringUtteranceMap;
        typedef shared_ptr<StringUtteranceMap>  StringUtteranceMapPtr;
        typedef StringMap<StringUtteranceMapPtr>  StringStringUtteranceMap;
        
        StringStringUtteranceMap  mMap;
      };
      
      Utterance    ObjectFactory::newUtterance() const
      {
        return makeWithImpl<UtteranceImpl>();
      }
      Link       ObjectFactory::newLink() const
      {
        return makeWithImpl<LinkImpl>();
      }
      Lattice      ObjectFactory::newLattice() const
      {
        return makeWithImpl<LatticeImpl>();
      }
      Domain     ObjectFactory::newDomain() const
      {
        return makeWithImpl<DomainImpl>();
      }
      State      ObjectFactory::newState(const Domain& inQuestions,
                                         const Domain& inAnswers,
                                         const Lattice& inLinks) const
      {
        return makeWithImpl<StateImpl>(inQuestions, inAnswers, inLinks);
      }
      Collection   ObjectFactory::newCollection() const
      {
        return makeWithImpl<CollectionImpl>();
      }
      
      Collection::utterance_index_type CollectionImpl::utterance_index() const
      {
        return makeUtteranceIndex<UtteranceIndexImpl>(shared_from_this());
      }
      
      OptionalUtterance UtteranceIndexImpl::utteranceWithFieldValue(
                                                                    const String& field,
                                                                    const String& value)
      {
        StringStringUtteranceMap::const_iterator  i = mMap.find(field);
        StringUtteranceMapPtr           mapptr;
        if (i == mMap.end()) {
          mapptr  = StringUtteranceMapPtr(new StringUtteranceMap);
          mMap.emplace(field, mapptr);
          for (const jerome::npc::Utterance& u : mCollection->utterances()) {
            if (u.has(field)) {
              mapptr->emplace(u.get(field), u);
            }
          }
        } else {
          mapptr  = i->second;
        }
        
        StringUtteranceMap::const_iterator  j = mapptr->find(value);
        if (j != mapptr->end()) {
          return j->second;
        }
        
        return OptionalUtterance();
      }
      
    }
  }
}