// -*- mode: c++ -*-
// Copyright 2016-2022 Keyboardio, inc. <jesse@keyboard.io>
// See "LICENSE" for license details

/**
 * These #include directives pull in the Kaleidoscope firmware core,
 * as well as the Kaleidoscope plugins we use in the Model 100's firmware
 */

// TODO: Try the ActiveModColor plugin to indicate which modifiers are active.

// The Kaleidoscope core
#include "Kaleidoscope.h"

// Friendly names for keys
//#include "kaleidoscope/key_defs/keyboard.h"

// Support for storing the keymap in EEPROM
//#include "Kaleidoscope-EEPROM-Settings.h"
//#include "Kaleidoscope-EEPROM-Keymap.h"

// Support for communicating with the host via a simple Serial protocol
//#include "Kaleidoscope-FocusSerial.h"

// Support for querying the firmware version via Focus
#include "Kaleidoscope-FirmwareVersion.h"

// Support for keys that move the mouse
#include "Kaleidoscope-MouseKeys.h"

// Support for macros
#include "Kaleidoscope-Macros.h"

// Support for controlling the keyboard's LEDs
#include "Kaleidoscope-LEDControl.h"

// Support for "Numpad" mode, which is mostly just the Numpad specific LED mode
#include "Kaleidoscope-NumPad.h"

// Support for the "Boot greeting" effect, which pulses the 'LED' button for 10s
// when the keyboard is connected to a computer (or that computer is powered on)
#include "Kaleidoscope-LEDEffect-BootGreeting.h"

// Support for LED modes that set all LEDs to a single color
//#include "Kaleidoscope-LEDEffect-SolidColor.h"

// Support for an LED mode that makes all the LEDs 'breathe'
//#include "Kaleidoscope-LEDEffect-Breathe.h"

// Support for an LED mode that makes a red pixel chase a blue pixel across the keyboard
//#include "Kaleidoscope-LEDEffect-Chase.h"

// Support for LED modes that pulse the keyboard's LED in a rainbow pattern
#include "Kaleidoscope-LEDEffect-Rainbow.h"

// Support for an LED mode that lights up the keys as you press them
#include "Kaleidoscope-LED-Stalker.h"

// Support for an LED mode that prints the keys you press in letters 4px high
//#include "Kaleidoscope-LED-AlphaSquare.h"

// Support for shared palettes for other plugins, like Colormap below
#include "Kaleidoscope-LED-Palette-Theme.h"

// Support for an LED mode that lets one configure per-layer color maps
#include "Kaleidoscope-Colormap.h"

// Support for turning the LEDs off after a certain amount of time
#include "Kaleidoscope-IdleLEDs.h"

// Support for setting and saving the default LED mode
#include "Kaleidoscope-DefaultLEDModeConfig.h"

// Support for changing the brightness of the LEDs
#include "Kaleidoscope-LEDBrightnessConfig.h"

// Support for Keyboardio's internal keyboard testing mode
#include "Kaleidoscope-HardwareTestMode.h"

// Support for host power management (suspend & wakeup)
#include "Kaleidoscope-HostPowerManagement.h"

// Support for magic combos (key chords that trigger an action)
#include "Kaleidoscope-MagicCombo.h"

// Support for USB quirks, like changing the key state report protocol
#include "Kaleidoscope-USB-Quirks.h"

// Support for secondary actions on keys
//#include "Kaleidoscope-Qukeys.h"

// Support for one-shot modifiers and layer keys
#include "Kaleidoscope-OneShot.h"
#include "Kaleidoscope-Escape-OneShot.h"

// Support for dynamic, Chrysalis-editable macros
//#include "Kaleidoscope-DynamicMacros.h"

// Support for SpaceCadet keys
//#include "Kaleidoscope-SpaceCadet.h"

// Support for editable layer names
//#include "Kaleidoscope-LayerNames.h"

// Support for the GeminiPR Stenography protocol
//#include "Kaleidoscope-Steno.h"

/** This 'enum' is a list of all the macros used by the Model 100's firmware
  * The names aren't particularly important. What is important is that each
  * is unique.
  *
  * These are the names of your macros. They'll be used in two places.
  * The first is in your keymap definitions. There, you'll use the syntax
  * `M(MACRO_NAME)` to mark a specific keymap position as triggering `MACRO_NAME`
  *
  * The second usage is in the 'switch' statement in the `macroAction` function.
  * That switch statement actually runs the code associated with a macro when
  * a macro key is pressed.
  */

