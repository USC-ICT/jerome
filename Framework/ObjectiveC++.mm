//
//  NSString+al.m
//  NPCEditor
//
//  Created by Anton Leuski on 10/21/15.
//  Copyright © 2015 Anton Leuski. All rights reserved.
//

#include <string>
#include <jerome/types.hpp>
#import "ObjectiveC++.h"

@implementation NSString (al)
+ (instancetype)stringWithCPPString:(const std::string&)cppString
{
	return [NSString stringWithUTF8String:cppString.c_str()];
}

+ (instancetype)stringWithOptionalCPPString:(const jerome::optional<std::string>&)cppString
{
	if (cppString)
		return [self stringWithCPPString:*cppString];
	else
		return nil;
}

- (jerome::String)cppString
{
	return jerome::String([self UTF8String]);
}

@end

@implementation NSDictionary (al)
+ (nonnull NSDictionary<NSString*, NSString*>*)dictionaryWithStringMap:(const jerome::StringStringMap&)map
{
	NSMutableDictionary* result = [NSMutableDictionary dictionary];
	for(const auto& x : map) {
		[result setObject:[NSString stringWithCPPString:x.second] forKey:[NSString stringWithCPPString:x.first]];
	}
	return result;
}

- (jerome::StringStringMap)stringMap
{
	jerome::StringStringMap map;
	for(NSString* key in self) {
		map.emplace(key.cppString, ((NSString*)self[key]).cppString);
	}
	return map;
}

@end

@implementation NSError (al)
+ (nonnull instancetype)errorWithError:(const jerome::Error&)error
{
	return [NSError errorWithDomain:@"jerome"
														 code:0
												 userInfo:@{ NSLocalizedDescriptionKey : [NSString stringWithCPPString:error.description()],
																		 NSLocalizedRecoverySuggestionErrorKey : [NSString stringWithCPPString:error.recoverySuggestion()]}];
}

+ (nullable instancetype)errorWithOptionalError:(const jerome::optional<jerome::Error>&)error
{
	return error ? [self errorWithError:*error] : nil;
}
@end