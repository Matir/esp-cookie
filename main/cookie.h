#ifndef _COOKIE_H
#define _COOKIE_H

#include "sdkconfig.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_touch.h"

// Function declarations for display and touchscreen
void cookie_display_init();
void cookie_display_update();
esp_lcd_panel_handle_t cookie_display_get_panel();
esp_lcd_touch_handle_t cookie_display_get_touch();

#endif /* _COOKIE_H */