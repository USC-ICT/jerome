//
//  user_class.hpp
//
//  Created by Anton Leuski on 8/21/14.
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

#ifndef __jerome_scripting_javascript_user_class_hpp__
#define __jerome_scripting_javascript_user_class_hpp__

#include <jerome/scripting/javascript/class.hpp>

namespace jerome { namespace javascript {
	struct UserClass : public Class {
		
		virtual const char*	className() const { return mClassName.c_str(); }
		virtual Class	parentClass() { return Class(); }
		
		virtual void*	newVoidPointer() const { return nullptr; }
		virtual void	destroy(void* inData) {}
		
		virtual void	initialize(Value& object) {
			if (!object.getPrivate())
				object.setPrivate(newVoidPointer());
		}
		virtual void	finalize(ValueBase& object) {
			void*	data = object.getPrivate();
			if (data) {
				object.setPrivate(nullptr);
				destroy(data);
			}
		}
		
		virtual bool	hasProperty(Value& object, const String& propertyName)
		{ return false; }
		virtual Value	getProperty(Value& object, const String& propertyName)
		{ throw get_property_undefined(); }
		virtual bool	setProperty(Value& object, const String& propertyName, const Value& value)
		{ return false;  }
		virtual bool	deleteProperty(Value& object, const String& propertyName)
		{ return false; }
		virtual Value	callAsFunction(Value& function, Value& thisObject, const std::vector<Value> arguments)
		{ throw object_is_not_function(); }
		virtual Value	callAsConstructor(Value& function, const std::vector<Value>& arguments)
		{ throw object_is_not_constructor(); }
		virtual bool	hasInstance(Value& constructor, const Value& possibleInstance)
		{ throw object_is_not_constructor(); }
		virtual Value	convertToType(Value& object, Value::Type type)
		{ throw cannot_convert_object(); }
		virtual std::vector<std::string>	getPropertyNames(const Value& object)
		{ return std::vector<std::string>(); }
		
		virtual std::vector<JSStaticValue>		staticValues() { return mStaticValues; }
		virtual std::vector<JSStaticFunction>	staticFunctions() { return mStaticFunctions; }
		
		// convert arguments and call the "objectivezed" counterpart, e.g., initializeCallback -> initialize, ...
		virtual void	initializeCallback(JSContextRef ctx, JSObjectRef object);
		virtual void	finalizeCallback(JSObjectRef object);
		virtual bool	hasPropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName)
		{ return false; }
		virtual JSValueRef getPropertyCallback(JSContextRef ctx, JSObjectRef object,
											   JSStringRef propertyName, JSValueRef* exc)
		{ return nullptr; }
		virtual bool	setPropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName,
											JSValueRef value, JSValueRef* exc)
		{ return false;}
		virtual bool	deletePropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName,
											   JSValueRef* exc)
		{ return false; }
		virtual JSValueRef callAsFunctionCallback(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject,
												  size_t argumentCount, const JSValueRef arguments[], JSValueRef* exc);
		virtual JSObjectRef callAsConstructorCallback(JSContextRef ctx, JSObjectRef function, size_t argumentCount,
													  const JSValueRef arguments[], JSValueRef* exc);
		virtual bool	hasInstanceCallback(JSContextRef ctx, JSObjectRef constructor, JSValueRef possibleInstance,
											JSValueRef* exc);
		virtual JSValueRef convertToTypeCallback(JSContextRef ctx, JSObjectRef object, JSType type, JSValueRef* exc);
		virtual void	getPropertyNamesCallback(JSContextRef ctx, JSObjectRef object,
												 JSPropertyNameAccumulatorRef propertyNames)
		{}
		
	protected:
		std::vector<JSStaticValue>		mStaticValues;
		std::vector<JSStaticFunction>	mStaticFunctions;
		String							mClassName;
		
		UserClass(const String& inName) : mClassName(inName) {}
		
		bool adaptHasPropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName);
		JSValueRef adaptGetPropertyCallback(JSContextRef ctx, JSObjectRef object,
											JSStringRef propertyName, JSValueRef* exc);
		bool adaptSetPropertyCallback(JSContextRef ctx, JSObjectRef object,
									  JSStringRef propertyName, JSValueRef value, JSValueRef* exc);
		bool adaptDeletePropertyCallback(JSContextRef ctx, JSObjectRef object,
										 JSStringRef propertyName, JSValueRef* exc);
		void adaptGetPropertyNamesCallback(JSContextRef ctx, JSObjectRef object,
										   JSPropertyNameAccumulatorRef propertyNames);
		
		// if you want use Value-based callbacks for properties, add to your subclass. By default we rely on static
		// values and functions
		
