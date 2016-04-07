//
//  model.hpp
//
//  Created by Anton Leuski on 1/25/12.
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

#ifndef __jerome_npc_model_hpp__
#define __jerome_npc_model_hpp__

#include <iostream>
#include <iomanip>
//#include <boost/range/iterator_range.hpp>

// boost:range contains an implementation of any_iterator that erases
// the concrete iterator implementation. However, it is not public. If things
// break at some point in the interators, you can always go back to
// using any_iterator implementation from
// http://thbecker.net/free_software_utilities/type_erasure_for_cpp_iterators/any_iterator.html
// for now we remove one library dependecy. Huray!
#include <boost/range/detail/any_iterator.hpp>
// #include <any_iterator.hpp>

#include <boost/range/algorithm/find_if.hpp>
#include <jerome/types.hpp>
#include <jerome/type/Record.hpp>

// I setup the model class hierarchy to abstract the implementation out
// so I can replace an in-memory implementation with a database
// based implementation at will. The way to it is to have virtual base class,
// right? But, we cannot store the class in a stl collection. (virtualization
// will not work) We can store
// pointers, so each class here is a "smart" pointer to the implementation.
// In addition, I need to abstract collection away as well, as an imlementation
// may rely on something other than stl::vector or stl::set.

namespace jerome {
  namespace npc {
    
    namespace detail {
      class ModelObjectImpl;
      class NamedModelObjectImpl;
      class UtteranceImpl;
      class LinkImpl;
      class DomainImpl;
      class LatticeImpl;
      class StateImpl;
      class CollectionImpl;
      class UtteranceIndexImpl;
      template <class T>
      struct Envelope;
      
      template <
      typename Value,
      typename CategoryOrTraversal,
      typename Reference = Value&,
      typename Difference = std::ptrdiff_t
      >
      using any_iterator = boost::range_detail::any_iterator<
      Value,
      CategoryOrTraversal,
      Reference,
      Difference>;
      
      typedef boost::forward_traversal_tag set_traversal_tag;
      //			typedef std::forward_iterator_tag set_traversal_tag;
      
      typedef boost::random_access_traversal_tag ordered_set_traversal_tag;
      //			typedef std::random_access_iterator_tag ordered_set_traversal_tag;
      
      template <class T>
      struct ImmutableSet
      {
        typedef any_iterator<
        T const
        , set_traversal_tag
        > const_iterator;
        
        typedef std::size_t size_type;
        typedef T value_type;
        
        ImmutableSet() = default;
        ImmutableSet(const ImmutableSet&) = delete;
        ImmutableSet(ImmutableSet&&) = delete;
        ImmutableSet& operator = (const ImmutableSet&) = delete;
        ImmutableSet& operator = (ImmutableSet&&) = delete;
        virtual ~ImmutableSet()
        {
        }
        
        virtual const_iterator  begin() const = 0;
        virtual const_iterator  end() const = 0;
        virtual size_type   size()  const = 0;
        virtual bool contains(const T&) const = 0;
      };
      
      template <class T>
      struct Set
      {
        typedef any_iterator<
        T
        , set_traversal_tag
        > iterator;
        
        typedef any_iterator<
        T const
        , set_traversal_tag
        > const_iterator;
        
        typedef std::size_t size_type;
        typedef T value_type;
        
        Set() = default;
        Set(const Set&) = delete;
        Set(Set&&) = delete;
        Set& operator = (const Set&) = delete;
        Set& operator = (Set&&) = delete;
        virtual ~Set()
        {
        }
        
        virtual void insert(const T& v) = 0;
        virtual void remove(const T& v) = 0;
        virtual const_iterator  begin() const = 0;
        virtual const_iterator  end() const = 0;
        virtual iterator    begin()     = 0;
        virtual iterator    end()     = 0;
        virtual size_type   size()  const = 0;
        virtual bool contains(const T&) const = 0;
      };
      
