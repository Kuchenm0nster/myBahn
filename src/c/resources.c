#include <pebble.h>
#include "resources.h"

static GBitmap *s_app_icon_bitmap;

GBitmap* getAppIcon() {
  if (!s_app_icon_bitmap) {
    s_app_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ICON_MAIN);
  }
  return s_app_icon_bitmap;
}

GRect computeEffectiveWindowBounds(Window* window, StatusBarLayer* status_bar_layer) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_unobstructed_bounds(window_layer);
  if (status_bar_layer != NULL) {
    GRect status_bar_frame = layer_get_frame(status_bar_layer_get_layer(status_bar_layer));
    bounds.size.h -= status_bar_frame.size.h;
    bounds.origin.y += status_bar_frame.size.h;
  }
  return bounds;
}

StatusBarLayer* addStatusBar(Window* window) {
  //return NULL;
  StatusBarLayer* status_bar = status_bar_layer_create();

  Layer *window_layer = window_get_root_layer(window);
  layer_add_child(window_layer, status_bar_layer_get_layer(status_bar));
  return status_bar;
}
