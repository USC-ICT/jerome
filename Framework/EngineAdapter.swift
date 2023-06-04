//
//  EngineAdapter.swift
//  JeromeNLU
//
//  Created by Anton Leuski on 6/3/23.
//

import Foundation

@objc final class EngineAdapter: NSObject {
  weak var classifier: Classifier?
  weak var dialogueManager: DialogueManager?

  init(clasifier: Classifier? = nil, dialogueManager: DialogueManager? = nil) {
    self.classifier = clasifier
    self.dialogueManager = dialogueManager
    super.init()
  }
}
