//
//  model.cpp
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

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "model.hpp"
#include "jni_utilities.hpp"
#include <jerome/scripting/scripts.hpp>

using namespace jerome::scripting;

static const String eventVhReset = "vhReset";
static const String eventVhToss = "vhToss";
static const String eventVhText = "vhText";

static const String paramStateName = "name";
static const String paramQuestionText = "text";
static const String paramTargetMachineName = "name";

static const String eventSendUtterance = "sendUtterance";
static const String eventMachineDone = "machineDone";

static const String paramStateMachineName = "machineName";
static const String paramUtteranceID = "utteranceID";

struct UtteranceCallback: public GlobalObjectReference {
  
  using GlobalObjectReference::GlobalObjectReference;
  
  void didSelectUtterance(JNIEnv* inEnv, jobject inUtterance, jobject inCompletion) {
    auto method = this->method(inEnv, "didSelectUtterance",
                               "(Ljerome/Utterance;Ljerome/Jerome$Completion;)V");
    if (method) {
      inEnv->CallVoidMethod(object(), method, inUtterance, inCompletion);    
    }
  }
  
  void didFinishProcessing(JNIEnv* inEnv) {
    auto method = this->method(inEnv, "didFinishProcessing", 
                               "()V");
    if (method) {
      inEnv->CallVoidMethod(object(), method);    
    }    
  }
};

void Model::resetDialogue() {
  callbacks.clear();
  if (!mainMachineName) return;
  platform.postEvent(eventVhReset, {}, *mainMachineName);
}

void Model::tossDialogueToStateWithName(const String& inName) {
  if (!mainMachineName) return;
  platform.postEvent(eventVhToss,
                     {{paramStateName, inName}}, 
                     *mainMachineName);
}

void Model::respondTo(JNIEnv * inEnv, jobject inCallback, 
                      const String &inText) {
  static boost::uuids::random_generator uuidGenerator;
  if (!mainMachineName) return;
  String eventID = boost::uuids::to_string(uuidGenerator());
  callbacks.emplace(eventID,
                    std::make_shared<UtteranceCallback>(inEnv, inCallback));
  platform.postEvent(eventVhText, 
                     {{paramQuestionText, inText},
                      {paramTargetMachineName, eventID}}, 
                     *mainMachineName);
}

void Model::installEngineHandler() {
  auto weak = weak_ptr<Model>(Model::shared_from_this());
  platform.setEngineEventHandler([weak](const EngineEvent& inEvent){
    AttachedThreadRegion region;
    if (!region.env) return;
    
    auto machineNameIter = inEvent.data().find(paramStateMachineName);
    if (machineNameIter == inEvent.data().end()) return;
    auto machineName = machineNameIter->second;
    
    auto model = weak.lock();
    if (!model) return;
    
    auto callbackIter = model->callbacks.find(machineName);
    if (callbackIter == model->callbacks.end()) return;
    auto callback = callbackIter->second;
    
    if (inEvent.name() == eventMachineDone) {
      model->callbacks.erase(callbackIter);
      callback->didFinishProcessing(region.env);
      return;
    }
    
    if (inEvent.name() == eventSendUtterance) {
      auto utteranceIDIter = inEvent.data().find(paramUtteranceID);
      if (utteranceIDIter == inEvent.data().end()) return;
      auto utteranceID = utteranceIDIter->second;
      auto utterance = model->utterance(region.env, utteranceID);
      auto completion = model->completion(region.env, inEvent.data());
      callback->didSelectUtterance(region.env, utterance, completion);
      if (utterance) region.env->DeleteLocalRef(utterance);
      if (completion) region.env->DeleteLocalRef(completion);
      return;
    }
    
    std::cerr << "Unknown event name " << inEvent.name() << std::endl;
  });
}

jobject Model::utterance(JNIEnv * inEnv, const String& inID) {
  auto utterance = this->platform.utteranceWithID(inID);
  if (!utterance) return nullptr;
  
  auto pointer = new Utterance(*utterance);
  return newObject(inEnv, "jerome/Utterance", 
                   "(J)V", 
                   reinterpret_cast<jlong>(pointer));
}

jobject Model::completion(JNIEnv * inEnv, const StringStringMap& inData) {
  auto pointer = new Completion(Model::shared_from_this(), inData);
  return newObject(inEnv, "jerome/Jerome$Completion", 
                   "(J)V", 
                   reinterpret_cast<jlong>(pointer));
} 

Completion::Completion(const shared_ptr<Model>& inModel, 
           const StringStringMap& inEventData)
: jerome(inModel)
, eventData(inEventData)
{}

void Completion::send(const String& inEvent) {
  auto model = jerome.lock();
  if (!model) return;
  
  // FSM name
  auto machineNameIter = eventData.find(paramStateMachineName);
  if (machineNameIter == eventData.end()) return;
  auto machineName = machineNameIter->second;

  // check if we still have the callback with this ID == we can process the
  // event
  auto callbackIter = model->callbacks.find(machineName);
  if (callbackIter == model->callbacks.end()) return;

  // figure out which event to send
  auto dataIter = eventData.find(inEvent);
  if (dataIter == eventData.end()) return;
  auto eventName = dataIter->second;
  
  model->platform.postEvent(eventName, {}, machineName);
}