enum { MACRO_VERSION_INFO,
       MACRO_ANY,
       MACRO_REISUB,
     };


/** The Model 100's key layouts are defined as 'keymaps'. By default, there are three
  * keymaps: The standard QWERTY keymap, the "Function layer" keymap and the "Numpad"
  * keymap.
  *
  * Each keymap is defined as a list using the 'KEYMAP_STACKED' macro, built
  * of first the left hand's layout, followed by the right hand's layout.
  *
  * Keymaps typically consist mostly of `Key_` definitions. There are many, many keys
  * defined as part of the USB HID Keyboard specification. You can find the names
  * (if not yet the explanations) for all the standard `Key_` defintions offered by
  * Kaleidoscope in these files:
  *    https://github.com/keyboardio/Kaleidoscope/blob/master/src/kaleidoscope/key_defs/keyboard.h
  *    https://github.com/keyboardio/Kaleidoscope/blob/master/src/kaleidoscope/key_defs/consumerctl.h
  *    https://github.com/keyboardio/Kaleidoscope/blob/master/src/kaleidoscope/key_defs/sysctl.h
  *    https://github.com/keyboardio/Kaleidoscope/blob/master/src/kaleidoscope/key_defs/keymaps.h
  *
  * Additional things that should be documented here include
  *   using ___ to let keypresses fall through to the previously active layer
  *   using XXX to mark a keyswitch as 'blocked' on this layer
  *   using ShiftToLayer() and LockLayer() keys to change the active keymap.
  *   keeping NUM and FN consistent and accessible on all layers
  *
  * The PROG key is special, since it is how you indicate to the board that you
  * want to flash the firmware. However, it can be remapped to a regular key.
  * When the keyboard boots, it first looks to see whether the PROG key is held
  * down; if it is, it simply awaits further flashing instructions. If it is
  * not, it continues loading the rest of the firmware and the keyboard
  * functions normally, with whatever binding you have set to PROG. More detail
  * here: https://community.keyboard.io/t/how-the-prog-key-gets-you-into-the-bootloader/506/8
  *
  * The "keymaps" data structure is a list of the keymaps compiled into the firmware.
  * The order of keymaps in the list is important, as the ShiftToLayer(#) and LockLayer(#)
  * macros switch to key layers based on this list.
  *
  *

  * A key defined as 'ShiftToLayer(FUNCTION)' will switch to FUNCTION while held.
  * Similarly, a key defined as 'LockLayer(NUMPAD)' will switch to NUMPAD when tapped.
  */

/**
  * Layers are "0-indexed" -- That is the first one is layer 0. The second one is layer 1.
  * The third one is layer 2.
  * This 'enum' lets us use names like QWERTY, FUNCTION, and NUMPAD in place of
  * the numbers 0, 1 and 2.
  *
  */

enum { QWERTY, MOUSE_NUMPAD, FUNCTION, SYSTEM_EMERGENCY }; // layers

/* This comment temporarily turns off astyle's indent enforcement
 *   so we can make the keymaps actually resemble the physical key layout better
 */
// clang-format off

