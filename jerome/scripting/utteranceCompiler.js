function Compiler(inName)
{
  this._out = "";
  this.name = (inName) ? inName : ("s" + new Date().getTime());
  this._counter = 0;
  this._scxml = false;
  this.sendUtteranceEventName = "sendUtterance";
  this.sendCommandEventName = "sendCommand";
  this.machineDoneEventName = "machineDone";
}

/*
   the code handling sendUtterance or sendCommand
   should post one of the three events that passed in the parameter data
   structure: "complete", "interrupted", or "error" to the state machine
   with name "machineName"
 */

Compiler.prototype = {

  _formatID : function(n) {
    return this.name + "." + ("00" + n).slice(-3);
  },

  _thisID : function() {
    return this._formatID(this._counter++);
  },

  _nextID : function() {
    return this._formatID(this._counter);
  },

  _assertOutIsNotSCXML : function() {
    if (this._scxml) {
      pm.log("Cannot combine SCXML and JS utterance code.");
      return false;
    }
    return true;
  },

  begin : function() {
    if (debugTrace) pm.log("begin");

  },

  end : function() {
    if (debugTrace) pm.log("end");
  },

  // inUtteranceInfo is either a string continaing the utterance external ID
  // or an object with utterance ID in the utteranceID field and
  // stage ID in the stageID field.
  schedule : function(inUtteranceInfo, inEvent) {

    var stageID;
    var utteranceID;
    if (typeof inUtteranceInfo === 'object') {
      stageID = inUtteranceInfo.stageID;
      utteranceID = inUtteranceInfo.utteranceID;
    } else {
      utteranceID = inUtteranceInfo;
    }

    if (debugTrace) {
      pm.log("schedule", inUtteranceInfo);
    }

    if (inEvent != null) {
      pm.log("cannot schedule on non null event (for now).", inUtteranceInfo);
      return;
    }

    var script  = null;
    
    if (utteranceID) {
      var utt = classifier.utteranceWithID(utteranceID);
      if (utt == null) {
        pm.log("no utterance with ID", utteranceID);
      } else {
        script  = utt.script;
      }
    }

    if (script == null || script.trim().length == 0) {

      if (!this._assertOutIsNotSCXML()) return;

      var thisID  = this._thisID();

      this._out += (
        '' +
        '<state id="$thisID">\n' +
        '	<onentry>\n' +
        '		<send id="$eventID" event="$eventName"\n'+
        '					type="$eventType" target="$eventTarget">\n'+
        (utteranceID
         ? '			<param name="utteranceID" expr="\'$expr\'"/>\n'
                .formatXML({expr : utteranceID.escapeJS()}) : '') +
        (stageID
         ? '     <param name="stageID" expr="\'$stageID\'"/>\n'
                .formatXML({stageID : stageID.escapeJS()}) : '') +
        '			<param name="machineName" expr="\'$name\'"/>\n'+
        '			<param name="complete" expr="\'$complete\'"/>\n'+
        '			<param name="interrupted" expr="\'$interrupted\'"/>\n'+
        '			<param name="error" expr="\'$error\'"/>\n'+
        '		</send>\n'+
        '	</onentry>\n' +
        '	<transition event="$eventID.*" target="$nextID">\n' +
        '	</transition>\n' +
        '</state>\n' +
        '\n').formatXML({
        thisID : thisID,
        nextID : this._nextID(),
        eventID : thisID,
        eventName : this.sendUtteranceEventName,
        eventType : jeromeEventType,
        eventTarget : jeromeEventTarget,
        complete : (thisID + ".complete").escapeJS(),
        interrupted : (thisID + ".interrupted").escapeJS(),
        error : (thisID + ".error").escapeJS(),
        name : this.name.escapeJS()
      });

    } else {
      script  = script.trim();
      var scxmlTag  = "<scxml";
      if (script.length < scxmlTag.length ||
          script.substr(0, scxmlTag.length).toLowerCase() != scxmlTag) {
        eval(script);
      } else {
        this._out = script;
        this._scxml = true;
      }
    }
  },

  rest : function(inDelaySeconds, inEvent) {
    if (debugTrace) pm.log("rest", inDelaySeconds);

    if (inEvent != null) {
      pm.log("cannot rest on non null event", inDelaySeconds);
      return;
    }

    if (!this._assertOutIsNotSCXML()) return;

    var thisID  = this._thisID();
    this._out += (
      '' +
      '<state id="$thisID">\n' +
      '	<onentry>\n' +
      '		<send event="$eventName" delay="$delay" id="$eventID">\n'+
      '		</send>\n'+
      '	</onentry>\n' +
      '	<transition event="$eventName" target="$nextID">\n' +
      '	</transition>\n' +
      '	<onexit>\n' +
      '		<cancel sendid="$eventID"/>\n'+
      '	</onexit>\n' +
      '</state>\n' +
      '\n').formatXML({
                   thisID : thisID,
                   nextID : this._nextID(),
                   eventName : thisID + '.tr',
                   delay : (Math.round(inDelaySeconds * 1000) + 'ms'),
                   eventID : thisID
                   });
  },

  send : function(inCommand, inEvent) {
    if (debugTrace) pm.log("send", inCommand);

    if (inEvent != null) {
      pm.log("cannot send on non null event", inCommand);
      return;
    }

    if (!this._assertOutIsNotSCXML()) return;

    var thisID  = this._thisID();
    this._out += (
      '' +
      '<state id="$thisID">\n' +
      '	<onentry>\n' +
      '		<send id="$eventID" event="$eventName"\n'+
      '					type="$eventType" target="$eventTarget">\n'+
      '			<param name="command" expr="\'$content\'"/>\n'+
      '			<param name="machineName" expr="\'$name\'"/>\n'+
      '			<param name="complete" expr="\'$complete\'"/>\n'+
      '			<param name="interrupted" expr="\'$interrupted\'"/>\n'+
      '			<param name="error" expr="\'$error\'"/>\n'+
      '		</send>\n'+
      '	</onentry>\n' +
      '	<transition event="$eventID.*" target="$nextID">\n' +
      '	</transition>\n' +
      '</state>\n' +
      '\n').formatXML({
      thisID : thisID,
      nextID : this._nextID(),
      content : inCommand.escapeJS(),
      eventID : thisID,
      eventName : this.sendCommandEventName,
      eventType : jeromeEventType,
      eventTarget : jeromeEventTarget,
      complete : (thisID + ".complete").escapeJS(),
      interrupted : (thisID + ".interrupted").escapeJS(),
      error : (thisID + ".error").escapeJS(),
      name : this.name.escapeJS()
    });

  },

  output : function() {
    if (this._scxml) return this._out;

    var thisID  = this._thisID();

    // last state
    this._out += ('' +
                  '<state id="$thisID">\n' +
                  '	<transition target="final"/>\n' +
                  '</state>\n' +
                  '\n').formatXML({ thisID : thisID });

    thisID  = this._thisID();

    return ('' +
            '<scxml\n' +
            '	xmlns="http://www.w3.org/2005/07/scxml"\n' +
            '	version="1.0"\n' +
            '	name="$thisName"\n' +
            '	profile="ecmascript"\n' +
            '	initial="$thisName">\n' +
            '<state id="$thisName">\n' +
            this._out +
            '	<history id="h">\n' +
            '	</history>\n' +
            '	<transition event="pause" target="paused">\n' +
            '	</transition>\n' +
            '</state>\n' +
            '	<state id="paused">\n' +
            '		<transition event="resume" target="h">\n'+
            '		</transition>\n'+
            '	</state>\n' +
            '	<state id="final">\n' +
            '		<onentry>\n'+
            '     <send id="$eventID" event="$eventName"\n'+
            '					type="$eventType" target="$eventTarget">\n'+
            '       <param name="machineName" expr="\'$stringThisName\'"/>\n'+
            '     </send>\n'+
            '			<script>\n'+
            '				deleteStateMachine("$thisName");\n'+
            '			</script>\n'+
            '		</onentry>\n'+
            '	</state>\n' +
            '</scxml>\n' +
            '\n').formatXML({
                         eventID : thisID,
                         eventName : this.machineDoneEventName,
                         eventType : jeromeEventType,
                         eventTarget : jeromeEventTarget,
                         thisName : this.name,
                         stringThisName : this.name.escapeJS()
                         });
  }
};
