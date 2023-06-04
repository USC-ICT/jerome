//
//  ALClassifierBridge.h
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

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@class ALUtterance;

@interface ALScriptLiteral : NSObject
@property(nonatomic, strong, readonly, nonnull) NSString* content;
@property(nonatomic, strong, readonly, nonnull) NSString* name;
@end

@interface ALDialogueManagerBridge : NSObject
@property(class, nonatomic, strong, readonly, nonnull) ALScriptLiteral* scion;
@property(class, nonatomic, strong, readonly, nonnull) ALScriptLiteral* startup;
@property(class, nonatomic, strong, readonly, nonnull) ALScriptLiteral* utteranceCompiler;
@end

@interface ALClassifierBridge : NSObject
- (nullable instancetype)initWithData:(NSData*)data error:(NSError**)error;
- (BOOL)writeCollectionToURL:(NSURL*)url error:(NSError**)error;
- (nullable ALUtterance*)utteranceWithID:(NSString*)identifier;
- (NSArray<ALUtterance*>*)answersInDomain:(nullable NSString*)identifier;
- (nullable NSArray<ALUtterance*>*)searchForString:(NSString*)query
                                          inDomain:(NSString*)identifier
                                             error:(NSError**)error;
- (nullable NSArray<ALUtterance*>*)searchForDictionary:(NSDictionary<NSString*, NSString*>*)query
                                              inDomain:(NSString*)identifier
                                                 error:(NSError**)error;
- (void)collectionWasUpdatedInDomain:(nullable NSString*)identifier;
@end

#ifdef __cplusplus
extern "C" {
#endif

extern void _alXmlFree(void*);

#ifdef __cplusplus
}
#endif

NS_ASSUME_NONNULL_END
