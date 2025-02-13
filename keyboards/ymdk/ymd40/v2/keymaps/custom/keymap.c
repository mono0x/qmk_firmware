/* Copyright 2021 James Young (@noroadsleft)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

enum layer_names {
  _QWERTY,
  _LOWER,
  _RAISE,
  _ADJUST
};

enum custom_keycodes {
    IMEON = 0x8000 /* Use a fixed value instead of SAFE_RANGE to refer custom keycodes from VIA */,
    IMEOFF,
};

#define KC_IMEON IMEON
#define KC_IMEOFF IMEOFF

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_QWERTY] = LAYOUT_ortho_4x12(
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_BSPC,
        KC_ESC,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT,
        KC_CAPS, KC_LCTL, KC_LALT, KC_LGUI, MO(1),   KC_SPC,  KC_SPC,  MO(2),   KC_UP,   KC_LEFT, KC_DOWN, KC_RGHT
    ),
    [_LOWER] = LAYOUT_ortho_4x12(
        KC_TILD, KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR,    KC_ASTR,    KC_LPRN, KC_RPRN, KC_BSPC,
        KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_UNDS,    KC_PLUS,    KC_LCBR, KC_RCBR, KC_PIPE,
        _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  S(KC_NUHS), S(KC_NUBS), KC_HOME, KC_END,  _______,
        _______, _______, _______, _______, _______, _______, _______, _______,    KC_MNXT,    KC_VOLD, KC_VOLU, KC_MPLY
    ),
    [_RAISE] = LAYOUT_ortho_4x12(
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSPC,
        KC_DEL,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_MINS, KC_EQL,  KC_LBRC, KC_RBRC, KC_BSLS,
        _______, KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_NUHS, KC_NUBS, KC_PGUP, KC_PGDN, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY
    ),
    [_ADJUST] = LAYOUT_ortho_4x12(
        QK_BOOT,   RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, _______, _______, _______, _______, _______, _______, DEBUG,
        _______, RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, AG_NORM, AG_SWAP, _______, _______, _______, _______, _______,
        _______, BL_TOGG, BL_DEC,  BL_INC,  BL_BRTG, _______, _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______
    )
};

static bool imeon_pressed = false;
static bool imeoff_pressed = false;

static void process_ime_switch_or_layer(keyrecord_t *record, bool *pressed, uint16_t layer, uint16_t keycode_win, uint16_t keycode_mac) {
    // QMK Firmware で Raise/Lower と変換/無変換を同じキーに割り当てる
    // https://okapies.hateblo.jp/entry/2019/02/02/133953
    if (record->event.pressed) {
        *pressed = true;

        layer_on(layer);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
    } else {
        layer_off(layer);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);

        if (*pressed) {
            // 自作キーボードで複数キーの同時出力を行うQMK Firmwareを作成する
            // https://qiita.com/ryohey/items/94e4102957d1d4bbb78f
            register_code(keycode_win);
            unregister_code(keycode_win);
            register_code(keycode_mac);
            unregister_code(keycode_mac);
        }
        *pressed = false;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case IMEON:
            process_ime_switch_or_layer(record, &imeon_pressed, _RAISE, KC_HENK, KC_LANG1);
            return false;

        case IMEOFF:
            process_ime_switch_or_layer(record, &imeoff_pressed, _LOWER, KC_MHEN, KC_LANG2);
            return false;
    }
    return true;
}
