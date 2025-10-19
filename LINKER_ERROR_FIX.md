# Linker Error Fix: zmk_keymap_position_binding

## Problem
The code was calling `zmk_keymap_position_binding()` which does not exist in the ZMK API, causing this linker error:
```
/tmp/zmk-config/zmk-naginata/src/behaviors/behavior_naginata.c:798: undefined reference to `zmk_keymap_position_binding'
```

## Root Cause
The implementation attempted to detect modifier and layer keys by querying the keymap binding for each key position using `zmk_keymap_position_binding()`. However, this function does not exist in ZMK's public API.

## Solution
Removed the functionality that depends on the non-existent API:

### Removed Code
1. **Static variables**:
   - `n_modifier` - counter for active modifier/layer keys
   - `naginata_layer_active` - flag to enable/disable naginata processing
   - `naginata_behavior_dev` - reference to naginata behavior device

2. **Functions**:
   - `is_modifier_or_layer_key()` - detected modifier/layer keys by checking behavior names
   - `naginata_position_state_changed_listener()` - event listener that called the non-existent function

3. **Event subscriptions**:
   - `ZMK_LISTENER` and `ZMK_SUBSCRIPTION` for position_state_changed events

4. **Conditional checks**:
   - Removed `if (!naginata_layer_active)` checks in `naginata_press()` and `naginata_release()`

## Impact

### Before
The naginata behavior would temporarily deactivate when:
- Modifier keys (Ctrl, Shift, Alt, GUI) were pressed
- Layer switching keys (mo, to, lt, etc.) were pressed

This allowed users to use keyboard shortcuts and switch layers without naginata processing interfering.

### After
The naginata behavior remains **active at all times**. Users on the naginata layer will:
- ✅ Can still type using the naginata input method
- ❌ Cannot use modifier keys for shortcuts while on the naginata layer
- ❌ May experience unexpected behavior when using layer switching keys

## Alternative Approaches

To implement modifier/layer key detection in ZMK without the non-existent API, consider:

1. **Manual configuration**: Allow users to specify which keys should disable naginata in the devicetree
2. **Different approach**: Instead of detecting at the event level, handle it at the behavior level by checking the behavior type in the binding
3. **ZMK enhancement**: Propose adding `zmk_keymap_position_binding()` or similar API to ZMK upstream

## Recommendations

For users who need modifier/layer key support:
1. Define separate layers for different input modes
2. Use explicit layer switching rather than temporary overrides
3. Consider using the naginata on/off combos (F+G and H+J) to switch modes

## Files Changed
- `src/behaviors/behavior_naginata.c`: Removed ~140 lines of code related to modifier detection
