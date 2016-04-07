function Compiler()
{
  this._out = "";
  this.log = scion.ext.platformModule.platform.log;
  this.name = "s" + new Date().getTime();
  this._counter = 0;
  this._scxml = false;
  this.sendUtteranceEventName = "sendUtterance";
  this.sendCommandEventName = "sendCommand";
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
      this.log("Cannot combine SCXML and JS utterance code.");
      return false;
    }
    return true;
  },

  begin : function() {
    if (debugTrace) this.log("begin");

  },

  end : function() {
    if (debugTrace) this.log("end");

  },

  schedule : function(inUtteranceID, inEvent) {
    if (debugTrace) this.log("schedule", inUtteranceID);

    if (inEvent != null) {
      this.log("cannot schedule on non null event", inUtteranceID);
      return;
    }

    var utt = classifier.utteranceWithID(inUtteranceID);
    if (utt == null) {
      this.log("no utterance with ID", inUtteranceID);
      return;
    }

    var script  = utt.script;

    if (script == null || script.trim().length == 0) {

      if (!this._assertOutIsNotSCXML()) return;

      var thisID  = this._thisID();

      this._out += (
        '' +
        '<state id="$thisID">\n' +
        '	<onentry>\n' +
        '		<send id="$eventID" event="$eventName"\n'+
        '					type="$eventType" target="$eventTarget">\n'+
        '			<param name="utteranceID" expr="\'$expr\'"/>\n'+
        '			<param name="machineName" expr="\'$name\'"/>\n'+
        '			<param name="complete" expr="\'$complete\'"/>\n'+
        '			<param name="interrupted" expr="\'$interrupted\'"/>\n'+
        '			<param name="error" expr="\'$error\'"/>\n'+
        '		</send>\n'+
        '	</onentry>\n' +
        '	<transition event="$eventID.*" target="$nextID">\n' +
        '	</transition>\n' +
        '</state>\n' +
        '\n').format({
        thisID : thisID,
        nextID : this._nextID(),
        eventID : thisID,
        eventName : this.sendUtteranceEventName,
        eventType : jeromeEventType,
        eventTarget : jeromeEventTarget,
        expr : inUtteranceID,
        complete : thisID + ".complete",
        interrupted : thisID + ".interrupted",
        error : thisID + ".error",
        name : this.name
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
    if (debugTrace) this.log("rest", inDelaySeconds);

    if (inEvent != null) {
      this.log("cannot rest on non null event", inDelaySeconds);
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
      '\n').format({ thisID : thisID,
                     nextID : this._nextID(), eventName : thisID + '.tr',
                     delay : (Math.round(inDelaySeconds * 1000) +
                              'ms'), eventID : thisID });
  },

  _escape : function(s) {
    s = s.replace("&", "&amp;");
    s = s.replace("<", "&lt;");
    s = s.replace(">", "&gt;");
    s = s.replace("\r", "\\r");
    s = s.replace("\n", "\\n");
    s = s.replace("'", "\\&apos;");
    s = s.replace("\"", "&quot;");
    return s;
  },

  send : function(inCommand, inEvent) {
    if (debugTrace) this.log("send", inCommand);

    if (inEvent != null) {
      this.log("cannot send on non null event", inCommand);
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
      '\n').format({
      thisID : thisID,
      nextID : this._nextID(),
      content : this._escape(inCommand),
      eventID : thisID,
      eventName : this.sendCommandEventName,
      eventType : jeromeEventType,
      eventTarget : jeromeEventTarget,
      complete : thisID + ".complete",
      interrupted : thisID + ".interrupted",
      error : thisID + ".error",
      name : this.name
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
                  '\n').format({ thisID : thisID });

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
            '			<script>\n'+
            '				deleteStateMachine("$thisName");\n'+
            '			</script>\n'+
            '		</onentry>\n'+
            '	</state>\n' +
            '</scxml>\n' +
            '\n').format({ thisName : this.name });
  }
};