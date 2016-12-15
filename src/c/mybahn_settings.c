#include <pebble.h>
#include "resources.h"
#include "mybahn_settings.h"

//#define CREATE_DUMMY_SETTINGS
static MyBahnSettings settings;

static void prv_validate_station(StationConfig* station) {
  station->is_valid = true;
  if (!station->is_enabled) {
    station->is_valid = false;
  } else if (strlen(station->ibnr) == 0) {
    station->is_valid = false;
  } else if (strlen(station->title) == 0) {
    strncpy(station->title, station->ibnr, IBNR_LENGTH);
    station->title[IBNR_LENGTH+1] = '\0';
  }
}

static void prv_validate_connection(ConnectionConfig* connection) {
  connection->is_valid = true;
  if (!connection->is_enabled) {
    connection->is_valid = false;
  } else if (strlen(connection->start_ibnr) == 0) {
    connection->is_valid = false;
  } else if (strlen(connection->dest_ibnr) == 0) {
    connection->is_valid = false;
  } else if (strlen(connection->title) == 0) {
    strcpy(connection->title, connection->start_ibnr);
    strcat(connection->subtitle, TO_MARKER);
    strcat(connection->subtitle, connection->dest_ibnr);
  }
}

static void prv_validate_settings() {
  settings.number_of_valid_stations = 0;
  for (int i = 0; i < NUMBER_OF_STATIONS; i++) {
    prv_validate_station(&settings.stations[i]);
    if (settings.stations[i].is_valid) {
      settings.valid_stations[settings.number_of_valid_stations++] = i;
    }
  }
  settings.number_of_valid_connections = 0;
  for (int i = 0; i < NUMBER_OF_CONNECTIONS; i++) {
    prv_validate_connection(&settings.connections[i]);
    if (settings.connections[i].is_valid) {
      settings.valid_connections[settings.number_of_valid_connections++] = i;
    }
  }
}

static void prv_create_default_settings() {
#ifdef CREATE_DUMMY_SETTINGS
  settings.default_products.allow_all = true;
  settings.default_products.ice = true;
  settings.default_products.ic = true;
  settings.default_products.ir = true;
  settings.default_products.zug = true;
  settings.default_products.sbahn = true;
  settings.default_products.bus = true;
  settings.default_products.schiff = true;
  settings.default_products.ubahn = true;
  settings.default_products.tram = true;
  settings.default_products.ast = true;
  
  int i = -1;

  i++;
  strcpy(settings.stations[i].ibnr, "8003368");
  strcpy(settings.stations[i].title, "Köln Messe/Deutz");
  settings.stations[i].is_enabled = true;
  settings.stations[i].use_default_products = true;

  i++;
  strcpy(settings.stations[i].ibnr, "8000266");
  strcpy(settings.stations[i].title, "Wpt. Hbf");
  settings.stations[i].is_enabled = true;
  settings.stations[i].use_default_products = true;

  i++;
  strcpy(settings.stations[i].ibnr, "8006719");
  strcpy(settings.stations[i].title, "Wpt. Oberbarmen");
  settings.stations[i].is_enabled = true;
  settings.stations[i].use_default_products = true;

  i++;
  strcpy(settings.stations[i].ibnr, "8006620");
  strcpy(settings.stations[i].title, "Wpt. Unterbarmen");
  settings.stations[i].is_enabled = true;
  settings.stations[i].use_default_products = true;

  i++;
  strcpy(settings.stations[i].ibnr, "8000152");
  strcpy(settings.stations[i].title, "Hannover Hbf");
  settings.stations[i].is_enabled = true;
  settings.stations[i].use_default_products = true;

  i++;
  strcpy(settings.stations[i].ibnr, "8000142");
  strcpy(settings.stations[i].title, "Hagen Hbf");
  settings.stations[i].is_enabled = true;
  settings.stations[i].use_default_products = true;

  i++;
  strcpy(settings.stations[i].ibnr, "8006226");
  strcpy(settings.stations[i].title, "Wattenscheid");
  settings.stations[i].is_enabled = true;
  settings.stations[i].use_default_products = true;

  i++;
  strcpy(settings.stations[i].ibnr, "8000085");
  strcpy(settings.stations[i].title, "D'dorf Hbf");
  settings.stations[i].is_enabled = true;
  settings.stations[i].use_default_products = true;
         
  i = -1;
  i++;
  strcpy(settings.connections[i].start_ibnr, "8003368");
  strcpy(settings.connections[i].dest_ibnr, "8000266");
  strcpy(settings.connections[i].title, "Köln Messe/Deutz");
  strcpy(settings.connections[i].subtitle, "->Wpt. Hbf");
  settings.connections[i].is_enabled = true;
  settings.connections[i].use_default_products = false;
  settings.connections[i].allowed_products.allow_all = false;
  settings.connections[i].allowed_products.ice = false;
  settings.connections[i].allowed_products.ic = false;
  settings.connections[i].allowed_products.ir = false;
  settings.connections[i].allowed_products.zug = true;
  settings.connections[i].allowed_products.sbahn = true;
  settings.connections[i].allowed_products.bus = true;
  settings.connections[i].allowed_products.schiff = true;
  settings.connections[i].allowed_products.ubahn = true;
  settings.connections[i].allowed_products.tram = true;
  settings.connections[i].allowed_products.ast = true;
         
  i++;
  strcpy(settings.connections[i].start_ibnr, "8000266");
  strcpy(settings.connections[i].dest_ibnr, "8003368");
  strcpy(settings.connections[i].title, "Wpt. Hbf");
  strcpy(settings.connections[i].subtitle, "->Köln Messe/Deutz");
  settings.connections[i].is_enabled = true;
  settings.connections[i].use_default_products = false;
  settings.connections[i].allowed_products.allow_all = false;
  settings.connections[i].allowed_products.ice = false;
  settings.connections[i].allowed_products.ic = false;
  settings.connections[i].allowed_products.ir = false;
  settings.connections[i].allowed_products.zug = true;
  settings.connections[i].allowed_products.sbahn = true;
  settings.connections[i].allowed_products.bus = true;
  settings.connections[i].allowed_products.schiff = true;
  settings.connections[i].allowed_products.ubahn = true;
  settings.connections[i].allowed_products.tram = true;
  settings.connections[i].allowed_products.ast = true;
#endif  
}

