#include <pebble.h>
#include "resources.h"

static GBitmap *s_app_icon_bitmap;

GBitmap* getAppIcon() {
  if (!s_app_icon_bitmap) {
    s_app_icon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ICON_MAIN);
  }
  return s_app_icon_bitmap;
}
