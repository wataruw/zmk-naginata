
#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>

#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/behavior.h>
#include <zmk/behavior_queue.h>
#include <zmk_naginata/naginata_func.h>

int64_t timestamp;

#define NG_WINDOWS (uint8_t)0
#define NG_MACOS (uint8_t)1
#define NG_LINUX (uint8_t)2
#define NG_IOS (uint8_t)3

typedef union {
    uint8_t os : 2;
    bool tategaki : true;
} user_config_t;

user_config_t naginata_config;

// 共通ユーティリティ: 1キーのタップ
static inline void tap_key(int keycode) {
    raise_zmk_keycode_state_changed_from_encoded(keycode, true, timestamp);
    raise_zmk_keycode_state_changed_from_encoded(keycode, false, timestamp);
}

// 薙刀式をオン
void naginata_on(void) {
    tap_key(LANG1);
    tap_key(INT4);
}

// 薙刀式をオフ
// void naginata_off(void) {
//     tap_key(LANG2);
//     tap_key(INT5);
// }

void nofunc() {}

void switch_to_hex_input() {
    switch (naginata_config.os) {
        case NG_MACOS:
            tap_key(LANG2);  // 未確定文字を確定する
            k_sleep(K_MSEC(10));
            tap_key(LC(F20));
            k_sleep(K_MSEC(50));
            return;
        case NG_WINDOWS:
            return;
        case NG_LINUX:
            return;
    }
}

void return_to_kana_input() {
    switch (naginata_config.os) {
        case NG_MACOS:
            tap_key(LS(LANG1));  // 未確定文字を確定する
            k_sleep(K_MSEC(10));
            tap_key(LANG1);
            return;
        case NG_WINDOWS:
        case NG_LINUX:
            return;
    }
}

void press_compose_key() {
    switch (naginata_config.os) {
        case NG_MACOS:
            raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, true, timestamp);
            k_sleep(K_MSEC(50));
            return;
        case NG_WINDOWS:
            tap_key(RIGHT_ALT);
            tap_key(U);
            k_sleep(K_MSEC(50));
            return;
        case NG_LINUX:
            tap_key(LC(LS(U)));
            k_sleep(K_MSEC(50));
            return;
    }
}

void release_compose_key() {
    switch (naginata_config.os) {
        case NG_MACOS:
            raise_zmk_keycode_state_changed_from_encoded(LEFT_ALT, false, timestamp);
            k_sleep(K_MSEC(50));
            return;
        case NG_WINDOWS:
            tap_key(ENTER);
            k_sleep(K_MSEC(50));
            return;
        case NG_LINUX:
            tap_key(LC(LS(U)));
            k_sleep(K_MSEC(50));
            return;
    }
}

void input_unicode_hex(int n1, int n2, int n3, int n4) {
    switch (naginata_config.os) {
        case NG_MACOS:
            switch_to_hex_input();
            press_compose_key();
            tap_key(n1);
            k_sleep(K_MSEC(10));
            tap_key(n2);
            k_sleep(K_MSEC(10));
            tap_key(n3);
            k_sleep(K_MSEC(10));
            tap_key(n4);
            k_sleep(K_MSEC(10));
            release_compose_key();
            return_to_kana_input();
            return;
        case NG_WINDOWS:
        case NG_LINUX:
            press_compose_key();
            tap_key(n1);
            k_sleep(K_MSEC(10));
            tap_key(n2);
            k_sleep(K_MSEC(10));
            tap_key(n3);
            k_sleep(K_MSEC(10));
            tap_key(n4);
            k_sleep(K_MSEC(10));
            release_compose_key();
            tap_key(ENTER);
            return_to_kana_input();
            return;
    }
}

// 共通ユーティリティ: 数字(0-9)を対応するキーコードに変換
static int digit_keycode(int d) {
    switch (d) {
    case 0: return N0;
    case 1: return N1;
    case 2: return N2;
    case 3: return N3;
    case 4: return N4;
    case 5: return N5;
    case 6: return N6;
    case 7: return N7;
    case 8: return N8;
    case 9: return N9;
    default: return N0;
    }
}

