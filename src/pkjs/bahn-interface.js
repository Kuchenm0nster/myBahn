var baseUrl = 'http://mobil.bahn.de/bin/query.exe/dox?start=1&rt=1';
var detailsUrlBase = 'http://reiseauskunft.bahn.de/bin/query2.exe/dox?';

function xhrRequest(url, type, callback) {
  var xhr = new XMLHttpRequest();
  var async = callback ? true : false;
  if (async) {
    xhr.onreadystatechange = function () {
      if (!this.readyState) {
        return;
      }
      if (this.readyState === 4) {
        if (!this.status) {
          callback(null);
        }
        if (this.status >= 200 && this.status < 300) {
          callback(this.responseText);
        } else {
          callback(null);
        }
      } 
    };
  }
  try {
    xhr.open(type, url, async);
    xhr.send();
  } catch(error) {
    if (callback) {
      callback(null);
    }
    return null;
  }
  if (!async) {
    if (xhr.status >= 200 && xhr.status < 300) {
      return xhr.responseText;
    } else {
      return null;
    }
  }
}

function extractConnectionResultInfos(text) {
  if (!text) {
    return null;
  }
  
  var detailLinkExp = /a href="([^"]*)"/;
  var zeitExp = /span class="bold" *>(\d*:\d*)</g;
  var vmExp = /<td class="overview iphonepfeil" *>([^<]*)</;
  var umstiegeUndDauerExp = /<td class="overview" *>(\d*) *<br *.>(\d*:\d*)/;
  var delaysExp = /<td class="overview tprt" *> *<span[^>]*>(.\d*)<.*<span[^>]*>(.\d*)</;
  var nurStartDelayExp = /<td class="overview tprt" *> *<span[^>]*>(.\d*)</;
  var nurZielDelayExp = /<td class="overview tprt" *> *&nbsp.*<span[^>]*>(.\d*)</;
  var achtungExp = /achtung_17x19_mitschatten.png/;
  var ausfallExp = /Icon_Zug_faellt_aus_mit_Schatten_17x19.png/;
  var frueherExp = /(http:\/\/reiseauskunft\.bahn\.de\/bin\/query2\.exe\/dox\?[^"]*amp;e=2[^"]*)"/;
  var spaeterExp = /(http:\/\/reiseauskunft\.bahn\.de\/bin\/query2\.exe\/dox\?[^"]*&amp;e=1[^"]*)"/;
  var ampExp = /&amp;/gm;
  
  var results = [];

  var resultStrings = text.replace(/(\r\n)|(\n|\r)|\t/gm,"").split('<td class="overview timelink">');
  
  var frueherLink = text.match(frueherExp);
  if (frueherLink) {
    frueherLink = frueherLink[1].replace(ampExp, "&").substring(detailsUrlBase.length);
    results.push({moreLink: frueherLink, earlier: true});
  }
  
  for (var i = 1; i < resultStrings.length; i++) {
    var connectionInfo = resultStrings[i].trim();
    
    var result = {};
    
    var detailLink = connectionInfo.match(detailLinkExp);
    if (detailLink) {
      result.detailLink = detailLink[1].replace(ampExp, "&").substring(detailsUrlBase.length);
    }
    
    zeitExp.lastIndex = 0;
    result.von = zeitExp.exec(connectionInfo)[1];
    result.bis = zeitExp.exec(connectionInfo)[1];
    
    var vm = connectionInfo.match(vmExp);
    if (vm) {
      result.vm = vm[1].trim();
    }
    
    var umstiegeUndDauer = connectionInfo.match(umstiegeUndDauerExp);
    if (umstiegeUndDauer) {
      result.umstiege = umstiegeUndDauer[1];
      result.dauer = umstiegeUndDauer[2];
    }
    
    var delays = connectionInfo.match(delaysExp);
    if (delays) {
      result.delayStart = delays[1];
      result.delayZiel = delays[2];
    } else {
      var nurZielDelay = connectionInfo.match(nurZielDelayExp);
      if (nurZielDelay) {
        result.delayZiel = nurZielDelay[1];
      } else {
        var nurStartDelay = connectionInfo.match(nurStartDelayExp);
        if (nurStartDelay) {
          result.delayStart = nurStartDelay[1];
        }
      }
    }
    
    if (connectionInfo.match(achtungExp)) {
      result.achtung = true;
    } else {
      result.achtung = false;
    }
    
    if (connectionInfo.match(ausfallExp)) {
      result.ausfall = true;
    } else {
      result.ausfall = false;
    }
    
    results.push(result);
  }
  
  var spaeterLink = text.match(spaeterExp);
  if (spaeterLink) {
    spaeterLink = spaeterLink[1].replace(ampExp, "&").substring(detailsUrlBase.length);
    results.push({moreLink: spaeterLink, later: true});
  }
  return results;
}

