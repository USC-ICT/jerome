//
//  ScionPlatform.swift
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
import libxml2
import Jerome_Private

@objc protocol ALXMLDocProtocol: JSExport {
  var documentElement: ALXMLNode { get }
}

@objc protocol ALURLProtocol: JSExport {
  func getPathFromUrl(_ url: String) -> String?
  func changeUrlPath(_ url: String, _ newPath: String) -> String?
}

@objc protocol ALDomProtocol: JSExport {
  func getChildren(_ node: ALXMLNode) -> [ALXMLNode]
  func localName(_ node: ALXMLNode) -> String?
  func getAttribute(_ node: ALXMLNode, _ attribute: String) -> String?
  func hasAttribute(_ node: ALXMLNode, _ attribute: String) -> Bool
  func namespaceURI(_ node: ALXMLNode) -> String?
  func createElementNS(
    _ doc: ALXMLDoc, _ ns: String?, _ localName: String) -> ALXMLNode
  func setAttribute(_ node: ALXMLNode, _ name: String, _ value: String)
  func appendChild(_ parent: ALXMLNode, _ child: ALXMLNode) -> ALXMLNode
  func textContent(_ node: ALXMLNode, _ txt: String?) -> String?
  func getElementChildren(_ node: ALXMLNode) -> [ALXMLNode]
}

@objc protocol ALPathProtocol: JSExport {
  func join(_ path1: String, _ path2: String) -> String
  func dirname(_ path: String) -> String
  func basename(_ path: String, _ ext: String?) -> String
  func extname(_ path: String) -> String?
}

@objc protocol ALScionPlatformProtocol: JSExport {
  var url: ALURLProtocol { get }
  var dom: ALDomProtocol { get }
  var path: ALPathProtocol { get }

  func getDocumentFromUrl(_ URL: String, _ cb: JSValue)
  func parseDocumentFromString(_ content: String) -> ALXMLDoc?
  func getDocumentFromFilesystem(_ path: String, _ cb: JSValue)
  func getResourceFromUrl(_ url: String, _ cb: JSValue)
  // func postDataToUrl(_ url: String, _ cb: ALStringRsrcCallback)
  func setTimeout(_ cb: JSValue, _ timeout: Double) -> UInt32
  func clearTimeout(_ timeoutID: UInt32)
  func log()
  func eval(_ content: String, _ name: String) -> JSValue?
}

extension String {
  init?(releaseXmlPtr ptr: UnsafeMutablePointer<xmlChar>!) {
    guard let ptr = ptr
    else { return nil }
    defer { _alXmlFree(ptr) }
    self.init(cString: ptr)
  }
}

@objc class ALXMLNode: NSObject {
  let ptr: xmlNodePtr
  var releaseWhenDone: Bool

  init(ptr: xmlNodePtr, releaseWhenDone: Bool = false) {
    self.ptr = ptr
    self.releaseWhenDone = releaseWhenDone
  }

  deinit {
    if releaseWhenDone {
      xmlFreeNode(ptr)
    }
  }
}

@objc class ALXMLDoc: NSObject, ALXMLDocProtocol {
  let ptr: xmlDocPtr

  var documentElement: ALXMLNode {
    ALXMLNode(ptr: xmlDocGetRootElement(ptr))
  }

  init?(contentsAtPath path: String) {
    guard let ptr = xmlReadFile(path, nil, 0)
    else { return nil }
    self.ptr = ptr
    super.init()
  }

  static func document(withContentsAtPath path: String) -> ALXMLDoc? {
    return ALXMLDoc(contentsAtPath: path)
  }

  init?(data: Data, name: String?) {
    let name = name ?? "noname.xml"
    guard let ptr = data.withUnsafeBytes({ bytes in
      name.withCString { name in
        xmlReadMemory(
          bytes.bindMemory(to: CChar.self).baseAddress,
          Int32(data.count), name, nil, 0)
      }
    })
    else { return nil }
    self.ptr = ptr
    super.init()
  }

  static func document(withData data: Data, name: String?) -> ALXMLDoc? {
    return ALXMLDoc(data: data, name: name)
  }

  static func document(withString string: String, name: String?) -> ALXMLDoc? {
    guard let data = string.data(using: .utf8) else {
      return nil
    }
    return ALXMLDoc(data: data, name: name)
  }

  deinit {
    xmlFreeDoc(ptr)
  }
}

@objc class ALURL: NSObject, ALURLProtocol {
  func getPathFromUrl(_ url: String) -> String? {
    URL(string: url)?.path
  }

  func changeUrlPath(_ url: String, _ newPath: String) -> String? {
    // Not implemented
    assertionFailure("changeUrlPath: not implemented")
    return nil
  }
}

