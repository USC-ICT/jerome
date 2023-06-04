//
//  Classifier.swift
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

import Foundation
import JavaScriptCore
import XCGLogger
import Jerome_Private

public typealias Utterance = ALUtterance

final public class Classifier {
  private let bridge: ALClassifierBridge
  public let logger: XCGLogger

  public init(data: Data, logger: XCGLogger) throws {
    self.bridge = try ALClassifierBridge(data: data)
    self.logger = logger
  }

  public func utterance(id: UtteranceIdentifier) -> Utterance? {
    bridge.utterance(withID: id.rawValue)
  }

  public func answers(in domain: String? = nil) -> [Utterance] {
    bridge.answers(inDomain: domain)
  }

  public func search(
    for query: String, in domain: String) throws -> [Utterance]
  {
    try bridge.search(for: query, inDomain: domain)
  }

  public func search(
    for query: [String: String], in domain: String) throws -> [Utterance]
  {
    try bridge.search(for: query, inDomain: domain)
  }

  public func collectionWasUpdated(state: String? = nil) {
    bridge.collectionWasUpdated(inDomain: state)
  }

  public convenience init(contentsOf url: URL, logger: XCGLogger) throws {
    try self.init(data: Data(contentsOf: url), logger: logger)
  }
}

public extension ALUtterance {
  //  @nonobjc subscript(key: UtteranceField) -> String? {
  //    get { value(forKey: key.rawValue) }
  //    set { setValue(newValue, forKey: key.rawValue) }
  //  }

  var text: String? {
    get { value(forKey: "text") }
    set { setValue(newValue, forKey: "text")}
  }
}

// this object is read-only
extension ALUtterance: @unchecked Sendable // valid Sendable
{}

@objc protocol ClassifierAdapterProtocol: JSExport {
  @objc func search(_ name: NSString, _ query: JSValue) -> [ALUtterance]
  @objc func answers(_ name: NSString) -> [ALUtterance]
  @objc func utteranceWithID(_ id: NSString) -> ALUtterance?
}

extension EngineAdapter: ClassifierAdapterProtocol {
  func search(_ name: NSString, _ query: JSValue) -> [ALUtterance] {
    guard let client = classifier else { return [] }

    do {
      if query.isString {
        return try client.search(for: query.toString(), in: name as String)
      }

      if query.isObject {
        let query = Dictionary(uniqueKeysWithValues: query.toDictionary()
          .compactMap { (key, value) -> (String, String)? in
            guard let key = key as? String,
                  let value = (value as? String)?.description ??
                    (value as? NSNumber)?.description
            else { return nil }
            return (key, value)
          })
        return try client.search(for: query, in: name as String)
      }
    } catch {
      client.logger.error(error)
    }

    return []
  }

  func answers(_ name: NSString) -> [ALUtterance] {
    classifier?.answers(
      in: JSContext.currentArguments().isEmpty ? nil : name as String) ?? []

  }

  func utteranceWithID(_ id: NSString) -> ALUtterance? {
    classifier?.utterance(id: UtteranceIdentifier(rawValue: id as String))
  }
}