      template <class T>
      struct SetOfNamed : public Set<T>
      {
        virtual optional<T> optionalObjectWithName(const String& inName) const = 0;
      };
      
      template <class T>
      struct OrderedSet
      {
        typedef any_iterator<
        T
        , ordered_set_traversal_tag
        > iterator;
        
        typedef any_iterator<
        T const
        , ordered_set_traversal_tag
        > const_iterator;
        
        typedef std::size_t size_type;
        typedef T value_type;
        
        OrderedSet() = default;
        OrderedSet(const OrderedSet&) = delete;
        OrderedSet(OrderedSet&&) = delete;
        OrderedSet& operator = (const OrderedSet&) = delete;
        OrderedSet& operator = (OrderedSet&&) = delete;
        virtual ~OrderedSet()
        {
        }
        
        virtual T at(size_type index) const = 0;
        virtual void insert(const T& v) = 0;
        virtual void remove(const T& v) = 0;
        virtual void remove(size_type index) = 0;
        virtual const_iterator  begin() const = 0;
        virtual const_iterator  end() const = 0;
        virtual iterator    begin()     = 0;
        virtual iterator    end()     = 0;
        virtual size_type   size()  const = 0;
        virtual bool contains(const T&) const = 0;
      };
      
    }
    
    struct JEROME_SYMBOL_VISIBLE ModelObject
    : public ReferenceClass
    {
    public:
      operator bool () const;
      std::size_t hash() const;
      bool equal_to(const ModelObject& x) const;
      
    protected:
      ModelObject() = default;
      
      typedef detail::ModelObjectImpl impl_type;
      explicit ModelObject(const shared_ptr<impl_type>& inImplementation);
      
      impl_type&       implementation();
      const impl_type& implementation() const;
      
    private:
      shared_ptr<impl_type>  mImplementation;
      
      template <typename T>
      friend struct detail::Envelope;
    };
    
    inline std::size_t hash_value(ModelObject const& x)
    {
      return x.hash();
    }
    
    struct JEROME_SYMBOL_VISIBLE Utterance
    : ModelObject
    {
    public:
      static constexpr const char* NAME = "Utterance";
      static constexpr const char* kFieldID = "id";
      static constexpr const char* kFieldText = "text";
      
      typedef detail::ImmutableSet<String> field_names_type;
      
      struct field_not_found_exception
      : public Exception { using Exception::Exception; };
      
      String  get(const String& key) const noexcept(false);
      String  get(const String& key, const String& inDefault) const;
      bool  has(const String& key) const;
      void  put(const String& key, const String& value);
      void  remove(const String& key);
      const field_names_type&   fieldNames() const;
      double count() const;
      void setCount(double count);
      
      Utterance() = default;
      
    protected:
      
      friend struct ObjectFactory;
      typedef ModelObject parent_type;
      typedef detail::UtteranceImpl impl_type;
      
      explicit Utterance(const shared_ptr<impl_type>& inImplementation);
      
      impl_type&   implementation();
      const impl_type& implementation() const;
    };
    
    typedef optional<Utterance> OptionalUtterance;
    
    struct JEROME_SYMBOL_VISIBLE Link
    : ModelObject
    {
    public:
      static constexpr const char* NAME = "Link";
      typedef uint8_t             value_type;
      typedef detail::Set<Utterance>   utterances_type;
      
      utterances_type&    utterances();
      const utterances_type&  utterances() const;
      value_type value() const;
      void setValue(value_type inValue);
      
    protected:
      Link() = delete;
      
      friend struct ObjectFactory;
      typedef ModelObject parent_type;
      typedef detail::LinkImpl impl_type;
      
      explicit Link(const shared_ptr<impl_type>& inImplementation);
      
      impl_type&   implementation();
      const impl_type& implementation() const;
    };
    
    struct NamedModelObject
    : ModelObject
    {
    public:
      String name() const;
      void setName(const String& inName);
      
    protected:
      NamedModelObject() = delete;
      
      typedef ModelObject parent_type;
      typedef detail::NamedModelObjectImpl impl_type;
      
      explicit NamedModelObject(const shared_ptr<impl_type>& inImplementation);
      
      impl_type&   implementation();
      const impl_type& implementation() const;
    };
    