@objc class ALDom: NSObject, ALDomProtocol {
  func getChildren(_ node: ALXMLNode) -> [ALXMLNode] {
    var result = [ALXMLNode]()
    var aNode = node.ptr.pointee.children
    while let ptr = aNode {
      result.append(ALXMLNode(ptr: ptr))
      aNode = ptr.pointee.next
    }
    return result
  }

  func localName(_ node: ALXMLNode) -> String? {
    node.ptr.pointee.name.map { name in String(cString: name) }
  }

  func getAttribute(_ node: ALXMLNode, _ attribute: String) -> String? {
    String(releaseXmlPtr: xmlGetProp(node.ptr, attribute))
  }

  func hasAttribute(_ node: ALXMLNode, _ attribute: String) -> Bool {
    return xmlHasProp(node.ptr, attribute) != nil
  }

  func namespaceURI(_ node: ALXMLNode) -> String? {
    guard let ns = node.ptr.pointee.ns, let value = ns.pointee.href else {
      return nil
    }
    return String(cString: value)
  }

  func createElementNS(
    _ doc: ALXMLDoc, _ ns: String?, _ localName: String) -> ALXMLNode
  {
    var theNs: xmlNsPtr?
    if let ns = ns {
      theNs = xmlSearchNsByHref(doc.ptr, xmlDocGetRootElement(doc.ptr), ns)
      if theNs == nil {
        print("Cannot find prefix for \(ns)")
        // theNs = xmlNewNs(xmlDocGetRootElement(theDoc), nsURL, nil)
      }
    }
    return ALXMLNode(
      ptr: xmlNewNode(theNs, localName), releaseWhenDone: true)
  }

  func setAttribute(_ node: ALXMLNode, _ name: String, _ value: String) {
    xmlNewProp(node.ptr, name, value)
  }

  func appendChild(_ parent: ALXMLNode, _ child: ALXMLNode) -> ALXMLNode {
    child.releaseWhenDone = false
    xmlAddChild(parent.ptr, child.ptr)
    return child
  }

  private static func elementIndex(_ node: xmlNodePtr) -> Int {
    var index = 1
    var n = node
    while let prev = n.pointee.prev {
      if n != node && strcmp(node.pointee.name, prev.pointee.name) == 0 {
        index += 1
      }
      n = prev
    }
    return index
  }

  private static func xpathToNode(_ node: xmlNodePtr) -> String {
    guard let parent = node.pointee.parent else {
      return ""
    }

    if strcmp(node.pointee.name, "scxml") == 0 {
      return String(releaseXmlPtr: xmlGetProp(node, "name"))
        .map { value in "/scxml[@name='\(value)']" } ?? "/scxml"
    }

    let parentXPath = xpathToNode(parent)
    let index = elementIndex(node)
    return "\(parentXPath)/\(String(cString: node.pointee.name))[\(index)]"
  }

  func textContent(_ node: ALXMLNode, _ txt: String?) -> String? {
    if JSContext.currentArguments().count >= 2, let txt = txt {
      xmlNodeSetContent(node.ptr, "")
      xmlNodeAddContent(node.ptr, txt)
      return txt
    } else {
      let content = String(releaseXmlPtr: xmlNodeGetContent(node.ptr)) ?? ""

      if strcmp(node.ptr.pointee.name, "script") == 0 {
        let xpath = ALDom.xpathToNode(node.ptr)
        return "\n/* \(xpath)/begin */\n\(content)\n/* \(xpath)/end */\n"
      }

      return content
    }
  }

  func getElementChildren(_ node: ALXMLNode) -> [ALXMLNode] {
    var result = [ALXMLNode]()
    var aNode = node.ptr.pointee.children
    while let ptr = aNode {
      if ptr.pointee.type == XML_ELEMENT_NODE {
        result.append(ALXMLNode(ptr: ptr))
      }
      aNode = ptr.pointee.next
    }
    return result
  }
}

@objc class ALPath: NSObject, ALPathProtocol {
  func join(_ path1: String, _ path2: String) -> String {
    return (path1 as NSString).appendingPathComponent(path2)
  }

  func dirname(_ path: String) -> String {
    return (path as NSString).deletingLastPathComponent
  }

  func basename(_ path: String, _ ext: String?) -> String {
    var result = (path as NSString).lastPathComponent
    if let ext = ext, result.hasSuffix(".\(ext)") {
      result = (result as NSString).deletingPathExtension
    }
    return result
  }

  func extname(_ path: String) -> String? {
    return (path as NSString).pathExtension
  }
}

struct JeromeLogMessage {
  let level: Logger.Level
  let message: String