// Read settings from persistent storage
void MyBahnSettings_load() {
  // Read settings from persistent storage, if they exist
  if (persist_exists(MESSAGE_KEY_DefAllProd)) {
    persist_read_data(MESSAGE_KEY_DefAllProd, &settings.default_products, sizeof(settings.default_products));
    for (int i = 0; i < NUMBER_OF_STATIONS; i++) {
      persist_read_data(MESSAGE_KEY_StatEnabled + i, &settings.stations[i], sizeof(StationConfig));
    }
    for (int i = 0; i < NUMBER_OF_CONNECTIONS; i++) {
      persist_read_data(MESSAGE_KEY_ConnEnabled + i, &settings.connections[i], sizeof(ConnectionConfig));
    }
  } else {
    prv_create_default_settings();
  }
  prv_validate_settings();
}

// Save the settings to persistent storage
void MyBahnSettings_save() {
  persist_write_data(MESSAGE_KEY_DefAllProd, &settings.default_products, sizeof(settings.default_products));
  for (int i = 0; i < NUMBER_OF_STATIONS; i++) {
    persist_write_data(MESSAGE_KEY_StatEnabled + i, &settings.stations[i], sizeof(StationConfig));
  }
  for (int i = 0; i < NUMBER_OF_CONNECTIONS; i++) {
    persist_write_data(MESSAGE_KEY_ConnEnabled + i, &settings.connections[i], sizeof(ConnectionConfig));
  }
}

static bool prv_receive_settings_string(char* dest, DictionaryIterator *iter, int msg_key, int length) {
    Tuple *config_tuple = dict_find(iter, msg_key);
    if (config_tuple) {
      strncpy(dest, config_tuple->value->cstring, length);
      dest[length] = '\0';
      return true;
    }
    return false;
}

#define RECEIVE_BOOL(received_flag, iter, msg_key, target) {\
    Tuple* t = dict_find(iter, msg_key); \
    if (t) { \
      received_flag = true; \
      target = t->value->int32 == 1; \
    } \
}