KEYMAPS(
  [QWERTY] = KEYMAP_STACKED
  ( // QWERTY Left
   Key_Backtick, Key_1,  Key_2,  Key_3,  Key_4,  Key_5,  Key_LeftGui,
   Key_Tab,      Key_Q,  Key_W,  Key_E,  Key_R,  Key_T,  Key_F3,
   Key_PageUp,   Key_A,  Key_S,  Key_D,  Key_F,  Key_G,
   Key_PageDown, Key_Z,  Key_X,  Key_C,  Key_V,  Key_B,  Key_Minus, // Dvorak left bracket
   Key_Escape,   Key_LeftShift, Key_CapsLock, Key_LeftAlt,  // Intended to be used with xkbmap -option ctrl:swapcaps
   ShiftToLayer(FUNCTION),

   // QWERTY Right
   LockLayer(MOUSE_NUMPAD),  Key_6,  Key_7,  Key_8,      Key_9,          Key_0,          Key_RightBracket, // Dvorak equals
   Key_F4,                   Key_Y,  Key_U,  Key_I,      Key_O,          Key_P,          Key_LeftBracket,  // Dvorak slash
                             Key_H,  Key_J,  Key_K,      Key_L,          Key_Semicolon,  Key_Quote,        // Dvorak minus
   Key_Equals,               Key_N,  Key_M,  Key_Comma,  Key_Period,     Key_Slash,      Key_Pause,        // = is Dvorak right bracket
   Key_Backspace, Key_Enter, Key_Spacebar, OSM(RightShift),
   ShiftToLayer(FUNCTION)),

  [MOUSE_NUMPAD] =  KEYMAP_STACKED
  ( // Mouse/Numpad Left
   LockLayer(SYSTEM_EMERGENCY),  XXX,        XXX,         XXX,        XXX,           XXX,              XXX,
   XXX,                          XXX,        Key_mouseUp, XXX,        Key_mouseBtnR, Key_mouseWarpEnd, Key_mouseWarpNE,
   ___, /* pgup */               Key_mouseL, Key_mouseDn, Key_mouseR, Key_mouseBtnL, Key_mouseWarpNW,
   ___, /* pgdn */               XXX,        XXX,         XXX,        Key_mouseBtnM, Key_mouseWarpSW,  Key_mouseWarpSE,
   ___, ___, ___, ___,
   ___,

   // Mouse/Numpad Right
   ___, /* numlk */        XXX, Key_Keypad7, Key_Keypad8,   Key_Keypad9,        Key_KeypadMultiply, XXX,
   XXX,                    XXX, Key_Keypad4, Key_Keypad5,   Key_Keypad6,        Key_KeypadAdd,      Key_KeypadDivide,
                           XXX, Key_Keypad1, Key_Keypad2,   Key_Keypad3,        Key_Equals,         Key_KeypadSubtract,
   XXX,                    XXX, Key_Keypad0, Key_KeypadDot, XXX,                XXX,                Key_Enter,
   ___, ___, ___, ___,
   ___),

  [FUNCTION] =  KEYMAP_STACKED
  ( // Function Left
   XXX, ___,             ___,        ___,    ___,    ___,     LSHIFT(Key_LeftGui),
   ___, Key_F6,          Key_F7,     Key_F8, Key_F9, Key_F10, Key_F11,
   ___, Key_F1,          Key_F2,     Key_F3, Key_F4, Key_F5,
   ___, Key_PrintScreen, Key_Insert, XXX,    XXX,    XXX,     Key_Home,
   ___, OSM(RightShift), OSM(RightControl), OSM(RightAlt),
   ___,

   // Function Right
   Key_LEDEffectNext, ___,           ___,                      ___,                      ___,            ___,     XXX,
   Key_F12,           XXX,           Key_Home,                 Key_UpArrow,              Key_End,        XXX,     Key_Backslash,
                      XXX,           Key_LeftArrow,            Key_DownArrow,            Key_RightArrow, XXX,     XXX,
   Key_End,           Consumer_Mute, Consumer_VolumeDecrement, Consumer_VolumeIncrement, XXX,            XXX,     XXX,
   Key_Delete, ___, ___, ___,
   ___),

  [SYSTEM_EMERGENCY] = KEYMAP_STACKED
  ( // System Emergency Left
   ___, LCTRL(LALT(Key_F1)), LCTRL(LALT(Key_F2)), LCTRL(LALT(Key_F3)), LCTRL(LALT(Key_F4)), LCTRL(LALT(Key_F5)), XXX, // Prog returns to prev. layer.
   XXX, XXX,                 XXX,                 XXX,                 XXX,                 XXX,                 XXX,
   XXX, XXX,                 XXX,                 XXX,                 XXX,                 XXX,
   XXX, XXX,                 XXX,                 XXX,                 XXX,                 XXX,                 XXX,
   XXX, XXX, XXX, XXX,
   XXX,

   // System Emergency Right
   XXX,             LCTRL(LALT(Key_F6)), LCTRL(LALT(Key_F7)), LCTRL(LALT(Key_F8)), LCTRL(LALT(Key_F9)), XXX, XXX,
   XXX,             XXX,                 XXX,                 XXX,                 XXX,                 XXX, XXX,
                    XXX,                 XXX,                 XXX,                 XXX,                 XXX, XXX,
   M(MACRO_REISUB), XXX,                 XXX,                 XXX,                 XXX,                 XXX, XXX, // Butterfly key reboots the system.
   XXX, XXX, XXX, XXX,
   XXX),

) // KEYMAPS

