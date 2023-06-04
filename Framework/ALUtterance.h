//
//  ALUtterance.h
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
#import <JavaScriptCore/JavaScriptCore.h>
#import <Jerome/Macros.h>

NS_ASSUME_NONNULL_BEGIN

@protocol ALUtterance <JSExport>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"

JSExportAs(has,
           - (BOOL)hasValueForKey:(NSString*)key
           );

JSExportAs(get,
           - (nullable NSString*)valueForKey:(NSString*)key
           );

JSExportAs(set,
           - (void)setValue:(nullable NSString*)value forKey:(NSString*)key
           );

#pragma clang diagnostic pop

- (nullable NSString*)objectForKeyedSubscript:(NSString*)key;
- (void)setObject:(nullable NSString*)obj forKeyedSubscript:(NSString*)key;

@end

JEROME_FRAMEWORK_SYMBOL_EXPORT
@interface ALUtterance : NSObject <ALUtterance>
@end

NS_ASSUME_NONNULL_END
