#include <pebble.h>
#include "resources.h"
#include "message_window.h"
#include "load_failed_window.h"

static Window *s_main_window;
static bool initialized = false;

static void window_load(Window *window) {
    if (!initialized) {
      GRect bounds = computeEffectiveWindowBounds(window, NULL);
      MessageWindow_build(window, bounds, LOAD_FAILED_MESSAGE, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
      initialized = true;
    }
}

void LoadFailedWindow_show() {
  if(!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load
    });
  }
  window_stack_push(s_main_window, true);
}