var pSlice = Array.prototype.slice;
var Object_keys = typeof Object.keys === 'function'
? Object.keys
: function (obj) {
	var keys = [];
	for (var key in obj) keys.push(key);
	return keys;
}
;

function deepEqual (actual, expected) {
	function isUndefinedOrNull(value) {
		return value === null || value === undefined;
	}
	
	function isArguments(object) {
		return Object.prototype.toString.call(object) == '[object Arguments]';
	}
	
	function objEquiv(a, b) {
		if (isUndefinedOrNull(a) || isUndefinedOrNull(b))
			return false;
		// an identical 'prototype' property.
		if (a.prototype !== b.prototype) return false;
		//~~~I've managed to break Object.keys through screwy arguments passing.
		//   Converting to array solves the problem.
		if (isArguments(a)) {
			if (!isArguments(b)) {
				return false;
			}
			a = pSlice.call(a);
			b = pSlice.call(b);
			return deepEqual(a, b);
		}
		try {
			var ka = Object_keys(a),
			kb = Object_keys(b),
			key, i;
		} catch (e) {//happens when one is a string literal and the other isn't
			return false;
		}
		// having the same number of owned properties (keys incorporates
		// hasOwnProperty)
		if (ka.length != kb.length)
			return false;
		//the same set of keys (although not necessarily the same order),
		ka.sort();
		kb.sort();
		//~~~cheap key test
		for (i = ka.length - 1; i >= 0; i--) {
			if (ka[i] != kb[i])
				return false;
		}
		//equivalent values for every corresponding key, and
		//~~~possibly expensive deep test
		for (i = ka.length - 1; i >= 0; i--) {
			key = ka[i];
			if (!deepEqual(a[key], b[key])) return false;
		}
		return true;
	}
	
	// 7.1. All identical values are equivalent, as determined by ===.
	if (actual === expected) {
		return true;
		
	} else if (actual instanceof Date && expected instanceof Date) {
		return actual.getTime() === expected.getTime();
		
		// 7.3. Other pairs that do not both pass typeof value == 'object',
		// equivalence is determined by ==.
	} else if (typeof actual != 'object' && typeof expected != 'object') {
		return actual == expected;
		
		// 7.4. For all other Object pairs, including Array objects, equivalence is
		// determined by having the same number of owned properties (as verified
		// with Object.prototype.hasOwnProperty.call), the same set of keys
		// (although not necessarily the same order), equivalent values for every
		// corresponding key, and an identical 'prototype' property. Note: this
		// accounts for both named and indexed properties on Arrays.
	} else {
		return objEquiv(actual, expected);
	}
}

var pm = scion.ext.platformModule.platform;

function testOneScionCase(name, scxmlFileName, eventsFileName,
						  failed, errored, passed, log)
{
	function validateConfiguration(current, required) {
		var myCurrent	= current.sort();
		var myRequired	= required.sort();
		if (!deepEqual(myCurrent, myRequired)) {
			errored("CURRENT: ", myCurrent, "REQUIRED:", myRequired);
			return false;
		} else {
			return true;
		}
	}

	function interpret(model, events) {
		
		function pushEvent() {
			var event = events.events.shift();
			if (event) {
				function sendEvent() {
					log("sending event", event.event);
					var conf = interpreter.gen(event.event);
					if (validateConfiguration(conf, event.nextConfiguration)) {
						pushEvent();
					}
				}
				
				if (event.after) {
					pm.setTimeout(sendEvent, event.after);
				} else {
					sendEvent();
				}
			} else {
				passed();
			}
		}
		
		try {
			var interpreter = new scion.SCXML(model);
			var conf = interpreter.start();
			
			if (validateConfiguration(conf, events.initialConfiguration)) {
				pushEvent();
			}
			
		} catch (e) {
			errored("exception in interpret", e);
		}
	}

	function assertModelRead(err) {
		if (err) {
			failed("failed reading model", err);
			return false;
		}
		return true;
	}
	
	function assertEventsRead(err) {
		if (err) {
			failed("failed reading events", err);
			return false;
		}
		return true;
	}
	
	function readModel(scxmlUrl, events)
	{
		scion.urlToModel(scxmlUrl,
			function(err, model)
			{
				if (assertModelRead(err)) {
					interpret(model, events);
				}
			});
	}

	function handleEvents(err, eventsText)
	{
		if (assertEventsRead(err)) {
			var events = JSON.parse(eventsText);
			readModel(scxmlFileName, events);
		}
	}
	
	try {
		pm.getResourceFromUrl(eventsFileName, handleEvents);
	} catch (e) {
		failed("exception in main loop", e);
	}

}


function test(fileNamePairs) {
	var testsPassed = [], testsFailed = [], testsErrored = [];
	
//	var string = "a\
//	b\
//	c";
//	
//	pm.log(string);
//	pm.log('a');
	
	function testPair(fileNamePair, doneHandler) {
		
		function failed() {
			var msg = ""
			for(var i=0; i < arguments.length; i++){
				msg += (String(arguments[i]));
				msg += " ";
			}
			
			pm.log(msg);
			testsFailed.push(fileNamePair);
			pm.log("FAILED ", fileNamePair);
			doneHandler();
		}
		
		function passed() {
			var msg = ""
			for(var i=0; i < arguments.length; i++){
				msg += (String(arguments[i]));
				msg += " ";
			}
			
			pm.log(msg);

			testsPassed.push(fileNamePair);
			pm.log("PASSED ", fileNamePair);
			doneHandler();
		}
		
		function errored() {
			testsErrored.push(fileNamePair);
			pm.log("ERROED ", fileNamePair);
			doneHandler();
		}
		
		pm.log("----", fileNamePair);
		testOneScionCase(name, fileNamePair[0], fileNamePair[1],
						 failed, errored, passed, pm.log);
		
	}
	
	function complete() {
		pm.log("SUMMARY");
		pm.log("passed:  ",testsPassed.length);
		pm.log("failed:  ",testsFailed.length);
		pm.log("errored: ",testsErrored.length);
		//	pm.log("Tests passed",testsPassed);
		//	pm.log("Tests failed",testsFailed);
		pm.log("Tests errored",testsErrored);
		
		scionTestsAreDone();
	}
	
	function nextStepHandler() {
		//		pm.log("FIRE");
		testPairLoop(fileNamePairs.shift());
	}
	
	function nextStep() {
		pm.setTimeout(nextStepHandler, 10);
	}
	
	function testPairLoop(pair) {
		if (pair) {
			testPair(pair, nextStep);
		} else {
			complete();
		}
	}
	
	nextStep();
}