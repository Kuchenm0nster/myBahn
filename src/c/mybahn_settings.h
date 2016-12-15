#pragma once
#include <pebble.h>

#define SETTINGS_KEY 1
#define IBNR_LENGTH 8
#define STATION_TITLE_LENGTH 20
#define STATION_SUBTITLE_LENGTH 25
#define CONNECTION_TITLE_LENGTH 20
#define CONNECTION_SUBTITLE_LENGTH 25
#define NUMBER_OF_STATIONS 10
#define NUMBER_OF_CONNECTIONS 10

typedef struct {
  bool allow_all;
  bool ice;
  bool ic;
  bool ir;
  bool zug;
  bool sbahn;
  bool bus;
  bool schiff;
  bool ubahn;
  bool tram;
  bool ast;
} AllowedProducts;

typedef struct {
  char ibnr[IBNR_LENGTH+1];
  char title[STATION_TITLE_LENGTH+1];
  char subtitle[STATION_SUBTITLE_LENGTH+1];
  bool use_default_products;
  AllowedProducts allowed_products;
  bool is_enabled;
  bool is_valid;
} StationConfig;

typedef struct {
  char title[CONNECTION_TITLE_LENGTH+1];
  char subtitle[CONNECTION_SUBTITLE_LENGTH+1];
  char start_ibnr[IBNR_LENGTH+1];
  char dest_ibnr[IBNR_LENGTH+1];
  bool use_default_products;
  AllowedProducts allowed_products;
  bool is_enabled;
  bool is_valid;
} ConnectionConfig;

// A structure containing our settings
typedef struct {
  AllowedProducts default_products;  
  StationConfig stations[NUMBER_OF_STATIONS];
  ConnectionConfig connections[NUMBER_OF_CONNECTIONS];
  int number_of_valid_stations;
  int number_of_valid_connections;
  int valid_stations[NUMBER_OF_STATIONS];
  int valid_connections[NUMBER_OF_STATIONS];
} MyBahnSettings;

bool MyBahnSettings_handle_app_message(DictionaryIterator *iter, void *context);
void MyBahnSettings_init();
void MyBahnSettings_load();
void MyBahnSettings_save();
AllowedProducts* MyBahnSettings_get_default_products();
ConnectionConfig* MyBahnSettings_get_valid_connection(int valid_connection_index);
StationConfig* MyBahnSettings_get_valid_station(int valid_station_index);
int MyBahnSettings_get_number_of_valid_connections();
int MyBahnSettings_get_number_of_valid_stations();
AllowedProducts* MyBahnSettings_get_allowed_products_for_connection(int connection_index);
AllowedProducts* MyBahnSettings_get_allowed_products_for_station(int station_index);