var numberOfConnections = 10;
var numberOfStations = 10;
var exports = [
  { 
    "type": "heading", 
    "defaultValue": "myBahn Konfiguration" 
  },
  {
    "type": "toggle",
    "id": "HowtoToggle",
    "label": "Anleitung anzeigen",
    "group": "Howto", 
    "defaultValue": false
  },
  {
    "type": "text",
    "group": "Howto", 
    "defaultValue": 
      'Im ersten Abschnitt stellst Du die Standard-Verkehrsmittel ein, die Du für Deine Abfragen verwenden willst. Diese kannst Du für einzelne Verbindungen oder Bahnhöfe bei Bedarf gezielt übersteuern.'
  },
  {
    "type": "text",
    "group": "Howto", 
    "defaultValue": 
      'Für die Konfiguration von Bahnhöfen bzw. Verbindungen brauchst Du die jeweilige, 8-stellige IBNR bzw. EVA-Nr der Haltestellen.<br/>' +
      'Ein Verzeichnis findest Du z.B. bei der Deutschen Bahn unter <a href="http://data.deutschebahn.com/dataset/data-haltestellen">http://data.deutschebahn.com/dataset/data-haltestellen</a>.<br/>' +
      'Oder Du benutzt die Online-Suche von Michael Dittrich unter <a href="http://www.michaeldittrich.de/ibnr/online.php">http://www.michaeldittrich.de/ibnr/online.php</a>.<br/>' +
      'Ungültige oder falsche IBNR führen dazu, dass die Abfragen fehlschlagen oder falsche Daten liefern.'
  },
  {
    "type": "text",
    "group": "Howto", 
    "defaultValue": 
      'Im Abschnitt Verbindungen konfigurierst Du maximal 10 Fahrtstrecken zwischen je zwei festgelegten Bahnhöfen, die Du oft brauchst und daher schnell und einfach abfragen willst.<br/>' +
      'Nicht benötigte Einträge kannst Du über den Schalter neben der Verbingungsüberschrift ausschalten; d.h. nur die eingeschalteten Verbindungen erscheinen in der Verbindungsliste auf der Pebble.</br>' +
      'Du musst Start- und Ziel-Bahnhof durch die zugehörige IBNR/EVA-Nr sowie einen Titel zur Anzeige in der Liste auf der Pebble spezifizieren. Optional kannst Du noch einen Untertitel für die Anzeige angeben.<br/>' +
      'Fehlt eine der IBNR/EVA-Nr oder der Titel, ist die Verbindung ungültig und erscheint nicht in der Liste auf der Pebble!</br>' +
      'Titel  werden automatisch auf 20 Zeichen gekürzt, Untertitel auf 25.</br>' +
      'Zusätzlich kannst Du abweichend von den konfigurierten Standard-Verkehrsmitteln eigene Einstellungen für die bei dieser Verbindung erlaubten Verkehrsmittel vornehmen.'
  },
  {
    "type": "text",
    "group": "Howto", 
    "defaultValue": 
      'Im Abschnitt Bahnhöfe konfigurierst Du maximal 10 Bahnhöfe, zwischen denen Du beliebige Abfragen machen kannst. D.h. Du wählst auf der Pebble einen der Bahnhöfe als Start und einen als Ziel aus und bekommst eine Auskunft für die Fahrtstrecke.<br/>' +
      'Nicht benötigte Einträge kannst Du über den Schalter neben der Bahnhofsüberschrift ausschalten; d.h. nur die eingeschalteten Bahnhöfe erscheinen in der Bahnhofsliste auf der Pebble.</br>' +
      'Du musst den Bahnhof durch die zugehörige IBNR/EVA-Nr sowie einen Titel zur Anzeige in der Liste auf der Pebble spezifizieren. Optional kannst Du noch einen Untertitel für die Anzeige angeben.<br/>' +
      'Fehlt die IBNR/EVA-Nr oder der Titel, ist der Bahnhof ungültig und erscheint nicht in der Liste auf der Pebble!</br>' +
      'Titel  werden automatisch auf 20 Zeichen gekürzt, Untertitel auf 25.</br>' +
      'Zusätzlich kannst Du abweichend von den konfigurierten Standard-Verkehrsmitteln eigene Einstellungen für die an diesem Bahnhof erlaubten Verkehrsmittel vornehmen. ' +
      'Bei der Abfrage von Bahnhof zu Bahnhof wird die Summe aller an beiden Bahnhöfen erlaubten Verkehrsmittel verwendet.'
  },
  {
    "type": "text",
    "group": "Howto", 
    "defaultValue": 
      'Innerhalb der Bahnhöfe und Verbindungen kannst Du jeweils über den Button "Nach oben" die Einträge umsortieren.'
  }
];
function createProductSection(withUseDefaultToggle, prefix, suffix) {
  if (!prefix) {
    prefix = '';
  }
  if (!suffix) {
    suffix = '';
  }
  var result = [];
  if (withUseDefaultToggle) {
    result.push(    
     {
       "type": "toggle",
       "messageKey": prefix + "UseDefProd" + suffix,
       "label": "Standard-Verkehrsmittel verwenden",
       "group": prefix + "Products" + suffix,
       "defaultValue": true
    });
  }
  result.push(
  {
    "type": "toggle",
    "messageKey": prefix + "AllProd" + suffix,
    "label": "Alle",
    "group": prefix + "Products" + suffix,
    "defaultValue": true
  });
  result.push(
  {
    "type": "toggle",
    "messageKey": prefix + "ICE" + suffix,
    "label": "ICE",
    "group": prefix + "Products" + suffix,
    "defaultValue": true
  });
  result.push(
  {
    "type": "toggle",
    "messageKey": prefix + "IC" + suffix,
    "label": "IC/EC",
    "group": prefix + "Products" + suffix,
    "defaultValue": true
  });
  result.push(
  {
    "type": "toggle",
    "messageKey": prefix + "IR" + suffix,
    "label": "Interregio und Schnellzug",
    "group": prefix + "Products" + suffix,
    "defaultValue": true
  });
  result.push(
  {
    "type": "toggle",
    "messageKey": prefix + "RE" + suffix,
    "label": "Regio- und Nahverkehr",
    "group": prefix + "Products" + suffix,
    "defaultValue": true
  });
  result.push(
  {
    "type": "toggle",
    "messageKey": prefix + "S" + suffix,
    "label": "S-Bahn",
    "group": prefix + "Products" + suffix,
    "defaultValue": true
  });
  result.push(
  {
    "type": "toggle",
    "messageKey": prefix + "Bus" + suffix,
    "label": "Bus",
    "group": prefix + "Products" + suffix,
    "defaultValue": true
  });
  result.push(
  {
    "type": "toggle",
    "messageKey": prefix + "Schiff" + suffix,
    "label": "Schiff",
    "group": prefix + "Products" + suffix,
    "defaultValue": true
  });
  result.push(
  {
    "type": "toggle",
    "messageKey": prefix + "U" + suffix,
    "label": "U-Bahn",
    "group": prefix + "Products" + suffix,
    "defaultValue": true
  });
  result.push(
  {
    "type": "toggle",
    "messageKey": prefix + "Tram" + suffix,
    "label": "Straßenbahn",
    "group": prefix + "Products" + suffix,
    "defaultValue": true
  });
  result.push(
  {
    "type": "toggle",
    "messageKey": prefix + "AST" + suffix,
    "label": "Anruf Sammeltaxi (AST)",
    "group": prefix + "Products" + suffix,
    "defaultValue": true
  });
  return result;
}