    struct JEROME_SYMBOL_VISIBLE Domain
    : NamedModelObject
    {
      static constexpr const char* NAME = "Domain";
			
      typedef detail::OrderedSet<Utterance>   utterances_type;
			
      utterances_type&  utterances();
      const utterances_type&  utterances() const;
      
    protected:
      //      Domain() = default;
      
      friend struct ObjectFactory;
      typedef NamedModelObject parent_type;
      typedef detail::DomainImpl impl_type;
      explicit Domain(const shared_ptr<impl_type>& inImplementation);
      
      impl_type&   implementation();
      const impl_type& implementation() const;
      
    };
    
    struct JEROME_SYMBOL_VISIBLE Lattice
    : NamedModelObject
    {
      static constexpr const char* NAME = "Lattice";

      typedef detail::Set<Link>      links_type;
      
      links_type& links();
      const links_type& links() const;
      
    protected:
      //      Lattice() = default;
      
      friend struct ObjectFactory;
      typedef NamedModelObject parent_type;
      typedef detail::LatticeImpl impl_type;
      explicit Lattice(const shared_ptr<impl_type>& inImplementation);
      
      impl_type&   implementation();
      const impl_type& implementation() const;
    };
		
    struct JEROME_SYMBOL_VISIBLE State
    : NamedModelObject
    {
      static constexpr const char* NAME = "State";
			static constexpr const char* PARAMETERS_KEY = "parameters";
			
      Domain   answers() const;
      void setAnswers(const Domain&  inAnswers);
      Domain   questions() const;
      void setQuestions(const Domain& inQuestions);
      Lattice  links() const;
      void setLinks(const Lattice& inLinks);
      
      
      typedef Record ranker_model_type;
      
      // note that in contrast to answers() or links(), this
      // gives you the value object. So if you want change some parts
      // get a copy, make changes and assign the copy back.
      ranker_model_type rankerModel() const;
      void setRankerModel(const ranker_model_type& inClassifier);
      
    protected:
      //      State() = default;
      
      friend struct ObjectFactory;
      typedef NamedModelObject parent_type;
      typedef detail::StateImpl impl_type;
      explicit State(const shared_ptr<impl_type>& inImplementation);
      impl_type&   implementation();
      const impl_type& implementation() const;
    };
    
    typedef optional<State> OptionalState;
    
    struct JEROME_SYMBOL_VISIBLE Collection
    : ModelObject
    {
      static constexpr const char* NAME = "Collection";
			
      typedef detail::SetOfNamed<Domain>     domains_type;
      typedef detail::SetOfNamed<State>      states_type;
      typedef detail::SetOfNamed<Lattice>    lattices_type;
      typedef detail::Set<Link>       links_type;
      typedef detail::Set<Utterance>  utterances_type;
      
      domains_type&     domains();
      const domains_type&   domains() const;
      states_type&      states();
      const states_type&    states() const;
      lattices_type&      lattices();
      const lattices_type&  lattices() const;
      links_type&       links();
      const links_type&   links() const;
      utterances_type&    utterances();
      const utterances_type&  utterances() const;
      
      struct utterance_index_type
      {
        OptionalUtterance  utteranceWithFieldValue(const String& field,
                                                   const String& value);
        OptionalUtterance  utteranceWithFieldValue(const String& field,
                                                   const String& value) const;
        String     expandFieldValue(const String& field,
                                    const String& inValue) const;
        operator bool () const;
        
      protected:
        typedef detail::UtteranceIndexImpl impl_type;
        explicit utterance_index_type(const shared_ptr<impl_type>&);
        friend detail::CollectionImpl;
        
      private:
        shared_ptr<impl_type>  mImplementation;
      };
      
      utterance_index_type  utterance_index() const;
      
    protected:
      //      Collection() = default;
      
      friend struct ObjectFactory;
      typedef ModelObject parent_type;
      typedef detail::CollectionImpl impl_type;
      explicit Collection(const shared_ptr<impl_type>& inImplementation);
      impl_type&   implementation();
      const impl_type& implementation() const;
    };
    
