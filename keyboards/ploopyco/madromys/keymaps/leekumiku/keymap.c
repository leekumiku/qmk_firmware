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

#define LOWRES_SCROLL_TICK_SIZE 32

static int16_t scroll_v_accumulator = 0;
static int16_t scroll_h_accumulator = 0;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        MS_BTN3, MS_BTN4, MS_BTN5, MS_BTN2,
        MS_BTN1,          DRAG_SCROLL
    )
};

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (is_drag_scroll) {
        // Windows and Linux: do nothing, ploopyco.c already calculated perfect hi-res deltas
        if (detected_host_os() != OS_WINDOWS && detected_host_os() != OS_LINUX) {
            // macOS / other: accumulate ploopyco's deltas into discrete notches
            scroll_v_accumulator += mouse_report.v;
            scroll_h_accumulator += mouse_report.h;

            mouse_report.v = 0;
            mouse_report.h = 0;

            if (scroll_v_accumulator >= LOWRES_SCROLL_TICK_SIZE) {
                mouse_report.v = 1;
                scroll_v_accumulator = 0;
            } else if (scroll_v_accumulator <= -LOWRES_SCROLL_TICK_SIZE) {
                mouse_report.v = -1;
                scroll_v_accumulator = 0;
            }

            if (scroll_h_accumulator >= LOWRES_SCROLL_TICK_SIZE) {
                mouse_report.h = 1;
                scroll_h_accumulator = 0;
            } else if (scroll_h_accumulator <= -LOWRES_SCROLL_TICK_SIZE) {
                mouse_report.h = -1;
                scroll_h_accumulator = 0;
            }
        }
    } else {
        scroll_v_accumulator = 0;
        scroll_h_accumulator = 0;
    }

    return mouse_report;
}