// 共通ユーティリティ: 「nu<nn>」の送出（N, U の後に数値の各桁をタイプし、Space→Enterで確定）
static void register_nu_number(int number) {
    if (number < 0) number = 0;
    if (number > 99) number = number % 100; // 想定外を最小限に丸める

    tap_key(N);
    tap_key(U);

    if (number >= 10) {
        tap_key(digit_keycode(number / 10));
    }
    tap_key(digit_keycode(number % 10));

    k_sleep(K_MSEC(300));
    tap_key(SPACE);
    k_sleep(K_MSEC(300));
    tap_key(ENTER);
}

// void ng_T() { ng_left(1); }

// void ng_Y() { ng_right(1); }

// void ng_ST() {
//     raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
//     ng_left(1);
//     raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
// }

// void ng_SY() {
//     raise_zmk_keycode_state_changed_from_encoded(LSHIFT, true, timestamp);
//     ng_right(1);
//     raise_zmk_keycode_state_changed_from_encoded(LSHIFT, false, timestamp);
// }

void ngh_JKQ() { // ぬ3 【】を「ぬ3」に登録
    register_nu_number(3);
}

void ngh_JKW() { // ぬ5 ……を「ぬ5」に登録
    register_nu_number(5);
}

void ngh_JKE() { // /*ディ*/
    tap_key(D);
    tap_key(H);
    tap_key(I);
}

void ngh_JKR() { // ぬ6 ――を「ぬ6」に登録
    register_nu_number(6);
}

void ngh_JKT() { // ・
    tap_key(SLASH);
}

void ngh_JKA() { // ぬ2 『』を「ぬ2」に登録
    register_nu_number(2);
}

void ngh_JKS() { //  ⇒を「ぬ1」に登録
    register_nu_number(1);
}

void ngh_JKD() { // ？{改行}
    tap_key(LS(SLASH));
    tap_key(ENTER);
}

void ngh_JKF() { // 「{改行}
    tap_key(RBKT);
    tap_key(ENTER);
}

void ngh_JKG() { // ({改行}
    tap_key(LS(N8)); // 未確定
}

void ngh_JKZ() { // ぬ4 《》を「ぬ4」に登録
    register_nu_number(4);
}

void ngh_JKX() { // F2
    tap_key(F2);
}

void ngh_JKC() { // ！{改行}
    tap_key(LS(N1));
    tap_key(ENTER);
}

void ngh_JKV() { // 」{改行}
    tap_key(NUHS);
    tap_key(ENTER);
}

void ngh_JKB() { // ){改行}
    tap_key(LS(N9)); // 未確定
}

void ngh_DFY() { // ^c
    ng_copy();
}

void ngh_DFU() { // ^x
    ng_cut();
}

void ngh_DFI() { // {vk1Csc079}
    ng_saihenkan();
}

void ngh_DFO() { // ^v
    ng_paste();
}

void ngh_DFP() { // ^z
    ng_undo();
}

void ngh_DFH() { // Ctrl(Cmd) + Shift + ←
    ng_ctrl_shift_left();
}

void ngh_DFJ() { // Shift + ←
    ng_shift_left();
}

void ngh_DFK() { // ↑
    tap_key(UP);
}

void ngh_DFL() { // Shift + →
    ng_shift_right();
}

void ngh_DFSCLN() { // Ctrl(Cmd) + Shift + →
    ng_ctrl_shift_right();
}

void ngh_DFN() { // Home
    ng_home();
}

void ngh_DFM() { // ←
    tap_key(LEFT);
}

void ngh_DFCOMM() { // ↓
    tap_key(DOWN);

}

void ngh_DFDOT() { // →
    tap_key(RIGHT);
}

void ngh_DFSLSH() { // End
    ng_end();
}

void ngh_MCQ() { // 「ぬ21」を用語辞書の「読み方」に登録
    register_nu_number(21);
}

void ngh_MCW() { // 「ぬ22」を用語辞書の「読み方」に登録
    register_nu_number(22);
}

void ngh_MCE() { // 「ぬ23」を用語辞書の「読み方」に登録
    register_nu_number(23);
}

void ngh_MCR() { // 「ぬ24」を用語辞書の「読み方」に登録
    register_nu_number(24);
}

