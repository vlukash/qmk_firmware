#include QMK_KEYBOARD_H
#include "bootloader.h"
#ifdef PROTOCOL_LUFA
  #include "lufa.h"
  #include "split_util.h"
#endif
#ifdef SSD1306OLED
  #include "ssd1306.h"
#endif

#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

extern uint8_t is_master;

enum layer_names {
  _QWERTY,
  _LOWER,
  _RAISE,
  _ADJUST
};

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  LOWER,
  RAISE,
  RGBRST,
  MBTN1,
  SCRL
};

#define KC______ KC_TRNS
#define KC_XXXXX KC_NO
#define KC_LOWER LOWER
#define KC_RAISE RAISE
#define KC_RST   RESET
#define KC_LRST  RGBRST
#define KC_LTOG  RGB_TOG
#define KC_LHUI  RGB_HUI
#define KC_LHUD  RGB_HUD
#define KC_LSAI  RGB_SAI
#define KC_LSAD  RGB_SAD
#define KC_LVAI  RGB_VAI
#define KC_LVAD  RGB_VAD
#define KC_LMOD  RGB_MOD

#define KC_CTLA CTL_T(KC_A)
#define KC_CTLSC CTL_T(KC_SCLN)
#define KC_SFTZ SFT_T(KC_Z)
#define KC_SFTSL SFT_T(KC_SLSH)
#define KC_WINX LWIN_T(KC_X)
#define KC_WINDO RWIN_T(KC_DOT)

#define KC_WINA LWIN_T(KC_A)
#define KC_WINSC RWIN_T(KC_SCLN)

#define KC_LALTG LALT_T(KC_GRAVE)

#define KC_SFTZ SFT_T(KC_Z)
#define KC_SFTSL SFT_T(KC_SLSH)

#define KC_CTLX CTL_T(KC_X)
#define KC_CTLDO CTL_T(KC_DOT)

#define KC_MBTN1  MBTN1
#define KC_SCRL   SCRL

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT_kc(
  //,-----------------------------------------.                ,-----------------------------------------.
        ESC,     Q,     W,     E,     R,     T,                      Y,     U,     I,     O,     P,  BSPC,
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
        TAB,  WINA,     S,     D,     F,     G,                      H,     J,     K,     L, WINSC,  QUOT,
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      LALTG,  SFTZ,  CTLX,     C,     V,     B,                      N,     M,  COMM, CTLDO, SFTSL,BSLASH,
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                  LOWER,   SPC,  SCRL,    MBTN1,   ENT, RAISE
                              //`--------------------'  `--------------------'
  ),

  [_LOWER] = LAYOUT_kc(
  //,-----------------------------------------.                ,-----------------------------------------.
        ESC, XXXXX,  PGUP,  PSCR,  PGDN,  LBRC,                   RBRC,     7,     8,     9, XXXXX, XXXXX,
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      XXXXX,  LWIN,  PLUS,  MINS,   EQL,  LPRN,                   RPRN,     4,     5,     6,  RWIN, XXXXX,
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
       LALT,  LSFT,  HOME, XXXXX,   END,  LCBR,                   RCBR,     1,     2,     3,  RSFT, XXXXX,
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                  LOWER,   SPC,  SCRL,     BTN2,   ENT,     0
                              //`--------------------'  `--------------------'
  ),

  [_RAISE] = LAYOUT_kc(
  //,-----------------------------------------.                ,-----------------------------------------.
        ESC, XXXXX,    F7,    F8,    F9,   F10,                  XXXXX, XXXXX,  MNXT,  MPRV,  MPLY,  MSTP,
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      XXXXX,  LWIN,    F4,    F5,    F6,   F11,                   LEFT,  DOWN,    UP, RIGHT,  RWIN, XXXXX,
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      XXXXX,  LSFT,    F1,    F2,    F3,   F12,                  XXXXX, XXXXX,  VOLU,  VOLD,  MUTE,  RSFT,
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                  LOWER,   SPC,  SCRL,    MBTN1,   ENT, RAISE
                              //`--------------------'  `--------------------'
  ),

  [_ADJUST] = LAYOUT_kc(
  //,-----------------------------------------.                ,-----------------------------------------.
        RST,  LRST, XXXXX, XXXXX, XXXXX, XXXXX,                  XXXXX, XXXXX, XXXXX, XXXXX, XXXXX,   RST,
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
       LTOG,  LHUI,  LSAI,  LVAI, XXXXX, XXXXX,                  XXXXX, XXXXX, XXXXX, XXXXX, XXXXX, XXXXX,
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
       LMOD,  LHUD,  LSAD,  LVAD, XXXXX, XXXXX,                  XXXXX, XXXXX, XXXXX, XXXXX, XXXXX, XXXXX,
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                  LOWER,   SPC,  SCRL,    MBTN1,   ENT, RAISE
                              //`--------------------'  `--------------------'
  )
};

int RGB_current_mode;

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}

// Setting ADJUST layer RGB back to default
void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
  if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
    layer_on(layer3);
  } else {
    layer_off(layer3);
  }
}

void matrix_init_user(void) {
    #ifdef RGBLIGHT_ENABLE
      RGB_current_mode = rgblight_config.mode;
    #endif
    //SSD1306 OLED init, make sure to add #define SSD1306OLED in config.h
    #ifdef SSD1306OLED
        iota_gfx_init(!has_usb());   // turns on the display
    #endif
}

//SSD1306 OLED update loop, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED

// When add source files to SRC in rules.mk, you can use functions.
const char *read_layer_state(void);
const char *read_logo(void);
void set_keylog(uint16_t keycode, keyrecord_t *record);
const char *read_keylog(void);
const char *read_keylogs(void);

void matrix_scan_user(void) {
   iota_gfx_task();
}

void matrix_render_user(struct CharacterMatrix *matrix) {
  if (is_master) {
    // If you want to change the display of OLED, you need to change here
    matrix_write_ln(matrix, read_layer_state());
    matrix_write_ln(matrix, read_keylog());
    matrix_write_ln(matrix, read_keylogs());
  } else {
    matrix_write(matrix, read_logo());
  }
}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
  if (memcmp(dest->display, source->display, sizeof(dest->display))) {
    memcpy(dest->display, source->display, sizeof(dest->display));
    dest->dirty = true;
  }
}

void iota_gfx_task_user(void) {
  struct CharacterMatrix matrix;
  matrix_clear(&matrix);
  matrix_render_user(&matrix);
  matrix_update(&display, &matrix);
}
#endif//SSD1306OLED

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
#ifdef SSD1306OLED
    set_keylog(keycode, record);
#endif
  }

  switch (keycode) {
    case QWERTY:
      if (record->event.pressed) {
        persistent_default_layer_set(1UL<<_QWERTY);
      }
      return false;
    case LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_LOWER);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
      return false;
    case RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_RAISE);
        update_tri_layer_RGB(_LOWER, _RAISE, _ADJUST);
      }
      return false;
    case RGB_MOD:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          rgblight_mode(RGB_current_mode);
          rgblight_step();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      return false;
    case RGBRST:
      #ifdef RGBLIGHT_ENABLE
        if (record->event.pressed) {
          eeconfig_update_rgblight_default();
          rgblight_enable();
          RGB_current_mode = rgblight_config.mode;
        }
      #endif
      break;
  }
  return true;
}

