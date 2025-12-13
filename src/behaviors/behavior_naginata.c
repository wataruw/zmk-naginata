/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT zmk_behavior_naginata

#include <zephyr/device.h>
#include <drivers/behavior.h>
#include <zephyr/logging/log.h>
#include <string.h>

#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>

#include <zmk_naginata/nglist.h>
#include <zmk_naginata/nglistarray.h>
#include <zmk_naginata/naginata_func.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);
extern int64_t timestamp;

#define NONE 0

// 薙刀式

// 31キーを32bitの各ビットに割り当てる
#define B_Q (1UL << 0)
#define B_W (1UL << 1)
#define B_E (1UL << 2)
#define B_R (1UL << 3)
#define B_T (1UL << 4)

#define B_Y (1UL << 5)
#define B_U (1UL << 6)
#define B_I (1UL << 7)
#define B_O (1UL << 8)
#define B_P (1UL << 9)

#define B_A (1UL << 10)
#define B_S (1UL << 11)
#define B_D (1UL << 12)
#define B_F (1UL << 13)
#define B_G (1UL << 14)

#define B_H (1UL << 15)
#define B_J (1UL << 16)
#define B_K (1UL << 17)
#define B_L (1UL << 18)
#define B_MINUS (1UL << 19)

#define B_Z (1UL << 20)
#define B_X (1UL << 21)
#define B_C (1UL << 22)
#define B_V (1UL << 23)
#define B_B (1UL << 24)

#define B_N (1UL << 25)
#define B_M (1UL << 26)
#define B_COMMA (1UL << 27)
#define B_DOT (1UL << 28)
#define B_SLASH (1UL << 29)

#define B_SPACE (1UL << 30)

static NGListArray nginput;
static uint32_t pressed_keys = 0UL; // 押しているキーのビットをたてる
static int8_t n_pressed_keys = 0;   // 押しているキーの数

// 薙刀式有効/無効
static bool naginata_layer_active = true; // 薙刀式レイヤーが有効かどうか

// 停止条件のカウンタ
static int8_t n_hid_modifiers = 0;   // HID修飾キー(0xE0-0xE7)の押下数
static int8_t n_layer_hold_keys = 0; // レイヤーキー(&mo, &lt)のホールド押下数

// 透過ビヘイビアのデバイス
// Zephyr/ZMKのバージョン差異で behavior_dev の型が異なる場合があるため、void*で保持する
static const void *trans_bhv_dev = NULL;
static const void *mo_bhv_dev = NULL; // &mo (momentary layer)
static const void *lt_bhv_dev = NULL; // &lt (layer tap)

// LOWER/RAISE レイヤーのインデックス（必要に応じて変更）
#ifndef NG_LOWER_LAYER_INDEX
#define NG_LOWER_LAYER_INDEX 1
#endif
#ifndef NG_RAISE_LAYER_INDEX
#define NG_RAISE_LAYER_INDEX 2
#endif
static bool ng_layer_hold_active = false;

// user_config_t と naginata_config は naginata_func.h で定義/宣言されている

// --- Press/Release 対応のための状態管理 --------------------------------------
// &ng が押下時に TRANSPARENT を返したキー位置を記録しておき、
// そのキーのリリース時も必ず TRANSPARENT を返すようにする。
// これにより、モディファイ押下などで途中で有効/無効が切り替わっても
// 押下と解放の処理主体が食い違わず、「押しっぱなし」にならない。

#ifndef NG_MAX_POSITIONS
// 取りこぼし防止のため拡大（以前: 256）
#define NG_MAX_POSITIONS 512
#endif

static uint32_t ng_transparent_pressed_bitmap[(NG_MAX_POSITIONS + 31) / 32];
// 透過として押されているポジション数（再有効化の抑止に使用）
static int ng_transparent_pressed_count = 0;

// 合成タップ（モディファイ押下中に即押下→即解放したキー）の押下記録
static uint32_t ng_chord_tap_pressed_bitmap[(NG_MAX_POSITIONS + 31) / 32];

static inline void ng_mark_transparent_press(uint16_t position) {
    if (position < NG_MAX_POSITIONS) {
        uint32_t mask = (1u << (position % 32));
        uint32_t *slot = &ng_transparent_pressed_bitmap[position / 32];
        if ((*slot & mask) == 0) {
            // 新規にセットされるときだけカウントアップ
            ng_transparent_pressed_count++;
            *slot |= mask;
        }
    }
}

static inline void ng_clear_transparent_press(uint16_t position) {
    if (position < NG_MAX_POSITIONS) {
        uint32_t mask = (1u << (position % 32));
        uint32_t *slot = &ng_transparent_pressed_bitmap[position / 32];
        if ((*slot & mask) != 0) {
            // 実際にクリアされるときだけカウントダウン
            ng_transparent_pressed_count--;
            *slot &= ~mask;
        }
    }
}

static inline bool ng_was_transparent_press(uint16_t position) {
    if (position < NG_MAX_POSITIONS) {
        return (ng_transparent_pressed_bitmap[position / 32] & (1u << (position % 32))) != 0;
    }
    return false;
}

static inline void ng_mark_chord_tap_press(uint16_t position) {
    if (position < NG_MAX_POSITIONS) {
        ng_chord_tap_pressed_bitmap[position / 32] |= (1u << (position % 32));
    }
}

static inline void ng_clear_chord_tap_press(uint16_t position) {
    if (position < NG_MAX_POSITIONS) {
        ng_chord_tap_pressed_bitmap[position / 32] &= ~(1u << (position % 32));
    }
}

static inline bool ng_was_chord_tap_press(uint16_t position) {
    if (position < NG_MAX_POSITIONS) {
        return (ng_chord_tap_pressed_bitmap[position / 32] & (1u << (position % 32))) != 0;
    }
    return false;
}

// &ng が処理対象とするキーかどうか（アルファ/句読点/スペース/エンター）
static inline bool is_ng_handled_keycode(uint32_t kc) {
    switch (kc) {
    case A ... Z:
    case SPACE:
    case ENTER:
    case DOT:
    case COMMA:
    case SLASH:
    case MINUS:
    case HOME:
    case END:
        return true;
    default:
        return false;
    }
}

static const uint32_t ng_key[] = {
    [A - A] = B_A,     [B - A] = B_B,         [C - A] = B_C,         [D - A] = B_D,
    [E - A] = B_E,     [F - A] = B_F,         [G - A] = B_G,         [H - A] = B_H,
    [I - A] = B_I,     [J - A] = B_J,         [K - A] = B_K,         [L - A] = B_L,
    [M - A] = B_M,     [N - A] = B_N,         [O - A] = B_O,         [P - A] = B_P,
    [Q - A] = B_Q,     [R - A] = B_R,         [S - A] = B_S,         [T - A] = B_T,
    [U - A] = B_U,     [V - A] = B_V,         [W - A] = B_W,         [X - A] = B_X,
    [Y - A] = B_Y,     [Z - A] = B_Z,         [MINUS - A] = B_MINUS, [COMMA - A] = B_COMMA,
    [DOT - A] = B_DOT, [SLASH - A] = B_SLASH, [SPACE - A] = B_SPACE, [ENTER - A] = B_SPACE,
};