void ngh_MCT() { // 「ぬ25」を用語辞書の「読み方」に登録
    register_nu_number(25);
}

void ngh_MCA() { // 「ぬ26」を用語辞書の「読み方」に登録
    register_nu_number(26);
}

void ngh_MCS() { // 「ぬ27」を用語辞書の「読み方」に登録
    register_nu_number(27);
}

void ngh_MCD() { // 「ぬ28」を用語辞書の「読み方」に登録
    register_nu_number(28);
}

void ngh_MCF() { // 「ぬ29」を用語辞書の「読み方」に登録
    register_nu_number(29);
}

void ngh_MCG() { // 「ぬ30」を用語辞書の「読み方」に登録
    register_nu_number(30);
}

void ngh_MCZ() { // 「ぬ31」を用語辞書の「読み方」に登録
    register_nu_number(31);
}

void ngh_MCX() { // 「ぬ32」を用語辞書の「読み方」に登録
    register_nu_number(32);
}

void ngh_MCC() { // 「ぬ33」を用語辞書の「読み方」に登録
    register_nu_number(33);
}

void ngh_MCV() { // 「ぬ34」を用語辞書の「読み方」に登録
    register_nu_number(34);
}

void ngh_MCB() { // 「ぬ35」を用語辞書の「読み方」に登録
    register_nu_number(35);
}

void ngh_CVY() { // 英字変換
    ng_eiji();
}

void ngh_CVU() { // ひらがな
    ng_hiragana();
}

void ngh_CVI() { // カタカナ
    ng_katakana();
}

void ngh_CVO() { // 半角変換
    ng_hankaku();
}

void ngh_CVP() { // 全角変換
    ng_zenkaku();
}

void ngh_CVH() { // F12
    tap_key(F12);
}

void ngh_CVJ() { // Ctrl(Cmd) + [
    ng_ctrl_bracket_left(); 
}

void ngh_CVK() { // Alt(Opt) + ↑
    ng_alt_up();
}

void ngh_CVL() { // Ctrl(Cmd) + ]
    ng_ctrl_bracket_right();
}

void ngh_CVSCLN() { // Ctrl(Cmd) + Shift + k
    ng_ctrl_shift_k();
}

void ngh_CVN() { // Shift + F12
    tap_key(LS(F12));
}

void ngh_CVM() { // Ctrl(Cmd) + Shift + [
    ng_ctrl_shift_bracket_left();
}
void ngh_CVCOMM() { // Alt(Opt) + ↓
    ng_alt_down();
}

void ngh_CVDOT() { // Ctrl(Cmd) + Shift + ]
    ng_ctrl_shift_bracket_right();
}

void ngh_CVSLSH() { // Alt(Opt) + Shift + ↓
    ng_alt_shift_down();
}

void ng_cut() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(LC(X));
        break;
    case NG_MACOS:
        tap_key(LG(X));
        break;
    }
}

void ng_copy() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(LC(C));
        break;
    case NG_MACOS:
        tap_key(LG(C));
        break;
    }
}

void ng_paste() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(LC(V));
        break;
    case NG_MACOS:
        tap_key(LG(V));
        break;
    }
}

void ng_up(uint8_t c) {
    for (uint8_t i = 0; i < c; i++) {
        tap_key(UP);
    }
}

void ng_down(uint8_t c) {
    for (uint8_t i = 0; i < c; i++) {
        tap_key(DOWN);
    }
}

void ng_left(uint8_t c) {
    for (uint8_t i = 0; i < c; i++) {
        tap_key(LEFT);
    }
}

void ng_right(uint8_t c) {
    for (uint8_t i = 0; i < c; i++) {
        tap_key(RIGHT);
    }
}

void ng_next_row() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        if (naginata_config.tategaki) {
            ng_left(1);
        } else{
            ng_down(1);
        }
        break;
    case NG_MACOS:
        tap_key(LG(N));
        break;
    }
}

void ng_prev_row() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        if (naginata_config.tategaki) {
            ng_right(1);
        } else {
            ng_up(1);
        }
        break;
    case NG_MACOS:
        tap_key(LG(P));
        break;
    }
}

