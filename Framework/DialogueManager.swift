//
//  DialogueManager.swift
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
import Jerome_Private

extension JSContext {
  @discardableResult
  func evaluate(_ scriptLiteral: ALScriptLiteral) -> JSValue! {
    evaluateScript(
      scriptLiteral.content,
      withSourceURL: URL(fileURLWithPath: scriptLiteral.name))
  }
}

// the context and queue are not sendable, but access
// to context is synchronized on the queue
final public class DialogueManager: @unchecked Sendable // valid Sendable
{
  enum Errors: Foundation.LocalizedError {
    case failedToInitializeMachine(String?)
    case missingMachineName
  }

  private let context: JSContext
  private let queue = DispatchQueue(label: "DialogueManager")

  public let name: String
  public let metadata: [String: Any]
  public let classifier: Classifier

  public typealias EventStream = AsyncStream<Event>

  public let eventStream: EventStream
  private let streamContinuation: EventStream.Continuation

  private typealias InitStateMachineCallback = @convention(block) (
    JSValue?, NSDictionary?) -> Void

  public init(source: String, classifier: Classifier) async throws {
    self.classifier = classifier
    
    let context: JSContext = .init()
    self.context = context
    self.context.exceptionHandler = { _, exception in
      classifier.logger.critical("\(String(describing: exception))")
    }

    self.context.setObject(
      ALScionPlatform(
        queue: queue, log: { message in
          classifier.logger.log(level: message.level, "\(message.message)")}),
      forKeyedSubscript: "__scionPlatform" as NSString)

    self.context.evaluate(ALDialogueManagerBridge.scion)
    self.context.evaluate(ALDialogueManagerBridge.startup)
    self.context.evaluate(ALDialogueManagerBridge.utteranceCompiler)

    var savedContinuation: EventStream.Continuation?
    self.eventStream = EventStream { continuation in
      savedContinuation = continuation
    }
    guard let savedContinuation = savedContinuation
    else { fatalError("cannot happen") }
    self.streamContinuation = savedContinuation

    let adapter = EngineAdapter(clasifier: classifier)
    context.setObject(
      adapter, forKeyedSubscript: "classifier" as NSString)

    let metadata: NSDictionary = try await withCheckedThrowingContinuation
    { continuation in
      let callback: InitStateMachineCallback = { error, data in
        if let data = data {
          continuation.resume(returning: data)
        } else {
          continuation.resume(
            throwing: Errors.failedToInitializeMachine(error?.description))
        }
      }
      let callbackObject = JSValue(object: callback, in: context) ??
      { fatalError("failed to create a JSValue from a callback") }()
      context.objectForKeyedSubscript("initStateMachineWithString")
        .call(withArguments: [source, callbackObject])
    }

    guard let name = metadata["name"] as? String
    else { throw Errors.missingMachineName }

    self.name = name
    self.metadata = (metadata as? [String: Any]) ?? [:]

    adapter.dialogueManager = self
  }

  public convenience init(
    contentsOf url: URL, classifier: Classifier) async throws
  {
    try await self.init(
      source: String(contentsOf: url), classifier: classifier)
  }

  public convenience init(data: Data, classifier: Classifier) async throws {
    try await self.init(
      source: String(decoding: data, as: UTF8.self),
      classifier: classifier)
  }

  deinit {
    streamContinuation.finish()
  }

  func emit(event: Event) {
    streamContinuation.yield(event)
  }

  private func _postEvent(
    name: String, to machine: String, context data: [String: Any])
  {
    self.context.objectForKeyedSubscript("postEventToStateMachine")
      .call(withArguments: [
        machine as NSString, name as NSString, data as NSDictionary])
  }

  public func postEvent(
    name: String, to machine: String, context: [String: any Sendable])
  {
    queue.async {
      self._postEvent(name: name, to: machine, context: context)
    }
  }

  public struct Event: Sendable {
    public let name: String
    public let type: String?
    public let target: String?
    public let origin: String?
    public let data: [String: any Sendable]
  }

  public static let didTransitionToStateEventName = "didTransitionToState"

}

@objc protocol DialogueManagerProtocol: JSExport {
  @objc func receiveEvent(
    _ name: NSString,
    _ type: NSString,
    _ target: NSString,
    _ origin: NSString,
    _ data: NSDictionary)
  @objc func didTransitionToState(_ name: NSString, _ data: NSDictionary)
}

extension EngineAdapter: DialogueManagerProtocol {
  func receiveEvent(
    _ name: NSString,
    _ type: NSString,
    _ target: NSString,
    _ origin: NSString,
    _ data: NSDictionary)
  {
    dialogueManager?.emit(event: .init(
      name: name as String,
      type: type as String,
      target: target as String,
      origin: origin as String,
      data: (data as? [String: Any]) ?? [:]))
  }

  func didTransitionToState(_ stateName: NSString, _ data: NSDictionary) {
    dialogueManager?.emit(event: .init(
      name: DialogueManager.didTransitionToStateEventName,
      type: nil,
      target: stateName as String,
      origin: nil, data: (data as? [String: Any]) ?? [:]))
  }
}
