#pragma once
#include <pebble.h>
#define APP_NAME "myBahn 1.2"
#define NOT_CONFIGURED "Nicht konfiguriert!"
#define PLEASE_CONFIGURE "Bitte konfigurieren!"
#define CONNECTIONS "Verbindungen"
#define START_STATIONS "Von Station"
#define TO_MARKER "->"
#define FRUEHER "Früher"
#define SPAETER "Später"
#define LOAD_FAILED_MESSAGE "Abfrage fehlgeschlagen.\nIBNR korrekt?\nKeine Verbindung zu Internet oder Phone?"
#define LOADING_MESSAGE "Abfrage läuft..."
#define APP_ICON getAppIcon()

GBitmap* getAppIcon();
StatusBarLayer* addStatusBar(Window* window);
GRect computeEffectiveWindowBounds(Window* window, StatusBarLayer* status_bar_layer);



