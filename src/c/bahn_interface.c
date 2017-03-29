#include <pebble.h>
#include "bahn_interface.h"
#include "loading_window.h"
#include "load_failed_window.h"

static int request_id = 0;

static ConnectionResults connectionResultBuffer;
static char detailsResultBuffer[CONNECTION_DETAILS_LENGTH + 1];
static void (*requestCompleteCallback)();

static void request_cancelled() {
  request_id++;
}

static void display_wait_screen() {
  LoadingWindow_show(request_cancelled);
}

static void hide_wait_screen() {
  LoadingWindow_hide();
}

static DictionaryIterator* prepare_request(int request_type) {
  display_wait_screen();
  DictionaryIterator* out_iter;
  AppMessageResult result = app_message_outbox_begin(&out_iter);
  if (result != APP_MSG_OK) {
    hide_wait_screen();
    LoadFailedWindow_show();
    return NULL;
  }
  dict_write_int(out_iter, MESSAGE_KEY_BahnReqId, &request_id, sizeof(int), true);
  dict_write_int(out_iter, MESSAGE_KEY_BahnReqType, &request_type, sizeof(int), true);
  return out_iter;
}

static void send_request(DictionaryIterator* out_iter) {
  dict_write_end(out_iter);
  app_message_outbox_send();
}

static void send_request_for_url(int request_type, char* request_url) {
  request_id++;
  DictionaryIterator* out_iter = prepare_request(request_type);
  if (out_iter == NULL) {
    return;
  }
  dict_write_int(out_iter, MESSAGE_KEY_BahnMaxLength, &request_type, sizeof(int), true);
  dict_write_tuplet(out_iter, &TupletCString(MESSAGE_KEY_BahnReqUrl, request_url));
  send_request(out_iter);
}

static void send_request_for_connections(char* start_station, char* dest_station, int products) {
  request_id++;
  DictionaryIterator* out_iter = prepare_request(MESSAGE_KEY_BahnReqTypeBuildConnResults);
  if (out_iter == NULL) {
    return;
  }
  dict_write_tuplet(out_iter, &TupletCString(MESSAGE_KEY_BahnReqStartStation, start_station));
  dict_write_tuplet(out_iter, &TupletCString(MESSAGE_KEY_BahnReqDestStation, dest_station));
  dict_write_int(out_iter, MESSAGE_KEY_BahnReqProducts, &products, sizeof(int), true);
  send_request(out_iter);
}

static void connection_result_received(DictionaryIterator *iter) {
  Tuple* t = dict_find(iter, MESSAGE_KEY_BahnRespPartNum);
  if (!t) {
    return;
  }
  
  int partNum = t->value->int32;
  if (partNum == 1) {
    connectionResultBuffer.number_of_entries = 0;
    connectionResultBuffer.has_frueher = false;
    connectionResultBuffer.has_spaeter = false;
  }
  
  t = dict_find(iter, MESSAGE_KEY_BahnFrueherUrl);
  if (t) {
    connectionResultBuffer.has_frueher = true;
    strncpy(connectionResultBuffer.frueher_link, t->value->cstring, CONNECTION_RESULT_DETAIL_LINK_LENGTH);
    connectionResultBuffer.frueher_link[CONNECTION_RESULT_DETAIL_LINK_LENGTH] = '\0';
    return;
  }
  
  t = dict_find(iter, MESSAGE_KEY_BahnSpaeterUrl);
  if (t) {
    connectionResultBuffer.has_spaeter = true;
    strncpy(connectionResultBuffer.spaeter_link, t->value->cstring, CONNECTION_RESULT_DETAIL_LINK_LENGTH);
    connectionResultBuffer.spaeter_link[CONNECTION_RESULT_DETAIL_LINK_LENGTH] = '\0';
    return;
  }
  
  int result_index = connectionResultBuffer.number_of_entries++;
  ConnectionResult* resultEntry = &connectionResultBuffer.entries[result_index];
  resultEntry->title[0] = '\0';
  resultEntry->subtitle[0] = '\0';
  resultEntry->detail_link[0] = '\0';
  
  t = dict_find(iter, MESSAGE_KEY_BahnConnTitle);
  if (t) {
    strncpy(resultEntry->title, t->value->cstring, CONNECTION_RESULT_TITLE_LENGTH);
    resultEntry->title[CONNECTION_RESULT_TITLE_LENGTH] = '\0';
  }
  
  t = dict_find(iter, MESSAGE_KEY_BahnConnSubtitle);
  if (t) {
    strncpy(resultEntry->subtitle, t->value->cstring, CONNECTION_RESULT_SUBTITLE_LENGTH);
    resultEntry->subtitle[CONNECTION_RESULT_SUBTITLE_LENGTH] = '\0';
  }
  
  t = dict_find(iter, MESSAGE_KEY_BahnConnDetailsUrl);
  if (t) {
    strncpy(resultEntry->detail_link, t->value->cstring, CONNECTION_RESULT_DETAIL_LINK_LENGTH);
    resultEntry->detail_link[CONNECTION_RESULT_DETAIL_LINK_LENGTH] = '\0';
  }
}

