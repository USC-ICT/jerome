//
//  model.h
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
#import <JavaScriptCore/JavaScriptCore.h>
#import <Jerome/Macros.h>

JEROME_FRAMEWORK_SYMBOL_EXPORT
@interface ALPlatformEvent : NSObject
@property (nonatomic, copy, nonnull)  NSString* name;
@property (nonatomic, copy, nullable) NSString* type;
@property (nonatomic, copy, nullable) NSString* target;
@property (nonatomic, copy, nullable) NSString* origin;
@property (nonatomic, copy, nonnull)  NSDictionary* data;
@end

//@protocol ALPlatformDelegate <NSObject>
//@optional
//- (void)platform:(nonnull ALPlatform*)platform didReceiveEvent:(nonnull ALPlatformEvent*)event;
//- (void)platform:(nonnull ALPlatform*)platform didLoadDialogueManagerWithName:(nonnull NSString*)name;
//- (void)platform:(nonnull ALPlatform*)platform didFailToLoadDialogueManager:(nonnull NSError*)error;
//@end
//
//@class JSContext;

JEROME_FRAMEWORK_SYMBOL_EXPORT
@interface ALTrainingState : NSObject
@end

@protocol ALUtterance <JSExport>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"

JSExportAs(has,
- (BOOL)hasValueForKey:(NSString* _Nonnull)key
);

JSExportAs(get,
- (NSString* _Nullable)valueForKey:(NSString* _Nonnull)key
);

JSExportAs(set,
- (void)setValue:(NSString* _Nullable)value forKey:(NSString* _Nonnull)key
);

#pragma clang diagnostic pop

- (NSString* _Nullable)objectForKeyedSubscript:(NSString* _Nonnull)key;
- (void)setObject:(NSString* _Nullable)obj forKeyedSubscript:(NSString* _Nonnull)key;

@end

JEROME_FRAMEWORK_SYMBOL_EXPORT
@interface ALUtterance : NSObject <ALUtterance>
@end

JEROME_FRAMEWORK_SYMBOL_EXPORT
@interface ALCollection : NSObject
@end

typedef void (^ALTrainingStateHandler)(ALTrainingState * _Nonnull);
typedef void (^ALDialogueScriptLoadedHandle)(NSString * _Nullable, NSError * _Nullable);
typedef void (^ALEngineEventHandler)(ALPlatformEvent * _Nonnull);
