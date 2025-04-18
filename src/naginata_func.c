
#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/behavior.h>
#include <zmk/behavior_queue.h>
#include <zmk_naginata/naginata_func.h>

// #define HOST_OS 0 // Windows
// #define HOST_OS 2 // Linux
#define HOST_OS 2 // MacOS

int64_t timestamp;

#define NG_WIN 0
#define NG_LINUX 1
#define NG_MAC 2
#define NG_IOS 3

typedef union {
    uint8_t os : NG_MAC;
    bool live_conv : true;
    bool tategaki : true;
    bool kouchi_shift : true;
} user_config_t;

user_config_t naginata_config;

// 薙刀式をオン
void naginata_on(void) {
    raise_zmk_keycode_state_changed_from_encoded(LANG1, true, timestamp);
    raise_zmk_keycode_state_changed_from_encoded(LANG1, false, timestamp);
    raise_zmk_keycode_state_changed_from_encoded(INT4, true, timestamp);
    raise_zmk_keycode_state_changed_from_encoded(INT4, false, timestamp);
}

// 薙刀式をオフ
// void naginata_off(void) {
//     raise_zmk_keycode_state_changed_from_encoded(LANG2, true, timestamp);
//     raise_zmk_keycode_state_changed_from_encoded(LANG2, false, timestamp);
//     raise_zmk_keycode_state_changed_from_encoded(INT5, true, timestamp);
//     raise_zmk_keycode_state_changed_from_encoded(INT5, false, timestamp);
// }

void nofunc() {}

void ng_T() { ng_left(1); }

void ng_Y() { ng_right(1); }

void ng_ST() {
    raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
    ng_left(1);
    raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
}

void ng_SY() {
    raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
    ng_right(1);
    raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
}

void ngh_JKQ() { // ^{End}
  ng_eof();
}