static void connection_details_received(DictionaryIterator *iter) {
  strncpy(detailsResultBuffer, dict_find(iter, MESSAGE_KEY_BahnConnDetails)->value->cstring, CONNECTION_DETAILS_LENGTH);
  detailsResultBuffer[CONNECTION_DETAILS_LENGTH] = '\0';
}

bool BahnInterface_handle_app_message(DictionaryIterator *iter, void *context) {
  Tuple *tuple = dict_find(iter, MESSAGE_KEY_BahnReqId);
  if (!tuple) {
    return false;
  }
    
  // This value was stored as JS Number, which is stored here as int32_t
  int32_t msg_request_id = tuple->value->int32;
  if (msg_request_id != request_id) {
    return true;
  }
  
  int partNum = dict_find(iter, MESSAGE_KEY_BahnRespPartNum)->value->int32;
  int parts = dict_find(iter, MESSAGE_KEY_BahnRespParts)->value->int32;
  
  unsigned int request_type = dict_find(iter, MESSAGE_KEY_BahnReqType)->value->int32;
  
  if (request_type == MESSAGE_KEY_BahnReqTypeGetConnResult) {
    connection_result_received(iter);
  } else if (request_type == MESSAGE_KEY_BahnReqTypeGetConnDetails) {
    connection_details_received(iter);
  } else if (request_type == MESSAGE_KEY_BahnRespTypeLoadFailed) {
    hide_wait_screen();
    LoadFailedWindow_show();
  }
  
  if (partNum == parts) {
    hide_wait_screen();
    if (requestCompleteCallback) {
      requestCompleteCallback();
    }
  }
  return true;
}

static int prv_get_product_code(AllowedProducts* p) {
  if (p->allow_all) {
    return 1023;
  }
  int result = 0;
  if (p->ice) {
    result +=1;
  }
  if (p->ic) {
    result +=2;
  }
  if (p->ir) {
    result +=4;
  }
  if (p->zug) {
    result +=8;
  }
  if (p->sbahn) {
    result +=16;
  }
  if (p->bus) {
    result +=32;
  }
  if (p->schiff) {
    result +=64;
  }
  if (p->ubahn) {
    result +=128;
  }
  if (p->tram) {
    result +=256;
  }
  if (p->ast) {
    result +=512;
  }
  return result;
}


int get_products_for_connection(int valid_connection_index) {
  return prv_get_product_code(MyBahnSettings_get_allowed_products_for_connection(valid_connection_index));
}

int get_products_for_station_to_station(int from_station_index, int to_station_index) {
  StationConfig* s1 = MyBahnSettings_get_valid_station(from_station_index);
  StationConfig* s2 = MyBahnSettings_get_valid_station(to_station_index);
  AllowedProducts* p1 = s1->use_default_products ? MyBahnSettings_get_default_products() : &s1->allowed_products;
  AllowedProducts* p2 = s2->use_default_products ? MyBahnSettings_get_default_products() : &s2->allowed_products;
  return  prv_get_product_code(p1) | prv_get_product_code(p2);
}

void BahnInterface_init() {
  request_id = 0;
}

void BahnInterface_read_connection_results(int connection_index, void (*completeCallback)()) {
  ConnectionConfig* conn = MyBahnSettings_get_valid_connection(connection_index);
  requestCompleteCallback = completeCallback;
  send_request_for_connections(conn->start_ibnr, conn->dest_ibnr, get_products_for_connection(connection_index));
}

void BahnInterface_read_station_to_station_results(int from_station_index, int to_station_index, void (*completeCallback)()) {
  StationConfig* start = MyBahnSettings_get_valid_station(from_station_index);
  StationConfig* dest = MyBahnSettings_get_valid_station(to_station_index);
  requestCompleteCallback = completeCallback;
  send_request_for_connections(start->ibnr, dest->ibnr, get_products_for_station_to_station(from_station_index, to_station_index));
}

void BahnInterface_read_query_link_results(char* query_link, void (*completeCallback)()) {
  requestCompleteCallback = completeCallback;
  send_request_for_url(MESSAGE_KEY_BahnReqTypeBuildConnResults, query_link);
}

void BahnInterface_read_connection_details(ConnectionResult* result, void (*completeCallback)()) {
  requestCompleteCallback = completeCallback;
  send_request_for_url(MESSAGE_KEY_BahnReqTypeGetConnDetails, result->detail_link);
}
                                
ConnectionResults* BahnInterface_get_current_connection_results() {
  return &connectionResultBuffer;
}
                                
char* BahnInterface_get_current_connection_details() {
  return detailsResultBuffer;
}
