//
//  model.m
//  jerome
//
//  Created by Anton Leuski on 5/11/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#include <jerome/npc/model.hpp>
#import "ObjectiveC++.h"
#import "model_private.h"

using namespace jerome;

@interface ALUtterance ()
@property (nonatomic, assign) npc::Utterance utterance;
@end

@implementation ALUtterance

- (BOOL)hasValueForKey:(NSString* _Nonnull)fieldName
{
  return self->_utterance.has(fieldName.cppString);
}

- (NSString* _Nullable)valueForKey:(NSString* _Nonnull)fieldName
{
  auto name = fieldName.cppString;
  return self->_utterance.has(name)
    ? [NSString stringWithCPPString:self->_utterance.get(name)]
    : nil;
}

- (void)setValue:(NSString* _Nullable)fieldValue forKey:(NSString* _Nonnull)fieldName
{
  self->_utterance.put(fieldName.cppString, fieldValue.cppString);
}

- (NSString* _Nullable)objectForKeyedSubscript:(NSString* _Nonnull)key
{
  return [self valueForKey:key];
}

- (void)setObject:(NSString* _Nullable)obj forKeyedSubscript:(NSString* _Nonnull)key
{
  [self setValue:obj forKey:key];
}

@end

@implementation ALUtterance (AL)

+ (ALUtterance* _Nonnull)utteranceWithUtterance:(const npc::Utterance&)utterance
{
  ALUtterance* utt = [ALUtterance new];
  utt.utterance = utterance;
  return utt;
}

+ (ALUtterance* _Nullable)utteranceWithOptionalUtterance:(const optional<npc::Utterance>&)utterance
{
  return utterance ? [self utteranceWithUtterance:*utterance] : nil;
}

@end

@implementation ALPlatformEvent
- (NSString*)description
{
  return [NSString stringWithFormat:@"PlatformEvent{name=\"%@\", type=\"%@\", "
          "target=\"%@\", origin=\"%@\", data=%@}", self.name, self.type,
          self.target, self.origin, self.data];
}
@end
