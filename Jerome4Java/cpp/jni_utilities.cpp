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
#include "jni_utilities.hpp"

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
    std::cerr << "Unknown class for method " << inName << std::endl;
    return nullptr;
  }
  
  auto method = inEnv->GetMethodID(clazz, inName, inSignature);
  if (!method) {
    std::cerr << "Unknown method " << inName << std::endl;
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
  
  auto clazz = inEnv->FindClass(inClassName);
  if (!clazz) {
    std::cerr << "Unknown class " << inClassName << std::endl;
    return nullptr;
  }
  
  auto constructor = inEnv->GetMethodID(clazz, "<init>", inConstructorSignature);
  if (!constructor) {
    std::cerr << "Unknown constructor " << inConstructorSignature 
    << " for class " << inClassName << std::endl;
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
    std::cerr << "Failed to attach current thread with JVM" << std::endl;
  }
}

AttachedThreadRegion::~AttachedThreadRegion() {
  gJVM->DetachCurrentThread();      
}