    namespace detail {
      template <typename T>
      struct ObjectFactoryHelper {};
    }
    
    // The universal object making method:
    //	ObjectFactory of;
    // of.makeNew<Utterance>();
    
    struct ObjectFactory
    {
      virtual ~ObjectFactory();
      
      template <typename Obj, typename ... Args>
      Obj makeNew(Args&& ... args) const
      {
        return detail::ObjectFactoryHelper<Obj>::create(this,
                                                        std::forward<Args>(args) ...);
      }
      
    protected:
      template <typename T>
      friend struct detail::Envelope;
      
      template <typename Impl, typename ... Args>
      static typename Impl::interface_type makeWithImpl(Args&& ... args)
      {
        return typename Impl::interface_type(
                                             std::make_shared<Impl>(std::forward<Args>(args) ...));
      }
      
    private:
      virtual Utterance   newUtterance() const = 0;
      virtual Link      newLink() const = 0;
      virtual Lattice     newLattice() const = 0;
      virtual Domain      newDomain() const = 0;
      virtual State     newState(const Domain& inQuestions,
                                 const Domain& inAnswers,
                                 const Lattice& inLinks) const = 0;
      virtual Collection    newCollection() const = 0;
      
      template <typename T>
      friend struct detail::ObjectFactoryHelper;
    };
    
    namespace detail {
      
#define JEROME_OBJECT_FACTORY_HELPER(C, F) \
template <> \
struct ObjectFactoryHelper<C> { \
static C create(const ObjectFactory* of) { \
return of->F(); \
} \
};
      
      JEROME_OBJECT_FACTORY_HELPER(Utterance, newUtterance)
      JEROME_OBJECT_FACTORY_HELPER(Link, newLink)
      JEROME_OBJECT_FACTORY_HELPER(Lattice, newLattice)
      JEROME_OBJECT_FACTORY_HELPER(Domain, newDomain)
      JEROME_OBJECT_FACTORY_HELPER(Collection, newCollection)
      
#undef JEROME_OBJECT_FACTORY_HELPER
      
      template <>
      struct ObjectFactoryHelper<State> {
        static State create(const ObjectFactory* of, const Domain& inQuestions,
                            const Domain& inAnswers,
                            const Lattice& inLinks) {
          return of->newState(inQuestions, inAnswers, inLinks);
        }
      };
      
      
    }
    
  }
}

namespace jerome {
  namespace npc {
    
    struct FieldValueExpander
    : std::unary_function<const Utterance&, Utterance>
    {
      FieldValueExpander(const Collection::utterance_index_type& inIndex);
      Utterance operator () (const Utterance& inUtterance) const;
      
    private:
      Collection::utterance_index_type  mIndex;
    };
  }
}

namespace std {
  
#define HASH_AND_EQUAL_TO(C) \
template <> \
struct hash<C> \
: public unary_function<C, std::size_t> { \
std::size_t operator () (C const& x) const { return x.hash(); } \
}; \
template <> \
struct equal_to<C> \
: public binary_function<C, C, bool> { \
bool operator () (const C& x, const C& y) const \
{ return x.equal_to(y); } \
};
  
  HASH_AND_EQUAL_TO(jerome::npc::ModelObject)
  HASH_AND_EQUAL_TO(jerome::npc::Utterance)
  HASH_AND_EQUAL_TO(jerome::npc::Link)
  HASH_AND_EQUAL_TO(jerome::npc::Domain)
  HASH_AND_EQUAL_TO(jerome::npc::Lattice)
  HASH_AND_EQUAL_TO(jerome::npc::State)
  
}

#endif // defined __jerome_npc_model_hpp__
