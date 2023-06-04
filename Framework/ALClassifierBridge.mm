//
//  ALClassifierBridge.m
//
//  Created by Anton Leuski on 6/3/23.
//  Copyright © 2023 Anton Leuski & ICT/USC. All rights reserved.
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

#include <fstream>
#include <jerome/npc/engine.hpp>
#include <jerome/scripting/scripts.hpp>
#import "ObjectiveC++.h"
#import "ALUtterance.private.h"
#import "ALClassifierBridge.h"

#include <libxml/globals.h>

void _alXmlFree(void* x) {
  xmlFree(x);
}

using namespace jerome::scripting;
using namespace jerome::npc;

@interface ALScriptLiteral ()
@property(nonatomic, strong, nonnull) NSString* content;
@property(nonatomic, strong, nonnull) NSString* name;
@end

@implementation ALScriptLiteral
- (instancetype)initWithLiteral:(const ScriptLiteral&)literal {
  if (self = [super init]) {
    self.content = [NSString stringWithUTF8String:literal.source];
    self.name = [NSString stringWithUTF8String:literal.name];
  }
  return self;
}
@end

@implementation ALDialogueManagerBridge
+ (ALScriptLiteral*)scion {
  static ALScriptLiteral* sharedScion = nil;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^{
    sharedScion = [[ALScriptLiteral alloc] initWithLiteral:SCION_SCRIPT];
  });
  return sharedScion;
}

+ (ALScriptLiteral*)startup {
  static ALScriptLiteral* sharedStartup = nil;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^{
    sharedStartup = [[ALScriptLiteral alloc] initWithLiteral:JEROME_STARTUP_SCRIPT];
  });
  return sharedStartup;
}

+ (ALScriptLiteral*)utteranceCompiler {
  static ALScriptLiteral* sharedUtteranceCompiler = nil;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^{
    sharedUtteranceCompiler = [[ALScriptLiteral alloc] initWithLiteral:JEROME_UTTERANCE_COMPILER_SCRIPT];
  });
  return sharedUtteranceCompiler;
}
@end

@interface ALClassifierBridge ()
@property (nonatomic, assign) detail::Engine platformStorage;
@property (nonatomic, assign, readonly) detail::Engine& platform;
@end

static NSArray* utteranceVectorToArray(const std::vector<Utterance>& utterances)
{
  NSMutableArray* array = [NSMutableArray new];
  for(const auto& u : utterances) {
    [array addObject:[ALUtterance utteranceWithUtterance:u]];
  }
  return array;
}

@implementation ALClassifierBridge

+ (void)initialize
{
  if (self == [ALClassifierBridge class]) {
    detail::Engine::initialize();
  }
}

- (detail::Engine&)platform
{
  return self->_platformStorage;
}

struct membuf : std::streambuf
{
  membuf(char* begin, char* end) {
    this->setg(begin, begin, end);
  }
};

- (nullable instancetype)initWithData:(NSData*)data error:(NSError**)outError
{
  if (self = [super init]) {
    membuf sbuf((char*)data.bytes, (char*)data.bytes + data.length);
    std::istream stream(&sbuf);
    auto result = self.platform.loadCollection(stream);
    if (outError) {
      *outError = [NSError errorWithOptionalError:result];
    }
    if (result) {
      self = nil;
    }
  }
  return self;
}

- (BOOL)writeCollectionToURL:(NSURL*)url error:(NSError**)outError
{
  std::ofstream  stream(url.absoluteURL.fileSystemRepresentation);
  auto result = self.platform.saveCollection(stream);
  if (outError) {
    *outError = [NSError errorWithOptionalError:result];
  }
  return !result;
}

- (nullable ALUtterance*)utteranceWithID:(NSString*)identifier
{
  return [ALUtterance utteranceWithOptionalUtterance:
          self.platform.utteranceWithID(identifier.cppString)];
}

- (NSArray<ALUtterance*>*)answersInDomain:(nullable NSString*)identifier
{
  return utteranceVectorToArray(identifier == nil
                                ? self.platform.answers()
                                : self.platform.answers(identifier.cppString));
}

- (nullable NSArray<ALUtterance*>*)searchForString:(NSString*)query
                                          inDomain:(NSString*)identifier
                                             error:(NSError**)outError
{
  try {
    Utterance q(detail::Engine::queryForString([query UTF8String]));
    return utteranceVectorToArray(self.platform.search(identifier.cppString, q));
  } catch (const jerome::Error& e) {
    if (outError)
      *outError = [NSError errorWithError:e];
    return nil;
  }
}

- (nullable NSArray<ALUtterance*>*)searchForDictionary:(NSDictionary<NSString*, NSString*>*)query
                                              inDomain:(NSString*)identifier
                                                 error:(NSError**)outError
{
  try {
    cpp::ObjectFactory   of;
    Utterance q(of.newUtterance());
    for (NSString* key in query) {
      q.put(key.cppString, query[key].cppString);
    }
    return utteranceVectorToArray(self.platform.search(identifier.cppString, q));
  } catch (const jerome::Error& e) {
    if (outError)
      *outError = [NSError errorWithError:e];
    return nil;
  }
}

- (void)collectionWasUpdatedInDomain:(nullable NSString*)identifier
{
  self.platform.collectionWasUpdated(identifier == nil
                                     ? jerome::OptionalString()
                                     : identifier.cppString);

}

@end
