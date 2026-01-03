# Little Troubles Cookie Counter - Agents Documentation

This document describes the agents (components, tasks, and modules) in the Little Troubles Cookie Counter project, a game tracker for the Little Troubles game running on ESP32 hardware.

## Overview

The Little Troubles Cookie Counter is a specialized device for tracking cookies and fights in the Little Troubles game. It runs on ESP32-C3 microcontroller with a 1.2" SPI display and I2C touchscreen interface.

## Hardware Agents

### 1. Main Controller Agent

**Hardware**: ESP32-C3 (ESP32-2424S012C-I)
**Responsibilities**:
- Core processing and control
- Memory management
- Peripheral coordination
- Power management
- Real-time clock operations

**Key Features**:
- Dual-core RISC-V processor
- 2.4 GHz Wi-Fi and Bluetooth 5 (LE)
- 400 KB SRAM, 384 KB ROM
- Secure boot and flash encryption

### 2. Display Agent

**Hardware**: 1.2" SPI Display (240x240 pixels)
**Driver**: GC9A01
**Component**: `espressif/esp_lcd_gc9a01` (v2.0.4)

**Responsibilities**:
- Render game interface
- Display cookie and fight counters
- Show game status information
- Handle screen updates and refresh
- Manage display power states

**Key Functions**:
- `esp_lcd_panel_init()` - Initialize display
- `esp_lcd_panel_draw_bitmap()` - Render graphics
- `esp_lcd_panel_disp_on_off()` - Control display power

### 3. Touchscreen Agent

**Hardware**: I2C Touchscreen
**Driver**: CST816D
**Component**: `espressif/esp_lcd_touch_cst816s` (v1.1.1)

**Responsibilities**:
- Detect user touch inputs
- Convert touch coordinates to screen positions
- Handle touch gestures (tap, swipe, etc.)
- Manage touch calibration
- Provide input events to the application

**Key Functions**:
- `esp_lcd_touch_init()` - Initialize touch controller
- `esp_lcd_touch_read_data()` - Get touch coordinates
- `esp_lcd_touch_get_gesture()` - Detect gestures

## Software Agents

### 4. Main Application Agent

**Location**: `main/`
**Files**:
- `cookie_main.c` - Main application entry point
- `cookie_display.c` - Display and UI logic
- `cookie.h` - Header file with project definitions
- `CMakeLists.txt` - Build configuration

**Responsibilities**:
- Initialize all hardware components
- Manage game state and counters
- Handle user interface interactions
- Coordinate between display and touch inputs
- Implement game-specific logic

### 5. Graphics Framework Agent

**Component**: `espressif/esp_lvgl_port` (v2.7.0)
**Dependencies**: LVGL (Light and Versatile Graphics Library)

**Responsibilities**:
- Provide graphical user interface framework
- Handle widget rendering (buttons, labels, etc.)
- Manage UI animations and transitions
- Optimize graphics performance for ESP32
- Provide touch input handling for UI elements

**Key Features**:
- Themes and styling support
- Anti-aliased graphics
- Hardware-accelerated rendering
- Memory-efficient design

### 6. LED Indicator Agent

**Component**: `espressif/led_strip` (v3.0.0)
**Hardware**: Addressable LED (optional)

**Responsibilities**:
- Provide visual feedback via LED
- Indicate game status (cookie count, fight status)
- Handle LED color changes
- Manage LED animations

**Supported Backends**:
- RMT (Remote Control) peripheral
- SPI peripheral

## Configuration Agents

### 7. Project Configuration Agent

**Location**: `sdkconfig`, `sdkconfig.defaults*`
**Interface**: `idf.py menuconfig`

**Responsibilities**:
- Manage build-time configuration
- Handle target-specific settings (ESP32-C3)
- Configure display parameters
- Set touchscreen calibration
- Control game-specific options

### 8. Kconfig Agent

**Location**: `main/Kconfig.projbuild`
**Interface**: `menuconfig`

**Responsibilities**:
- Define project-specific configuration options
- Set default values for game parameters
- Provide user-friendly configuration interface
- Validate configuration choices

## Build System Agents

### 9. CMake Build Agent

**Location**: `CMakeLists.txt`, `main/CMakeLists.txt`
**Dependencies**: ESP-IDF build system

**Responsibilities**:
- Configure the build process for ESP32-C3
- Manage source files and include directories
- Handle component dependencies
- Generate build artifacts
- Support Little Troubles-specific requirements

### 10. Component Manager Agent

**Location**: `idf_component.yml`, `dependencies.lock`
**Dependencies**: ESP-IDF component manager

**Responsibilities**:
- Manage external component dependencies
- Handle version constraints for display and touch drivers
- Resolve component conflicts
- Download and cache components
- Ensure reproducible builds

## Runtime Agents

### 11. FreeRTOS Agent

