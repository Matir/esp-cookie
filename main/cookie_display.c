#include "cookie.h"
#include "esp_log.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_gc9a01.h"
#include "esp_lcd_touch_cst816s.h"
#include "driver/spi_master.h"
#include "driver/i2c.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"

// Tag for logging
static const char *TAG = "cookie_display";

// Display configuration (using Kconfig values)
#define LCD_H_RES CONFIG_LCD_H_RES
#define LCD_V_RES CONFIG_LCD_V_RES
#define LCD_BUF_SIZE (LCD_H_RES * LCD_V_RES * sizeof(uint16_t))

// SPI configuration for display (using Kconfig values)
#define LCD_SPI_HOST CONFIG_DISP_SPI_HOST
#ifdef CONFIG_DISP_SPI_TYPE_4WIRE
#  define LCD_SPI_MISO CONFIG_DISP_SPI_MISO_GPIO
#else
#  define LCD_SPI_MISO GPIO_NUM_NC
#endif
#define LCD_SPI_MOSI CONFIG_DISP_SPI_MOSI_GPIO
#define LCD_SPI_SCLK CONFIG_DISP_SPI_SCLK_GPIO
#define LCD_SPI_CS CONFIG_DISP_SPI_CS_GPIO
#define LCD_SPI_DC CONFIG_DISP_SPI_DC_GPIO
#define LCD_SPI_RST CONFIG_DISP_RST_GPIO
#define LCD_SPI_BL CONFIG_DISP_BL_GPIO
#define LCD_SPI_FREQ (CONFIG_DISP_SPI_FREQ * 1000 * 1000) // Convert MHz to Hz

// I2C configuration for touchscreen (using Kconfig values)
#define I2C_MASTER_SCL_IO CONFIG_TP_SCL_GPIO
#define I2C_MASTER_SDA_IO CONFIG_TP_SDA_GPIO
#define I2C_MASTER_FREQ_HZ (CONFIG_I2C_MASTER_FREQ * 1000) // Convert kHz to Hz
#define I2C_MASTER_PORT_NUM 0
#define I2C_MASTER_TIMEOUT_MS 1000

// Touch configuration (using Kconfig values)
#define TOUCH_INT_GPIO CONFIG_TP_INT_GPIO
#define TOUCH_RST_GPIO CONFIG_TP_RST_GPIO

// Global variables
static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_touch_handle_t tp_handle = NULL;

// Initialize SPI bus for display
static esp_lcd_panel_io_handle_t init_spi_bus() {
    spi_bus_config_t buscfg = {
        .sclk_io_num = LCD_SPI_SCLK,
        .mosi_io_num = LCD_SPI_MOSI,
        .miso_io_num = LCD_SPI_MISO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = LCD_BUF_SIZE,
    };
    
    ESP_ERROR_CHECK(spi_bus_initialize(LCD_SPI_HOST, &buscfg, SPI_DMA_CH_AUTO));
    
    esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = LCD_SPI_DC,
        .cs_gpio_num = LCD_SPI_CS,
        .pclk_hz = LCD_SPI_FREQ, // Use configured frequency
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
        .on_color_trans_done = NULL,
        .user_ctx = NULL,
    };
    
    esp_lcd_panel_io_handle_t io_handle = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)LCD_SPI_HOST, &io_config, &io_handle));
    
    return io_handle;
}

// Initialize display panel
static void init_display_panel(esp_lcd_panel_io_handle_t io_handle) {
    esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = LCD_SPI_RST,
        .color_space = ESP_LCD_COLOR_SPACE_RGB,
        .bits_per_pixel = 16,
    };
    
    ESP_ERROR_CHECK(esp_lcd_new_panel_gc9a01(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));
    
    // Initialize backlight
    gpio_config_t bk_gpio_config = {
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << LCD_SPI_BL)
    };
    ESP_ERROR_CHECK(gpio_config(&bk_gpio_config));
    ESP_ERROR_CHECK(gpio_set_level(LCD_SPI_BL, 1));
}

// Initialize I2C bus for touchscreen
static void init_i2c_bus() {
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    
    ESP_ERROR_CHECK(i2c_param_config(I2C_MASTER_PORT_NUM, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_MASTER_PORT_NUM, conf.mode, 0, 0, 0));
}

// Initialize touchscreen
static void init_touchscreen() {
    esp_lcd_touch_config_t tp_cfg = {
        .x_max = LCD_H_RES,
        .y_max = LCD_V_RES,
        .rst_gpio_num = TOUCH_RST_GPIO,
        .int_gpio_num = TOUCH_INT_GPIO,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
    };
    
    ESP_ERROR_CHECK(esp_lcd_touch_new_i2c_cst816s(I2C_MASTER_PORT_NUM, &tp_cfg, &tp_handle));
}

// Initialize LVGL
static void init_lvgl(esp_lcd_panel_io_handle_t io_handle) {
    // Initialize LVGL using ESP LVGL port (works with both LVGL 8 and 9)
    const lvgl_port_cfg_t lvgl_cfg = {
        .task_priority = 4,
        .task_stack = 4096,
        .task_affinity = -1,
        .task_max_sleep_ms = 500,
        .timer_period_ms = 5
    };
    ESP_ERROR_CHECK(lvgl_port_init(&lvgl_cfg));

    // Configure display using ESP LVGL port
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = LCD_H_RES * 50, // Use smaller buffer for better performance
        .double_buffer = true,
        .hres = LCD_H_RES,
        .vres = LCD_V_RES,
        .monochrome = false,
#if LVGL_VERSION_MAJOR >= 9
        .color_format = LV_COLOR_FORMAT_RGB565,
#endif
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        },
        .flags = {
            .buff_dma = true,
#if LVGL_VERSION_MAJOR >= 9
            .swap_bytes = false,
#endif
        }
    };

    // Add display using ESP LVGL port
    lv_display_t *disp = lvgl_port_add_disp(&disp_cfg);

    // Configure touch using ESP LVGL port
    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = disp,
        .handle = tp_handle,
    };
    lv_indev_t *indev = lvgl_port_add_touch(&touch_cfg);
}

// Main display initialization function
void cookie_display_init() {
    ESP_LOGI(TAG, "Initializing display...");

    // Initialize SPI bus and display
    esp_lcd_panel_io_handle_t io_handle = init_spi_bus();
    init_display_panel(io_handle);

    // Initialize I2C bus and touchscreen
    ESP_LOGI(TAG, "Initializing touchscreen...");
    init_i2c_bus();
    init_touchscreen();

    // Initialize LVGL
    ESP_LOGI(TAG, "Initializing LVGL...");
    init_lvgl(io_handle);

    ESP_LOGI(TAG, "Display and touchscreen initialized successfully");
}

// Update display
void cookie_display_update() {
    // This will be called by LVGL automatically
    // You can add custom update logic here if needed
}

// Get display handle
esp_lcd_panel_handle_t cookie_display_get_panel() {
    return panel_handle;
}

// Get touch handle
esp_lcd_touch_handle_t cookie_display_get_touch() {
    return tp_handle;
}