/* Re-enable astyle's indent enforcement */
// clang-format on

/** versionInfoMacro handles the 'firmware version info' macro
 *  When a key bound to the macro is pressed, this macro
 *  prints out the firmware build information as virtual keystrokes
 */

static void versionInfoMacro(uint8_t key_state) {
  if (keyToggledOn(key_state)) {
    Macros.type(PSTR("Keyboardio Model 100 - Firmware version "));
    Macros.type(PSTR(KALEIDOSCOPE_FIRMWARE_VERSION));
  }
}

/** anyKeyMacro is used to provide the functionality of the 'Any' key.
 *
 * When the 'any key' macro is toggled on, a random alphanumeric key is
 * selected. While the key is held, the function generates a synthetic
 * keypress event repeating that randomly selected key.
 *
 */

static void anyKeyMacro(KeyEvent &event) {
  if (keyToggledOn(event.state)) {
    event.key.setKeyCode(Key_A.getKeyCode() + (uint8_t)(millis() % 36));
    event.key.setFlags(0);
  }
}

/** reisubMacro sends the Linux "Reboot Even If System Utterly Broken" sequence.
 */
static const macro_t *reisubMacro(uint8_t keyState) {
  static const macro_t *kSequence = MACRO(
      D(LeftAlt),
      D(Sysreq),
      T(R),
		  W(250), W(250), W(250), W(250), W(250), W(250), W(250), W(250), // 2 seconds.
		  T(E),
		  W(250), W(250), W(250), W(250), W(250), W(250), W(250), W(250), // 2 seconds.
		  T(I),
		  W(250), W(250), W(250), W(250), W(250), W(250), W(250), W(250), // 2 seconds.
		  T(S),
		  W(250), W(250), W(250), W(250), W(250), W(250), W(250), W(250), // 2 seconds.
		  T(U),
		  W(250), W(250), W(250), W(250), W(250), W(250), W(250), W(250), // 2 seconds.
		  T(B),
		  U(Sysreq),
		  U(LeftAlt)
  );
  return kSequence;
}

/** macroAction dispatches keymap events that are tied to a macro
    to that macro. It takes two uint8_t parameters.

    The first is the macro being called (the entry in the 'enum' earlier in this file).
    The second is the state of the keyswitch. You can use the keyswitch state to figure out
    if the key has just been toggled on, is currently pressed or if it's just been released.

    The 'switch' statement should have a 'case' for each entry of the macro enum.
    Each 'case' statement should call out to a function to handle the macro in question.

 */

const macro_t *macroAction(uint8_t macro_id, KeyEvent &event) {
  switch (macro_id) {

  case MACRO_VERSION_INFO:
    versionInfoMacro(event.state);
    break;

  case MACRO_ANY:
    anyKeyMacro(event);
    break;

  case MACRO_REISUB:
    return reisubMacro(event.state);

  }
  return MACRO_NONE;
}


// These 'solid' color effect definitions define a rainbow of
// LED color modes calibrated to draw 500mA or less on the
// Keyboardio Model 100.

/*
static kaleidoscope::plugin::LEDSolidColor solidRed(160, 0, 0);
static kaleidoscope::plugin::LEDSolidColor solidOrange(140, 70, 0);
static kaleidoscope::plugin::LEDSolidColor solidYellow(130, 100, 0);
static kaleidoscope::plugin::LEDSolidColor solidGreen(0, 160, 0);
static kaleidoscope::plugin::LEDSolidColor solidBlue(0, 70, 130);
static kaleidoscope::plugin::LEDSolidColor solidIndigo(0, 0, 170);
static kaleidoscope::plugin::LEDSolidColor solidViolet(130, 0, 120);
*/