var sectionDefaultJourneyProducts = createProductSection(false, "Def");
sectionDefaultJourneyProducts.unshift(
  {
    "type": "text", 
    "defaultValue": 
      "Stelle hier die Verkehrsmittel ein, die Du standardmäßig für Abfragen erlauben willst."
  }
);
sectionDefaultJourneyProducts.unshift(
  { 
    "type": "heading", 
    "defaultValue": "Standard-Verkehrsmittel"
  }
);

var sectionStationsItems = [
  { 
    "type": "heading", 
    "defaultValue": "Bahnhöfe" 
  }, 
  {
    "type": "text", 
    "defaultValue": 
      "Trage hier die Bahnhöfe ein, die Du in Bahnhof-zu-Bahnhof-Abfragen verwenden willst."

  }
];
for (var i = 0; i < numberOfStations; i++) {
  var statIndex = "[" + i +"]";
  var items = [
      {
        "type": "toggle",
        "messageKey": "StatEnabled"+statIndex,
        "group": "StatEnabled"+statIndex,
        "label": "Bahnhof " + (i+1)
      },
      {
        "type": "input",
        "messageKey": "StatTitle"+statIndex,
        "group": "StatEnabled"+statIndex,
        "label": "Titel",
        "attributes": {
          "limit": 20,
        }
      },
      {
        "type": "input",
        "messageKey": "StatSubTitle"+statIndex,
        "group": "StatEnabled"+statIndex,
        "label": "Untertitel (optional)",
        "attributes": {
          "limit": 25,
        }
      },
      {
        "type": "input",
        "messageKey": "StatIBNR"+statIndex,
        "group": "StatEnabled"+statIndex,
        "label": "IBNR/EVA-Nr",
        "attributes": {
          "limit": 8,
        }
      }
  ];
  
  var stationProducts = createProductSection(true, "Stat", statIndex);
  items.push(stationProducts);
  
  if (i > 0) {
    items.push(
      {
        "type": "button",
        "id" : "StatUp"+i,
        "primary": false,
        "group": "StatEnabled"+statIndex,
        "defaultValue": "Nach oben"
      }
    );
  }

  sectionStationsItems.push(
  {
    "type": "section",
    "items": items
  });

}  

