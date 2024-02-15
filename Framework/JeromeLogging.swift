//
//  JeromeLogging.swift
//
//  Created by Anton Leuski on 1/10/24.
//  Copyright © 2024 Anton Leuski & ICT/USC. All rights reserved.
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
import Logging

public typealias Logger = Logging.Logger

extension Logger.Level {
  init?(jerome string: String) {
    switch string.lowercased() {
    case "verbose": self = .trace
    case "debug": self = .debug
    case "info": self = .info
    case "notice": self = .notice
    case "warning": self = .warning
    case "error": self = .error
    case "severe": self = .critical
    case "alert": self = .critical
    case "emergency": self = .critical
    default: return nil
    }
  }
}
