//
//  vrExpress.cpp
//  jerome
//
//  Created by Anton Leuski on 9/30/15.
//  Copyright © 2015 Anton Leuski & ICT/USC. All rights reserved.
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

#include <sstream>
#include <jerome/xml/writer.hpp>
#include <jerome/xml/writer_manip.hpp>
#include "vrExpress.hpp"

namespace ict {
  namespace vhmsg {

    String vrExpress::compose(const String& inSpeakerID,
                              const String& inText,
                              const String& inReferenceID,
                              const String& inInterruptID)
    {
			std::ostringstream out;
			xml::writer	writer(out);
			
			writer.stream()
				<< xml::indent(" ")
				<< xml::startElement("act");
			
			if (inSpeakerID.length()) {
				writer.stream()
					<< xml::startElement("participant")
					<< xml::attribute("id", inSpeakerID)
					<< xml::attribute("role", "actor")
					<< xml::endElement;
			}

			if (inText.length()) {
				writer.stream()
					<< xml::startElement("fml")
				
					<< xml::startElement("turn")
					<< xml::attribute("start", "take")
					<< xml::attribute("end", "give")
					<< xml::endElement
				
					<< xml::startElement("affect")
					<< xml::attribute("type", "neutral")
					<< xml::attribute("target", "addressee")
					<< xml::endElement

					<< xml::startElement("culture")
					<< xml::attribute("type", "neutral")
					<< xml::endElement

					<< xml::startElement("personality")
					<< xml::attribute("type", "neutral")
					<< xml::endElement
				
					<< xml::endElement;
			}

			{
				writer.stream()
					<< xml::startElement("bml");

//		if (nonemptyText(inPrefix)) {
//			sb.append(nl).append(sp).append(sp).append(inPrefix);
//		}

				if (inText.length()) {
					writer.stream()
						<< xml::startElement("speech")
						<< xml::attribute("id", "sp1")
						<< xml::attribute("type", "application/ssml+xml");
					
					if (inReferenceID.length()) {
						writer.stream()
							<< xml::attribute("ref", inReferenceID);
					}
					
					writer.stream()
						<< inText
						<< xml::endElement;
				}

				if (inInterruptID.length()) {
					writer.stream()
						<< xml::startElement("sbm:interrupt")
						<< xml::attribute("act", inInterruptID);
					
					if (inText.length()) {
						writer.stream()
							<< xml::attribute("end", "sp1:start");
					}
					
					writer.stream()
						<< xml::endElement;
				}
				
//		if (nonemptyText(inSuffix)) {
//			sb.append(nl).append(sp).append(sp).append(inSuffix);
//		}

				writer.stream()
					<< xml::endElement;
			}
			
			writer.stream()
				<< xml::endElement;
			writer.close();
			
			return out.str();
    }

  }
}