#define AL_JS_USE_ADAPTED_CALLBACKS \
virtual bool hasPropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName) override { \
return adaptHasPropertyCallback(ctx, object, propertyName); \
} \
virtual JSValueRef getPropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, \
										JSValueRef* exc) override { \
return adaptGetPropertyCallback(ctx, object, propertyName, exc); \
} \
virtual bool setPropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, \
										JSValueRef* exc) override { \
return adaptSetPropertyCallback(ctx, object, propertyName, value, exc); \
} \
virtual bool deletePropertyCallback(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, \
										JSValueRef* exc) override { \
return adaptDeletePropertyCallback(ctx, object, propertyName, exc); \
} \
virtual void getPropertyNamesCallback(JSContextRef ctx, JSObjectRef object, \
										JSPropertyNameAccumulatorRef propertyNames) override { \
adaptGetPropertyNamesCallback(ctx, object, propertyNames); \
}
		
		
	};
	
	template <JSPropertyAttributes A = kJSPropertyAttributeNone>
	struct UserProperty {
		static const JSPropertyAttributes	attributes = A;
		
		static JSValueRef get(JSContextRef ctx, JSObjectRef object,  JSStringRef propertyName, JSValueRef* exc)
		{
			return JSValueMakeUndefined(ctx);
		}
		
		static bool set(JSContextRef ctx, JSObjectRef object, JSStringRef propertyName, JSValueRef value, JSValueRef* exc)
		{
			return false;
		}
	};
	
	template <JSPropertyAttributes A = kJSPropertyAttributeNone>
	struct UserFunction {
		static const JSPropertyAttributes	attributes = A;
		static JSValueRef call(JSContextRef ctx, JSObjectRef function, JSObjectRef thisObject, size_t argumentCount,
							   const JSValueRef arguments[], JSValueRef* exc)
		{
			return JSValueMakeUndefined(ctx);
		}
	};
	
	namespace detail {
		
		
		struct Class_deleter
		{
			void operator()(JSClassRef p)
			{
				if(p) JSClassRelease(p);
			}
		};
		
		template <typename, typename T, T>
		struct Callback;
		
		template <class C, class CR>
		struct ParameterizedClass : public UserClass {
			typedef C					object_type;	// c type to map to JS
			typedef CR					reference_type;	// reference version of the type
			typedef ClassTraits<C>		traits_type;
			
			virtual void*				newVoidPointerWithObject(const object_type& inCObject) const = 0;
			virtual reference_type		representedObject(const ValueBase& inObject) const final
			{ return representedObjectForRef((JSObjectRef)inObject.valueRef()); }
			virtual reference_type		representedObjectForRef(JSObjectRef inObjectRef) const = 0;
			
			static traits_type& instance() {
				traits_type&	x(privateInstance());
				x.init();
				return x;
			}
			
			virtual operator JSClassRef() { return data().get(); }
			virtual operator JSClassRef() const { return data().get(); }
			
		protected:
			using UserClass::UserClass;
			
		private:
			template <typename, typename T, T> friend struct Callback;
			
			static traits_type& privateInstance() {
        // STATIC
				static auto	shared = new traits_type;
				return *shared;
			}
			
			void init() {
				if (!mData) mData = classDescriptionData();
			}
			
			static std::shared_ptr<OpaqueJSClass>& classDescriptionData();
			
			std::shared_ptr<OpaqueJSClass> mData;
			
			std::shared_ptr<OpaqueJSClass>& data() {
				assert(mData);
				return mData;
			}
			
			const std::shared_ptr<OpaqueJSClass>& data() const {
				assert(mData);
				return mData;
			}
			
			// we might need to enable these if we ever play with class hierarchies.
			ParameterizedClass(const ParameterizedClass&) = delete;
			ParameterizedClass(ParameterizedClass&&) = delete;
			
		};
		
		namespace tag {
			enum {
				DEFAULT = 0,
				OBJECT,
				POINTER
			};
			
			template <typename T, typename = void>
			struct tag_for_type;
			
			template <typename T>
			struct tag_for_type<T, typename std::enable_if<std::is_class<T>::value>::type> {
				static constexpr int value = OBJECT;
			};
			
			template <typename T>
			struct tag_for_type<T, typename std::enable_if<std::is_pointer<T>::value>::type> {
				static constexpr int value = POINTER;
			};
			
		}
		
		template <typename T, int = tag::tag_for_type<T>::value>
		struct ClassTemplate;
		
		template <typename C>
		struct ClassTemplate<C, tag::OBJECT> : public ParameterizedClass<C, C&> {
			typedef ParameterizedClass<C, C&>				parent_type;
			typedef typename parent_type::object_type		object_type;
			typedef typename parent_type::reference_type	reference_type;
			
			using parent_type::parent_type;
			
			virtual void*	newVoidPointerWithObject(const object_type& inCObject) const override {
				return reinterpret_cast<void*>(new object_type(inCObject));
			}
			virtual void*	newVoidPointer() const override {
				return reinterpret_cast<void*>(new object_type());
			}
			virtual reference_type		representedObjectForRef(JSObjectRef ref) const override {
				return *reinterpret_cast<object_type*>(JSObjectGetPrivate(ref));
			}
			void	destroy(void* data) override {
				delete reinterpret_cast<object_type*>(data);
			}
			
		};
		
		template <typename C>
		struct ClassTemplate<C, tag::POINTER> : public ParameterizedClass<C, C> {
			typedef ParameterizedClass<C, C>				parent_type;
			typedef typename parent_type::object_type		object_type;
			typedef typename parent_type::reference_type	reference_type;
			
			using parent_type::parent_type;
			
			void*	newVoidPointerWithObject(const object_type& inCObject) const override {
				return reinterpret_cast<void*>((object_type)inCObject);
			}
			void*	newVoidPointer() const override {
				assert(false);
				throw std::logic_error("fatal error: newVoidPointer is undefined");
			}
			reference_type		representedObjectForRef(JSObjectRef ref) const override {
				return reinterpret_cast<object_type>(JSObjectGetPrivate(ref));
			}
			
		};
		
	}

}}


#endif // defined __jerome_scripting_javascript_user_class_hpp__
