//
//  jerome_Jerome_Completion.cpp
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

#include <stdio.h>

#ifdef __APPLE__
# include<JavaVM/jni.h>
#else
# include<jni.h>
#endif

#include <stdio.h>
#include <fstream>
#include "jni_utilities.hpp"
#include "jerome_Jerome_Completion.h"
#include "model.hpp"

static const String statusComplete = "complete";
static const String statusError = "error";
static const String statusInterrupted = "interrupted";

static Completion* model(JNIEnv * inEnv, jobject inObject) {
  return nativePointer<Completion>(inEnv, inObject);
}

static void send(JNIEnv* inEnv, jobject inObject, const String& inStatus) {
  auto object = model(inEnv, inObject);
  if (!object) return;
  object->send(inStatus);
}

JNIEXPORT void JNICALL 
Java_jerome_Jerome_00024Completion_succeeded(JNIEnv* inEnv, 
                                             jobject inObject)
{
  send(inEnv, inObject, statusComplete);
}

/*
 * Class:     jerome_Jerome_Completion
 * Method:    canceled
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_jerome_Jerome_00024Completion_canceled(JNIEnv* inEnv, 
                                            jobject inObject)
{
  send(inEnv, inObject, statusError);
}

/*
 * Class:     jerome_Jerome_Completion
 * Method:    interrupted
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_jerome_Jerome_00024Completion_interrupted(JNIEnv* inEnv, 
                                               jobject inObject)
{
  send(inEnv, inObject, statusInterrupted);
}

/*
 * Class:     jerome_Jerome_Completion
 * Method:    release
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_jerome_Jerome_00024Completion_release(JNIEnv* inEnv, 
                                           jobject inObject)
{
  auto fieldID = nativePointerFieldID(inEnv, inObject);
  auto object = nativePointer<Completion>(inEnv, inObject, fieldID);
  if (object) {
    inEnv->SetLongField(inObject, fieldID, 0);
    delete object;
  }
}

