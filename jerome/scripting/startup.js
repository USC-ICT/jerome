
// setup our platform

var scion = require('scion');
scion.ext.platformModule.platform = __scionPlatform;

var debugTrace = false;
var pm = scion.ext.platformModule.platform;
var jeromeEventType = "http://www.ict.usc.edu";
var jeromeEventTarget = "jerome:send";

// JavaScriptCore does not implement bind() in the current verion. Adding...

if (!Function.prototype.bind) {
  Function.prototype.bind = function(oThis) {
    if (typeof this !== "function") {
      // closest thing possible to the ECMAScript 5 internal IsCallable function
      throw new TypeError(
        "Function.prototype.bind - what is trying to be bound is not callable");
    }

    var aArgs = Array.prototype.slice.call(arguments, 1),
        fToBind = this,
        fNOP = function() {
    },
        fBound = function() {
      return fToBind.apply(this instanceof fNOP && oThis
        ? this
        : oThis,
        aArgs.concat(Array.prototype.slice.call(arguments)));
    };

    fNOP.prototype = this.prototype;
    fBound.prototype = new fNOP();

    return fBound;
  };
}

if (!Array.prototype.findAll) {
  Array.prototype.findAll = function(filter)
  {
    var result = new Array();
    for (var i = 0; this.length > i; i++) {
      if (filter(this[i])) {
        result.push(this[i]);
      }
    }
    return result;
  };
}

if (!Array.prototype.shuffle) {
  Array.prototype.shuffle = function() {
    var i = this.length, j, tempi, tempj;
    if (i == 0) return false;
    while (--i) {
      j = Math.floor(Math.random() * (i + 1));
      tempi = this[i];
      tempj = this[j];
      this[i] = tempj;
      this[j] = tempi;
    }
    return this;
  };
}

/* warning. This will break if one argument name is a prefix of another! */
String.prototype.format = function() {
  s = this;
  if (arguments.length == 1 && arguments[0].constructor == Object) {
    for (var key in arguments[0]) {
      s = s.replace(new RegExp("\\$" + key, "g"), arguments[0][key]);
    }
  } else {
    for (var i = 0; i < arguments.length; i++) {
      s = s.replace(new RegExp("\\$" + (i + 1), "g"), arguments[i]);
    }
  }
  return s;
};

String.prototype.formatXML = function() {
  s = this;
  if (arguments.length == 1 && arguments[0].constructor == Object) {
    for (var key in arguments[0]) {
      s = s.replace(new RegExp("\\$" + key, "g"), arguments[0][key].escapeXML());
    }
  } else {
    for (var i = 0; i < arguments.length; i++) {
      s = s.replace(new RegExp("\\$" + (i + 1), "g"), arguments[i].escapeXML());
    }
  }
  return s;
};

String.prototype.escapeJS = function() {
  return this.replace(
    new RegExp("[\\'\"]", "g"),
    function (c) {
      switch (c) {
        case "\\": return "\\\\";
        case "'": return "\\'";
        case "\"": return "\\\"";
      }
    });
}

String.prototype.jsStringLiteral = function() {
  return "'" + this.escapeJS() + "'";
}

String.prototype.escapeXML = function() {
  return this.replace(
    new RegExp("[<>&'\"]", "g"),
    function (c) {
      switch (c) {
        case "<": return "&lt;";
        case ">": return "&gt;";
        case "&": return "&amp;";
        case "'": return "&apos;";
        case "\"": return "&quot;";
      }
    });
}


var _send = function(event, options, fnRaise) {
  var callback, timeoutId,
    _this = this;
  if (pm.setTimeout) {
    if (debugTrace) {
      pm.log("sending event",
        event.name,
        "of type",
        event.type,
        "with content",
        event.data,
        "with origin",
        event.origin,
        "to target",
        event.target,
        "after delay",
        options.delay);
    }

    if (event.type == jeromeEventType) {
      classifier.receiveEvent(event.name, event.type, event.target,
				event.origin, event.data);
    } else {
      callback = function() {
        return _this.gen(event);
      };
      timeoutId = pm.setTimeout(callback, options.delay);
      if (options.sendid) return this._timeoutMap[options.sendid] = timeoutId;
    }

  } else {
    throw new Error("setTimeout function not set");
  }
};

var stateMachines = {};

function loadStateMachineCompleteBlock(cb)
{
  return function(err, model) {
           if (err) {
             if (debugTrace) pm.log("failed reading model", err);
             if (cb) cb(err);
           } else {
             if (debugTrace) pm.log("success reading model", model.name);
             var sm = new scion.SCXML(model, { send: _send });
             stateMachines[model.name] = sm;
             if (cb) cb(null, model);
             var conf = sm.start();
             if (debugTrace) pm.log("conf", conf, model.name);
           }
  }
}

// str - the scxml source
// cb - callback(err, name) that is called when the SM is loaded

function initStateMachineWithString(str, cb, prefix)
{
	var doc = pm.parseDocumentFromString(str);
	var ourCB = loadStateMachineCompleteBlock(cb);

	if (doc) {
		var root = doc.documentElement;
		var name = pm.dom.getAttribute(root, "name");
		prefix = typeof prefix !== 'undefined' ? prefix : "sm";
		scion.documentToModel("jerome:" + prefix + ":" + name, doc, ourCB);
	} else {
		ourCB("failed to parse csxml", null)
	}
}

var engine;

// inUtteranceInfo is either a string continaing the utterance external ID
// or an object with utterance ID in the utteranceID field and
// stage ID in the stageID field.
function triggerUtterance(inUtteranceInfo, inName, cb)
{
  if (debugTrace) {
    pm.log("triggerUtterance:", inUtteranceInfo, inName);
  }

  var callback = function() {
    if (debugTrace) {
      pm.log("generating utterance FSA");
    }

    engine = new Compiler(inName);

    if (debugTrace) {
      pm.log("got compiler");
    }

    engine.schedule(inUtteranceInfo);

    if (debugTrace) {
      pm.log("output:", engine.output());
      pm.log("asking the scion to read scxml from the output");
    }
		
		initStateMachineWithString(engine.output(), null, "_sm");
		
//    scion.documentStringToModel(engine.output(),
//      loadStateMachineCompleteBlock(cb));

    if (debugTrace) {
      pm.log("done reading scxml");
    }
  }

  // I want to handle the utterance _after_ the event loop tick that caused it
  // completes
  pm.setTimeout(callback, 0);
}

function deleteStateMachine(inSMID)
{
		//	pm.didDestroyStateMachine(inSMID)
  stateMachines[inSMID] = null;
}

function postEventToStateMachine(inSMID, inEventName, inEventData)
{
  if (debugTrace) pm.log("calling", inSMID, inEventName, inEventData);

  var sm = stateMachines[inSMID];
  if (sm != null) {
    if (debugTrace) pm.log("calling", inEventName, inEventData);
    var conf = sm.gen(inEventName, inEventData);
    if (debugTrace) pm.log("conf", conf);
  }
}
