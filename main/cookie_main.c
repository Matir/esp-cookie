#include "cookie.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Tag for logging
static const char *TAG = "cookie_main";

// Main application entry point
void app_main() {
    ESP_LOGI(TAG, "Starting Little Troubles Cookie Counter...");
    
    // Initialize display and touchscreen
    cookie_display_init();
    
    // Main application loop
    while (1) {
        // Add your game logic here
        // For example:
        // - Update cookie counter
        // - Handle fight tracking
        // - Process touch events
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}