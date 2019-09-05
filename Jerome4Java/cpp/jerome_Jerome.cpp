//
//  jerome_Jerome.cpp
//
//  Created by Anton Leuski on 9/2/19.
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
#include <jerome/npc/platform.hpp>
#include <jerome/scripting/scripts.hpp>
#include "jni_utilities.hpp"
#include "jerome_Jerome.h"
#include "model.hpp"

using namespace jerome;
using namespace jerome::scripting;
using namespace jerome::npc;

static shared_ptr<Model> model(JNIEnv * inEnv, jobject inJerome) {
  return *nativePointer<shared_ptr<Model>>(inEnv, inJerome);
}

static jobject
to_java_exception(JNIEnv * inEnv, OptionalError inError) {
  if (!inError)
    return nullptr;
  
  return newObject(inEnv, "jerome/JeromeException", 
                   "(Ljava/lang/String;)V", 
                   inEnv->NewStringUTF(inError->description().c_str()));
}

static void 
executeLoadModelCallback(JNIEnv * inEnv, 
                         jobject inCallback, 
                         OptionalError inError)
{
  auto method = ::method(inEnv, inCallback, 
                         "didLoadModel", 
                         "(Ljerome/JeromeException;)V");
  
  if (method) {
    auto exception = to_java_exception(inEnv, inError);
    inEnv->CallVoidMethod(inCallback, method, exception);
    if (exception) inEnv->DeleteLocalRef(exception);
  }  
}

JNIEXPORT void JNICALL 
Java_jerome_Jerome_loadModel(JNIEnv * inEnv, 
                             jobject inJerome, 
                             jstring inCollectionPath, 
                             jstring inDMPath, 
                             jstring inInitialState,
                             jobject inCallback)
{
  auto jerome = model(inEnv, inJerome);
  if (!jerome) return;
  
  jerome->platform.setUsingLookupTable(true);
  jerome->platform.setEngineEventHandler([](const EngineEvent&){});
  jerome->mainMachineName = OptionalString();
  
  auto collectionPath = to_string(inEnv, inCollectionPath);
  std::cout << "collection path " << collectionPath << std::endl;
  
  std::ifstream  collectionStream(collectionPath);
  auto collectionResult = jerome->platform.loadCollection(collectionStream);
  if (collectionResult) {
    executeLoadModelCallback(inEnv, inCallback, collectionResult);
    return;
  }
  
  auto dmPath = to_string(inEnv, inDMPath);
  std::cout << "dm path " << dmPath << std::endl;

  std::ifstream  dmStream(dmPath);
  GlobalObjectReference callback(inEnv, inCallback);
  
  auto initialState = to_string(inEnv, inInitialState);
  
  jerome->platform.loadDialogueManager
    (dmStream, [jerome, callback, initialState](const Result<String>& optionalName) {    
      AttachedThreadRegion region;
      if (!region.env) return;
      if (optionalName) {
        std::cout << "name " << optionalName.value() << std::endl;
        jerome->mainMachineName = optionalName.value();
        jerome->tossDialogueToStateWithName(initialState);
        jerome->installEngineHandler();
        executeLoadModelCallback(region.env, callback.object(), OptionalError());
      } else {
        std::cout << "error " << optionalName.error() << std::endl;
        executeLoadModelCallback(region.env, callback.object(), optionalName.error());
      }
  });
}

/*
 * Class:     jerome_Jerome
 * Method:    collectionWasUpdated
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_jerome_Jerome_collectionWasUpdated(JNIEnv * inEnv, 
                                        jobject inJerome)
{
  auto jerome = model(inEnv, inJerome);
  if (!jerome) return;
  jerome->platform.collectionWasUpdated();
}

/*
 * Class:     jerome_Jerome
 * Method:    collectionWasUpdatedInState
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL 
Java_jerome_Jerome_collectionWasUpdatedInState(JNIEnv * inEnv, 
                                               jobject inJerome, 
                                               jstring inStateName)
{
  auto jerome = model(inEnv, inJerome);
  if (!jerome) return;
  jerome->platform.collectionWasUpdated(to_string(inEnv, inStateName));
}

/*
 * Class:     jerome_Jerome
 * Method:    utteranceWithID
 * Signature: (Ljava/lang/String;)Ljerome/Utterance;
 */
JNIEXPORT jobject JNICALL 
Java_jerome_Jerome_utteranceWithID(JNIEnv * inEnv, 
                                   jobject inJerome, 
                                   jstring inID)
{
  auto jerome = model(inEnv, inJerome);
  if (!jerome) return nullptr;
  return jerome->utterance(inEnv, to_string(inEnv, inID));
}

/*
 * Class:     jerome_Jerome
 * Method:    resetDialogue
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_jerome_Jerome_resetDialogue(JNIEnv * inEnv, 
                                 jobject inJerome)
{
  auto jerome = model(inEnv, inJerome);
  if (!jerome) return;
  jerome->resetDialogue();
}

/*
 * Class:     jerome_Jerome
 * Method:    tossDialogueToStateWithName
 * Signature: (Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL 
Java_jerome_Jerome_tossDialogueToStateWithName(JNIEnv * inEnv, 
                                               jobject inJerome, 
                                               jstring inStateName)
{
  auto jerome = model(inEnv, inJerome);
  if (!jerome) return;
  jerome->tossDialogueToStateWithName(to_string(inEnv, inStateName));
}

/*
 * Class:     jerome_Jerome
 * Method:    respondTo
 * Signature: (Ljava/lang/String;Ljerome/Jerome/ResponseCallback;)V
 */
JNIEXPORT void JNICALL 
Java_jerome_Jerome_respondTo(JNIEnv * inEnv, 
                             jobject inJerome, 
                             jstring inText, 
                             jobject inCallback)
{

  auto jerome = model(inEnv, inJerome);
  if (!jerome) return;
  jerome->respondTo(inEnv, inCallback, to_string(inEnv, inText));
}

/*
 * Class:     jerome_Jerome
 * Method:    release
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_jerome_Jerome_release(JNIEnv * inEnv, 
                           jobject inJerome)
{
  auto fieldID = nativePointerFieldID(inEnv, inJerome);
  auto jerome = nativePointer<shared_ptr<Model>>(inEnv, inJerome, fieldID);
  if (jerome) {
    inEnv->SetLongField(inJerome, fieldID, 0);
    delete jerome;
  }
}

/*
 * Class:     jerome_Jerome
 * Method:    init
 * Signature: ()J
 */
JNIEXPORT jlong JNICALL 
Java_jerome_Jerome_init(JNIEnv * inEnv, 
                        jclass)
{
  static bool isInitialized = false;
  if (!isInitialized) {
    isInitialized = true;
    Platform::initialize();
    AttachedThreadRegion::initialize(inEnv);
  }
  auto pointer = new shared_ptr<Model>;
  *pointer = std::make_shared<Model>();
  return reinterpret_cast<jlong>(pointer);
}
