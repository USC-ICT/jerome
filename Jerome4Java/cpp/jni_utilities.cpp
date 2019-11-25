//
//  jni_utilities.cpp
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

#include <iostream>
#include <jerome/logger.hpp>
#include "jni_utilities.hpp"

static std::unordered_map<std::string, GlobalObjectReference> sJavaClassCache;

jclass findClass(JNIEnv * inEnv, const char* inClassName) {
  auto iter = sJavaClassCache.find(inClassName);
  if (iter != sJavaClassCache.end()) {
    return static_cast<jclass>(iter->second.object());
  }

  auto clazz = inEnv->FindClass(inClassName);
  if (!clazz) {
    jerome::log::error() << "Unknown class " << inClassName;
    return nullptr;
  }

  sJavaClassCache.emplace(inClassName, GlobalObjectReference(inEnv, static_cast<jobject>(clazz)));

  return clazz;
}

std::string to_string(JNIEnv * inEnv, jstring inString) {
  auto value = inEnv->GetStringUTFChars(inString, nullptr);
  auto result = std::string(value);
  inEnv->ReleaseStringUTFChars(inString, value);
  return result;
}

jmethodID method(JNIEnv* inEnv, jobject inObject, 
                 const char* inName, const char* inSignature) 
{
  auto clazz = inEnv->GetObjectClass(inObject);
  if (!clazz) {
    jerome::log::error() << "Unknown class for method " << inName ;
    return nullptr;
  }
  
  auto method = inEnv->GetMethodID(clazz, inName, inSignature);
  if (!method) {
    jerome::log::error() << "Unknown method " << inName;
  }
  
  return method;
}

jobject newObject(JNIEnv* inEnv, 
                  const char* inClassName, 
                  const char* inConstructorSignature,
                  ...) 
{
  va_list a_list;
  va_start(a_list, inConstructorSignature);
  
  auto clazz = findClass(inEnv, inClassName);
  if (!clazz) {
    jerome::log::error() << "Unknown class " << inClassName;
    return nullptr;
  }
  
  auto constructor = inEnv->GetMethodID(clazz, "<init>", inConstructorSignature);
  if (!constructor) {
    jerome::log::error() << "Unknown constructor " << inConstructorSignature
    << " for class " << inClassName;
    return nullptr;
  }
  
  return inEnv->NewObjectV(clazz, constructor, a_list);
}

static JavaVM* gJVM = nullptr;

void AttachedThreadRegion::initialize(JNIEnv* inEnv) {
  inEnv->GetJavaVM(&gJVM);
}

AttachedThreadRegion::AttachedThreadRegion() {
  gJVM->AttachCurrentThread((void **)&env, nullptr);
  if (!env) {
    jerome::log::error() << "Failed to attach current thread with JVM";
  }
}

AttachedThreadRegion::~AttachedThreadRegion() {
  gJVM->DetachCurrentThread();      
}
