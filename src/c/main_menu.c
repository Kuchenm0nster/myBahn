#include <pebble.h>
#include "resources.h"
#include "mybahn_settings.h"
#include "to_stations_menu.h"
#include "connection_results_menu.h"
#include "connection_details_window.h"
#include "main_menu.h"

static MenuLayer *s_menu_layer;
static StatusBarLayer *s_status_bar;

static bool is_connections_section(uint16_t section_index) {
  if (section_index > 0) {
    return false;
  }
  if (MyBahnSettings_get_number_of_valid_connections() == 0) {
    return false;
  }
  return true;
}

static bool is_stations_section(uint16_t section_index) {
  if (section_index == 1) {
    return true;
  }
  if (section_index == 0 
      && MyBahnSettings_get_number_of_valid_connections() == 0 
      && MyBahnSettings_get_number_of_valid_stations() > 0) {
    return true;
  }
  return false;
}

static void select_connection(int connection_index) {
  ConnectionResultsMenu_show_for_connection(connection_index);
}

static void select_start_station(int station_index) {
  ToStationsMenu_display(station_index);
}

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  uint16_t result = 0;
  if (MyBahnSettings_get_number_of_valid_connections() > 0) {
    result++;
  }
  if (MyBahnSettings_get_number_of_valid_stations() > 0) {
    result++;
  }
  if (result == 0) {
    result = 1;
  }
  return result;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  if (is_connections_section(section_index)) {
    return MyBahnSettings_get_number_of_valid_connections();
  } else if (is_stations_section(section_index)) {
    return MyBahnSettings_get_number_of_valid_stations();
  }
  return 1;
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
  return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  // Determine which section we're working with
  if (is_connections_section(section_index)) {
    menu_cell_basic_header_draw(ctx, cell_layer, CONNECTIONS);
  } else if (is_stations_section(section_index)) {
    menu_cell_basic_header_draw(ctx, cell_layer, START_STATIONS);
  }
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  // Determine which section we're going to draw in
  if (is_connections_section(cell_index->section)) {
    ConnectionConfig* connection = MyBahnSettings_get_valid_connection(cell_index->row);
    menu_cell_basic_draw(ctx, cell_layer, connection->title, connection->subtitle, NULL);
  } else if (is_stations_section(cell_index->section)) {
    StationConfig* station = MyBahnSettings_get_valid_station(cell_index->row);
    menu_cell_basic_draw(ctx, cell_layer, station->title, station->subtitle, NULL);
  } else {
    menu_cell_basic_draw(ctx, cell_layer, APP_NAME, PLEASE_CONFIGURE, APP_ICON);
  }
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  if (is_connections_section(cell_index->section)) {
    select_connection(cell_index->row);
  } else if (is_stations_section(cell_index->section)) {
    if (MyBahnSettings_get_number_of_valid_stations() > 1) {
      select_start_station(cell_index->row);
    }
  }
}

void MainMenu_create(Window *window) {
  MainMenu_destroy();

  Layer *window_layer = window_get_root_layer(window);
  s_status_bar = addStatusBar(window);
  GRect bounds = computeEffectiveWindowBounds(window, s_status_bar);

  s_menu_layer = menu_layer_create(bounds);

  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

void MainMenu_destroy() {
  if (!s_menu_layer) {
    return;
  }
  menu_layer_destroy(s_menu_layer);
  s_menu_layer = NULL;
  status_bar_layer_destroy(s_status_bar);
  s_status_bar = NULL;
  ToStationsMenu_destroy();
  ConnectionResultsMenu_destroy();
  ConnectionDetailsWindow_destroy();
}

void MainMenu_refresh() {
  menu_layer_reload_data(s_menu_layer);
}