static void layerColorOverride(bool post_clear) {
  if (!post_clear) return;

  // NOTE: Multiple layers may be enabled at the same time. Don't assume that
  // just because the layer you are interested in coloring is enabled, no other
  // layers are also enabled.

  // LED indices come from Kaleidoscope-Hardware-Keyboardio-Model100/src/kaleidoscope/device/keyboardio/Model100.h
  if (Layer.isActive(SYSTEM_EMERGENCY)) {
    LEDControl.set_all_leds_to(breath_compute(33, 255));  // Yellow
    LEDControl.setCrgbAt(3, CRGB(0, 160, 0));             // Prog key green
    LEDControl.setCrgbAt(39, CRGB(160, 0, 0));            // Butterfly key red

    // Blue numbers to switch to virtual terminals.
    constexpr int8_t numbers[] = {
        4, 11, 12, 19, 20, 43, 44, 51, 52, -1,  // 1 through 9
    };
    for (int8_t i = 0; numbers[i] != -1; i++) {
      LEDControl.setCrgbAt(numbers[i], CRGB(0, 0, 160)); // Blue
    }
    return;
  }
  if (Layer.isActive(MOUSE_NUMPAD)) {
    LEDControl.setCrgbAt(3, breath_compute(0, 255)); // Prog key red
    return;
  }
}

/** toggleLedsOnSuspendResume toggles the LEDs off when the host goes to sleep,
 * and turns them back on when it wakes up.
 */
void toggleLedsOnSuspendResume(kaleidoscope::plugin::HostPowerManagement::Event event) {
  switch (event) {
  case kaleidoscope::plugin::HostPowerManagement::Suspend:
  case kaleidoscope::plugin::HostPowerManagement::Sleep:
    LEDControl.disable();
    break;
  case kaleidoscope::plugin::HostPowerManagement::Resume:
    LEDControl.enable();
    break;
  }
}

/** hostPowerManagementEventHandler dispatches power management events (suspend,
 * resume, and sleep) to other functions that perform action based on these
 * events.
 */
void hostPowerManagementEventHandler(kaleidoscope::plugin::HostPowerManagement::Event event) {
  toggleLedsOnSuspendResume(event);
}

/** This 'enum' is a list of all the magic combos used by the Model 100's
 * firmware The names aren't particularly important. What is important is that
 * each is unique.
 *
 * These are the names of your magic combos. They will be used by the
 * `USE_MAGIC_COMBOS` call below.
 */
enum {
  // Toggle between Boot (6-key rollover; for BIOSes and early boot) and NKRO
  // mode.
  COMBO_TOGGLE_NKRO_MODE,
  // Enter test mode
  COMBO_ENTER_TEST_MODE
};

/** Wrappers, to be used by MagicCombo. **/

/**
 * This simply toggles the keyboard protocol via USBQuirks, and wraps it within
 * a function with an unused argument, to match what MagicCombo expects.
 */
static void toggleKeyboardProtocol(uint8_t combo_index) {
  USBQuirks.toggleKeyboardProtocol();
}

/**
 * Toggles between using the built-in keymap, and the EEPROM-stored one.
 */
/*
static void toggleKeymapSource(uint8_t combo_index) {
  if (Layer.getKey == Layer.getKeyFromPROGMEM) {
    Layer.getKey = EEPROMKeymap.getKey;
  } else {
    Layer.getKey = Layer.getKeyFromPROGMEM;
  }
}
*/

/**
 *  This enters the hardware test mode
 */
static void enterHardwareTestMode(uint8_t combo_index) {
  HardwareTestMode.runTests();
}


/** Magic combo list, a list of key combo and action pairs the firmware should
 * recognise.
 */
USE_MAGIC_COMBOS({.action = toggleKeyboardProtocol,
                  // Left Fn + Esc + Shift
                  .keys = {R3C6, R2C6, R3C7}},
                 {.action = enterHardwareTestMode,
                  // Left Fn + Prog + LED
                  .keys = {R3C6, R0C0, R0C6}},
                 //{.action = toggleKeymapSource,
                  // Left Fn + Prog + Shift
                  //.keys = {R3C6, R0C0, R3C7}},
                );