bool MyBahnSettings_handle_app_message(DictionaryIterator *iter, void *context) {
  bool has_received_settings = false;
  
  // Standard Verkehrsmittel
  RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_DefAllProd, settings.default_products.allow_all);
  RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_DefICE, settings.default_products.ice);
  RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_DefIC, settings.default_products.ic);
  RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_DefIR, settings.default_products.ir);
  RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_DefRE, settings.default_products.zug);
  RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_DefS, settings.default_products.sbahn);
  RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_DefS, settings.default_products.sbahn);
  RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_DefBus, settings.default_products.bus);
  RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_DefSchiff, settings.default_products.schiff);
  RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_DefU, settings.default_products.ubahn);
  RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_DefTram, settings.default_products.tram);
  RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_DefAST, settings.default_products.ast);
  
  // Station-Settings
  for (int i = 0; i < NUMBER_OF_STATIONS; i++) {
    has_received_settings |= prv_receive_settings_string(settings.stations[i].title, iter, MESSAGE_KEY_StatTitle + i, STATION_TITLE_LENGTH);
    has_received_settings |= prv_receive_settings_string(settings.stations[i].subtitle, iter, MESSAGE_KEY_StatSubTitle + i, STATION_SUBTITLE_LENGTH);
    has_received_settings |= prv_receive_settings_string(settings.stations[i].ibnr, iter, MESSAGE_KEY_StatIBNR + i, IBNR_LENGTH);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatEnabled + i, settings.stations[i].is_enabled);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatUseDefProd + i, settings.stations[i].use_default_products);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatAllProd + i, settings.stations[i].allowed_products.allow_all);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatICE + i, settings.stations[i].allowed_products.ice);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatIC + i, settings.stations[i].allowed_products.ic);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatIR + i, settings.stations[i].allowed_products.ir);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatRE + i, settings.stations[i].allowed_products.zug);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatS + i, settings.stations[i].allowed_products.sbahn);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatS + i, settings.stations[i].allowed_products.sbahn);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatBus + i, settings.stations[i].allowed_products.bus);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatSchiff + i, settings.stations[i].allowed_products.schiff);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatU + i, settings.stations[i].allowed_products.ubahn);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatTram + i, settings.stations[i].allowed_products.tram);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_StatAST + i, settings.stations[i].allowed_products.ast);
  }

  // Connection-Settings
  for (int i = 0; i < NUMBER_OF_CONNECTIONS; i++) {
    has_received_settings |= prv_receive_settings_string(settings.connections[i].title, iter, MESSAGE_KEY_ConnTitle + i, CONNECTION_TITLE_LENGTH);
    has_received_settings |= prv_receive_settings_string(settings.connections[i].subtitle, iter, MESSAGE_KEY_ConnSubTitle + i, CONNECTION_SUBTITLE_LENGTH);
    has_received_settings |= prv_receive_settings_string(settings.connections[i].start_ibnr, iter, MESSAGE_KEY_ConnStartIBNR + i, IBNR_LENGTH);
    has_received_settings |= prv_receive_settings_string(settings.connections[i].dest_ibnr, iter, MESSAGE_KEY_ConnDestIBNR + i, IBNR_LENGTH);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnEnabled + i, settings.connections[i].is_enabled);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnUseDefProd + i, settings.connections[i].use_default_products);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnAllProd + i, settings.connections[i].allowed_products.allow_all);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnICE + i, settings.connections[i].allowed_products.ice);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnIC + i, settings.connections[i].allowed_products.ic);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnIR + i, settings.connections[i].allowed_products.ir);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnRE + i, settings.connections[i].allowed_products.zug);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnS + i, settings.connections[i].allowed_products.sbahn);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnS + i, settings.connections[i].allowed_products.sbahn);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnBus + i, settings.connections[i].allowed_products.bus);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnSchiff + i, settings.connections[i].allowed_products.schiff);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnU + i, settings.connections[i].allowed_products.ubahn);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnTram + i, settings.connections[i].allowed_products.tram);
    RECEIVE_BOOL(has_received_settings, iter, MESSAGE_KEY_ConnAST + i, settings.connections[i].allowed_products.ast);
  }
  
  if (has_received_settings) {
    prv_validate_settings();
    MyBahnSettings_save();
  }
  return has_received_settings;
}

void MyBahnSettings_init() {
  MyBahnSettings_load();
}

ConnectionConfig* MyBahnSettings_get_valid_connection(int valid_connection_index) {
  if (valid_connection_index >= settings.number_of_valid_connections) {
    return NULL;
  }
  return &settings.connections[settings.valid_connections[valid_connection_index]];
}

StationConfig* MyBahnSettings_get_valid_station(int valid_station_index) {
  if (valid_station_index >= settings.number_of_valid_stations) {
    return NULL;
  }
  return &settings.stations[settings.valid_stations[valid_station_index]];
}

AllowedProducts* MyBahnSettings_get_default_products() {
  return &settings.default_products;
}

int MyBahnSettings_get_number_of_valid_connections() {
  return settings.number_of_valid_connections;
}

int MyBahnSettings_get_number_of_valid_stations() {
  return settings.number_of_valid_stations;
}

AllowedProducts* MyBahnSettings_get_allowed_products_for_connection(int connection_index) {
  ConnectionConfig* conn = MyBahnSettings_get_valid_connection(connection_index);
  if (&conn->allowed_products == NULL || conn->use_default_products) {
    return &settings.default_products;
  }
  return &conn->allowed_products;
}

AllowedProducts* MyBahnSettings_get_allowed_products_for_station(int station_index) {
  StationConfig* s = MyBahnSettings_get_valid_station(station_index);
  if (&s->allowed_products == NULL || s->use_default_products) {
    return &settings.default_products;
  }
  return &s->allowed_products;
  
}