var sectionConnectionsItems = [
  { 
    "type": "heading", 
    "defaultValue": "Verbindungen" 
  }, 
  { 
    "type": "text", 
    "defaultValue": 
      "Konfiguriere hier die Verbindungen, die Du häufig benutzt."
  }
];
for (var i = 0; i < numberOfConnections; i++) {
  var connIndex = "[" + i +"]";
  var items = [
      {
        "type": "toggle",
        "messageKey": "ConnEnabled"+connIndex,
        "group": "ConnEnabled"+connIndex,
        "label": "Verbindung " + (i+1)
      },
      {
        "type": "input",
        "messageKey": "ConnTitle"+connIndex,
        "group": "ConnEnabled"+connIndex,
        "label": "Titel",
        "attributes": {
          "limit": 20,
        }
      },
      {
        "type": "input",
        "messageKey": "ConnSubTitle"+connIndex,
        "group": "ConnEnabled"+connIndex,
        "label": "Untertitel (optional)",
        "attributes": {
          "limit": 25,
        }
      },
      {
        "type": "input",
        "messageKey": "ConnStartIBNR"+connIndex,
        "group": "ConnEnabled"+connIndex,
        "label": "Start IBNR/EVA-Nr",
        "attributes": {
          "limit": 8,
        }
      },
      {
        "type": "input",
        "messageKey": "ConnDestIBNR"+connIndex,
        "group": "ConnEnabled"+connIndex,
        "label": "Ziel IBNR/EVA-Nr",
        "attributes": {
          "limit": 8,
        }
      }
  ];
  
  var connectionProducts = createProductSection(true, "Conn", connIndex);
  items.push(connectionProducts);
  
  if (i > 0) {
    items.push(
      {
        "type": "button",
        "id" : "ConnUp"+i,
        "primary": false,
        "group": "ConnEnabled"+connIndex,
        "defaultValue": "Nach oben"
      }
    );
  }

  sectionConnectionsItems.push(
  {
    "type": "section",
    "items": items
  });
}

exports.push({
  "type" : "section",
  "items": sectionDefaultJourneyProducts
});
exports.push({
  "type" : "section",
  "items": sectionConnectionsItems
});
exports.push({
  "type" : "section",
  "items": sectionStationsItems
});
exports.push(
{
  "type": "submit",
  "defaultValue": "Speichern"
});
module.exports = exports;

