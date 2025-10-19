# Testing Guide for Modifier/Layer Key Support

## ⚠️ IMPORTANT: Feature Removed (2025-10-19)

**The modifier and layer key detection feature described in this document has been removed** due to the use of a non-existent ZMK API function (`zmk_keymap_position_binding`).

See [LINKER_ERROR_FIX.md](LINKER_ERROR_FIX.md) for details on why this feature was removed.

**Current Behavior**: The naginata behavior remains active at all times and does not automatically disable when modifier or layer keys are pressed.

## Current Testing Guidelines

### Test 1: Basic Naginata Input
**Setup:** Switch to the naginata layer (layer 1) using the H+J combo
**Steps:**
1. Type various key combinations that should produce Japanese characters
2. Test single key presses (e.g., Q, W, E, R, T)
3. Test simultaneous key presses (e.g., Q+W, E+R)
4. Test with shift keys (Space, Enter)

**Expected Result:**
- Japanese characters should be output correctly according to the naginata layout
- All key combinations should work as expected

### Test 2: Switching Between Naginata and Regular Input
**Setup:** Be on the default layer (layer 0)
**Steps:**
1. Press H+J simultaneously to activate naginata layer
2. Type some Japanese using naginata
3. Press F+G simultaneously to deactivate naginata layer
4. Type regular characters

**Expected Result:**
- Switching between layers should work correctly
- Japanese input works when naginata is active
- Regular input works when naginata is deactivated

### Test 3: Known Limitation - Modifier Keys
**Setup:** Be on the naginata layer (layer 1)
**Steps:**
1. Try to use Ctrl+C, Ctrl+V, or other keyboard shortcuts
2. Try to switch to other layers using &mo or &to keys

**Expected Result (Current Limitation):**
- Modifier keys and shortcuts may not work as expected on the naginata layer
- Workaround: Switch back to layer 0 (using F+G combo) before using shortcuts
- Workaround: Define layer switching keys on non-naginata layers

---

## Historical Testing Documentation

The following test cases are for the **previous implementation** that has been removed.

## Test Cases

### Test 1: Basic Layer Switching with MO (Momentary Layer)
**Setup:** Be on the naginata layer (layer 1)
**Steps:**
1. Press and hold a key bound to `&mo LOWER` or `&mo RAISE`
2. Press keys on the target layer
3. Release the momentary layer key
4. Try typing in naginata again

**Expected Result:**
- Keys on the momentary layer should work normally (not processed as naginata)
- After releasing the momentary layer key, naginata input should work again

### Test 2: Layer Switching with TO (Switch to Layer)
**Setup:** Be on the naginata layer (layer 1)
**Steps:**
1. Press a key bound to `&to 0` to switch to base layer
2. Try typing - should output regular characters
3. Switch back to naginata layer with `&to 1` or combo

**Expected Result:**
- When on base layer, keys should output normal characters
- Naginata input should work after switching back to naginata layer

### Test 3: Modifier Keys (Ctrl, Shift, Alt, GUI)
**Setup:** Be on the naginata layer (layer 1)
**Steps:**
1. Press and hold Ctrl key (or any modifier)
2. Press other keys while Ctrl is held
3. Release Ctrl key
4. Try typing in naginata

**Expected Result:**
- Key combinations with Ctrl should work (e.g., Ctrl+C for copy)
- Keys should not be processed as naginata while Ctrl is held
- After releasing Ctrl, naginata input should work again

### Test 4: Mod-Tap Keys
**Setup:** Be on the naginata layer (layer 1)
**Steps:**
1. Press and hold a mod-tap key (e.g., `&mt LSHIFT SPACE`)
2. Press other keys while held
3. Release the mod-tap key
4. Try typing in naginata

**Expected Result:**
- When held long enough to activate the modifier, naginata should be disabled
- Keys pressed during modifier hold should not be processed as naginata
- After release, naginata input should work again

### Test 5: Multiple Modifiers
**Setup:** Be on the naginata layer (layer 1)
**Steps:**
1. Press and hold Ctrl
2. Press and hold Shift (both held)
3. Press other keys
4. Release Shift
5. Press other keys (only Ctrl held)
6. Release Ctrl
7. Try typing in naginata

**Expected Result:**
- Naginata should remain disabled as long as ANY modifier is held
- Naginata should only re-activate after ALL modifiers are released

### Test 6: Pending Input Clearing
**Setup:** Be on the naginata layer (layer 1)
**Steps:**
1. Start typing a naginata key combination (press but don't complete)
2. While keys are still pressed, press a modifier or layer key
3. Release all keys

**Expected Result:**
- The incomplete naginata combination should not output anything
- Input buffer should be cleared when the modifier/layer key is pressed

## Debugging

If the feature doesn't work as expected, enable debug logging:

1. In your ZMK config, set `CONFIG_ZMK_LOG_LEVEL_DBG=y`
2. Check the logs for messages like:
   - `Naginata layer deactivated (n_modifier=X)`
   - `Naginata layer activated (n_modifier=X)`
   - `Detected modifier keycode: 0xXX`
   - `Detected layer/mod-tap behavior: BEHAVIOR_NAME`

3. The logs will show:
   - When modifiers/layer keys are detected
   - The behavior device names being checked
   - When naginata is being activated/deactivated

## Known Limitations

1. The behavior name detection uses string matching, which may not work with custom behaviors that don't follow standard naming conventions
2. Only standard ZMK behaviors are detected (MOMENTARY, LAYER, TOGGLE, MOD_TAP, STICKY)
3. Custom behaviors that should disable naginata may need to be added to the detection function

## Troubleshooting

### Issue: Naginata doesn't deactivate when pressing modifiers
**Check:**
- Are the modifier keys on your keymap using standard ZMK behaviors?
- Enable debug logging and check if the modifier is being detected

### Issue: Naginata doesn't reactivate after releasing modifiers
**Check:**
- Are all modifiers released?
- Check debug logs for the `n_modifier` counter - it should reach 0

### Issue: Keys are processed as naginata when they shouldn't be
**Check:**
- Is the key bound to a layer/modifier behavior?
- Check debug logs to see if the behavior is being detected
