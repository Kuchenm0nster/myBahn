#include <pebble.h>
#include "mybahn_settings.h"
#include "bahn_interface.h"
#include "main_menu.h"

static Window *s_main_window;

static void main_window_load(Window *window) {
  // create main menu layer
  MainMenu_create(window);
}

static void main_window_unload(Window *window) {
  // Destroy main menu layer
  MainMenu_destroy();
}

static void inbox_received_callback(DictionaryIterator *iter, void *context) {
  if (BahnInterface_handle_app_message(iter, context)) {
    return;
  }
  if (MyBahnSettings_handle_app_message(iter, context)) {
    MainMenu_refresh();
    return;
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
}

static void init() {

  const int inbox_size = 2048;
  const int outbox_size = 512;
  
  MyBahnSettings_init();
  BahnInterface_init();
  
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  app_message_open(inbox_size, outbox_size);
  
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