**Component**: ESP-IDF FreeRTOS integration
**Dependencies**: FreeRTOS kernel

**Responsibilities**:
- Provide real-time operating system functionality
- Manage tasks for game tracking
- Handle inter-task communication
- Provide memory management for game state
- Support for timers and event groups

### 12. Hardware Abstraction Agent

**Component**: ESP-IDF hardware abstraction layer
**Dependencies**: ESP32-C3-specific hardware drivers

**Responsibilities**:
- Provide unified API for hardware access
- Handle SPI communication for display
- Manage I2C communication for touchscreen
- Provide power management
- Handle system timing

## Game-Specific Agents

### 13. Cookie Counter Agent

**Location**: `main/cookie_display.c` and `main/cookie_main.c`
**Responsibilities**:
- Track cookie count in Little Troubles game
- Handle cookie increment/decrement operations
- Display current cookie count
- Manage cookie-related UI elements

### 14. Fight Tracker Agent

**Location**: `main/cookie_display.c` and `main/cookie_main.c`
**Responsibilities**:
- Track fight status and progress
- Handle fight state transitions
- Display fight information
- Manage fight-related UI elements

### 15. Game State Manager

**Location**: `main/cookie.h` and implementation files
**Responsibilities**:
- Maintain overall game state
- Coordinate between cookie and fight trackers
- Handle game initialization and reset
- Manage game configuration

## Development Agents

### 16. Testing Agent

**Location**: `pytest_cookie.py`
**Dependencies**: Python testing framework

**Responsibilities**:
- Provide automated testing for game logic
- Validate cookie counter functionality
- Test fight tracker behavior
- Perform integration testing
- Generate test reports

### 17. VSCode Integration Agent

**Location**: `.vscode/`
**Dependencies**: VSCode IDE

**Responsibilities**:
- Provide IDE configuration for development
- Manage debugging profiles
- Handle build tasks
- Provide code navigation
- Support for ESP-IDF extensions

## Agent Interaction Diagram

```
Main Application Agent
│
├── Display Agent (GC9A01)
├── Touchscreen Agent (CST816D)
├── Graphics Framework Agent (LVGL)
├── LED Indicator Agent (optional)
│
├── Cookie Counter Agent
├── Fight Tracker Agent
└── Game State Manager
│
Project Configuration Agent
│
CMake Build Agent
│
Component Manager Agent
│
FreeRTOS Agent
│
Hardware Abstraction Agent
```

## Configuration Flow

1. **Developer** configures project via `idf.py menuconfig`
2. **Project Configuration Agent** stores settings in `sdkconfig`
3. **CMake Build Agent** reads configuration and builds project
4. **Component Manager Agent** resolves and downloads display/touch dependencies
5. **Main Application Agent** initializes based on configuration
6. **Hardware Agents** (Display, Touchscreen) initialize their respective peripherals
7. **Game Agents** (Cookie Counter, Fight Tracker) initialize game state
8. **FreeRTOS Agent** manages runtime execution

## Usage Examples

### Basic Game Tracking
```c
// Initialize game state
cookie_counter_init();
fight_tracker_init();

// Update cookie count
cookie_counter_increment();
cookie_counter_decrement();

// Update fight status  
fight_tracker_start();
fight_tracker_end();

// Display current state
cookie_display_update();
```

### Touch Interaction Handling
```c
// Touch event callback
touch_event_handler(touch_event_t event) {
    if (event.type == TOUCH_TAP) {
        // Handle tap on cookie increment button
        if (is_cookie_button_pressed(event.x, event.y)) {
            cookie_counter_increment();
            cookie_display_update();
        }
    }
}
```

## Troubleshooting Agents

If issues arise, check these agents:

1. **Display Agent** - Verify SPI connections and GC9A01 initialization
2. **Touchscreen Agent** - Check I2C connections and CST816D calibration
3. **Configuration Agent** - Review `sdkconfig` for correct hardware settings
4. **Build Agent** - Ensure all components are properly included
5. **Game State Agent** - Verify game logic and counter operations

## Future Enhancements

Potential agents that could be added:
- **Network Agent** - For WiFi connectivity and cloud sync
- **Storage Agent** - For persistent game state storage
- **Audio Agent** - For sound effects and notifications
- **Battery Agent** - For power management and monitoring
- **Multiplayer Agent** - For networked game tracking

## References

- [ESP-IDF Documentation](https://docs.espressif.com/projects/esp-idf/)
- [ESP32-C3 Technical Reference](https://www.espressif.com/en/products/socs/esp32-c3)
- [GC9A01 Display Driver](https://components.espressif.com/component/espressif/esp_lcd_gc9a01)
- [CST816D Touch Controller](https://components.espressif.com/component/espressif/esp_lcd_touch_cst816s)
- [LVGL Graphics Library](https://lvgl.io/)
