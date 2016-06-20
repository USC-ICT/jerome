//
//  model.h
//  jerome
//
//  Created by Anton Leuski on 5/11/16.
//  Copyright © 2016 Anton Leuski & ICT/USC. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <JavaScriptCore/JavaScriptCore.h>
#import <Jerome/Macros.h>

JEROME_FRAMEWORK_SYMBOL_EXPORT
@interface ALPlatformEvent : NSObject
@property (nonatomic, copy, nonnull)  NSString* name;
@property (nonatomic, copy, nullable)  NSString* type;
@property (nonatomic, copy, nullable)  NSString* target;
@property (nonatomic, copy, nullable)  NSString* origin;
@property (nonatomic, copy, nonnull)  NSDictionary<NSString*, NSString*>* data;
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
- (void)setValue:(NSString* _Nullable)value forKeyPath:(NSString* _Nonnull)keyPath
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
typedef void (^ALDialoguScriptLoadedHandled)(NSString * _Nullable, NSError * _Nullable);
typedef void (^ALEngineEventHandler)(ALPlatformEvent * _Nonnull);