function extractConnectionDetailInfos(text) {
  if (!text) {
    return null;
  }
  var achtungExp = /<[^<]*achtung_17x19_mitschatten.png[^>]*>/gm;
  var ausfallExp = /<[^<]*Icon_Zug_faellt_aus_mit_Schatten_17x19.png[^>]*>/gm;
  var warnungExp = /<div class="red bold haupt" *>([^<]*)<\/div>/;
  var spanExp = /<\/?span[^>]*>/gm;
  var himDetailsLinkExp = /"(http:\/\/reiseauskunft\.bahn\.de\/bin\/query2\.exe\/dox\?[^"]*&amp;him=[^"]*)"/;

  var htmlTagExp = /<[^<>]*>/gm;
  var brSurrogateExp = /&#br;/gm;

  var results = '';

  text = text
    .replace(/(\r\n)|(\n|\r)|\t/gm, '')
    .replace(/<br *\/?>/gm, '&#br;')
    .replace(spanExp, '');

  var resultLines = text
    .replace(/<div class="rlinebottom">.*/, '')
    .replace(achtungExp, ' !')
    .replace(ausfallExp, ' X')
    .replace(/Umstiegszeit/, '')
    .replace(/<div class="motSection">/gm, '> ')
    .replace(/<div class="interSection">/gm, '> ')
    .split('<div class="rline');
  
  var warnung = text.match(warnungExp);
  if (warnung) {
    warnung = warnung[1]
      .replace(brSurrogateExp, '\n')
      .replace(/^\s*/, '')
      .replace(/\s*$/, '');

    results += '! ' + warnung + '\n';
  }

  for (var i = 1; i < resultLines.length; i++) {
    var detailLine = '<div class="rline' + resultLines[i];
    
    detailLine = detailLine
      .replace(htmlTagExp, '')
      .replace(brSurrogateExp, '\n')
      .replace(/^\s*/, '')
      .replace(/\s*$/, '');
    
    if (results.length > 0) {
      results += '\n';
    }
    results += detailLine;
  }

  results = results
    .replace(/ Gl. /gm, "\nGl. ");

  // Hinweise extrahieren
  resultLines = text
    .replace(/(\r\n)|(\n|\r)|\t/gm,"")
    .replace(/<form action=.*/, '')
    .split('<div class="him">');

  for (i = 1; i < resultLines.length; i++) {
    var himLine = resultLines[i];
    
    himDetailsLinkExp.lastIndex = 0;
    var himDetailsLink = himLine.match(himDetailsLinkExp);
    if (himDetailsLink) {
      himLine = extractHimDetails(himDetailsLink[1]);
    }
    
    if (!himLine) {
      continue;
    }
    
    himLine = himLine
      .replace(/<\/div>.*/, '')
      .replace(brSurrogateExp, '\n')
      .replace(/^\s*/, '')
      .replace(/\s*$/, '');
    
    if (results.length > 0) {
      results += '\n';
    }
    results += himLine;
  }

  results = results
    .replace(/&nbsp;/gm, ' ')
    .replace(/&#196;/gm, "Ä")
    .replace(/&#214;/gm, "Ö")
    .replace(/&#220;/gm, "Ü")
    .replace(/&#223;/gm, "ß")
    .replace(/&#228;/gm, "ä")
    .replace(/&#246;/gm, "ö")
    .replace(/&#252;/gm, "ü");

  return results;
}

function extractHimDetails(himDetailsUrl) {
  var himLine = '';
  var himDetailsResponseText = xhrRequest(himDetailsUrl, 'GET');
  if (himDetailsResponseText) {
      var himDetailsLines = himDetailsResponseText
        .replace(/(\r\n)|(\n|\r)|\t/gm,"")
        .split('<div class="him">');
      himLine = himDetailsLines[1];
  }
  return himLine;
}

function getConnectionsFromUrl(url, resultCallback) {
  if (url.substring(0, 4) != 'http') {
    url = detailsUrlBase + url;
  }
  xhrRequest(url, 'GET', function(responseText) {
    var results = extractConnectionResultInfos(responseText);
    resultCallback(results);
  });
}

function getConnections(startKey, zielKey, journeyProducts, resultCallback) {
  var productParam = '';
  if (journeyProducts) {
    productParam = '&journeyProducts=' + journeyProducts;
  }
  var url = baseUrl + '&S=' + startKey + '&Z=' + zielKey + productParam;
  return getConnectionsFromUrl(url, resultCallback);
}

function getConnectionDetailInfos(detailLink, resultCallback) {
  xhrRequest(detailsUrlBase + detailLink, 'GET', function(responseText){
    var result = extractConnectionDetailInfos(responseText);
    resultCallback(result);
  }); 
}

module.exports = {
  getConnections : getConnections,
  getConnectionsFromUrl : getConnectionsFromUrl,
  getConnectionDetailInfos : getConnectionDetailInfos
};

