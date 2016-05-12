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
- (BOOL)has:(NSString* _Nonnull)fieldName
{
  return self->_utterance.has(fieldName.cppString);
}

- (NSString* _Nonnull)get:(NSString* _Nonnull)fieldName
{
  return [NSString stringWithCPPString:self->_utterance.get(fieldName.cppString)];
}

- (void)set:(NSString* _Nonnull)fieldName :(NSString* _Nonnull)fieldValue
{
  self->_utterance.put(fieldName.cppString, fieldValue.cppString);
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