// カナ変換テーブル
typedef struct {
    uint32_t shift;
    uint32_t douji;
    uint32_t kana[6];
    void (*func)(void);
} naginata_kanamap;

static naginata_kanamap ngdickana[] = {
    // 清音
    {.shift = NONE    , .douji = B_J            , .kana = {A, NONE, NONE, NONE, NONE, NONE}, .func = nofunc }, // あ
    {.shift = NONE    , .douji = B_K            , .kana = {I, NONE, NONE, NONE, NONE, NONE}, .func = nofunc }, // い
    {.shift = NONE    , .douji = B_L            , .kana = {U, NONE, NONE, NONE, NONE, NONE}, .func = nofunc }, // う
    {.shift = B_SPACE , .douji = B_O            , .kana = {E, NONE, NONE, NONE, NONE, NONE}, .func = nofunc }, // え
    {.shift = B_SPACE , .douji = B_N            , .kana = {O, NONE, NONE, NONE, NONE, NONE}, .func = nofunc }, // お
    {.shift = NONE    , .douji = B_F            , .kana = {K, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // か
    {.shift = NONE    , .douji = B_W            , .kana = {K, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // き
    {.shift = NONE    , .douji = B_H            , .kana = {K, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // く
    {.shift = NONE    , .douji = B_S            , .kana = {K, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // け
    {.shift = NONE    , .douji = B_V            , .kana = {K, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // こ
    {.shift = B_SPACE , .douji = B_U            , .kana = {S, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // さ 誤爆するので裏面も復活
    {.shift = NONE    , .douji = B_U            , .kana = {S, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // さ Cornixは親指にDelを割り当てているため、B_SPACEを外す ⇒ 誤爆するので裏面も復活
    {.shift = NONE    , .douji = B_R            , .kana = {S, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // し
    {.shift = NONE    , .douji = B_O            , .kana = {S, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // す
    {.shift = B_SPACE , .douji = B_A            , .kana = {S, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // せ
    {.shift = NONE    , .douji = B_B            , .kana = {S, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // そ
    {.shift = NONE    , .douji = B_N            , .kana = {T, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // た
    {.shift = B_SPACE , .douji = B_G            , .kana = {T, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ち
    {.shift = B_SPACE , .douji = B_L            , .kana = {T, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // つ
    {.shift = NONE    , .douji = B_E            , .kana = {T, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // て
    {.shift = NONE    , .douji = B_D            , .kana = {T, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // と
    {.shift = NONE    , .douji = B_M            , .kana = {N, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // な
    {.shift = B_SPACE , .douji = B_D            , .kana = {N, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // に
    {.shift = B_SPACE , .douji = B_W            , .kana = {N, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぬ
    {.shift = B_SPACE , .douji = B_COMMA        , .kana = {N, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ね
    {.shift = B_SPACE , .douji = B_J            , .kana = {N, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // の
    {.shift = NONE    , .douji = B_C            , .kana = {H, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // は
    {.shift = NONE    , .douji = B_X            , .kana = {H, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ひ
    {.shift = B_SPACE , .douji = B_X            , .kana = {H, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ひ
    {.shift = B_SPACE , .douji = B_MINUS        , .kana = {H, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ふ
    {.shift = NONE    , .douji = B_P            , .kana = {H, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // へ
    {.shift = NONE    , .douji = B_Z            , .kana = {H, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ほ
    {.shift = B_SPACE , .douji = B_Z            , .kana = {H, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ほ
    {.shift = B_SPACE , .douji = B_F            , .kana = {M, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ま
    {.shift = B_SPACE , .douji = B_S            , .kana = {M, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // み
    {.shift = B_SPACE , .douji = B_B            , .kana = {M, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // む
    {.shift = B_SPACE , .douji = B_R            , .kana = {M, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // め
    {.shift = B_SPACE , .douji = B_K            , .kana = {M, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // も
    {.shift = B_SPACE , .douji = B_H            , .kana = {Y, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // や
    {.shift = B_SPACE , .douji = B_P            , .kana = {Y, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ゆ
    {.shift = B_SPACE , .douji = B_I            , .kana = {Y, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // よ
    {.shift = NONE    , .douji = B_DOT          , .kana = {R, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ら
    {.shift = B_SPACE , .douji = B_E            , .kana = {R, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // り
    {.shift = NONE    , .douji = B_I            , .kana = {R, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // る
    {.shift = NONE    , .douji = B_SLASH        , .kana = {R, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // れ
    {.shift = B_SPACE , .douji = B_SLASH        , .kana = {R, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // れ
    {.shift = NONE    , .douji = B_A            , .kana = {R, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ろ
    {.shift = B_SPACE , .douji = B_DOT          , .kana = {W, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // わ
    {.shift = B_SPACE , .douji = B_C            , .kana = {W, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // を
    {.shift = NONE    , .douji = B_COMMA        , .kana = {N, N, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ん
    {.shift = NONE    , .douji = B_MINUS        , .kana = {MINUS, NONE, NONE, NONE, NONE, NONE}, .func = nofunc }, // ー

    // 濁音
    {.shift = NONE    , .douji = B_J|B_F        , .kana = {G, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // が
    {.shift = NONE    , .douji = B_J|B_W        , .kana = {G, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぎ
    {.shift = 0UL     , .douji = B_F|B_H        , .kana = {G, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぐ
    {.shift = NONE    , .douji = B_J|B_S        , .kana = {G, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // げ
    {.shift = NONE    , .douji = B_J|B_V        , .kana = {G, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ご
    {.shift = 0UL     , .douji = B_F|B_U        , .kana = {Z, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ざ
    {.shift = NONE    , .douji = B_J|B_R        , .kana = {Z, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // じ
    {.shift = 0UL     , .douji = B_F|B_O        , .kana = {Z, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ず
    {.shift = NONE    , .douji = B_J|B_A        , .kana = {Z, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぜ
    {.shift = NONE    , .douji = B_J|B_B        , .kana = {Z, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぞ
    {.shift = 0UL     , .douji = B_F|B_N        , .kana = {D, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // だ
    {.shift = NONE    , .douji = B_J|B_G        , .kana = {D, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぢ
    {.shift = 0UL     , .douji = B_F|B_L        , .kana = {D, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // づ
    {.shift = NONE    , .douji = B_J|B_E        , .kana = {D, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // で
    {.shift = NONE    , .douji = B_J|B_D        , .kana = {D, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ど
    {.shift = NONE    , .douji = B_J|B_C        , .kana = {B, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ば
    {.shift = NONE    , .douji = B_J|B_X        , .kana = {B, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // び
    {.shift = 0UL     , .douji = B_F|B_MINUS    , .kana = {B, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぶ
    {.shift = 0UL     , .douji = B_F|B_P        , .kana = {B, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // べ
    {.shift = NONE    , .douji = B_J|B_Z        , .kana = {B, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぼ
    {.shift = 0UL     , .douji = B_F|B_L|B_MINUS, .kana = {V, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ゔ

    // 半濁音
    {.shift = NONE    , .douji = B_M|B_C        , .kana = {P, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぱ
    {.shift = NONE    , .douji = B_M|B_X        , .kana = {P, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぴ
    {.shift = NONE    , .douji = B_V|B_MINUS    , .kana = {P, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぷ
    {.shift = NONE    , .douji = B_V|B_P        , .kana = {P, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぺ
    {.shift = NONE    , .douji = B_M|B_Z        , .kana = {P, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぽ

    // 小書き
    {.shift = NONE    , .douji = B_Q|B_H        , .kana = {X, Y, A, NONE, NONE, NONE      }, .func = nofunc }, // ゃ
    {.shift = NONE    , .douji = B_Q|B_P        , .kana = {X, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // ゅ
    {.shift = NONE    , .douji = B_Q|B_I        , .kana = {X, Y, O, NONE, NONE, NONE      }, .func = nofunc }, // ょ
    {.shift = NONE    , .douji = B_Q|B_J        , .kana = {X, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぁ
    {.shift = NONE    , .douji = B_Q|B_K        , .kana = {X, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぃ
    {.shift = NONE    , .douji = B_Q|B_L        , .kana = {X, U, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぅ
    {.shift = NONE    , .douji = B_Q|B_O        , .kana = {X, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぇ
    {.shift = NONE    , .douji = B_Q|B_N        , .kana = {X, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ぉ
    {.shift = NONE    , .douji = B_Q|B_DOT      , .kana = {X, W, A, NONE, NONE, NONE      }, .func = nofunc }, // ゎ
    {.shift = NONE    , .douji = B_G            , .kana = {X, T, U, NONE, NONE, NONE      }, .func = nofunc }, // っ
    {.shift = NONE    , .douji = B_Q|B_S        , .kana = {X, K, E, NONE, NONE, NONE      }, .func = nofunc }, // ヶ
    {.shift = NONE    , .douji = B_Q|B_F        , .kana = {X, K, A, NONE, NONE, NONE      }, .func = nofunc }, // ヵ

    // 清音拗音 濁音拗音 半濁拗音
    {.shift = NONE    , .douji = B_R|B_H        , .kana = {S, Y, A, NONE, NONE, NONE      }, .func = nofunc }, // しゃ
    {.shift = NONE    , .douji = B_R|B_P        , .kana = {S, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // しゅ
    {.shift = NONE    , .douji = B_R|B_I        , .kana = {S, Y, O, NONE, NONE, NONE      }, .func = nofunc }, // しょ
    {.shift = NONE    , .douji = B_J|B_R|B_H    , .kana = {Z, Y, A, NONE, NONE, NONE      }, .func = nofunc }, // じゃ
    {.shift = NONE    , .douji = B_J|B_R|B_P    , .kana = {Z, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // じゅ
    {.shift = NONE    , .douji = B_J|B_R|B_I    , .kana = {Z, Y, O, NONE, NONE, NONE      }, .func = nofunc }, // じょ
    {.shift = NONE    , .douji = B_W|B_H        , .kana = {K, Y, A, NONE, NONE, NONE      }, .func = nofunc }, // きゃ
    {.shift = NONE    , .douji = B_W|B_P        , .kana = {K, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // きゅ
    {.shift = NONE    , .douji = B_W|B_I        , .kana = {K, Y, O, NONE, NONE, NONE      }, .func = nofunc }, // きょ
    {.shift = NONE    , .douji = B_J|B_W|B_H    , .kana = {G, Y, A, NONE, NONE, NONE      }, .func = nofunc }, // ぎゃ
    {.shift = NONE    , .douji = B_J|B_W|B_P    , .kana = {G, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // ぎゅ
    {.shift = NONE    , .douji = B_J|B_W|B_I    , .kana = {G, Y, O, NONE, NONE, NONE      }, .func = nofunc }, // ぎょ
    {.shift = NONE    , .douji = B_G|B_H        , .kana = {T, Y, A, NONE, NONE, NONE      }, .func = nofunc }, // ちゃ
    {.shift = NONE    , .douji = B_G|B_P        , .kana = {T, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // ちゅ
    {.shift = NONE    , .douji = B_G|B_I        , .kana = {T, Y, O, NONE, NONE, NONE      }, .func = nofunc }, // ちょ
    {.shift = NONE    , .douji = B_J|B_G|B_H    , .kana = {D, Y, A, NONE, NONE, NONE      }, .func = nofunc }, // ぢゃ
    {.shift = NONE    , .douji = B_J|B_G|B_P    , .kana = {D, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // ぢゅ
    {.shift = NONE    , .douji = B_J|B_G|B_I    , .kana = {D, Y, O, NONE, NONE, NONE      }, .func = nofunc }, // ぢょ
    {.shift = NONE    , .douji = B_D|B_H        , .kana = {N, Y, A, NONE, NONE, NONE      }, .func = nofunc }, // にゃ
    {.shift = NONE    , .douji = B_D|B_P        , .kana = {N, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // にゅ
    {.shift = NONE    , .douji = B_D|B_I        , .kana = {N, Y, O, NONE, NONE, NONE      }, .func = nofunc }, // にょ
    {.shift = NONE    , .douji = B_X|B_H        , .kana = {H, Y, A, NONE, NONE, NONE      }, .func = nofunc }, // ひゃ
    {.shift = NONE    , .douji = B_X|B_P        , .kana = {H, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // ひゅ
    {.shift = NONE    , .douji = B_X|B_I        , .kana = {H, Y, O, NONE, NONE, NONE      }, .func = nofunc }, // ひょ
    {.shift = NONE    , .douji = B_J|B_X|B_H    , .kana = {B, Y, A, NONE, NONE, NONE      }, .func = nofunc }, // びゃ
    {.shift = NONE    , .douji = B_J|B_X|B_P    , .kana = {B, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // びゅ
    {.shift = NONE    , .douji = B_J|B_X|B_I    , .kana = {B, Y, O, NONE, NONE, NONE      }, .func = nofunc }, // びょ
    {.shift = NONE    , .douji = B_M|B_X|B_H    , .kana = {P, Y, A, NONE, NONE, NONE      }, .func = nofunc }, // ぴゃ
    {.shift = NONE    , .douji = B_M|B_X|B_P    , .kana = {P, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // ぴゅ
    {.shift = NONE    , .douji = B_M|B_X|B_I    , .kana = {P, Y, O, NONE, NONE, NONE      }, .func = nofunc }, // ぴょ
    {.shift = NONE    , .douji = B_S|B_H        , .kana = {M, Y, A, NONE, NONE, NONE      }, .func = nofunc }, // みゃ
    {.shift = NONE    , .douji = B_S|B_P        , .kana = {M, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // みゅ
    {.shift = NONE    , .douji = B_S|B_I        , .kana = {M, Y, O, NONE, NONE, NONE      }, .func = nofunc }, // みょ
    {.shift = NONE    , .douji = B_E|B_H        , .kana = {R, Y, A, NONE, NONE, NONE      }, .func = nofunc }, // りゃ
    {.shift = NONE    , .douji = B_E|B_P        , .kana = {R, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // りゅ
    {.shift = NONE    , .douji = B_E|B_I        , .kana = {R, Y, O, NONE, NONE, NONE      }, .func = nofunc }, // りょ

    // 清音外来音 濁音外来音
    {.shift = NONE    , .douji = B_M|B_E|B_K    , .kana = {T, H, I, NONE, NONE, NONE      }, .func = nofunc }, // てぃ
    {.shift = NONE    , .douji = B_M|B_E|B_P    , .kana = {T, E, X, Y, U, NONE            }, .func = nofunc }, // てゅ
    {.shift = NONE    , .douji = B_J|B_E|B_K    , .kana = {D, H, I, NONE, NONE, NONE      }, .func = nofunc }, // でぃ
    {.shift = NONE    , .douji = B_J|B_E|B_P    , .kana = {D, H, U, NONE, NONE, NONE      }, .func = nofunc }, // でゅ
    {.shift = NONE    , .douji = B_M|B_D|B_L    , .kana = {T, O, X, U, NONE, NONE         }, .func = nofunc }, // とぅ
    {.shift = NONE    , .douji = B_J|B_D|B_L    , .kana = {D, O, X, U, NONE, NONE         }, .func = nofunc }, // どぅ
    {.shift = NONE    , .douji = B_M|B_R|B_O    , .kana = {S, Y, E, NONE, NONE, NONE      }, .func = nofunc }, // しぇ
    {.shift = NONE    , .douji = B_M|B_G|B_O    , .kana = {T, Y, E, NONE, NONE, NONE      }, .func = nofunc }, // ちぇ
    {.shift = NONE    , .douji = B_J|B_R|B_O    , .kana = {Z, Y, E, NONE, NONE, NONE      }, .func = nofunc }, // じぇ
    {.shift = NONE    , .douji = B_J|B_G|B_O    , .kana = {D, Y, E, NONE, NONE, NONE      }, .func = nofunc }, // ぢぇ
    {.shift = NONE    , .douji = B_V|B_MINUS|B_J, .kana = {F, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ふぁ
    {.shift = NONE    , .douji = B_V|B_MINUS|B_K, .kana = {F, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ふぃ
    {.shift = NONE    , .douji = B_V|B_MINUS|B_O, .kana = {F, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ふぇ
    {.shift = NONE    , .douji = B_V|B_MINUS|B_N, .kana = {F, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ふぉ
    {.shift = NONE    , .douji = B_V|B_MINUS|B_P, .kana = {F, Y, U, NONE, NONE, NONE      }, .func = nofunc }, // ふゅ
    {.shift = NONE    , .douji = B_V|B_K|B_O    , .kana = {I, X, E, NONE, NONE, NONE      }, .func = nofunc }, // いぇ
    {.shift = NONE    , .douji = B_V|B_L|B_K    , .kana = {W, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // うぃ
    {.shift = NONE    , .douji = B_V|B_L|B_O    , .kana = {W, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // うぇ
    {.shift = NONE    , .douji = B_V|B_L|B_N    , .kana = {U, X, O, NONE, NONE, NONE      }, .func = nofunc }, // うぉ
    {.shift = NONE    , .douji = B_F|B_L|B_J    , .kana = {V, A, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ゔぁ
    {.shift = NONE    , .douji = B_F|B_L|B_K    , .kana = {V, I, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ゔぃ
    {.shift = NONE    , .douji = B_F|B_L|B_O    , .kana = {V, E, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ゔぇ
    {.shift = NONE    , .douji = B_F|B_L|B_N    , .kana = {V, O, NONE, NONE, NONE, NONE   }, .func = nofunc }, // ゔぉ
    {.shift = NONE    , .douji = B_F|B_L|B_P    , .kana = {V, U, X, Y, U, NONE            }, .func = nofunc }, // ゔゅ
    {.shift = NONE    , .douji = B_V|B_H|B_J    , .kana = {K, U, X, A, NONE, NONE         }, .func = nofunc }, // くぁ
    {.shift = NONE    , .douji = B_V|B_H|B_K    , .kana = {K, U, X, I, NONE, NONE         }, .func = nofunc }, // くぃ
    {.shift = NONE    , .douji = B_V|B_H|B_O    , .kana = {K, U, X, E, NONE, NONE         }, .func = nofunc }, // くぇ
    {.shift = NONE    , .douji = B_V|B_H|B_N    , .kana = {K, U, X, O, NONE, NONE         }, .func = nofunc }, // くぉ
    {.shift = NONE    , .douji = B_V|B_H|B_DOT  , .kana = {K, U, X, W, A, NONE            }, .func = nofunc }, // くゎ
    {.shift = NONE    , .douji = B_F|B_H|B_J    , .kana = {G, U, X, A, NONE, NONE         }, .func = nofunc }, // ぐぁ
    {.shift = NONE    , .douji = B_F|B_H|B_K    , .kana = {G, U, X, I, NONE, NONE         }, .func = nofunc }, // ぐぃ
    {.shift = NONE    , .douji = B_F|B_H|B_O    , .kana = {G, U, X, E, NONE, NONE         }, .func = nofunc }, // ぐぇ
    {.shift = NONE    , .douji = B_F|B_H|B_N    , .kana = {G, U, X, O, NONE, NONE         }, .func = nofunc }, // ぐぉ
    {.shift = NONE    , .douji = B_F|B_H|B_DOT  , .kana = {G, U, X, W, A, NONE            }, .func = nofunc }, // ぐゎ
    {.shift = NONE    , .douji = B_V|B_L|B_J    , .kana = {T, S, A, NONE, NONE, NONE      }, .func = nofunc }, // つぁ
    
    // 追加
    {.shift = NONE    , .douji = B_SPACE        , .kana = {SPACE, NONE, NONE, NONE, NONE, NONE  }, .func = nofunc},
    {.shift = B_SPACE , .douji = B_V            , .kana = {COMMA, ENTER, NONE, NONE, NONE, NONE }, .func = nofunc},
    {.shift = NONE    , .douji = B_Q            , .kana = {NONE, NONE, NONE, NONE, NONE, NONE   }, .func = nofunc},
    {.shift = B_SPACE , .douji = B_M            , .kana = {DOT, ENTER, NONE, NONE, NONE, NONE   }, .func = nofunc},
//    {.shift = NONE    , .douji = B_U            , .kana = {BSPC, NONE, NONE, NONE, NONE, NONE   }, .func = nofunc},  // Cornixでは親指にBSPCを割り当てているため削除

    {.shift = NONE    , .douji = B_V|B_M        , .kana = {ENTER, NONE, NONE, NONE, NONE, NONE  }, .func = nofunc}, // enter

//    {.shift = NONE    , .douji = B_T            , .kana = {NONE, NONE, NONE, NONE, NONE, NONE   }, .func = ng_T}, // Cornixでは右下にカーソルキーが割り当てられているため「、」に変更
//    {.shift = NONE    , .douji = B_Y            , .kana = {NONE, NONE, NONE, NONE, NONE, NONE   }, .func = ng_Y}, // Cornixでは右下にカーソルキーが割り当てられているため「。」に変更
    {.shift = NONE    , .douji = B_T            , .kana = {COMMA, ENTER, NONE, NONE, NONE, NONE }, .func = nofunc}, // Cornixでは右下にカーソルキーが割り当てられているため「、」に変更
    {.shift = NONE    , .douji = B_Y            , .kana = {DOT, ENTER, NONE, NONE, NONE, NONE   }, .func = nofunc}, // Cornixでは右下にカーソルキーが割り当てられているため「。」に変更
//    {.shift = B_SPACE , .douji = B_T            , .kana = {NONE, NONE, NONE, NONE, NONE, NONE   }, .func = ng_ST}, // 自分は使わないので削除
//    {.shift = B_SPACE , .douji = B_Y            , .kana = {NONE, NONE, NONE, NONE, NONE, NONE   }, .func = ng_SY}, // 自分は使わないので削除
    {.shift = NONE    , .douji = B_H|B_J        , .kana = {NONE, NONE, NONE, NONE, NONE, NONE   }, .func = naginata_on}, // 　かなオン

    // 編集モード（省略せず元のまま）
    {.shift = B_J|B_K    , .douji = B_Q     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKQ    }, // ^{End}
    {.shift = B_J|B_K    , .douji = B_W     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKW    }, // ／{改行}
    {.shift = B_J|B_K    , .douji = B_E     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKE    }, // /*ディ*/
    {.shift = B_J|B_K    , .douji = B_R     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKR    }, // ^s
    {.shift = B_J|B_K    , .douji = B_T     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKT    }, // ・
    {.shift = B_J|B_K    , .douji = B_A     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKA    }, // ……{改行}
    {.shift = B_J|B_K    , .douji = B_S     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKS    }, // 『{改行}
    {.shift = B_J|B_K    , .douji = B_D     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKD    }, // ？{改行}
    {.shift = B_J|B_K    , .douji = B_F     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKF    }, // 「{改行}
    {.shift = B_J|B_K    , .douji = B_G     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKG    }, // ({改行}
    {.shift = B_J|B_K    , .douji = B_Z     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKZ    }, // ――{改行}
    {.shift = B_J|B_K    , .douji = B_X     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKX    }, // 』{改行}
    {.shift = B_J|B_K    , .douji = B_C     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKC    }, // ！{改行}
    {.shift = B_J|B_K    , .douji = B_V     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKV    }, // 」{改行}
    {.shift = B_J|B_K    , .douji = B_B     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_JKB    }, // ){改行}
    {.shift = B_D|B_F    , .douji = B_Y     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFY    }, // {Home}
    {.shift = B_D|B_F    , .douji = B_U     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFU    }, // +{End}{BS}
    {.shift = B_D|B_F    , .douji = B_I     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFI    }, // {vk1Csc079}
    {.shift = B_D|B_F    , .douji = B_O     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFO    }, // {Del}
    {.shift = B_D|B_F    , .douji = B_P     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFP    }, // +{Esc 2}
    {.shift = B_D|B_F    , .douji = B_H     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFH    }, // {Enter}{End}
    {.shift = B_D|B_F    , .douji = B_J     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFJ    }, // {↑}
    {.shift = B_D|B_F    , .douji = B_K     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFK    }, // +{↑}
    {.shift = B_D|B_F    , .douji = B_L     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFL    }, // +{↑ 7}
    {.shift = B_D|B_F    , .douji = B_MINUS , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFSCLN }, // ^i
    {.shift = B_D|B_F    , .douji = B_N     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFN    }, // {End}
    {.shift = B_D|B_F    , .douji = B_M     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFM    }, // {↓}
    {.shift = B_D|B_F    , .douji = B_COMMA , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFCOMM }, // +{↓}
    {.shift = B_D|B_F    , .douji = B_DOT   , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFDOT  }, // +{↓ 7}
    {.shift = B_D|B_F    , .douji = B_SLASH , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_DFSLSH }, // ^u
    {.shift = B_M|B_COMMA, .douji = B_Q     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCQ    }, // ｜{改行}
    {.shift = B_M|B_COMMA, .douji = B_W     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCW    }, // 　　　×　　　×　　　×{改行 2}
    {.shift = B_M|B_COMMA, .douji = B_E     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCE    }, // {Home}{→}{End}{Del 2}{←}
    {.shift = B_M|B_COMMA, .douji = B_R     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCR    }, // {Home}{改行}{Space 1}{←}
    {.shift = B_M|B_COMMA, .douji = B_T     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCT    }, // 〇{改行}
    {.shift = B_M|B_COMMA, .douji = B_A     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCA    }, // 《{改行}
    {.shift = B_M|B_COMMA, .douji = B_S     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCS    }, // 【{改行}
    {.shift = B_M|B_COMMA, .douji = B_D     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCD    }, // {Home}{→}{End}{Del 4}{←}
    {.shift = B_M|B_COMMA, .douji = B_F     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCF    }, // {Home}{改行}{Space 3}{←}
    {.shift = B_M|B_COMMA, .douji = B_G     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCG    }, // {Space 3}
    {.shift = B_M|B_COMMA, .douji = B_Z     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCZ    }, // 》{改行}
    {.shift = B_M|B_COMMA, .douji = B_X     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCX    }, // 】{改行}
    {.shift = B_M|B_COMMA, .douji = B_C     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCC    }, // 」{改行}{改行}
    {.shift = B_M|B_COMMA, .douji = B_V     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCV    }, // 」{改行}{改行}「{改行}
    {.shift = B_M|B_COMMA, .douji = B_B     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_MCB    }, // 」{改行}{改行}{Space}
    {.shift = B_C|B_V    , .douji = B_Y     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVY    }, // +{Home}
    {.shift = B_C|B_V    , .douji = B_U     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVU    }, // ^x
    {.shift = B_C|B_V    , .douji = B_I     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVI    }, // {vk1Csc079}
    {.shift = B_C|B_V    , .douji = B_O     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVO    }, // ^v
    {.shift = B_C|B_V    , .douji = B_P     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVP    }, // ^z
    {.shift = B_C|B_V    , .douji = B_H     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVH    }, // ^c
    {.shift = B_C|B_V    , .douji = B_J     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVJ    }, // {←}
    {.shift = B_C|B_V    , .douji = B_K     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVK    }, // {→}
    {.shift = B_C|B_V    , .douji = B_L     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVL    }, // {改行}{Space}+{Home}^x{BS}
    {.shift = B_C|B_V    , .douji = B_MINUS , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVSCLN }, // ^y
    {.shift = B_C|B_V    , .douji = B_N     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVN    }, // +{End}
    {.shift = B_C|B_V    , .douji = B_M     , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVM    }, // +{←}
    {.shift = B_C|B_V    , .douji = B_COMMA , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVCOMM }, // +{→}
    {.shift = B_C|B_V    , .douji = B_DOT   , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVDOT  }, // +{← 7}
    {.shift = B_C|B_V    , .douji = B_SLASH , .kana = {NONE, NONE, NONE, NONE, NONE, NONE} , .func = ngh_CVSLSH }, // +{→ 7}

};

// Helper function for counting matches/candidates
static int count_kana_entries(NGList *keys, bool exact_match) {
  if (keys->size == 0) return 0;

  int count = 0;
  uint32_t keyset0 = 0UL, keyset1 = 0UL, keyset2 = 0UL;
  
  // keysetを配列にしたらバイナリサイズが増えた
  switch (keys->size) {
    case 1:
      keyset0 = ng_key[keys->elements[0] - A];
      break;
    case 2:
      keyset0 = ng_key[keys->elements[0] - A];
      keyset1 = ng_key[keys->elements[1] - A];
      break;
    default:
      keyset0 = ng_key[keys->elements[0] - A];
      keyset1 = ng_key[keys->elements[1] - A];
      keyset2 = ng_key[keys->elements[2] - A];
      break;
  }

  for (int i = 0; i < sizeof ngdickana / sizeof ngdickana[i]; i++) {
    bool matches = false;

    switch (keys->size) {
      case 1:
        if (exact_match) {
          matches = (ngdickana[i].shift == keyset0) || 
                   (ngdickana[i].shift == 0UL && ngdickana[i].douji == keyset0);
        } else {
          matches = ((ngdickana[i].shift & keyset0) == keyset0) ||
                   (ngdickana[i].shift == 0UL && (ngdickana[i].douji & keyset0) == keyset0);
        }
        break;
      case 2:
        if (exact_match) {
          matches = (ngdickana[i].shift == (keyset0 | keyset1)) ||
                   (ngdickana[i].shift == keyset0 && ngdickana[i].douji == keyset1) ||
                   (ngdickana[i].shift == 0UL && ngdickana[i].douji == (keyset0 | keyset1));
        } else {
          matches = (ngdickana[i].shift == (keyset0 | keyset1)) ||
                   (ngdickana[i].shift == keyset0 && (ngdickana[i].douji & keyset1) == keyset1) ||
                   (ngdickana[i].shift == 0UL && (ngdickana[i].douji & (keyset0 | keyset1)) == (keyset0 | keyset1));
          // しぇ、ちぇ、など2キーで確定してはいけない
          if (matches && (ngdickana[i].shift | ngdickana[i].douji) != (keyset0 | keyset1)) {
            count = 2;
          }
        }
        break;
      default:
        if (exact_match) {
          matches = (ngdickana[i].shift == (keyset0 | keyset1) && ngdickana[i].douji == keyset2) ||
                   (ngdickana[i].shift == keyset0 && ngdickana[i].douji == (keyset1 | keyset2)) ||
                   (ngdickana[i].shift == 0UL && ngdickana[i].douji == (keyset0 | keyset1 | keyset2));
        } else {
          matches = (ngdickana[i].shift == (keyset0 | keyset1) && (ngdickana[i].douji & keyset2) == keyset2) ||
                   (ngdickana[i].shift == keyset0 && (ngdickana[i].douji & (keyset1 | keyset2)) == (keyset1 | keyset2)) ||
                   (ngdickana[i].shift == 0UL && (ngdickana[i].douji & (keyset0 | keyset1 | keyset2)) == (keyset0 | keyset1 | keyset2));
        }
        break;
    }

    if (matches) {
      count++;
      if (count > 1) break;
    }
  }

  return count;
}

int number_of_matches(NGList *keys) {  
  int result = count_kana_entries(keys, true);
  return result;
}

int number_of_candidates(NGList *keys) {
  int result = count_kana_entries(keys, false);
  return result;
}

// キー入力を文字に変換して出力する
void ng_type(NGList *keys) {
    LOG_DBG(">NAGINATA NG_TYPE");

    if (keys->size == 0)
        return;

    if (keys->size == 1 && keys->elements[0] == ENTER) {
        LOG_DBG(" NAGINATA type keycode 0x%02X", ENTER);
        raise_zmk_keycode_state_changed_from_encoded(ENTER, true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(ENTER, false, timestamp);
        return;
    }

    uint32_t keyset = 0UL;
    for (int i = 0; i < keys->size; i++) {
        keyset |= ng_key[keys->elements[i] - A];
    }

    for (int i = 0; i < sizeof ngdickana / sizeof ngdickana[0]; i++) {
        if ((ngdickana[i].shift | ngdickana[i].douji) == keyset) {
            if (ngdickana[i].kana[0] != NONE) {
                for (int k = 0; k < 6; k++) {
                    if (ngdickana[i].kana[k] == NONE)
                        break;
                    LOG_DBG(" NAGINATA type keycode 0x%02X", ngdickana[i].kana[k]);
                    raise_zmk_keycode_state_changed_from_encoded(ngdickana[i].kana[k], true,
                                                                 timestamp);
                    raise_zmk_keycode_state_changed_from_encoded(ngdickana[i].kana[k], false,
                                                                 timestamp);
                }
            } else {
                ngdickana[i].func();
            }
            return;
        }
    }

    // JIみたいにJIを含む同時押しはたくさんあるが、JIのみの同時押しがないとき
    // 最後の１キーを別に分けて変換する
    // NGList a, b;
    // initializeList(&a);
    // initializeList(&b);
    // for (int i = 0; i < keys->size - 1; i++) {
    //     addToList(&a, keys->elements[i]);
    // }
    // addToList(&b, keys->elements[keys->size - 1]);
    // ng_type(&a);
    // ng_type(&b);

    LOG_DBG("<NAGINATA NG_TYPE");
}

// 薙刀式の入力処理
bool naginata_press(struct zmk_behavior_binding *binding, struct zmk_behavior_binding_event event) {
    LOG_DBG(">NAGINATA PRESS");

    // モディファイ/レイヤーホールド中は無効化
    if (!naginata_layer_active) {
        LOG_DBG("<NAGINATA PRESS (inactive)");
        return true;
    }

    uint32_t keycode = binding->param1;

    switch (keycode) {
    case A ... Z:
    case SPACE:
    case ENTER:
    case DOT:
    case COMMA:
    case SLASH:
    case MINUS:
        n_pressed_keys++;
        pressed_keys |= ng_key[keycode - A]; // キーの重ね合わせ

        if (keycode == SPACE || keycode == ENTER) {
            NGList a;
            initializeList(&a);
            addToList(&a, keycode);
            addToListArray(&nginput, &a);
        } else {
            NGList a;
            NGList b;
            if (nginput.size > 0) {
                copyList(&(nginput.elements[nginput.size - 1]), &a);
                copyList(&a, &b);
                addToList(&b, keycode);
            }

            // 前のキーとの同時押しの可能性があるなら前に足す
            // 同じキー連打を除外
            if (nginput.size > 0 && a.elements[a.size - 1] != keycode &&
                number_of_candidates(&b) > 0) {
                removeFromListArrayAt(&nginput, nginput.size - 1);
                addToListArray(&nginput, &b);
            } else {
                // 連続シフトではない
                NGList e;
                initializeList(&e);
                addToList(&e, keycode);
                addToListArray(&nginput, &e);
            }
        }

        // 連続シフト
        static uint32_t rs[10][2] = {{D, F},     {C, V}, {J, K}, {M, COMMA}, {SPACE, 0},
                                     {ENTER, 0}, {F, 0}, {V, 0}, {J, 0},     {M, 0}};

        uint32_t keyset = 0UL;
        for (int i = 0; i < nginput.elements[0].size; i++) {
            keyset |= ng_key[nginput.elements[0].elements[i] - A];
        }
        for (int i = 0; i < 10; i++) {
            NGList rskc;
            initializeList(&rskc);
            addToList(&rskc, rs[i][0]);
            if (rs[i][1] > 0) {
                addToList(&rskc, rs[i][1]);
            }

            int c = includeList(&rskc, keycode);
            uint32_t brs = 0UL;
            for (int j = 0; j < rskc.size; j++) {
                brs |= ng_key[rskc.elements[j] - A];
            }

            NGList l = nginput.elements[nginput.size - 1];
            for (int j = 0; j < l.size; j++) {
                addToList(&rskc, l.elements[j]);
            }

            if (c < 0 && ((brs & pressed_keys) == brs) && (keyset & brs) != brs && number_of_matches(&rskc) > 0) {
                nginput.elements[nginput.size - 1] = rskc;
                break;
            }
        }

        if (nginput.size > 1 || number_of_candidates(&(nginput.elements[0])) == 1) {
            ng_type(&(nginput.elements[0]));
            removeFromListArrayAt(&nginput, 0);
        }
        break;
    }

    LOG_DBG("<NAGINATA PRESS");

    return true;
}

bool naginata_release(struct zmk_behavior_binding *binding,
                      struct zmk_behavior_binding_event event) {
    LOG_DBG(">NAGINATA RELEASE");

    uint32_t keycode = binding->param1;


    switch (keycode) {
    case A ... Z:
    case SPACE:
    case ENTER:
    case DOT:
    case COMMA:
    case SLASH:
    case MINUS:
        if (n_pressed_keys > 0)
            n_pressed_keys--;
        if (n_pressed_keys == 0)
            pressed_keys = 0UL;

        pressed_keys &= ~ng_key[keycode - A]; // キーの重ね合わせ

        if (pressed_keys == 0UL) {
            while (nginput.size > 0) {
                ng_type(&(nginput.elements[0]));
                removeFromListArrayAt(&nginput, 0);
            }
        } else {
            if (nginput.size > 0 && number_of_candidates(&(nginput.elements[0])) == 1) {
                ng_type(&(nginput.elements[0]));
                removeFromListArrayAt(&nginput, 0);
            }
        }
        break;
    }

    LOG_DBG("<NAGINATA RELEASE");

    return true;
}

// 有効/無効の切替ヘルパ
static void naginata_deactivate_if_needed(void) {
    if (naginata_layer_active) {
        naginata_layer_active = false;
        // 未確定の入力をクリア
        nginput.size = 0;
        pressed_keys = 0UL;
        n_pressed_keys = 0;
        LOG_INF("Naginata deactivated (mods=%d, layer_holds=%d)", n_hid_modifiers, n_layer_hold_keys);
    }
}

static void naginata_try_activate(void) {
    if (!naginata_layer_active && n_hid_modifiers == 0 && n_layer_hold_keys == 0) {
        naginata_layer_active = true;
        LOG_INF("Naginata activated (mods=%d, layer_holds=%d)", n_hid_modifiers, n_layer_hold_keys);
    }
}

// HID修飾キーの検出: keycode_state_changed を購読
static int naginata_keycode_state_changed_listener(const zmk_event_t *eh) {
    const struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);
    if (!ev) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    uint32_t kc = ev->keycode;
    if (kc >= 0xE0 && kc <= 0xE7) {
        if (ev->state) {
            n_hid_modifiers++;
            naginata_deactivate_if_needed();
        } else {
            if (n_hid_modifiers > 0) {
                n_hid_modifiers--;
            }
            naginata_try_activate();
        }
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(behavior_naginata_keycode, naginata_keycode_state_changed_listener);
ZMK_SUBSCRIPTION(behavior_naginata_keycode, zmk_keycode_state_changed);

// レイヤーキー（&mo）のホールド検出
static inline bool is_layer_hold_behavior_ptr(const void *dev_ptr) {
    return dev_ptr && (dev_ptr == mo_bhv_dev || dev_ptr == lt_bhv_dev);
}

// レイヤーキー（&mo）のホールド検出は、対象レイヤーのアクティブ状態で判断する

static const struct zmk_behavior_binding *
get_effective_binding_for_position(uint32_t position) {
    // 上位レイヤーからデフォルトまで走査
    for (int layer_idx = ZMK_KEYMAP_LAYERS_LEN - 1; layer_idx >= 0; layer_idx--) {
        zmk_keymap_layer_id_t layer_id = zmk_keymap_layer_index_to_id((zmk_keymap_layer_index_t)layer_idx);
        if (layer_id == ZMK_KEYMAP_LAYER_ID_INVAL) {
            continue;
        }
        if (!zmk_keymap_layer_active(layer_id)) {
            continue;
        }
        const struct zmk_behavior_binding *binding =
            zmk_keymap_get_layer_binding_at_idx(layer_id, (uint8_t)position);
        if (!binding || !binding->behavior_dev) {
            continue;
        }
        // 型の不一致警告を避けるために void* にキャストして比較
        if (trans_bhv_dev && (const void *)binding->behavior_dev == trans_bhv_dev) {
            // 透過はスキップ
            continue;
        }
        return binding; // 最初に見つかった非透過が実効バインディング
    }
    return NULL;
}

static int naginata_position_state_changed_listener(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *ev = as_zmk_position_state_changed(eh);
    if (!ev) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    // 透過/レイヤーホールド系ビヘイビアのデバイスを取得（初回だけ）
    if (!trans_bhv_dev) {
        // 戻り値は実体としては struct device* だが、上で void* として保持
        trans_bhv_dev = (const void *)zmk_behavior_get_binding("TRANS");
        if (!trans_bhv_dev) {
            trans_bhv_dev = (const void *)zmk_behavior_get_binding("trans");
        }
    }
    if (!mo_bhv_dev) {
        mo_bhv_dev = (const void *)zmk_behavior_get_binding("MO");
        if (!mo_bhv_dev) {
            mo_bhv_dev = (const void *)zmk_behavior_get_binding("mo");
        }
    }
    if (!lt_bhv_dev) {
        lt_bhv_dev = (const void *)zmk_behavior_get_binding("LT");
        if (!lt_bhv_dev) {
            lt_bhv_dev = (const void *)zmk_behavior_get_binding("lt");
        }
    }
    // LOWER/RAISE レイヤのアクティブ状態でホールド中かを判定
    const struct zmk_behavior_binding *binding = get_effective_binding_for_position(ev->position);
    if (binding && binding->behavior_dev && is_layer_hold_behavior_ptr((const void *)binding->behavior_dev)) {
        if (ev->state) {
            n_layer_hold_keys = 1;
            naginata_deactivate_if_needed();
            LOG_DBG("Layer hold press (by binding) detected at pos %u", ev->position);
        } else {
            n_layer_hold_keys = 0;
            naginata_try_activate();
            LOG_DBG("Layer hold release (by binding) detected at pos %u", ev->position);
        }
    }

    // 次に、LOWER/RAISE レイヤのアクティブ状態でホールド中かを判定（フォールバック）
    zmk_keymap_layer_id_t lower_id = zmk_keymap_layer_index_to_id((zmk_keymap_layer_index_t)NG_LOWER_LAYER_INDEX);
    zmk_keymap_layer_id_t raise_id = zmk_keymap_layer_index_to_id((zmk_keymap_layer_index_t)NG_RAISE_LAYER_INDEX);
    bool lower_active = lower_id != ZMK_KEYMAP_LAYER_ID_INVAL ? zmk_keymap_layer_active(lower_id) : false;
    bool raise_active = raise_id != ZMK_KEYMAP_LAYER_ID_INVAL ? zmk_keymap_layer_active(raise_id) : false;
    bool hold_now_active = lower_active || raise_active;

    if (hold_now_active != ng_layer_hold_active) {
        ng_layer_hold_active = hold_now_active;
        if (ng_layer_hold_active) {
            n_layer_hold_keys = 1; // 0/1 のフラグとして利用
            naginata_deactivate_if_needed();
            LOG_DBG("Layer hold active (LOWER=%d, RAISE=%d)", lower_active, raise_active);
        } else {
            n_layer_hold_keys = 0;
            naginata_try_activate();
            LOG_DBG("Layer hold inactive");
        }
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(behavior_naginata_position, naginata_position_state_changed_listener);
ZMK_SUBSCRIPTION(behavior_naginata_position, zmk_position_state_changed);

// 薙刀式

static int behavior_naginata_init(const struct device *dev) {
    LOG_DBG("NAGINATA INIT");

    initializeListArray(&nginput);
    pressed_keys = 0UL;
    n_pressed_keys = 0;

    naginata_layer_active = true;
    n_hid_modifiers = 0;
    n_layer_hold_keys = 0;
    ng_layer_hold_active = false;

    if (!trans_bhv_dev) {
        trans_bhv_dev = (const void *)zmk_behavior_get_binding("TRANS");
        if (!trans_bhv_dev) {
            trans_bhv_dev = (const void *)zmk_behavior_get_binding("trans");
        }
    }

    // NVSから設定を読み込み（settings_load()はZMK起動時に自動で呼ばれ、
    // naginata_configは既に読み込み済みの状態になっている）
    naginata_config_init();

    // 透明押下記録をクリア
    for (int i = 0; i < (NG_MAX_POSITIONS + 31) / 32; i++) {
        ng_transparent_pressed_bitmap[i] = 0u;
    }
    // 合成タップ記録をクリア
    for (int i = 0; i < (NG_MAX_POSITIONS + 31) / 32; i++) {
        ng_chord_tap_pressed_bitmap[i] = 0u;
    }

    return 0;
};

static int on_keymap_binding_pressed(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    LOG_DBG("position %d keycode 0x%02X", event.position, binding->param1);

    // F15..F19 設定切り替え（設定変更後はNVSに保存）
    switch (binding->param1) {
        case F15:
            naginata_config.os = NG_WINDOWS;
            naginata_config_save();
            return ZMK_BEHAVIOR_OPAQUE;
        case F16:
            naginata_config.os = NG_MACOS;
            naginata_config_save();
            return ZMK_BEHAVIOR_OPAQUE;
        case F17:
            naginata_config.os = NG_LINUX;
            naginata_config_save();
            return ZMK_BEHAVIOR_OPAQUE;
        case F18:
            naginata_config.tategaki = true;
            naginata_config_save();
            return ZMK_BEHAVIOR_OPAQUE;
        case F19:
            naginata_config.tategaki = false;
            naginata_config_save();
            return ZMK_BEHAVIOR_OPAQUE;
    }

    timestamp = event.timestamp;

    // OS別: macOS のときは LCTRL と LGUI を入れ替える（&ng で直接送出）
    if (binding->param1 == LCTRL || binding->param1 == LGUI) {
        uint32_t out = binding->param1;
        if (naginata_config.os == NG_MACOS) {
            out = (binding->param1 == LCTRL) ? LGUI : LCTRL;
        }
        raise_zmk_keycode_state_changed_from_encoded(out, true, timestamp);
        return ZMK_BEHAVIOR_OPAQUE;
    }

    // HOME/END を OS 別に処理
    if (binding->param1 == HOME) {
        ng_home();
        return ZMK_BEHAVIOR_OPAQUE;
    }
    if (binding->param1 == END) {
        ng_end();
        return ZMK_BEHAVIOR_OPAQUE;
    }

    bool handled_key = is_ng_handled_keycode(binding->param1);

    // 追加: モディファイア押下中は透過にせず、対象キーは「合成タップ」を送出して残留入力を防止
    if (handled_key && n_hid_modifiers > 0) {
        // 現在押下中のHIDモディファイアが効いた状態で、対象キーを即時押下→解放
        raise_zmk_keycode_state_changed_from_encoded(binding->param1, true, timestamp);
        raise_zmk_keycode_state_changed_from_encoded(binding->param1, false, timestamp);
        // 物理リリースはスワローするために記録
        ng_mark_chord_tap_press(event.position);
        return ZMK_BEHAVIOR_OPAQUE;
    }

    bool use_ng = handled_key && naginata_layer_active;

    if (!use_ng) {
        // 押下は透過として扱う -> リリースも透過に揃えるため記録
        if (handled_key) {
            ng_mark_transparent_press(event.position);
        }
        return ZMK_BEHAVIOR_TRANSPARENT;
    }

    // &ng で処理する場合は記録をクリア（万一残っていてもミスマッチしないように）
    ng_clear_transparent_press(event.position);
    naginata_press(binding, event);
    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_keymap_binding_released(struct zmk_behavior_binding *binding,
                                      struct zmk_behavior_binding_event event) {
    LOG_DBG("position %d keycode 0x%02X", event.position, binding->param1);

    timestamp = event.timestamp;

    // OS別: macOS のときは LCTRL と LGUI を入れ替える（&ng で直接解放）
    if (binding->param1 == LCTRL || binding->param1 == LGUI) {
        uint32_t out = binding->param1;
        if (naginata_config.os == NG_MACOS) {
            out = (binding->param1 == LCTRL) ? LGUI : LCTRL;
        }
        raise_zmk_keycode_state_changed_from_encoded(out, false, timestamp);
        return ZMK_BEHAVIOR_OPAQUE;
    }

    // 合成タップとして処理したキーの物理リリースはスワロー
    if (ng_was_chord_tap_press(event.position)) {
        ng_clear_chord_tap_press(event.position);
        return ZMK_BEHAVIOR_OPAQUE;
    }

    // Swallow releases for HOME/END when bound through &ng
    if (binding->param1 == HOME || binding->param1 == END) {
        return ZMK_BEHAVIOR_OPAQUE;
    }
    // &ng が扱わないキー（モディファイ、&mo など）は常に透過
    if (!is_ng_handled_keycode(binding->param1)) {
        return ZMK_BEHAVIOR_TRANSPARENT;
    }

    // 押下時に透過として扱ったキーは、リリースも透過へルーティング
    if (ng_was_transparent_press(event.position)) {
        ng_clear_transparent_press(event.position);
        return ZMK_BEHAVIOR_TRANSPARENT;
    }

    // 押下を &ng で処理したキーは、現在が非アクティブでも必ず &ng で解放処理
    naginata_release(binding, event);
    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_naginata_driver_api = {
    .binding_pressed = on_keymap_binding_pressed, .binding_released = on_keymap_binding_released};

#define KP_INST(n)                                                                                 \
    BEHAVIOR_DT_INST_DEFINE(n, behavior_naginata_init, NULL, NULL, NULL, POST_KERNEL,              \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &behavior_naginata_driver_api);

DT_INST_FOREACH_STATUS_OKAY(KP_INST)