void ng_next_char() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        if (naginata_config.tategaki) {
            ng_down(1);
        } else {
            ng_right(1);
        }
        break;
    case NG_MACOS:
        tap_key(LG(F));
        break;
    }
}

void ng_prev_char() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        if (naginata_config.tategaki) {
            ng_up(1);
        } else {
            ng_left(1);
        }
        break;
    case NG_MACOS:
        tap_key(LG(B));
        break;
    }
}

void ng_home() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(HOME);
        break;
    case NG_MACOS:
        tap_key(LC(A));
        break;
    }
}

void ng_end() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(END);
        break;
    case NG_MACOS:
        tap_key(LC(E));
        break;
    }
}

void ng_eiji() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
    case NG_MACOS:
        tap_key(LC(T));
        break;
    }
}

void ng_hiragana() {
    switch (naginata_config.os) {
        case NG_WINDOWS:
        case NG_LINUX:
        case NG_MACOS:
            tap_key(LC(U));
            break;
    }
}

void ng_katakana() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
    case NG_MACOS:
        tap_key(LC(I));
        break;
    }
}

void ng_hankaku() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(LC(O));
        break;
    case NG_MACOS: // Macは半角カタカナのショートカットがでデフォルトでは無いので全角カタカナにしておく 今時使わないだろ？
        tap_key(LC(I));
        break;
    }
}

void ng_zenkaku() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
    case NG_MACOS:
        tap_key(LC(P));
        break;
    }
}

void ng_save() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(LC(S));
        break;
    case NG_MACOS:
        tap_key(LG(S));
        break;
    }
}

void ng_redo() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(LC(Y));
        break;
    case NG_MACOS:
        tap_key(LS(LG((S))));
        break;
    }
}

void ng_undo() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(LC(Z));
        break;
    case NG_MACOS:
        tap_key(LG(Z));
        break;
    }
}

void ng_saihenkan() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
        tap_key(LG(FSLH));  // Win + / の方が強力
        break;
    case NG_LINUX:
        tap_key(INT4);
        break;
    case NG_MACOS:
        tap_key(LANG1);
        tap_key(LANG1);
        break;
    }
}

void ng_eof() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(LC(END));
        break;
    case NG_MACOS:
        tap_key(LG(DOWN));
        break;
    }
}

void ng_ctrl_bracket_left() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(LC(RBKT));
        break;
    case NG_MACOS:
        tap_key(LG(RBKT));
        break;
    }
}

void ng_ctrl_bracket_right() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(LC(NUHS));
        break;
    case NG_MACOS:
        tap_key(LG(NUHS));
        break;
    }
}

void ng_alt_down() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
    case NG_MACOS:
        tap_key(LA(DOWN));
        break;
    }
}

void ng_alt_up() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
    case NG_MACOS:
        tap_key(LA(UP));
        break;
    }
}

void ng_ctrl_shift_bracket_left() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
    case NG_MACOS:
        tap_key(LC(LS(RBKT)));
        break;
    }
}

void ng_ctrl_shift_bracket_right() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
    case NG_MACOS:
        tap_key(LC(LS(NUHS)));
        break;
    }
}

void ng_alt_shift_down() {  // void ng_alt_shift_up()はng_alt_shift_down()と同じ動作になるため削除
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
    case NG_MACOS:
        tap_key(LA(LS(DOWN)));
        break;
    }
}

void ng_ctrl_shift_k() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(LC(LS(K)));
        break;
    case NG_MACOS:
        tap_key(LG(LS(K)));
        break;
    }
}

void ng_ctrl_shift_left() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(LC(LS(LEFT)));
        break;
    case NG_MACOS:
        tap_key(LG(LS(LEFT)));
        break;
    }
}

void ng_ctrl_shift_right() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
        tap_key(LC(LS(RIGHT)));
        break;
    case NG_MACOS:
        tap_key(LG(LS(RIGHT)));
        break;
    }
}

void ng_shift_left() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
    case NG_MACOS:
        tap_key(LS(LEFT));
        break;
    }
}

void ng_shift_right() {
    switch (naginata_config.os) {
    case NG_WINDOWS:
    case NG_LINUX:
    case NG_MACOS:
        tap_key(LS(RIGHT));
        break;
    }
}
