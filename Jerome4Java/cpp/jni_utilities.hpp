//
//  jni_utilities.hpp
//
//  Created by Anton Leuski on 9/4/19.
//  Copyright © 2019 Anton Leuski & ICT/USC. All rights reserved.
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

#ifndef jni_utilities_hpp
#define jni_utilities_hpp

#ifdef __APPLE__
# include<JavaVM/jni.h>
#else
# include<jni.h>
#endif

std::string to_string(JNIEnv * inEnv, jstring inString);
jmethodID method(JNIEnv* inEnv, jobject inObject, 
                 const char* inName, const char* inSignature);

jobject newObject(JNIEnv* inEnv, 
                  const char* inClassName, 
                  const char* inConstructorSignature,
                  ...);

struct AttachedThreadRegion {
  static void initialize(JNIEnv* inEnv);
  AttachedThreadRegion();
  ~AttachedThreadRegion();  
  JNIEnv* env;
};

struct GlobalObjectReference {
  jobject object;
  GlobalObjectReference(JNIEnv * inEnv, 
                        jobject inObject)
  : object(inEnv->NewGlobalRef(inObject))
  {}

  ~GlobalObjectReference() {
    AttachedThreadRegion region;
    if (!region.env) return;
    region.env->DeleteGlobalRef(object);
  }
  
  jmethodID method(JNIEnv* inEnv, const char* inName, const char* inSignature) {
    return ::method(inEnv, object, inName, inSignature);
  }
  
private:
  GlobalObjectReference(const GlobalObjectReference&) = delete;
  GlobalObjectReference& operator= (const GlobalObjectReference&) = delete;
};

inline jfieldID nativePointerFieldID(JNIEnv * inEnv, jobject inObject) {
  auto clazz = inEnv->GetObjectClass(inObject);
  return inEnv->GetFieldID(clazz, "mPointer", "J");  
}

template <typename T>
static T* nativePointer(JNIEnv * inEnv, jobject inObject, jfieldID inFieldID) {
  auto value = inEnv->GetLongField(inObject, inFieldID);
  return value == 0 ? nullptr : reinterpret_cast<T*>(value);
}

template <typename T>
static T* nativePointer(JNIEnv * inEnv, jobject inObject) {
  auto value = inEnv->GetLongField(inObject, 
                                   nativePointerFieldID(inEnv, inObject));
  return value == 0 ? nullptr : reinterpret_cast<T*>(value);
}

#endif /* jni_utilities_hpp */
