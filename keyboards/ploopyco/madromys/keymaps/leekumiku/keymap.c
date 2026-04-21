/* Copyright 2023 Colin Lam (Ploopy Corporation)
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
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

// https://getreuer.info/posts/keyboards/faqs/index.html#mt-doesnt-work-with-this-keycode-qmk
#define LT_DRAGSCROLL LT(1, KC_NO)

void manageDPI(bool up);
static uint16_t custom_dpi = 1400;
static uint16_t minimum_dpi = 200;
static uint16_t max_dpi = 12000;
static uint16_t step_size = 100;

enum custom_keycodes {
    DPI_UP = SAFE_RANGE,
    DPI_DOWN,
};

enum combo_events {
    SHOW_DPI_COMBO,
    LH_TOGGLE_COMBO,
    HIRES_SCROLL_TOGGLE_COMBO,
};

const uint16_t PROGMEM dpi_combo_keys[] = {DPI_DOWN, DPI_UP, COMBO_END};
const uint16_t PROGMEM lh_combo_keys[] = {LCA(LSFT(KC_TAB)), LCA(KC_TAB), COMBO_END};
const uint16_t PROGMEM hires_scroll_combo_keys[] = {KC_ENT, LCA(KC_TAB), COMBO_END};

combo_t key_combos[] = {
    [SHOW_DPI_COMBO] = COMBO_ACTION(dpi_combo_keys),
    [LH_TOGGLE_COMBO] = COMBO_ACTION(lh_combo_keys),
    [HIRES_SCROLL_TOGGLE_COMBO] = COMBO_ACTION(hires_scroll_combo_keys)
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT( 
        MS_BTN3, MS_BTN4, MS_BTN5, MS_BTN2, 
        MS_BTN1,          LT_DRAGSCROLL
    ),

    [1] = LAYOUT( 
        LCA(LSFT(KC_TAB)), DPI_DOWN, DPI_UP, LCA(KC_TAB), 
        KC_ENT,           KC_TRNS
    )
};


// Define the swap table
// https://docs.qmk.fm/features/swap_hands
const keypos_t PROGMEM hand_swap_config[MATRIX_ROWS][MATRIX_COLS] = {
    // Row 0 contains all 6 buttons
    [0] = {
        {5, 0}, // 0 swaps with 5 (Bottoms)
        {4, 0}, // 1 swaps with 4 (Outer Tops)
        {2, 0}, // 2 swaps with 2 (Inner Tops)
        {3, 0}, // 3 swaps with 3 (not swapping Back/Forward and DPI_DOWN/DPI_UP)
        {1, 0}, // 4 swaps with 1
        {0, 0}  // 5 swaps with 0
    }
};

void process_combo_event(uint16_t combo_index, bool pressed) {
    switch(combo_index) {
        case SHOW_DPI_COMBO:
            if (pressed) {
                char dpi_str[6];
                itoa(custom_dpi, dpi_str, 10);
                send_string(dpi_str);
            }
            break;
        case LH_TOGGLE_COMBO:
            if (pressed) {
                swap_hands_toggle();
            }
            break;
        case HIRES_SCROLL_TOGGLE_COMBO:
            if (pressed) {
                toggle_hires_scroll();
            }
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LT_DRAGSCROLL:
            if (record->tap.count) {
                if (record->event.pressed) {
                    // Using the built-in Ploopy functio1400
                    toggle_drag_scroll();
                }
                return false;
            }
            break;

        case DPI_UP:
            if (record->event.pressed) {
                manageDPI(true);
            }
            return false;

        case DPI_DOWN:
            if (record->event.pressed) {
                manageDPI(false);
            }
            return false;
    }
    return true;
}

void manageDPI(bool up) {
    uint16_t step = step_size;
    // if (custom_dpi >= 5000) {
    //     step = 1000;
    // } else if (custom_dpi >= 1500) {
    //     step = 500;
    // }

    if (up) {
        custom_dpi += step;
    } else {
        if (custom_dpi > step) {
            custom_dpi -= step;
        }
    }

    if (custom_dpi < minimum_dpi) custom_dpi = minimum_dpi;
    if (custom_dpi > max_dpi)     custom_dpi = max_dpi;

    pointing_device_set_cpi(custom_dpi);
}