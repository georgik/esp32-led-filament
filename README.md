# ESP32 LED Filament Controller

ESP-IDF project for controlling an LED filament with smooth dimming and glow effects on ESP32-C6.

## Hardware

### Components
- ESP32-C6 development board
- Optosupply OSFX3002AY5-3V LED filament (300mm, yellow, 3V/20mA)
- Optosupply OSFX1301AB4-3V LED filament (300mm, blue, 3V/20mA)
- 2x BC5468 NPN transistors (or similar general-purpose NPN)
- 2x 1kΩ resistors
- [Wire Wrap](https://youtu.be/L-463vchW0o?si=xDqNH_X4M9rla1Cx)

### Circuit (Dual Filaments)
```
Yellow Filament + → BC5468-1 Collector (right pin)
Yellow Filament - → ESP32-C6 GND
ESP32-C6 3.3V → BC5468-1 Emitter (left pin)
ESP32-C6 GPIO 2 → 1kΩ resistor → BC5468-1 Base (center pin)

Blue Filament + → BC5468-2 Collector (right pin)
Blue Filament - → ESP32-C6 GND
ESP32-C6 3.3V → BC5468-2 Emitter (left pin)
ESP32-C6 GPIO 3 → 1kΩ resistor → BC5468-2 Base (center pin)
```

### Transistor Pinout (BC5468)
Viewing the flat side with legs facing down:
- Left: Emitter (E)
- Center: Base (B)
- Right: Collector (C)

## Software

### Features
- Dual PWM-based brightness control using ESP32 LEDC peripheral
- Inverse glow sequence (yellow fades in while blue fades out, and vice versa)
- Hardware-accelerated fade in/out effects
- Continuous alternating glow animation
- 13-bit PWM resolution (8192 levels)
- 5kHz PWM frequency (flicker-free)

### Configuration
Constants in `main/esp32-led-filament.c`:

```c
#define YELLOW_FILAMENT_GPIO   2        // GPIO pin for yellow filament PWM
#define BLUE_FILAMENT_GPIO     3        // GPIO pin for blue filament PWM
#define LEDC_FREQUENCY         5000     // PWM frequency in Hz
#define LED_FADE_TIME_MS       1000     // Fade transition time in ms
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

1. **Dual LEDC Initialization**: Sets up two PWM channels sharing one timer for GPIO 2 & 3
2. **FreeRTOS Task**: Dedicated task runs the inverse glow animation loop
3. **Simultaneous PWM Control**: Hardware-accelerated fade functions handle smooth concurrent transitions
4. **Animation Pattern**: Yellow fades in/blue fades out → 1s pause → Yellow fades out/blue fades in → 1s pause → repeat

## Customization

### Change Glow Speed
Edit `LED_FADE_TIME_MS`:
- `500` = faster transitions
- `2000` = slower, more gentle breathing

### Change Brightness Range
Modify the duty cycle in the fade functions:
```c
// Current: 80% max brightness
LEDC_DUTY_CYCLE_MAX * 0.8

// Full brightness
LEDC_DUTY_CYCLE_MAX

// Half brightness
LEDC_DUTY_CYCLE_MAX * 0.5
```

### Use Different GPIO Pins
Change `YELLOW_FILAMENT_GPIO` and `BLUE_FILAMENT_GPIO` to any available ESP32-C6 GPIO pins (avoid pins 12+ during programming).

### Different Animation Patterns
Modify the sequence in the task:
- **Synchronized glow**: Remove inverse behavior (fade both in/out together)
- **Different timing**: Separate fade times for each color
- **More colors**: Add additional GPIO channels

## Troubleshooting

### No LED Activity
- Check transistor pinouts (E-B-C order) for both transistors
- Verify 1kΩ resistors are connected to bases
- Confirm GPIO 2 & 3 are properly configured
- Check power supply to ESP32-C6

### Only One Filament Works
- Check connections for the non-working filament
- Verify the second transistor is functional
- Test with different GPIO pins if needed
- Monitor serial output for channel-specific errors

### Dim LEDs
- Verify 3.3V connections to transistor emitters
- Check for voltage drops in wiring
- Confirm transistors are NPN type
- Measure actual current draw with multimeter

### No Fading Effect or Synchronization Issues
- Check LEDC timer configuration
- Verify fade function installation
- Monitor serial output for error messages
- Ensure both channels use the same timer

### LEDs Glow Together (Not Inverse)
- Check the fade function calls are properly paired
- Verify duty cycle initialization (blue starts at 80%)
- Confirm LEDC channels are correctly assigned

## Technical Details

- **PWM Resolution**: 13-bit (0-8191)
- **Update Frequency**: 5kHz (above human flicker detection)
- **Current Handling**: Up to 100mA per transistor (200mA total)
- **Power Dissipation**: 500mW per transistor (1W total)
- **Color Sequence**: Yellow (590nm) ↔ Blue (470nm) inverse glow
- **Initial State**: Yellow off, Blue at 80% brightness

## Dependencies

- ESP-IDF v5.0+
- ESP32-C6 target
- FreeRTOS (included with ESP-IDF)
