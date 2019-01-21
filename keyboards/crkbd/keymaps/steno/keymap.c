#include QMK_KEYBOARD_H
#include "bootloader.h"
#ifdef PROTOCOL_LUFA
  #include "lufa.h"
  #include "split_util.h"
#endif
#ifdef SSD1306OLED
  #include "ssd1306.h"
#endif

#include "keymap_steno.h"

extern keymap_config_t keymap_config;

#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
#endif

extern uint8_t is_master;

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
#define STENO 0

enum custom_keycodes {
  QWERTY = SAFE_RANGE
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [STENO] = LAYOUT(\
  //,-----------------------------------------.                ,-----------------------------------------.
      STN_N1,STN_N2,STN_N3,STN_N4,STN_N5,STN_N6,                 STN_N7,STN_N8,STN_N9,STN_NA,STN_NB,STN_NC, \
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
      STN_FN,STN_S1,STN_TL,STN_PL,STN_HL,STN_ST1,                STN_ST3,STN_FR,STN_PR,STN_LR,STN_TR,STN_DR, \
  //|------+------+------+------+------+------|                |------+------+------+------+------+------|
     STN_PWR,STN_S2,STN_KL,STN_WL,STN_RL,STN_ST2,                STN_ST4,STN_RR,STN_BR,STN_GR,STN_SR,STN_ZR, \
  //|------+------+------+------+------+------+------|  |------+------+------+------+------+------+------|
                                  STN_A, STN_O, RGB_MODE_FORWARD,   RGB_TOG, STN_E, STN_U \
                              //`--------------------'  `--------------------'
  )
};

void matrix_init_user(void) {
    //SSD1306 OLED init, make sure to add #define SSD1306OLED in config.h
    #ifdef SSD1306OLED
        iota_gfx_init(!has_usb());   // turns on the display
    #endif
    steno_set_mode(STENO_MODE_GEMINI);
}

static bool updateDisplay = true;
static bool activeChord[STENO_KEY_COUNT];

bool send_steno_chord_user(steno_mode_t mode, uint8_t chord[6]) {
  memset(activeChord, 0, sizeof activeChord);
  updateDisplay = true;
  return true;
}

bool postprocess_steno_user(uint16_t keycode, keyrecord_t *record, steno_mode_t mode, uint8_t chord[6], int8_t pressed) {
  int reducedKeycode = keycode - QK_STENO;
  activeChord[reducedKeycode] = true;
  updateDisplay = true;
  if (is_master) {
    switch (keycode) {
      case STN_N1:
        rgblight_setrgb_at(255, 0, 0, 15);
        break;
      case STN_FN:
        break;
      case STN_PWR:
        break;

      case STN_N2:
        break;
      case STN_S1:
        break;
      case STN_S2:
        break;

      case STN_N3:
        break;
      case STN_TL:
        break;
      case STN_KL:
        break;
        
      case STN_N4:
        break;
      case STN_PL:
        break;
      case STN_WL:
        break;
            
      case STN_N5:
        break;
      case STN_HL:
        break;
      case STN_RL:
        break;

      case STN_A:
        break;
      case STN_O:
        break;

      case STN_N6:
        break;
      case STN_ST1:
        break;
      case STN_ST2:
        break;
    }
  } else {
    switch(keycode) {
      case STN_E:
        break;
      case STN_U:
        break;

      case STN_N7:
        break;
      case STN_ST3:
        break;
      case STN_ST4:
        break;

      case STN_N8:
        break;
      case STN_FR:
        break;
      case STN_RR:
        break;

      case STN_N9:
        break;
      case STN_PR:
        break;
      case STN_BR:
        break;
        
      case STN_NA:
        break;
      case STN_LR:
        break;
      case STN_GR:
        break;
            
      case STN_NB:
        break;
      case STN_TR:
        break;
      case STN_SR:
        break;

      case STN_NC:
        rgblight_setrgb_at(255, 0, 0, 15);
        break;
      case STN_DR:
        break;
      case STN_ZR:
        break;
    }
  }
  return true;
}

//SSD1306 OLED update loop, make sure to add #define SSD1306OLED in config.h
#ifdef SSD1306OLED
static char paperTape[24];
const char *read_steno(void) {
  if (is_master) {
    if (activeChord[STN_N1 - QK_STENO] == true) {
      snprintf(paperTape, sizeof(paperTape), "Number 1");
    } else {
      snprintf(paperTape, sizeof(paperTape), " ");
    }
  } else {
    if (activeChord[STN_NC - QK_STENO] == true) {
      snprintf(paperTape, sizeof(paperTape), "Number C!");
    } else {
      snprintf(paperTape, sizeof(paperTape), " ");
    }
  }

  return paperTape;
}

// When add source files to SRC in rules.mk, you can use functions.
//const char *read_logo(void);
// const char *read_sl(void);
// const char *read_layer_state(void);
void matrix_scan_user(void) {
   iota_gfx_task();
}

void matrix_render_user(struct CharacterMatrix *matrix) {
  if (is_master) {
    matrix_write_ln(matrix, read_steno());
  } else {
    matrix_write_ln(matrix, read_steno());
  }
}

void matrix_update(struct CharacterMatrix *dest, const struct CharacterMatrix *source) {
  if (memcmp(dest->display, source->display, sizeof(dest->display))) {
    memcpy(dest->display, source->display, sizeof(dest->display));
    dest->dirty = true;
  }
}

void iota_gfx_task_user(void) {
  if (updateDisplay) {
    updateDisplay = false;
    struct CharacterMatrix matrix;
    matrix_clear(&matrix);
    matrix_render_user(&matrix);
    matrix_update(&display, &matrix);
  }
}
#endif//SSD1306OLED

