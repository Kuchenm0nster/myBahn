#include <pebble.h>
#include "mybahn_settings.h"
#include "resources.h"
#include "connection_results_menu.h"
#include "to_stations_menu.h"

static MenuLayer* s_menu_layer;
static Window* s_window;
static char s_header[STATION_TITLE_LENGTH + 10];

static int from_station_index;

static void select_to_station(int to_station_index) {
  ConnectionResultsMenu_show_station_to_station(from_station_index, to_station_index);
}

static int compute_to_station_index(int station_index) {
  if (station_index < from_station_index) {
    return station_index;
  }
  return station_index + 1;
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return 1;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MyBahnSettings_get_number_of_valid_stations() - 1;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  int to_station_index = compute_to_station_index(cell_index->row);
  StationConfig* station = MyBahnSettings_get_valid_station(to_station_index);
  menu_cell_basic_draw(ctx, cell_layer, station->title, station->subtitle, NULL);
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  menu_cell_basic_header_draw(ctx, cell_layer, s_header);
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  int to_station_index = compute_to_station_index(cell_index->row);
  select_to_station(to_station_index);
}

void ToStationsMenu_display(int _from_station_index) {
  ToStationsMenu_destroy();
  
  from_station_index = _from_station_index;
  
  StationConfig* start_station = MyBahnSettings_get_valid_station(from_station_index);
  s_header[0] = '\0';
  strcat(s_header, start_station->title);
  strcat(s_header, TO_MARKER);

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

void ToStationsMenu_destroy() {
  if (!s_window) {
    return;
  }
  menu_layer_destroy(s_menu_layer);
  window_destroy(s_window);
  s_menu_layer = NULL;
  s_window = NULL;
}