  init?(_ args: [Any]) {
    func _reformat<T: Sequence>(_ value: T) -> String {
      value.map {
        value in
        guard let value = value as? JSValue else { return "\(value)" }
        if value.isObject {
          return "\(value.toDictionary() ?? [:])"
        }
        return "\(value)"
      }.joined(separator: ", ")
    }
    guard let firstArgument = args.first else { return nil }
    if
      let level = (firstArgument as? String)?.lowercased(),
      let loggerLevel = Logger.Level(jerome: level)
    {
      self.level = loggerLevel
      self.message = _reformat(args.dropFirst())
    } else {
      self.level = .debug
      self.message = _reformat(args)
    }
  }
}

typealias ALLogHandlerBlock = (JeromeLogMessage) -> Void

@objc class ALScionPlatform: NSObject, ALScionPlatformProtocol {
  private(set) var url: ALURLProtocol
  private(set) var dom: ALDomProtocol
  private(set) var path: ALPathProtocol
  private var queue: DispatchQueue
  private var timers = [UInt32: DispatchSourceTimer]()
  private var lastUsedTimerID: UInt32 = 0
  private var logHandler: ALLogHandlerBlock

  init(queue: DispatchQueue, log: @escaping ALLogHandlerBlock) {
    self.url = ALURL()
    self.dom = ALDom()
    self.path = ALPath()
    self.queue = queue
    self.logHandler = log
  }

  class func scionPlatform(
    withQueue queue: DispatchQueue,
    log: @escaping ALLogHandlerBlock) -> ALScionPlatform
  {
    return ALScionPlatform(queue: queue, log: log)
  }

  deinit {
    let timers = self.timers.values.map { $0 }
    for timer in timers {
      timer.cancel()
    }
  }

  func getDocumentFromUrl(_ url: String, _ cb: JSValue) {
    if
      let theURL = URL(string: url),
      let data = try? Data(contentsOf: theURL),
      let doc = ALXMLDoc(data: data, name: nil)
    {
      cb.call(withArguments: [NSNull(), doc])
    } else {
      cb.call(withArguments: ["failed to parse XML document", NSNull()])
    }
  }

  func parseDocumentFromString(_ content: String) -> ALXMLDoc? {
    ALXMLDoc.document(withString: content, name: nil)
  }

  func getDocumentFromFilesystem(_ path: String, _ cb: JSValue) {
    if let doc = ALXMLDoc.document(withContentsAtPath: path) {
      cb.call(withArguments: [NSNull(), doc])
      return
    }
    cb.call(withArguments: ["failed to read and parse XML document", NSNull()])
  }

  func getResourceFromUrl(_ url: String, _ cb: JSValue) {
    if let actualURL = URL(string: url),
       let result = try? String(contentsOf: actualURL, encoding: .utf8) {
      cb.call(withArguments: [NSNull(), result])
      return
    }
    cb.call(withArguments: ["failed to load XML resource", NSNull()])
  }

  //    func postDataToUrl(_ url: String, _ cb: ALStringRsrcCallback) {}

  private func _clearTimeout(_ timeoutID: UInt32) {
    if let timer = timers.removeValue(forKey: timeoutID) {
      timer.cancel()
    }
  }

  @discardableResult
  func setTimeout(
    _ cb: JSValue, _ timeoutMilliSeconds: Double) -> UInt32
  {
    let timer = DispatchSource.makeTimerSource(queue: queue)
    while timers[lastUsedTimerID] != nil {
      if lastUsedTimerID == UInt32.max {
        lastUsedTimerID = 0
      } else {
        lastUsedTimerID += 1
      }
    }
    let identifier = lastUsedTimerID
    timers[identifier] = timer

    let interval = Int(timeoutMilliSeconds * Double(NSEC_PER_MSEC))

    timer.schedule(deadline: .now() + .nanoseconds(interval))

    timer.setEventHandler { [weak self] in
      guard let self = self else { return }
      self._log(["info", "calling timer callback"])
      timer.cancel()
      //      DispatchQueue.main.async {
      cb.call(withArguments: [])
      //      }
      self._clearTimeout(identifier)
    }

    timer.resume()

    return identifier
  }

  func clearTimeout(_ timeoutID: UInt32) {
    _clearTimeout(timeoutID)
  }

  private func _log(_ args: [Any]!) {
    guard
      let args = args,
      let message = JeromeLogMessage(args)
    else { return }
    logHandler(message)
  }

  func log() {
    _log(JSContext.currentArguments())
  }

  func eval(_ content: String, _ name: String) -> JSValue? {
    let src = "(function(){\nreturn \(content);})();"
    return JSContext.current()?.evaluateScript(
      src, withSourceURL: URL(fileURLWithPath: name))
  }
}
