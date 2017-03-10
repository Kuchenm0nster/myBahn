#include <pebble.h>
#include "resources.h"
#include "message_window.h"
#include "loading_window.h"

static Window *s_main_window;
static void (*s_back_callback)();
static bool initialized = false;

static void back_single_click_handler(ClickRecognizerRef recognizer, void *context) {
  window_stack_remove(s_main_window, true);
  if (s_back_callback) {
    s_back_callback();
  }
}

static void config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_BACK, back_single_click_handler);
}

static void window_load(Window *window) {
  if (!initialized) {
    GRect bounds = computeEffectiveWindowBounds(window, NULL);
    MessageWindow_build(s_main_window, bounds, LOADING_MESSAGE, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
    window_set_click_config_provider(s_main_window, config_provider);
    initialized = true;
  }
}

static void window_unload(Window *window) {
  s_back_callback = NULL;
}

void LoadingWindow_show(void (*back_callback)()) {
  if(!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload
    });
  }
  s_back_callback = back_callback;
  window_stack_push(s_main_window, true);
}

void LoadingWindow_hide() {
  window_stack_remove(s_main_window, true);
}