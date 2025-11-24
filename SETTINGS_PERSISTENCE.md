# Settings Persistence Implementation

## Overview

This document describes the settings persistence feature that allows OS and tategaki (vertical writing) settings to be saved to non-volatile storage and restored after keyboard reboots.

## Problem

Previously, the OS setting (Windows/Mac/Linux) and tategaki setting were reset to their default values every time the keyboard restarted. This was caused by:

1. **Hardcoded initialization**: `naginata_config.os = NG_MACOS;` was executed every time in `behavior_naginata_init`
2. **No persistence layer**: Settings were only stored in RAM and lost on reboot
3. **Data structure bug**: `user_config_t` was defined as a `union`, causing `os` and `tategaki` to share the same memory, corrupting one when the other was changed

## Solution

### 1. Fixed Data Structure

Changed `user_config_t` from `union` to `struct`:

```c
// Before (INCORRECT - causes corruption)
typedef union {
    uint8_t os : 2;
    bool tategaki : true;
} user_config_t;

// After (CORRECT - independent fields)
typedef struct {
    uint8_t os : 2;
    bool tategaki : 1;
} user_config_t;
```

This change was made in both:
- `src/behaviors/behavior_naginata.c`
- `src/naginata_func.c`

### 2. Implemented Zephyr Settings Subsystem

Added settings persistence using Zephyr's built-in Settings subsystem:

#### Settings Handler

```c
static int naginata_settings_set(const char *name, size_t len, settings_read_cb read_cb,
                                  void *cb_arg) {
    const char *next;
    
    if (settings_name_steq(name, NAGINATA_SETTINGS_CONFIG_KEY, &next) && !next) {
        if (len != sizeof(user_config_t)) {
            return -EINVAL;
        }
        
        return read_cb(cb_arg, &naginata_config, sizeof(user_config_t));
    }
    
    return -ENOENT;
}
```

#### Settings Registration

In `behavior_naginata_init`:

```c
// Initialize with default values
naginata_config.os = NG_WINDOWS;  // Default to Windows (0)
naginata_config.tategaki = false;

// Register settings handler and load saved settings
int ret = settings_register(&naginata_settings_conf);
if (ret < 0) {
    LOG_WRN("Failed to register naginata settings: %d", ret);
} else {
    ret = settings_load_subtree(NAGINATA_SETTINGS_SUBTREE);
    if (ret < 0) {
        LOG_WRN("Failed to load naginata settings: %d", ret);
    } else {
        LOG_INF("Naginata settings loaded: os=%d, tategaki=%d", 
                naginata_config.os, naginata_config.tategaki);
    }
}
```

### 3. Save Settings on Change

When F15-F19 keys are pressed, settings are saved:

```c
case F15:
    naginata_config.os = NG_WINDOWS;
    settings_save_one(NAGINATA_SETTINGS_SUBTREE "/" NAGINATA_SETTINGS_CONFIG_KEY,
                    &naginata_config, sizeof(user_config_t));
    LOG_INF("OS setting changed to Windows");
    return ZMK_BEHAVIOR_OPAQUE;
```

## Key Bindings

- **F15**: Set OS to Windows (and save)
- **F16**: Set OS to macOS (and save)
- **F17**: Set OS to Linux (and save)
- **F18**: Enable tategaki/vertical writing (and save)
- **F19**: Disable tategaki/vertical writing (and save)

## Settings Storage

Settings are stored in the Zephyr Settings subsystem under:
- Subtree: `"naginata"`
- Key: `"config"`
- Full path: `"naginata/config"`

The settings are persisted to the keyboard's non-volatile storage (usually Flash memory).

## Default Values

If no saved settings are found (first boot or after settings reset):
- OS: `NG_WINDOWS` (0)
- Tategaki: `false`

**Note**: The default was changed from `NG_MACOS` to `NG_WINDOWS` to provide a more universally compatible default.

## Logging

When debug logging is enabled (`CONFIG_ZMK_LOG_LEVEL_DBG=y`), you will see:

```
[INF] Naginata settings loaded: os=0, tategaki=0
[INF] OS setting changed to macOS
[INF] Tategaki setting changed to true
```

## Technical Details

### Memory Layout

The `user_config_t` struct uses bit fields for compact storage:
- `os`: 2 bits (supports values 0-3)
- `tategaki`: 1 bit (boolean)
- Total size: 1 byte

### Initialization Order

1. Init function is called by ZMK during startup
2. Default values are set
3. Settings handler is registered
4. Saved settings are loaded from storage (overwrites defaults if found)
5. Current settings are logged

### Persistence Timing

Settings are saved immediately when changed (synchronous write). The Zephyr Settings subsystem handles:
- Flash wear leveling
- Atomic writes
- Corruption protection

## Testing

To test settings persistence:

1. Flash the firmware
2. Press F16 to set OS to macOS (you should see log message)
3. Restart the keyboard (power cycle or reset button)
4. Check that the OS setting is still macOS (behavior should match macOS)
5. Test with other settings (F15, F17, F18, F19)

## Troubleshooting

### Settings not persisting

**Check:**
1. Ensure ZMK Settings subsystem is enabled in your build configuration
2. Check that non-volatile storage is properly configured for your board
3. Enable debug logging to see if settings are being loaded/saved
4. Verify that the storage area has not been corrupted

### Settings corruption

**Check:**
1. Ensure you're using the corrected struct version (not union)
2. Try clearing settings: Some ZMK builds support clearing settings via specific key combinations
3. Reflash the firmware

### Wrong default value

If you prefer a different default OS, modify line in `behavior_naginata_init`:

```c
naginata_config.os = NG_MACOS;  // or NG_LINUX
```

## Compatibility

This implementation uses standard Zephyr APIs and should be compatible with:
- ZMK 3.0 and later
- All boards that support the Settings subsystem
- Both split and non-split keyboards

## Future Enhancements

Potential improvements:
1. Add configuration option to set default OS via Kconfig
2. Support for additional settings
3. Settings export/import functionality
4. Settings reset via key combination
