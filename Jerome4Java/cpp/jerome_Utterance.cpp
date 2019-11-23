//
//  jerome_Utterance.cpp
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

#ifdef __APPLE__
# include<JavaVM/jni.h>
#else
# include<jni.h>
#endif

#include <stdio.h>
#include <fstream>
#include "jni_utilities.hpp"
#include "jerome_Utterance.h"
#include "model.hpp"

static Utterance* model(JNIEnv * inEnv, jobject inObject) {
  return nativePointer<Utterance>(inEnv, inObject);
}

/*
 * Class:     jerome_Utterance
 * Method:    hasValueForKey
 * Signature: (Ljava/lang/String;)Z
 */
JNIEXPORT jboolean JNICALL 
Java_jerome_Utterance_hasValueForKey(JNIEnv* inEnv, 
                                     jobject inObject, 
                                     jstring inKey)
{
  auto object = model(inEnv, inObject);
  if (!object || !inKey) return 0;
  return object->has(to_string(inEnv, inKey));
}

/*
 * Class:     jerome_Utterance
 * Method:    valueForKey
 * Signature: (Ljava/lang/String;)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL 
Java_jerome_Utterance_valueForKey(JNIEnv* inEnv, 
                                  jobject inObject, 
                                  jstring inKey)
{
  auto object = model(inEnv, inObject);
  if (!object || !inKey) return 0;
  auto key = to_string(inEnv, inKey);
  if (!object->has(key)) return nullptr;
  return inEnv->NewStringUTF(object->get(key).c_str());
}

/*
 * Class:     jerome_Utterance
 * Method:    setValueForKey
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL 
Java_jerome_Utterance_setValueForKey(JNIEnv* inEnv, 
                                     jobject inObject, 
                                     jstring inValue, 
                                     jstring inKey)
{
  auto object = model(inEnv, inObject);
  if (!object || !inKey) return;
  auto key = to_string(inEnv, inKey);
  if (inValue) {
    object->put(key, to_string(inEnv, inValue));
  } else {
    object->remove(key);
  }
}

/*
 * Class:     jerome_Utterance
 * Method:    getCount
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL 
Java_jerome_Utterance_getCount(JNIEnv* inEnv, 
                               jobject inObject)
{
  auto object = model(inEnv, inObject);
  if (!object) return 0;
  return object->count();
}

/*
 * Class:     jerome_Utterance
 * Method:    setCount
 * Signature: (D)V
 */
JNIEXPORT void JNICALL 
Java_jerome_Utterance_setCount(JNIEnv* inEnv, 
                               jobject inObject, 
                               jdouble inValue)
{
  auto object = model(inEnv, inObject);
  if (!object) return;
  object->setCount(inValue);
}

/*
 * Class:     jerome_Utterance
 * Method:    getFieldNamed
 * Signature: ()[Ljava/lang/String;
 */
JNIEXPORT jobjectArray JNICALL 
Java_jerome_Utterance_getFieldNames(JNIEnv* inEnv, 
                                    jobject inObject)
{
  auto object = model(inEnv, inObject);
  auto array = inEnv->NewObjectArray(object ? (int)object->fieldNames().size() : 0,
                                     findClass(inEnv, "java/lang/String"),
                                     inEnv->NewStringUTF(""));
  if (!object) return array;
  int index = 0;
  for(auto name: object->fieldNames()) {
    inEnv->SetObjectArrayElement(array, 
                                 index++, 
                                 inEnv->NewStringUTF(name.c_str()));
  }
  return array;
}

/*
 * Class:     jerome_Utterance
 * Method:    release
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_jerome_Utterance_release(JNIEnv* inEnv, 
                              jobject inObject)
{
  auto fieldID = nativePointerFieldID(inEnv, inObject);
  auto object = nativePointer<Utterance>(inEnv, inObject, fieldID);
  if (object) {
    inEnv->SetLongField(inObject, fieldID, 0);
    delete object;
  }
}
