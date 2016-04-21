//
//  NSString+al.h
//  NPCEditor
//
//  Created by Anton Leuski on 10/21/15.
//  Copyright © 2015 Anton Leuski. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface NSString (al)
+ (nullable instancetype)stringWithCPPString:(const std::string&)cppString;
+ (nullable instancetype)stringWithOptionalCPPString:(const jerome::optional<std::string>&)cppString;
- (jerome::String)cppString;
@end

@interface NSDictionary (al)
+ (nonnull NSDictionary<NSString*, NSString*>*)dictionaryWithStringMap:(const jerome::StringStringMap&)map;
- (jerome::StringStringMap)stringMap;
@end

@interface NSError (al)
+ (nonnull instancetype)errorWithError:(const jerome::Error&)error;
+ (nullable instancetype)errorWithOptionalError:(const jerome::optional<jerome::Error>&)error;
@end