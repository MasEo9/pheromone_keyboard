#include QMK_KEYBOARD_H
#ifdef POT_ENABLE
  #include "analog.h"
#endif
#include "midi.h"

extern MidiDevice midi_device;

#include "print.h"

#define _BASE 0

int16_t pot_oldVal = 0;
int16_t pot_val    = 0;
int16_t pot_ccVal  = 0;
#define POT_TOLERANCE 12

#define POT_UP_KEYCODE   KC_VOLU  // Volume Up
#define POT_DOWN_KEYCODE KC_VOLD  // Volume Down

// static char current_alpha_oled = '\0';

// volatile int8_t scroll_direction = 0; // -1 = down, 0 = stopped, 1 = up
// volatile uint8_t scroll_speed     = 1; // Adjust for faster/slower scrolling

/*
//,-----------------------------------------------------------------------------------------------------------------------------------------------------.
//|   ESC   |   1 !   |   2 @   |   3 #   |   4 $   |   5 %   |   6 ^   |   7 &   |   8 *   |   9 (   |   0 )   |   - _   |   = +   |   ~ `   |  Bksp  |
//'-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- '
//|   Tab   |    Q    |    W    |    E    |    R    |    T    |    Y    |    U    |    I    |    O    |    P    |   { [   |   } ]   |  Enter  |
//'-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +         '
//|Capslock |    A    |    S    |    D    |    F    |    G    |    H    |    J    |    K    |    L    |   : ;   |   " '   |   \ |   |         |
//'-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- '
//|  Shift  |    Z    |    X    |    C    |    V    |    B    |    N    |    M    |   < ,   |   > .   |   ? /   |  Shift  |   Up    |   Fn    |
//'-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- '
//|   Ctrl  |   Alt   |   Win   |                                      space                                    |   Left  |  Down   |  Right  |
//'-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +-------- +---------'
*/

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT_63_iso(KC_ESC, KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, KC_MINS, KC_EQL, KC_GRV, KC_BSPC,
                            //---
                            KC_TAB, KC_Q, KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, KC_LBRC, KC_RBRC,
                            //---
                            KC_CAPS, KC_A, KC_S, KC_D, KC_F, KC_G, KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT, KC_BSLS, KC_ENT,
                            //---
                            KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_RSFT, KC_UP, KC_RSFT,
                            //---
                            KC_LCTL, KC_LWIN, KC_LALT, KC_SPC, KC_LEFT, KC_DOWN, KC_RGHT),
};

void keyboard_post_init_user(void) {
  // Customise these values to desired behaviour
  debug_enable=true;
  debug_matrix=true;
  debug_keyboard=true;
  //debug_mouse=true;
}

void matrix_init_user(void) {
#ifdef POT_ENABLE
    analogReference(ADC_REF_POWER);
#endif
}

void matrix_scan_user(void) {
#ifdef POT_ENABLE
    pot_val   = (analogReadPin(F5));
    pot_ccVal = pot_val / 8;
    if (abs(pot_val - pot_oldVal) > POT_TOLERANCE) {
        if ((pot_ccVal & 0x3FF) > (70 & 0x3FF)) {
            // Increased: Send POT_UP_KEYCODE
            register_code(KC_UP);
            unregister_code(KC_DOWN);
            dprintf("Pot Up: Sending MS_WHLU\n");
        } else if ((pot_ccVal & 0x3FF) < (55 & 0x3FF)) {
            // Decreased: Send POT_DOWN_KEYCODE
            register_code(KC_DOWN);
            unregister_code(KC_UP);
            dprintf("Pot Down: Sending MS_WHLD\n");
        } else {
            unregister_code(KC_UP);
            unregister_code(KC_DOWN);
        }
        pot_oldVal = pot_val;
        dprintf("pot_val: %u, pot_ccVal: %u\n", pot_val & 0x3FF, pot_ccVal & 0x3FF);
    }
#endif
}

#ifdef OLED_DRIVER_ENABLE
void oled_task_user(void) {
    static const char PROGMEM image[] = {0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92, 0x93, 0x94, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF, 0xD0, 0xD1, 0xD2, 0xD3, 0xD4};
    oled_write_P(image, false);
    uint8_t led_usb_state = host_keyboard_leds();
    switch (biton32(layer_state)) {
        case _BASE:
        case _FUNC:
            oled_write_P(led_usb_state & (1 << USB_LED_CAPS_LOCK) ? PSTR("}~LOCK") : PSTR("{|    "), false);
            oled_write_P(PSTR("   ["), false);
            if (current_alpha_oled == '\0') {
                oled_write_char('*', false);
            } else {
                oled_write_char(current_alpha_oled, false);
            }
            oled_write_P(PSTR("]   "), false);
            break;
    }
    switch (get_highest_layer(layer_state)) {
        case _BASE:
            oled_write_P(PSTR("1 BASE"), false);
            break;
        case _FUNC:
            oled_write_P(PSTR("2 FUNC"), false);
            break;
        default:
            oled_write_ln_P(PSTR("  NONE"), false);
    }
}
#endif

