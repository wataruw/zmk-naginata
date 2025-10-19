# Implementation Notes: Modifier and Layer Key Support

## Overview
This document describes the implementation of modifier and layer key support for the ZMK naginata behavior, which allows users to temporarily disable naginata input processing when switching layers or using modifier keys.

## Background

### Problem
In the original ZMK implementation, when on the naginata layer (typically layer 1), pressing modifier keys or layer switching keys (like `&mo LOWER`, `&to 0`) would not work properly. The naginata behavior would continue processing keys even when the user intended to switch to another layer or use keyboard shortcuts.

### QMK Reference Implementation
In QMK firmware (naginata_v16.c, lines 541-556), this was handled by the `process_modifier` function:

```c
bool process_modifier(uint16_t keycode, keyrecord_t *record) {
  if (IS_MODIFIER_KEYCODE(keycode) || IS_QK_MOD_TAP(keycode) || 
      keycode == TO(1) || keycode == TO(2) || 
      keycode == MO(1) || keycode == MO(2)) {
    if (record->event.pressed) {
      n_modifier++;
      layer_off(naginata_layer);
    } else {
      n_modifier--;
      if (n_modifier <= 0) {
        n_modifier = 0;
        layer_on(naginata_layer);
      }
    }
    return true;
  }
  return false;
}
```

## ZMK Implementation

### Architecture Differences
ZMK's architecture differs from QMK in several key ways:

1. **Layer Management**: In ZMK, layers are managed declaratively through the keymap and behavior system, not imperatively through code
2. **Event System**: ZMK uses an event-driven architecture with event listeners
3. **Behaviors**: Key functionality is implemented as behaviors that respond to binding events

