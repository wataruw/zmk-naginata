# OS-Specific Key Swapping

## Overview

This feature allows ZMK-Naginata to automatically swap GUI (Windows/Command) and Ctrl modifier keys based on your operating system setting. This is particularly useful for macOS users who want to use their keyboard with the standard Mac Command key behavior.

## Behavior

### Windows Mode (NG_WINDOWS)
Keys are sent as-is without any modification:
- Physical LEFT_CTRL → LEFT_CTRL
- Physical LEFT_GUI → LEFT_GUI
- Physical RIGHT_CTRL → RIGHT_CTRL
- Physical RIGHT_GUI → RIGHT_GUI

### macOS Mode (NG_MACOS)
GUI and Ctrl keys are swapped:
- Physical LEFT_CTRL → LEFT_GUI (Command key)
- Physical LEFT_GUI → LEFT_CTRL
- Physical RIGHT_CTRL → RIGHT_GUI (Command key)
- Physical RIGHT_GUI → RIGHT_CTRL

### Linux/iOS Mode
Behaves the same as Windows mode (no swapping).

## How to Use

### Switching OS Modes

Use the following key bindings to switch between OS modes:

- **F15**: Switch to Windows mode
- **F16**: Switch to macOS mode
- **F17**: Switch to Linux mode

To use these, add `&ng F15`, `&ng F16`, or `&ng F17` to your keymap.

### Example Keymap Configuration

```
keymap {
    compatible = "zmk,keymap";
    
    default_layer {
        bindings = <
            // ... other keys ...
            &ng F16  // macOS mode
            &ng F15  // Windows mode
        >;
    };
};
```

## Use Cases

### For macOS Users

When using a keyboard with physical Ctrl and GUI keys on macOS:
1. Press `&ng F16` to enable macOS mode
2. Now when you press the physical Ctrl key, it acts as Command (⌘)
3. Common shortcuts work as expected:
   - Physical Ctrl+C → Command+C (copy)
   - Physical Ctrl+V → Command+V (paste)
   - Physical Ctrl+Tab → Command+Tab (app switcher)

### For Windows Users

1. Press `&ng F15` to enable Windows mode
2. Keys work normally without swapping

### For Multi-OS Users

Switch between modes based on which computer you're connected to:
- Use F16 when connected to your Mac
- Use F15 when connected to your Windows PC

## Technical Details

- The swapping works on **all keyboard layers** (0, 1, 2, 3)
- Only affects GUI and Ctrl modifier keys (LEFT_CTRL, LEFT_GUI, RIGHT_CTRL, RIGHT_GUI)
- Other modifiers (Shift, Alt) are not affected
- The setting persists until changed (default is macOS mode)

## Implementation

The feature is implemented in `src/behaviors/behavior_naginata.c` through a keycode event listener that:
1. Intercepts HID modifier key events
2. Checks the current OS setting (`naginata_config.os`)
3. Swaps keycodes if in macOS mode
4. Emits the modified (or original) keycode event

## References

- HID Usage Tables: USB HID specification page 0x07
- ZMK Event System: Event-based keycode handling