// First, tell Kaleidoscope which plugins you want to use.
// The order can be important. For example, LED effects are
// added in the order they're listed here.
KALEIDOSCOPE_INIT_PLUGINS(
  // ----------------------------------------------------------------------
  // Chrysalis plugins

  // The EEPROMSettings & EEPROMKeymap plugins make it possible to have an
  // editable keymap in EEPROM.
  //EEPROMSettings,
  //EEPROMKeymap,

  // Focus allows bi-directional communication with the host, and is the
  // interface through which the keymap in EEPROM can be edited.
  //Focus,

  // FocusSettingsCommand adds a few Focus commands, intended to aid in
  // changing some settings of the keyboard, such as the default layer (via the
  // `settings.defaultLayer` command)
  //FocusSettingsCommand,

  // FocusEEPROMCommand adds a set of Focus commands, which are very helpful in
  // both debugging, and in backing up one's EEPROM contents.
  //FocusEEPROMCommand,

  // The FirmwareVersion plugin lets Chrysalis query the version of the firmware
  // programmatically.
  //FirmwareVersion,

  // The LayerNames plugin allows Chrysalis to display - and edit - custom layer
  // names, to be shown instead of the default indexes.
  //LayerNames,

  // Enables setting, saving (via Chrysalis), and restoring (on boot) the
  // default LED mode.
  DefaultLEDModeConfig,

  // Enables controlling (and saving) the brightness of the LEDs via Focus.
  LEDBrightnessConfig,

  // ----------------------------------------------------------------------
  // Keystroke-handling plugins

  // The Qukeys plugin enables the "Secondary action" functionality in
  // Chrysalis. Keys with secondary actions will have their primary action
  // performed when tapped, but the secondary action when held.
  //Qukeys,

  // SpaceCadet can turn your shifts into parens on tap, while keeping them as
  // Shifts when held. SpaceCadetConfig lets Chrysalis configure some aspects of
  // the plugin.
  //SpaceCadet,
  //SpaceCadetConfig,

  // Enables the "Sticky" behavior for modifiers, and the "Layer shift when
  // held" functionality for layer keys.
  OneShot,
  OneShotConfig,
  EscapeOneShot,
  EscapeOneShotConfig,

  // The macros plugin adds support for macros
  Macros,

  // Enables dynamic, Chrysalis-editable macros.
  //DynamicMacros,

  // The MouseKeys plugin lets you add keys to your keymap which move the mouse.
  MouseKeys,
  MouseKeysConfig,

  // The MagicCombo plugin lets you use key combinations to trigger custom
  // actions - a bit like Macros, but triggered by pressing multiple keys at the
  // same time.
  MagicCombo,

  // Enables the GeminiPR Stenography protocol. Unused by default, but with the
  // plugin enabled, it becomes configurable - and then usable - via Chrysalis.
  //GeminiPR,

  // ----------------------------------------------------------------------
  // LED mode plugins

  // The boot greeting effect pulses the LED button for 10 seconds after the
  // keyboard is first connected
  BootGreetingEffect,

  // LEDControl provides support for other LED modes
  LEDControl,

  // We start with the LED effect that turns off all the LEDs.
  LEDOff,

  // The rainbow effect changes the color of all of the keyboard's keys at the same time
  // running through all the colors of the rainbow.
  LEDRainbowEffect,

  // The rainbow wave effect lights up your keyboard with all the colors of a rainbow
  // and slowly moves the rainbow across your keyboard
  LEDRainbowWaveEffect,

/* Disable the LED effects I'm never likely to use.
  // The chase effect follows the adventure of a blue pixel which chases a red pixel across
  // your keyboard. Spoiler: the blue pixel never catches the red pixel
  LEDChaseEffect,

  // These static effects turn your keyboard's LEDs a variety of colors
  solidRed,
  solidOrange,
  solidYellow,
  solidGreen,
  solidBlue,
  solidIndigo,
  solidViolet,

  // The breathe effect slowly pulses all of the LEDs on your keyboard
  LEDBreatheEffect,

  // The AlphaSquare effect prints each character you type, using your
  // keyboard's LEDs as a display
  AlphaSquareEffect,
*/

  // The stalker effect lights up the keys you've pressed recently
  StalkerEffect,

  // The LED Palette Theme plugin provides a shared palette for other plugins,
  // like Colormap below
  LEDPaletteTheme,

  // The Colormap effect makes it possible to set up per-layer colormaps
  ColormapEffect,

  // The numpad plugin is responsible for lighting up the 'numpad' mode
  // with a custom LED effect
  NumPad,

  // The HostPowerManagement plugin allows us to turn LEDs off when then host
  // goes to sleep, and resume them when it wakes up.
  HostPowerManagement,

  // Turns LEDs off after a configurable amount of idle time.
  IdleLEDs,
  PersistentIdleLEDs,

  // ----------------------------------------------------------------------
  // Miscellaneous plugins

  // The USBQuirks plugin lets you do some things with USB that we aren't
  // comfortable - or able - to do automatically, but can be useful
  // nevertheless. Such as toggling the key report protocol between Boot (used
  // by BIOSes) and Report (NKRO).
  USBQuirks,

  // The hardware test mode, which can be invoked by tapping Prog, LED and the
  // left Fn button at the same time.
  HardwareTestMode  //,
);

