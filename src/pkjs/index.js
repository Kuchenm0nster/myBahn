var BahnInterface = require('./bahn-interface');
var keys = require('message_keys');

var Clay = require('pebble-clay');
var clayConfig = require('./config');
var customClay = require('./custom-clay');
var clay = new Clay(clayConfig, customClay, { autoHandleEvents: false });

var maxConfigKeysPerMessage = 100;
var msgBuffer = [];


Pebble.addEventListener('showConfiguration', function(e) {
  Pebble.openURL(clay.generateUrl());
});

// Overridden to fragment the config dictionary
Pebble.addEventListener('webviewclosed', function(e) {
  if (e && !e.response) {
    return;
  }

  // Get the keys and values from each config item
  var dict = clay.getSettings(e.response);

  var dictToSend = {};
  var count = 0;
  for (var key in dict) {
    if (dict.hasOwnProperty(key)) {
      count++;
      dictToSend[key] = dict[key];
      if (count >= maxConfigKeysPerMessage) {
        msgBuffer.push(dictToSend);
        count = 0;
        dictToSend = {};
      }
    }
  }
  if (count > 0) {
    msgBuffer.push(dictToSend);
  }
  flushMsgBuffer();
});

function flushMsgBuffer() {
  if (msgBuffer.length === 0) {
    return;
  }
  var dictToSend = msgBuffer.shift();
  Pebble.sendAppMessage(dictToSend, appMessageSent, appMessageFailed);
}

function appMessageSent(e) {
  flushMsgBuffer();
}

function appMessageFailed(e) {
    console.log('Failed to send data!');
    console.log(JSON.stringify(e));
}

// Listen for when the watchface is opened
Pebble.addEventListener('ready', 
  function(e) {
  }
);

// Listen for when an AppMessage is received
Pebble.addEventListener('appmessage',
  function(e) {
    var dict = e.payload;
    if (dict.BahnReqType) {
      handleBahninterfaceRequest(dict);
    }
  }                     
);

function handleBahninterfaceRequest(dict) {
  var requestType = dict.BahnReqType;
  var requestId = dict.BahnReqId;
  if (requestType == keys.BahnReqTypeBuildConnResults) {
    if (dict.BahnReqUrl) {
      var url = dict.BahnReqUrl;
      BahnInterface.getConnectionsFromUrl(url, function(results) {
        sendConnectionResults(requestId, results);
        flushMsgBuffer();
      });
    } else {
      var startStation = dict.BahnReqStartStation;
      var destStation = dict.BahnReqDestStation;
      var products = dict.BahnReqProducts;
      BahnInterface.getConnections(startStation, destStation, products, function(results) {
        sendConnectionResults(requestId, results);
        flushMsgBuffer();
      });
    }
  } else if (requestType == keys.BahnReqTypeGetConnDetails) {
    var detailLink = dict.BahnReqUrl;
    var maxLength = dict.BahnReqMaxLength;
    BahnInterface.getConnectionDetailInfos(detailLink, function(details) {
      sendResponse(requestId, keys.BahnReqTypeGetConnDetails, 1, 1, function(dict) {
        dict.BahnConnDetails = details ? details.substring(0, maxLength) : 'Keine Details gefunden!';
      });
      flushMsgBuffer();
    });
  }
}

function sendConnectionResults(requestId, results) {
  if (!results || results.length === 0) {
    sendResponse(requestId, keys.BahnRespTypeLoadFailed, 0, 0, function(dict) {
    });
    return;
  }
  
  var parts = results.length;
  for (var i = 0; i < parts; i++) {
    sendConnectionResult(requestId, results[i], i+1, parts);
  }
}

function sendConnectionResult(requestId, result, partNum, parts) {
    sendResponse(requestId, keys.BahnReqTypeGetConnResult, partNum, parts, function(dict) {
      if (result.moreLink) {
        if (result.earlier) {
          dict.BahnFrueherUrl = result.moreLink;
        } else {
          dict.BahnSpaeterUrl = result.moreLink;
        }
        return;          
      }
      
      var von = result.von + (result.delayStart ? ' '+result.delayStart : '');
      var bis = result.bis + (result.delayZiel ? ' '+result.delayZiel : '');
      var achtung = result.achtung ? ' !' : '';
      var ausfall = result.ausfall ? ' X' : '';
      var dauer = result.dauer ? ' ' + result.dauer : '';
      var umstiege = result.umstiege ? ' ' + result.umstiege : '';
      var vm = result.vm ? ' ' + result.vm : '';
    
      var resultTitle = von + achtung + ausfall;
      var resultSubtitle = bis + dauer + umstiege + vm;
      
      dict.BahnConnTitle = resultTitle;
      dict.BahnConnSubtitle = resultSubtitle;
      dict.BahnConnDetailsUrl = result.detailLink;
    });

}

function sendResponse(requestId, requestType, partNum, numParts, responseDataBuilder) {
  var dict = {};
  dict.BahnReqId = requestId;
  dict.BahnReqType = requestType;
  dict.BahnRespParts = numParts;
  dict.BahnRespPartNum = partNum;
  
  responseDataBuilder(dict);
  msgBuffer.push(dict);
}