### Solution Approach
Rather than directly controlling layers (which isn't possible in ZMK), we:
1. Subscribe to all keyboard events via the event system
2. Track when modifier or layer keys are active
3. Temporarily disable naginata processing when these keys are pressed
4. Re-enable when all such keys are released

## Implementation Details

### State Management

#### Global Variables
```c
static int8_t n_modifier = 0;                 // Counter for active modifier/layer keys
static bool naginata_layer_active = true;     // Flag to enable/disable naginata
static const struct device *naginata_behavior_dev = NULL;  // Reference to naginata behavior
```

### Event Listener

#### Subscription
```c
ZMK_LISTENER(behavior_naginata_global, naginata_position_state_changed_listener);
ZMK_SUBSCRIPTION(behavior_naginata_global, zmk_position_state_changed);
```

The event listener intercepts ALL position state change events (key presses/releases) before they reach the keymap layer.

#### Event Handler Logic
1. Extract the key position from the event
2. Look up the corresponding behavior binding in the keymap
3. Check if it's a modifier or layer key
4. Update the counter and naginata state accordingly
5. Clear input buffer when deactivating

### Modifier/Layer Key Detection

#### Detection Strategy
Three-tier approach for maximum compatibility:

**Tier 1: Modifier Keycodes**
- Detect HID modifier keycodes (0xE0-0xE7)
- Covers: Left/Right Ctrl, Shift, Alt, GUI

**Tier 2: Short Behavior Names (Exact Match)**
- Check for exact matches: "mo", "to", "lt", "mt", "sl", "tog"
- Prevents false positives from custom behaviors with similar names

**Tier 3: Long Behavior Names (Pattern Match)**
- Exact match: "BEHAVIOR_MOMENTARY_LAYER", "BEHAVIOR_TO_LAYER", etc.
- Substring match: "*_layer", "*_tap" for custom behaviors

#### Why This Approach?
- **Flexibility**: Works with standard ZMK behaviors and custom behaviors
- **Precision**: Exact matching for short names avoids false positives
- **Robustness**: Pattern matching catches variations in naming

### Processing Logic

#### Activation/Deactivation
```c
if (modifier_pressed) {
    n_modifier++;
    if (naginata_layer_active) {
        naginata_layer_active = false;
        // Clear pending input
        nginput.size = 0;
        pressed_keys = 0UL;
        n_pressed_keys = 0;
    }
}
```

```c
if (modifier_released) {
    if (n_modifier > 0) {
        n_modifier--;
    }
    if (n_modifier == 0) {
        if (!naginata_layer_active) {
            naginata_layer_active = true;
        }
    }
}
```

#### Key Points
- Counter tracks number of active modifiers
- Naginata only reactivates when ALL modifiers released
- Input buffer cleared to prevent incomplete input
- Underflow protection prevents negative counter values

### Integration with Naginata Behavior

#### Press Handler
```c
bool naginata_press(...) {
    if (!naginata_layer_active) {
        return true;  // Early return, let key pass through
    }
    // Normal naginata processing...
}
```

#### Release Handler
```c
bool naginata_release(...) {
    if (!naginata_layer_active) {
        return true;  // Early return
    }
    // Normal naginata processing...
}
```

## Edge Cases Handled

### 1. Multiple Simultaneous Modifiers
- Counter tracks all active modifiers
- Only reactivates when counter reaches 0
- Example: Ctrl+Shift held together

### 2. Counter Underflow
- Bounds checking: `if (n_modifier > 0) n_modifier--`
- Prevents negative values from spurious release events

### 3. Incomplete Input
- Buffer cleared when deactivating
- Prevents partial naginata combinations from outputting

### 4. Null Checks
- All pointer accesses are null-checked
- Device name, binding, event all validated

### 5. Self-Exclusion
- Naginata behavior keys are excluded from detection
- Prevents naginata from deactivating itself

## Performance Considerations

### Event Handler Overhead
- **Minimal**: Simple checks and early returns
- **Non-blocking**: Returns immediately after processing
- **Efficient lookups**: Direct array access for short names

### Buffer Clearing
- **O(1)**: Direct size reset instead of loop
- **Previous**: `while (size > 0) remove(0)` - O(n²)
- **Current**: `size = 0` - O(1)

### Memory Impact
- **3 static variables**: ~6 bytes total
- **No dynamic allocation**: All operations on stack or static memory
- **Negligible impact**: Well within embedded system constraints

## Testing and Validation

### Debug Logging
Enable debug logging to see detection in action:

```c
CONFIG_ZMK_LOG_LEVEL_DBG=y
```

Expected log messages:
- `Detected modifier keycode: 0xE0`
- `Detected layer/mod behavior: mo`
- `Naginata layer deactivated (n_modifier=1)`
- `Naginata layer activated (n_modifier=0)`

### Test Scenarios
See TESTING.md for comprehensive test cases covering:
- Basic layer switching
- Modifier keys
- Mod-tap keys
- Multiple modifiers
- Edge cases

## Limitations and Future Work

### Current Limitations
1. **Naming-based Detection**: Relies on behavior name patterns
2. **Custom Behaviors**: May require manual addition to detection function
3. **No Type Introspection**: ZMK doesn't expose behavior types

### Potential Improvements
1. **Configuration Options**: Allow users to specify additional behaviors to detect
2. **Behavior Type System**: If ZMK adds type introspection, use it instead of name matching
3. **Layer State Tracking**: Optionally only activate on specific layers

### Why Not Implement These Now?
- Current solution covers 99% of use cases
- Configuration adds complexity
- ZMK doesn't currently support behavior type introspection
- Layer state tracking would require significant additional code

## Integration Guide

### For Users
No changes required to existing keymaps. The feature activates automatically.

### For Developers
If adding custom behaviors that should disable naginata:
1. Follow ZMK naming conventions (*_layer, *_tap)
2. OR modify `is_modifier_or_layer_key()` to include your behavior name

### For Maintainers
Key files:
- `src/behaviors/behavior_naginata.c`: Main implementation
- `TESTING.md`: Test documentation
- This file: Implementation documentation

## Conclusion
This implementation successfully adapts QMK's `process_modifier` functionality to ZMK's event-driven architecture. It provides seamless integration with minimal performance impact and comprehensive edge case handling.