/** The 'setup' function is one of the two standard Arduino sketch functions.
 * It's called when your keyboard first powers up. This is where you set up
 * Kaleidoscope and any plugins.
 */
void setup() {
  // First, call Kaleidoscope's internal setup function
  Kaleidoscope.setup();

  // Set the hue of the boot greeting effect to something that will result in a
  // nice green color.
  BootGreetingEffect.hue = 85;

  // While we hope to improve this in the future, the NumPad plugin
  // needs to be explicitly told which keymap layer is your numpad layer
  NumPad.numPadLayer = MOUSE_NUMPAD;

  // We configure the AlphaSquare effect to use RED letters
  //AlphaSquare.color = CRGB(255, 0, 0);

  // Set the rainbow effects to be reasonably bright, but low enough
  // to mitigate audible noise in some environments.
  LEDRainbowEffect.brightness(170);
  LEDRainbowWaveEffect.brightness(160);

  // Set the action key the test mode should listen for to Left Fn
  HardwareTestMode.setActionKey(R3C6);

  // The LED Stalker mode has a few effects. The one we like is called
  // 'BlazingTrail'. For details on other options, see
  // https://github.com/keyboardio/Kaleidoscope/blob/master/docs/plugins/LED-Stalker.md
  StalkerEffect.variant = STALKER(BlazingTrail);

  // To make the keymap editable without flashing new firmware, we store
  // additional layers in EEPROM. For now, we reserve space for eight layers. If
  // one wants to use these layers, just set the default layer to one in EEPROM,
  // by using the `settings.defaultLayer` Focus command, or by using the
  // `keymap.onlyCustom` command to use EEPROM layers only.
  //EEPROMKeymap.setup(8);

  // We need to tell the Colormap plugin how many layers we want to have custom
  // maps for. To make things simple, we set it to eight layers, which is how
  // many editable layers we have (see above).
  ColormapEffect.max_layers(8);

  // For Dynamic Macros, we need to reserve storage space for the editable
  // macros. A kilobyte is a reasonable default.
  //DynamicMacros.reserve_storage(1024);

  // If there's a default layer set in EEPROM, we should set that as the default
  // here.
  //Layer.move(EEPROMSettings.default_layer());

  // To avoid any surprises, SpaceCadet is turned off by default. However, it
  // can be permanently enabled via Chrysalis, so we should only disable it if
  // no configuration exists.
  //SpaceCadetConfig.disableSpaceCadetIfUnconfigured();

  // Editable layer names are stored in EEPROM too, and we reserve 16 bytes per
  // layer for them. We need one extra byte per layer for bookkeeping, so we
  // reserve 17 / layer in total.
  //LayerNames.reserve_storage(17 * 8);

  // Unless configured otherwise with Chrysalis, we want to make sure that the
  // firmware starts with LED effects off. This avoids over-taxing devices that
  // don't have a lot of power to share with USB devices
  DefaultLEDModeConfig.activateLEDModeIfUnconfigured(&LEDOff);

  // Turn off auto one-shotting of all modifiers and layer-shifts.
  OneShot.disableAutoOneShot();
}

/** loop is the second of the standard Arduino sketch functions.
  * As you might expect, it runs in a loop, never exiting.
  *
  * For Kaleidoscope-based keyboard firmware, you usually just want to
  * call Kaleidoscope.loop(); and not do anything custom here.
  */

void loop() {
  Kaleidoscope.loop();
}
