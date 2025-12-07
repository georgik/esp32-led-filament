# ESP32 LED Filament Controller

ESP-IDF project for controlling an LED filament with smooth dimming and glow effects on ESP32-C6.

## Hardware

### Components
- ESP32-C6 development board
- Optosupply OSFX3002AY5-3V LED filament (300mm, yellow, 3V/20mA)
- BC5468 NPN transistor (or similar general-purpose NPN)
- 1kΩ resistor
- [Wire Wrap](https://youtu.be/L-463vchW0o?si=xDqNH_X4M9rla1Cx)

### Circuit
```
LED filament + → BC5468 Collector (right pin)
LED filament - → ESP32-C6 GND
ESP32-C6 3.3V → BC5468 Emitter (left pin)
ESP32-C6 GPIO 2 → 1kΩ resistor → BC5468 Base (center pin)
```

### Transistor Pinout (BC5468)
Viewing the flat side with legs facing down:
- Left: Emitter (E)
- Center: Base (B)
- Right: Collector (C)

## Software

### Features
- PWM-based brightness control using ESP32 LEDC peripheral
- Hardware-accelerated fade in/out effects
- Continuous breathing/glow animation
- 13-bit PWM resolution (8192 levels)
- 5kHz PWM frequency (flicker-free)

### Configuration
Constants in `main/esp32-led-filament.c`:

```c
#define LED_FILAMENT_GPIO     2        // GPIO pin for PWM output
#define LEDC_FREQUENCY        5000     // PWM frequency in Hz
#define LED_FADE_TIME_MS      1000     // Fade transition time in ms
```

### Build & Flash

```bash
# Setup ESP-IDF environment
get_idf

# Build the project
idf.py build

# Flash to ESP32-C6
idf.py flash monitor

# Clean build (if needed)
idf.py fullclean
```

## How It Works

1. **LEDC Initialization**: Sets up PWM timer and channel for GPIO 2
2. **FreeRTOS Task**: Dedicated task runs the glow animation loop
3. **PWM Control**: Hardware-accelerated fade functions handle smooth transitions
4. **Animation Pattern**: Fade in → 500ms pause → Fade out → 500ms pause → repeat

## Customization

### Change Glow Speed
Edit `LED_FADE_TIME_MS`:
- `500` = faster transitions
- `2000` = slower, more gentle breathing

### Change Brightness Range
Modify the duty cycle in the fade function:
```c
// Current: 80% max brightness
LEDC_DUTY_CYCLE_MAX * 0.8

// Full brightness
LEDC_DUTY_CYCLE_MAX

// Half brightness
LEDC_DUTY_CYCLE_MAX * 0.5
```

### Use Different GPIO
Change `LED_FILAMENT_GPIO` to any available ESP32-C6 GPIO pin (avoid pins 12+ during programming).

## Troubleshooting

### No LED Activity
- Check transistor pinout (E-B-C order)
- Verify 1kΩ resistor is connected to base
- Confirm GPIO 2 is properly configured
- Check power supply to ESP32-C6

### Dim LED
- Verify 3.3V connection to transistor emitter
- Check for voltage drops in wiring
- Confirm transistor is NPN type

### No Fading Effect
- Check LEDC timer configuration
- Verify fade function installation
- Monitor serial output for error messages

## Technical Details

- **PWM Resolution**: 13-bit (0-8191)
- **Update Frequency**: 5kHz (above human flicker detection)
- **Current Handling**: Up to 100mA (transistor limited)
- **Power Dissipation**: 500mW (transistor limit)

## Dependencies

- ESP-IDF v5.0+
- ESP32-C6 target
- FreeRTOS (included with ESP-IDF)
