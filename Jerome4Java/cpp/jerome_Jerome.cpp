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

#include <stdio.h>
#include <fstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <jerome/npc/platform.hpp>
#include <jerome/scripting/scripts.hpp>
#include "jerome_Jerome.h"

using namespace jerome;
using namespace jerome::scripting;
using namespace jerome::npc;

static JavaVM* gJVM = nullptr;

const String eventVhReset = "vhReset";
const String eventVhToss = "vhToss";
const String eventVhText = "vhText";

const String paramStateName = "name";
const String paramQuestionText = "text";
const String paramTargetMachineName = "name";

const String eventSendUtterance = "sendUtterance";
const String eventMachineDone = "machineDone";

const String paramStateMachineName = "machineName";
const String paramUtteranceID = "utteranceID";

const String statusComplete = "complete";
const String statusError = "error";
const String statusInterrupted = "interrupted";

static jmethodID method(JNIEnv* inEnv, jobject inObject, 
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

static jobject newObject(JNIEnv* inEnv, 
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

struct AttachedThreadRegion {
  AttachedThreadRegion() {
    gJVM->AttachCurrentThread((void **)&env, nullptr);
    if (!env) {
      std::cerr << "Failed to attach current thread with JVM" << std::endl;
    }
  }

  ~AttachedThreadRegion() {
    gJVM->DetachCurrentThread();      
  }
  
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

struct UtteranceCallback: public GlobalObjectReference {
  void didSelectUtterance(JNIEnv* inEnv, jobject inUtterance, jobject inCompletion) {
    auto method = this->method(inEnv, "didSelectUtterance", 
                               "(Ljerome/Utterance;Ljerome/Jerome$Completion;)V");
    if (method) {
      inEnv->CallVoidMethod(object, method, inUtterance, inCompletion);    
    }
  }
  
  void didFinishProcessing(JNIEnv* inEnv) {
    auto method = this->method(inEnv, "didFinishProcessing", 
                               "()V");
    if (method) {
      inEnv->CallVoidMethod(object, method);    
    }    
  }
};

struct Model;

static jfieldID modelFieldID(JNIEnv * inEnv, jobject inJerome) {
  auto clazz = inEnv->GetObjectClass(inJerome);
  return inEnv->GetFieldID(clazz, "mPointer", "J");  
}

static shared_ptr<Model>* model(JNIEnv * inEnv, jobject inJerome, jfieldID inFieldID) {
  auto value = inEnv->GetLongField(inJerome, inFieldID);
  return value == 0 ? nullptr : reinterpret_cast<shared_ptr<Model>*>(value);
}

static shared_ptr<Model> model(JNIEnv * inEnv, jobject inJerome) {
  return *model(inEnv, inJerome, modelFieldID(inEnv, inJerome));
}

static String to_string(JNIEnv * inEnv, jstring inString) {
  auto value = inEnv->GetStringUTFChars(inString, nullptr);
  auto result = String(value);
  inEnv->ReleaseStringUTFChars(inString, value);
  return result;
}

struct Model {
  Platform platform;
  OptionalString mainMachineName;
  StringMap<shared_ptr<UtteranceCallback>> callbacks;
  
  void reset() {
    callbacks.clear();
  }
  
  void installEngineHandler() {
    platform.setEngineEventHandler([this](const EngineEvent& inEvent){
      AttachedThreadRegion region;
      if (!region.env) return;
      
      auto machineNameIter = inEvent.data().find(paramStateMachineName);
      if (machineNameIter == inEvent.data().end()) return;
      auto machineName = machineNameIter->second;
      
      auto callbackIter = this->callbacks.find(machineName);
      if (callbackIter == this->callbacks.end()) return;
      
      auto callback = callbackIter->second;
      
      if (inEvent.name() == eventSendUtterance) {
        auto utteranceIDIter = inEvent.data().find(paramUtteranceID);
        if (utteranceIDIter == inEvent.data().end()) return;
        auto utteranceID = utteranceIDIter->second;
        auto utterance = this->utterance(region.env, utteranceID);
        auto completion = this->completion(region.env, machineName);
        callback->didSelectUtterance(region.env, utterance, completion);
        if (utterance) region.env->DeleteLocalRef(utterance);
        if (completion) region.env->DeleteLocalRef(completion);
      } else if (inEvent.name() == eventMachineDone) {
        this->callbacks.erase(callbackIter);
        callback->didFinishProcessing(region.env);
      } else {
        std::cerr << "Unknown event name " << inEvent.name() << std::endl;
      }
    });
  }
  
  jobject utterance(JNIEnv * inEnv, const String& inID) {
    return nullptr;
    #warning "implement"
  }
  
  jobject completion(JNIEnv * inEnv, const String& inMachineName) {
    
    return nullptr;
    #warning "implement"
  } 
  
  void addCallback(JNIEnv * inEnv, jobject inCallback, const String& inID) {
    callbacks.emplace(inID,
                      std::make_shared<UtteranceCallback>(inEnv, inCallback));
  }
};

struct Completion {
  weak_ptr<Model> jerome;
  StringStringMap eventData;
  String machineName;
  
  Completion(const shared_ptr<Model>& inModel, 
             const String& inMachineName,
             const StringStringMap& inEventData)
  : jerome(inModel)
  , machineName(inMachineName)
  , eventData(inEventData)
  {}
  
  void send(const String& inEvent) {
    auto model = jerome.lock();
    if (!model) return;
    
    auto dataIter = eventData.find(inEvent);
    if (dataIter == eventData.end()) return;
    auto eventName = dataIter->second;
    
    model->platform.postEvent(eventName, {}, machineName);
  }
};

static jobject
to_java_exception(JNIEnv * inEnv, OptionalError inError) {
  if (!inError)
    return nullptr;
  
  return newObject(inEnv, "jerome.JeromeException", 
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

static jobject utterance(JNIEnv * inEnv, const String& inID) {
  return nullptr;
  #warning "implement"
}

static jobject 
completion(JNIEnv * inEnv, 
           const String& inMachineName, 
           const shared_ptr<Model> inModel,
           const StringStringMap& inEventData) 
{
  auto pointer = new shared_ptr<Completion>(new Completion(inModel, 
                                                           inMachineName,
                                                           inEventData));
  auto result = reinterpret_cast<jlong>(pointer);
  return newObject(inEnv, <#const char *inClassName#>, <#const char *inConstructorSignature, ...#>)
  
  return nullptr;
  #warning "implement"
} 

static void 
installEngineHandler(const shared_ptr<Model>& inModel) {
  auto weakModel = weak_ptr<Model>(inModel);
  
  inModel->platform.setEngineEventHandler([weakModel](const EngineEvent& inEvent){
    AttachedThreadRegion region;
    if (!region.env) return;
    
    auto model = weakModel.lock();
    if (!model) return;
    
    auto machineNameIter = inEvent.data().find(paramStateMachineName);
    if (machineNameIter == inEvent.data().end()) return;
    auto machineName = machineNameIter->second;
    
    auto callbackIter = model->callbacks.find(machineName);
    if (callbackIter == model->callbacks.end()) return;    
    auto callback = callbackIter->second;
    
    if (inEvent.name() == eventSendUtterance) {
      auto utteranceIDIter = inEvent.data().find(paramUtteranceID);
      if (utteranceIDIter == inEvent.data().end()) return;
      auto utteranceID = utteranceIDIter->second;
      auto utterance = model->utterance(region.env, utteranceID);
      auto completion = ::completion(region.env, machineName, 
                                     model, inEvent.data());
      callback->didSelectUtterance(region.env, utterance, completion);
      if (utterance) region.env->DeleteLocalRef(utterance);
      if (completion) region.env->DeleteLocalRef(completion);
    } else if (inEvent.name() == eventMachineDone) {
      model->callbacks.erase(callbackIter);
      callback->didFinishProcessing(region.env);
    } else {
      std::cerr << "Unknown event name " << inEvent.name() << std::endl;
    }
  });
}




JNIEXPORT void JNICALL 
Java_jerome_Jerome_loadModel(JNIEnv * inEnv, 
                             jobject inJerome, 
                             jstring inCollectionPath, 
                             jstring inDMPath, 
                             jobject inCallback)
{
  auto jerome = model(inEnv, inJerome);
  if (!jerome) return;
  
  jerome->platform.setEngineEventHandler([](const EngineEvent&){});
  jerome->mainMachineName = OptionalString();
  
  std::ifstream  collectionStream(to_string(inEnv, inCollectionPath));
  auto collectionResult = jerome->platform.loadCollection(collectionStream);
  if (collectionResult) {
    executeLoadModelCallback(inEnv, inCallback, collectionResult);
    return;
  }
  
  std::ifstream  dmStream(to_string(inEnv, inDMPath));
  jerome->platform.loadDialogueManager
    (dmStream, [inJerome, inCallback](const Result<String>& optionalName) {    
      AttachedThreadRegion region;
      if (!region.env) return;
      auto jerome = model(region.env, inJerome);
      if (!jerome) return;      
      if (optionalName) {
        jerome->mainMachineName = optionalName.value();
        jerome->installEngineHandler();
        executeLoadModelCallback(region.env, inCallback, OptionalError());
      } else {
        executeLoadModelCallback(region.env, inCallback, optionalName.error());
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
  if (!jerome || !jerome->mainMachineName) return;
  jerome->reset();
  jerome->platform.postEvent(eventVhReset, {}, *jerome->mainMachineName);
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
  if (!jerome || !jerome->mainMachineName) return;
  jerome->platform.postEvent(eventVhToss, 
                             {{paramStateName, to_string(inEnv, inStateName)}}, 
                             *jerome->mainMachineName);
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
  static boost::uuids::random_generator uuidGenerator;

  auto jerome = model(inEnv, inJerome);
  if (!jerome || !jerome->mainMachineName) return;
  
  String eventID = boost::uuids::to_string(uuidGenerator());
  jerome->addCallback(inEnv, inCallback, eventID);
  jerome->platform.postEvent(eventVhText, 
                             {{paramQuestionText, to_string(inEnv, inText)},
                              {paramTargetMachineName, eventID}}, 
                             *jerome->mainMachineName);
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
  auto fieldID = modelFieldID(inEnv, inJerome);
  auto jerome = model(inEnv, inJerome, fieldID);
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
    inEnv->GetJavaVM(&gJVM);
  }
  auto pointer = new shared_ptr<Model>(new Model); 
  return reinterpret_cast<jlong>(pointer);
}
