//
//  vrExpress.cpp
//  jerome
//
//  Created by Anton Leuski on 9/30/15.
//  Copyright © 2015 Anton Leuski & ICT/USC. All rights reserved.
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