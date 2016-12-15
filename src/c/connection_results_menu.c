#include <pebble.h>
#include "resources.h"
#include "mybahn_settings.h"
#include "bahn_interface.h"
#include "connection_details_window.h"
#include "connection_results_menu.h"

static MenuLayer* s_menu_layer;
static Window* s_window;
static char s_header[2 * CONNECTION_SUBTITLE_LENGTH + 10];

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  ConnectionResults* results = BahnInterface_get_current_connection_results();
  int result = results->number_of_entries;
  if (results->has_frueher) {
    result ++;
  }
  if (results->has_spaeter) {
    result ++;
  }
  return result;
}

static bool is_frueher_entry(int menu_index) {
  ConnectionResults* results = BahnInterface_get_current_connection_results();
  if (menu_index == 0 && results->has_frueher) {
    return true;
  }
  return false;
}

static bool is_spaeter_entry(int menu_index) {
  ConnectionResults* results = BahnInterface_get_current_connection_results();
  int result_index = menu_index;
  if (results->has_frueher) {
    result_index--;
  }
  if (result_index >= results->number_of_entries && results->has_spaeter) {
    return true;
  }
  return false;
}

static int compute_result_index(int menu_index) {
  ConnectionResults* results = BahnInterface_get_current_connection_results();
  if (results->has_frueher) {
    return menu_index-1;
  }
  return menu_index;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  ConnectionResults* results = BahnInterface_get_current_connection_results();
  if (is_frueher_entry(cell_index->row)) {
    menu_cell_basic_draw(ctx, cell_layer, FRUEHER, NULL, NULL);
  } else if (is_spaeter_entry(cell_index->row)) {
    menu_cell_basic_draw(ctx, cell_layer, SPAETER, NULL, NULL);
  } else {
    int result_index = compute_result_index(cell_index->row);
    menu_cell_basic_draw(ctx, cell_layer, results->entries[result_index].title, results->entries[result_index].subtitle, NULL);
  }
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, s_header);
}

static void reload_menu() {
  menu_layer_reload_data(s_menu_layer);
}

static void update_connections(char* connection_query_link) {
  BahnInterface_read_query_link_results(connection_query_link, reload_menu);
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  ConnectionResults* results = BahnInterface_get_current_connection_results();
  if (is_frueher_entry(cell_index->row)) {
    update_connections(results->frueher_link);
  } else if (is_spaeter_entry(cell_index->row)) {
    update_connections(results->spaeter_link);
  } else {
    ConnectionDetailsWindow_show(&results->entries[compute_result_index(cell_index->row)]);
  }
}

static void build_connection_results_window() {
  ConnectionResults* results = BahnInterface_get_current_connection_results();
  if (results->number_of_entries == 0) {
    return;
  }
  
  s_window = window_create();

  Layer *window_layer = window_get_root_layer(s_window);
  GRect bounds = layer_get_frame(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(s_menu_layer, s_window);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){ 
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  }); 

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
  
  window_stack_push(s_window, true);
}

void ConnectionResultsMenu_show_for_connection(int connection_index) {
  ConnectionResultsMenu_destroy();
  
  ConnectionConfig* connection = MyBahnSettings_get_valid_connection(connection_index);
  
  s_header[0] = '\0';
  strcat(s_header, connection->title);
  strcat(s_header, " ");
  strcat(s_header, connection->subtitle);
  
  BahnInterface_read_connection_results(connection_index, build_connection_results_window);
}

void ConnectionResultsMenu_show_station_to_station(int from_station_index, int to_station_index) {
  ConnectionResultsMenu_destroy();
  
  StationConfig* from_station = MyBahnSettings_get_valid_station(from_station_index);
  StationConfig* to_station = MyBahnSettings_get_valid_station(to_station_index);
  
  s_header[0] = '\0';
  strcat(s_header, from_station->title);
  strcat(s_header, TO_MARKER);
  strcat(s_header, to_station->title);

  BahnInterface_read_station_to_station_results(from_station_index, to_station_index, build_connection_results_window);
}

void ConnectionResultsMenu_destroy() {
  if (!s_window) {
    return;
  }
  
  menu_layer_destroy(s_menu_layer);
  window_destroy(s_window);
  s_menu_layer = NULL;
  s_window = NULL;
}
