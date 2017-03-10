#include <pebble.h>
#include "resources.h"
#include "message_window.h"

#define DIALOG_MESSAGE_WINDOW_MARGIN 10

static void icon_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GRect bitmap_bounds = gbitmap_get_bounds(APP_ICON);
  graphics_context_set_compositing_mode(ctx, GCompOpSet);
  graphics_draw_bitmap_in_rect(ctx, APP_ICON, (GRect){.origin = bounds.origin, .size = bitmap_bounds.size});
}

void MessageWindow_build(Window *window, GRect bounds, char* message, GFont message_font) {
  TextLayer *s_label_layer;
  Layer *s_icon_layer;
  TextLayer *s_message_layer;

  Layer *window_layer = window_get_root_layer(window);
#if defined(PBL_ROUND)
    int original_height = bounds.size.h;
    int original_width = bounds.size.w;
    
    bounds.size.w -= 20;
    bounds.size.h = original_height * 2 / 3;
    
    bounds.origin.x = (original_width - bounds.size.w) / 2;
    bounds.origin.y = (original_height - bounds.size.h) / 2;
#endif
  bounds.origin.x += DIALOG_MESSAGE_WINDOW_MARGIN;
  bounds.origin.y += DIALOG_MESSAGE_WINDOW_MARGIN;
  bounds.size.w -= DIALOG_MESSAGE_WINDOW_MARGIN * 2;
  bounds.size.h -= DIALOG_MESSAGE_WINDOW_MARGIN * 2;

  GRect bitmap_bounds = gbitmap_get_bounds(APP_ICON);

  s_icon_layer = layer_create(GRect(bounds.origin.x, bounds.origin.y, bitmap_bounds.size.w, bitmap_bounds.size.h));
  layer_set_update_proc(s_icon_layer, icon_update_proc);
  layer_add_child(window_layer, s_icon_layer);

  s_label_layer = text_layer_create(GRect(bounds.origin.x + bitmap_bounds.size.w + 5, bounds.origin.y, bounds.size.w - bitmap_bounds.size.w - 5, bounds.size.h));
  text_layer_set_text(s_label_layer, APP_NAME);
  text_layer_set_background_color(s_label_layer, GColorClear);
  text_layer_set_text_alignment(s_label_layer, GTextAlignmentLeft);
  text_layer_set_font(s_label_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(s_label_layer));

  s_message_layer = text_layer_create(GRect(bounds.origin.x, bounds.origin.y + bitmap_bounds.size.h, bounds.size.w, bounds.size.h));
  text_layer_set_text(s_message_layer, message);
  text_layer_set_background_color(s_message_layer, GColorClear);
  text_layer_set_text_alignment(s_message_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
  text_layer_set_font(s_message_layer, message_font);
  layer_add_child(window_layer, text_layer_get_layer(s_message_layer));
}
