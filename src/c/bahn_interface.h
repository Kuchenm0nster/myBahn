#pragma once
#include "mybahn_settings.h"

#define CONNECTION_RESULT_TITLE_LENGTH 20
#define CONNECTION_RESULT_SUBTITLE_LENGTH 25
#define CONNECTION_RESULT_DETAIL_LINK_LENGTH 172
#define CONNECTION_DETAILS_LENGTH 1024
#define MAX_CONNECTION_RESULTS 20

typedef struct {
  char title[CONNECTION_RESULT_TITLE_LENGTH + 1];
  char subtitle[CONNECTION_RESULT_SUBTITLE_LENGTH + 1];
  char detail_link[CONNECTION_RESULT_DETAIL_LINK_LENGTH + 1];
} ConnectionResult;

typedef struct {
  ConnectionResult entries[MAX_CONNECTION_RESULTS];
  int number_of_entries;
  bool has_frueher;
  bool has_spaeter;
  char frueher_link[CONNECTION_RESULT_DETAIL_LINK_LENGTH + 1];
  char spaeter_link[CONNECTION_RESULT_DETAIL_LINK_LENGTH + 1];
} ConnectionResults;

void BahnInterface_init();
void BahnInterface_read_connection_results(int connection_index, void (*completeCallback)());
void BahnInterface_read_station_to_station_results(int from_station_index, int to_station_index, void (*completeCallback)());
void BahnInterface_read_query_link_results(char* query_link, void (*completeCallback)());
void BahnInterface_read_connection_details(ConnectionResult* result, void (*completeCallback)());
bool BahnInterface_handle_app_message(DictionaryIterator *iter, void *context);
ConnectionResults* BahnInterface_get_current_connection_results();
char* BahnInterface_get_current_connection_details();
