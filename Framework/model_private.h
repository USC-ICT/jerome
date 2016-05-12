//
//  model_private.h
//  jerome
//
//  Created by Anton Leuski on 5/11/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#import "model.h"

@interface ALUtterance (AL)
+ (ALUtterance* _Nonnull)utteranceWithUtterance:(const jerome::npc::Utterance&)utterance;
+ (ALUtterance* _Nullable)utteranceWithOptionalUtterance:(const jerome::optional<jerome::npc::Utterance>&)utterance;
@end
