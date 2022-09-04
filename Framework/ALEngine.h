//
//  ALEngine.h
//  jerome
//
//  Created by Anton Leuski on 5/11/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
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
#import <Jerome/model.h>

JEROME_FRAMEWORK_SYMBOL_EXPORT
@interface ALEngine : NSObject
@property(nonatomic, strong, readonly) ALCollection* _Nullable colleciton;
@property(nonatomic, copy) void (^_Nonnull log)(NSArray<NSObject*>* _Nonnull);

- (BOOL)readCollectionFromURL:(NSURL* _Nonnull)url
                        error:(NSError * _Nullable * _Nullable)outError;

- (BOOL)readCollectionFromData:(NSData* _Nonnull)data
                         error:(NSError * _Nullable * _Nullable)outError;

- (BOOL)writeCollectionToURL:(NSURL* _Nonnull)url
                       error:(NSError * _Nullable * _Nullable)outError;

- (ALUtterance* _Nullable)classifier:(NSString* _Nonnull)stateName
                           respondTo:(NSString* _Nonnull)question
                               error:(NSError* _Nullable * _Nullable)outError;

- (void)classifier:(NSString* _Nonnull)stateName
         respondTo:(NSString* _Nonnull)question
  completionHandle:(void(^ _Nonnull)(ALUtterance* _Nullable,
                                     NSError* _Nullable))handle;

- (void)readDialogueManagerFromURL:(NSURL* _Nonnull)url
                  completionHandle:(ALDialogueScriptLoadedHandle _Nonnull)completionHandle;

- (ALDialogueManagerMetadata* _Nullable)readDialogueManagerFromURL:(NSURL* _Nonnull)url
                                                             error:(NSError*  _Nullable * _Nullable)outError;

- (ALDialogueManagerMetadata* _Nullable)readDialogueManagerFromData:(NSData* _Nonnull)data
                                                              error:(NSError* _Nullable * _Nullable)outError;

- (void)postEventWithName:(NSString* _Nonnull)name
                     data:(NSDictionary<NSString*, id>* _Nonnull)data
                toMachine:(NSString* _Nonnull)machine
NS_SWIFT_NAME(postEvent(name:data:toMachine:))
;

- (void)setEngineEventHandler:(ALEngineEventHandler _Nonnull)eventHandler;

- (ALUtterance* _Nullable)utteranceWithID:(NSString* _Nonnull)utteranceID;

- (void)collectionWasUpdated;
- (void)collectionWasUpdatedInState:(NSString* _Nonnull)stateName;

@end