void ngh_JKW() { // ／{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(F, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(F, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(F, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

// void ngh_JKE() { // /*ディ*/
  
// }

void ngh_JKR() { // ^s
  ng_save();
}

void ngh_JKT() { // ・
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(F, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(B, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(B, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_JKA() { // ……{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N2, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N2, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N2, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N2, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N6, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N6, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N2, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N2, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N2, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N2, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N6, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N6, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_JKS() { // 『{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(E, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(E, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_JKD() { // ？{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(F, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(F, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N1, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N1, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(F, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_JKF() { // 「{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(C, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(C, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_JKG() { // ({改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N2, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N2, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N8, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N8, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_JKZ() { // ――{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N2, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N2, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N1, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N1, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N5, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N5, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N2, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N2, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N1, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N1, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N5, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N5, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_JKX() { // 』{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(F, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_JKC() { // ！{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(F, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(F, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N1, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N1, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_JKV() { // 」{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(D, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(D, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_JKB() { // ){改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N2, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N2, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N9, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N9, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_DFY() { // {Home}
  ng_home();
}

void ngh_DFU() { // +{End}{BS}
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
  ng_end();
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(BSPC, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(BSPC, false, timestamp);
}

void ngh_DFI() { // {vk1Csc079}
  ng_saihenkan();
}

void ngh_DFO() { // {Del}
  raise_zmk_keycode_state_changed_from_encoded(DELETE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(DELETE, false, timestamp);
}

void ngh_DFP() { // +{Esc 2}
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ESC, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ESC, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ESC, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ESC, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
}

void ngh_DFH() { // {Enter}{End}
  raise_zmk_keycode_state_changed_from_encoded(ENTER, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, false, timestamp);
  ng_end();
}

void ngh_DFJ() { // {↑}
  ng_up(1);
}

void ngh_DFK() { // +{↑}
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
  ng_up(1);
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
}

void ngh_DFL() { // +{↑ 7}
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
  ng_up(7);
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
}

void ngh_DFSCLN() { // ^i
  ng_katakana();
}

void ngh_DFN() { // {End}
  ng_end();
}

void ngh_DFM() { // {↓}
  ng_down(1);
}

void ngh_DFCOMM() { // +{↓}
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
  ng_down(1);
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
}

void ngh_DFDOT() { // +{↓ 7}
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
  ng_down(7);
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
}

void ngh_DFSLSH() { // ^u
  ng_hiragana();
}

void ngh_MCQ() { // ｜{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(F, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(F, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N5, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N5, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(C, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(C, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_MCW() { // 　　　×　　　×　　　×{改行 2}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(D, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(D, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N7, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N7, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(D, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(D, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N7, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N7, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(D, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(D, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N7, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N7, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, false, timestamp);
}

void ngh_MCE() { // {Home}{→}{End}{Del 2}{←}
  ng_home();
  ng_right(1);
  ng_end();
  raise_zmk_keycode_state_changed_from_encoded(DELETE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(DELETE, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(DELETE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(DELETE, false, timestamp);
  ng_left(1);
}

void ngh_MCR() { // {Home}{改行}{Space 1}{←}
  ng_home();
  raise_zmk_keycode_state_changed_from_encoded(ENTER, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, false, timestamp);
  ng_left(1);
}

void ngh_MCT() { // 〇{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N7, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N7, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_MCA() { // 《{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(A, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(A, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_MCS() { // 【{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N1, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N1, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_MCD() { // {Home}{→}{End}{Del 4}{←}
  ng_home();
  ng_right(1);
  ng_end();
  raise_zmk_keycode_state_changed_from_encoded(DELETE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(DELETE, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(DELETE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(DELETE, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(DELETE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(DELETE, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(DELETE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(DELETE, false, timestamp);
  ng_left(1);
}

void ngh_MCF() { // {Home}{改行}{Space 3}{←}
  ng_home();
  raise_zmk_keycode_state_changed_from_encoded(ENTER, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, false, timestamp);
  ng_left(1);
}

void ngh_MCG() { // {Space 3}
  raise_zmk_keycode_state_changed_from_encoded(SPACE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, false, timestamp);
}

void ngh_MCZ() { // 》{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(B, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(B, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_MCX() { // 】{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N1, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N1, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N1, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N1, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_MCC() { // 」{改行}{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(D, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(D, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, false, timestamp);
}

void ngh_MCV() { // 」{改行}{改行}「{改行}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(D, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(D, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(C, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(C, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
}

void ngh_MCB() { // 」{改行}{改行}{Space}
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F20, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N3, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N3, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(N0, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(N0, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(D, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(D, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
  k_sleep(K_MSEC(100));
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(F19, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LEFT_CONTROL, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, false, timestamp);
}


void ngh_CVY() { // +{Home}
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
  ng_home();
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
}

void ngh_CVU() { // ^x
  ng_cut();
}

void ngh_CVI() { // {vk1Csc079}
  ng_saihenkan();
}

void ngh_CVO() { // ^v
  ng_paste();
}

void ngh_CVP() { // ^z
  ng_undo();
}

void ngh_CVH() { // ^c
  ng_copy();
}

void ngh_CVJ() { // {←}
  ng_left(1);
}

void ngh_CVK() { // {→}
  ng_right(1);
}

void ngh_CVL() { // {改行}{Space}+{Home}^x{BS}
  raise_zmk_keycode_state_changed_from_encoded(ENTER, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(ENTER, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(SPACE, false, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
  ng_home();
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
  ng_cut();
  raise_zmk_keycode_state_changed_from_encoded(BSPC, true, timestamp);
  raise_zmk_keycode_state_changed_from_encoded(BSPC, false, timestamp);
}

void ngh_CVSCLN() { // ^y
  ng_redo();
}

void ngh_CVN() { // +{End}
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
  ng_end();
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
}

void ngh_CVM() { // +{←}
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
  ng_left(1);
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
}

void ngh_CVCOMM() { // +{→}
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
  ng_right(1);
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
}

void ngh_CVDOT() { // +{← 7}
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
  ng_left(7);
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
}

void ngh_CVSLSH() { // +{→ 7}
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
  ng_right(7);
  raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
}

void ng_cut() {
    switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
        raise_zmk_keycode_state_changed_from_encoded(LC(X), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LC(X), false, timestamp);
        break;
    case NG_MAC:
        raise_zmk_keycode_state_changed_from_encoded(LG(X), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LG(X), false, timestamp);
        break;
    }
}

void ng_copy() {
    switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
        raise_zmk_keycode_state_changed_from_encoded(LC(C), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LC(C), false, timestamp);
        break;
    case NG_MAC:
        raise_zmk_keycode_state_changed_from_encoded(LG(C), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LG(C), false, timestamp);
        break;
    }
}

void ng_paste() {
    switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
        raise_zmk_keycode_state_changed_from_encoded(LC(V), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LC(V), false, timestamp);
        break;
    case NG_MAC:
        raise_zmk_keycode_state_changed_from_encoded(LG(V), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LG(V), false, timestamp);
        break;
    }
}

void ng_up(uint8_t c) {
    for (uint8_t i = 0; i < c; i++) { // サイズ削減
        if (naginata_config.tategaki) {
            raise_zmk_keycode_state_changed_from_encoded(UP, true, timestamp);
            raise_zmk_keycode_state_changed_from_encoded(UP, false, timestamp);
        } else {
            raise_zmk_keycode_state_changed_from_encoded(LEFT, true, timestamp);
            raise_zmk_keycode_state_changed_from_encoded(LEFT, false, timestamp);
        }
    }
}

void ng_down(uint8_t c) {
    for (uint8_t i = 0; i < c; i++) {
        if (naginata_config.tategaki) {
            raise_zmk_keycode_state_changed_from_encoded(DOWN, true, timestamp);
            raise_zmk_keycode_state_changed_from_encoded(DOWN, false, timestamp);
        } else {
            raise_zmk_keycode_state_changed_from_encoded(RIGHT, true, timestamp);
            raise_zmk_keycode_state_changed_from_encoded(RIGHT, false, timestamp);
        }
    }
}

void ng_left(uint8_t c) {
    for (uint8_t i = 0; i < c; i++) {
        if (naginata_config.tategaki) {
            raise_zmk_keycode_state_changed_from_encoded(LEFT, true, timestamp);
            raise_zmk_keycode_state_changed_from_encoded(LEFT, false, timestamp);
        } else {
            raise_zmk_keycode_state_changed_from_encoded(DOWN, true, timestamp);
            raise_zmk_keycode_state_changed_from_encoded(DOWN, false, timestamp);
        }
    }
}

void ng_right(uint8_t c) {
    for (uint8_t i = 0; i < c; i++) {
        if (naginata_config.tategaki) {
            raise_zmk_keycode_state_changed_from_encoded(RIGHT, true, timestamp);
            raise_zmk_keycode_state_changed_from_encoded(RIGHT, false, timestamp);
        } else {
            raise_zmk_keycode_state_changed_from_encoded(UP, true, timestamp);
            raise_zmk_keycode_state_changed_from_encoded(UP, false, timestamp);
        }
    }
}

void ng_home() {
    switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
        raise_zmk_keycode_state_changed_from_encoded(HOME, true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(HOME, false, timestamp);
        break;
    case NG_MAC:
        raise_zmk_keycode_state_changed_from_encoded(LC(A), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LC(A), false, timestamp);
        break;
    }
}

void ng_end() {
    switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
        raise_zmk_keycode_state_changed_from_encoded(END, true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(END, false, timestamp);
        break;
    case NG_MAC:
        raise_zmk_keycode_state_changed_from_encoded(LC(E), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LC(E), false, timestamp);
        break;
    }
}

void ng_katakana() {
    switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
        raise_zmk_keycode_state_changed_from_encoded(LC(I), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LC(I), false, timestamp);
        break;
    case NG_MAC:
        raise_zmk_keycode_state_changed_from_encoded(LC(K), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LC(K), false, timestamp);
        break;
    }
}

void ng_save() {
    switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
        raise_zmk_keycode_state_changed_from_encoded(LC(S), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LC(S), false, timestamp);
        break;
    case NG_MAC:
        raise_zmk_keycode_state_changed_from_encoded(LG(S), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LG(S), false, timestamp);
        break;
    }
}

void ng_hiragana() {
    switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
        raise_zmk_keycode_state_changed_from_encoded(LC(U), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LC(U), false, timestamp);
        break;
    case NG_MAC:
        raise_zmk_keycode_state_changed_from_encoded(LC(J), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LC(J), false, timestamp);
        break;
    }
}

void ng_redo() {
    switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
        raise_zmk_keycode_state_changed_from_encoded(LC(Y), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LC(Y), false, timestamp);
        break;
    case NG_MAC:
        raise_zmk_keycode_state_changed_from_encoded(LS(LG((S))), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LS(LG((S))), false, timestamp);
        break;
    }
}

void ng_undo() {
    switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
        raise_zmk_keycode_state_changed_from_encoded(LC(Z), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LC(Z), false, timestamp);
        break;
    case NG_MAC:
        raise_zmk_keycode_state_changed_from_encoded(LG(Z), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LG(Z), false, timestamp);
        break;
    }
}

void ng_saihenkan() {
    switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
        raise_zmk_keycode_state_changed_from_encoded(INT4, true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(INT4, false, timestamp);
        break;
    case NG_MAC:
        raise_zmk_keycode_state_changed_from_encoded(LANG1, true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LANG1, false, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LANG1, true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LANG1, false, timestamp);
        break;
    }
}

void ng_eof() {
    switch (naginata_config.os) {
    case NG_WIN:
    case NG_LINUX:
        raise_zmk_keycode_state_changed_from_encoded(LC(END), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LC(END), false, timestamp);
        break;
    case NG_MAC:
        raise_zmk_keycode_state_changed_from_encoded(LG(DOWN), true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(LG(DOWN), false, timestamp);
        break;
    }
}
