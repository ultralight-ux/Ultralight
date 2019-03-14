///
/// @file KeyCodes.h
///
/// @brief The header for the KeyCodes definitions.
///
/// @author
///
/// This file is a part of Ultralight, a fast, lightweight, HTML UI engine
///
/// Website: <http://ultralig.ht>
///
/// Copyright (C) 2019 Ultralight, Inc. All rights reserved.
///
#pragma once

namespace ultralight {

///
/// Namespace containing all the key-code definitions for KeyboardEvent.
/// Most of these correspond directly to the key-code values on Windows.
  ///
namespace KeyCodes {

  // GK_BACK (08) BACKSPACE key
  const int GK_BACK = 0x08;

  // GK_TAB (09) TAB key
  const int GK_TAB = 0x09;

  // GK_CLEAR (0C) CLEAR key
  const int GK_CLEAR = 0x0C;

  // GK_RETURN (0D)
  const int GK_RETURN = 0x0D;

  // GK_SHIFT (10) SHIFT key
  const int GK_SHIFT = 0x10;

  // GK_CONTROL (11) CTRL key
  const int GK_CONTROL = 0x11;

  // GK_MENU (12) ALT key
  const int GK_MENU = 0x12;

  // GK_PAUSE (13) PAUSE key
  const int GK_PAUSE = 0x13;

  // GK_CAPITAL (14) CAPS LOCK key
  const int GK_CAPITAL = 0x14;

  // GK_KANA (15) Input Method Editor (IME) Kana mode
  const int GK_KANA = 0x15;

  // GK_HANGUEL (15) IME Hanguel mode (maintained for compatibility; use GK_HANGUL)
  // GK_HANGUL (15) IME Hangul mode
  const int GK_HANGUL = 0x15;

  // GK_JUNJA (17) IME Junja mode
  const int GK_JUNJA = 0x17;

  // GK_FINAL (18) IME final mode
  const int GK_FINAL = 0x18;

  // GK_HANJA (19) IME Hanja mode
  const int GK_HANJA = 0x19;

  // GK_KANJI (19) IME Kanji mode
  const int GK_KANJI = 0x19;

  // GK_ESCAPE (1B) ESC key
  const int GK_ESCAPE = 0x1B;

  // GK_CONVERT (1C) IME convert
  const int GK_CONVERT = 0x1C;

  // GK_NONCONVERT (1D) IME nonconvert
  const int GK_NONCONVERT = 0x1D;

  // GK_ACCEPT (1E) IME accept
  const int GK_ACCEPT = 0x1E;

  // GK_MODECHANGE (1F) IME mode change request
  const int GK_MODECHANGE = 0x1F;

  // GK_SPACE (20) SPACEBAR
  const int GK_SPACE = 0x20;

  // GK_PRIOR (21) PAGE UP key
  const int GK_PRIOR = 0x21;

  // GK_NEXT (22) PAGE DOWN key
  const int GK_NEXT = 0x22;

  // GK_END (23) END key
  const int GK_END = 0x23;

  // GK_HOME (24) HOME key
  const int GK_HOME = 0x24;

  // GK_LEFT (25) LEFT ARROW key
  const int GK_LEFT = 0x25;

  // GK_UP (26) UP ARROW key
  const int GK_UP = 0x26;

  // GK_RIGHT (27) RIGHT ARROW key
  const int GK_RIGHT = 0x27;

  // GK_DOWN (28) DOWN ARROW key
  const int GK_DOWN = 0x28;

  // GK_SELECT (29) SELECT key
  const int GK_SELECT = 0x29;

  // GK_PRINT (2A) PRINT key
  const int GK_PRINT = 0x2A;

  // GK_EXECUTE (2B) EXECUTE key
  const int GK_EXECUTE = 0x2B;

  // GK_SNAPSHOT (2C) PRINT SCREEN key
  const int GK_SNAPSHOT = 0x2C;

  // GK_INSERT (2D) INS key
  const int GK_INSERT = 0x2D;

  // GK_DELETE (2E) DEL key
  const int GK_DELETE = 0x2E;

  // GK_HELP (2F) HELP key
  const int GK_HELP = 0x2F;

  // (30) 0 key
  const int GK_0 = 0x30;

  // (31) 1 key
  const int GK_1 = 0x31;

  // (32) 2 key
  const int GK_2 = 0x32;

  // (33) 3 key
  const int GK_3 = 0x33;

  // (34) 4 key
  const int GK_4 = 0x34;

  // (35) 5 key;
  const int GK_5 = 0x35;

  // (36) 6 key
  const int GK_6 = 0x36;

  // (37) 7 key
  const int GK_7 = 0x37;

  // (38) 8 key
  const int GK_8 = 0x38;

  // (39) 9 key
  const int GK_9 = 0x39;

  // (41) A key
  const int GK_A = 0x41;

  // (42) B key
  const int GK_B = 0x42;

  // (43) C key
  const int GK_C = 0x43;

  // (44) D key
  const int GK_D = 0x44;

  // (45) E key
  const int GK_E = 0x45;

  // (46) F key
  const int GK_F = 0x46;

  // (47) G key
  const int GK_G = 0x47;

  // (48) H key
  const int GK_H = 0x48;

  // (49) I key
  const int GK_I = 0x49;

  // (4A) J key
  const int GK_J = 0x4A;

  // (4B) K key
  const int GK_K = 0x4B;

  // (4C) L key
  const int GK_L = 0x4C;

  // (4D) M key
  const int GK_M = 0x4D;

  // (4E) N key
  const int GK_N = 0x4E;

  // (4F) O key
  const int GK_O = 0x4F;

  // (50) P key
  const int GK_P = 0x50;

  // (51) Q key
  const int GK_Q = 0x51;

  // (52) R key
  const int GK_R = 0x52;

  // (53) S key
  const int GK_S = 0x53;

  // (54) T key
  const int GK_T = 0x54;

  // (55) U key
  const int GK_U = 0x55;

  // (56) V key
  const int GK_V = 0x56;

  // (57) W key
  const int GK_W = 0x57;

  // (58) X key
  const int GK_X = 0x58;

  // (59) Y key
  const int GK_Y = 0x59;

  // (5A) Z key
  const int GK_Z = 0x5A;

  // GK_LWIN (5B) Left Windows key (Microsoft Natural keyboard)
  const int GK_LWIN = 0x5B;

  // GK_RWIN (5C) Right Windows key (Natural keyboard)
  const int GK_RWIN = 0x5C;

  // GK_APPS (5D) Applications key (Natural keyboard)
  const int GK_APPS = 0x5D;

  // GK_SLEEP (5F) Computer Sleep key
  const int GK_SLEEP = 0x5F;

  // GK_NUMPAD0 (60) Numeric keypad 0 key
  const int GK_NUMPAD0 = 0x60;

  // GK_NUMPAD1 (61) Numeric keypad 1 key
  const int GK_NUMPAD1 = 0x61;

  // GK_NUMPAD2 (62) Numeric keypad 2 key
  const int GK_NUMPAD2 = 0x62;

  // GK_NUMPAD3 (63) Numeric keypad 3 key
  const int GK_NUMPAD3 = 0x63;

  // GK_NUMPAD4 (64) Numeric keypad 4 key
  const int GK_NUMPAD4 = 0x64;

  // GK_NUMPAD5 (65) Numeric keypad 5 key
  const int GK_NUMPAD5 = 0x65;

  // GK_NUMPAD6 (66) Numeric keypad 6 key
  const int GK_NUMPAD6 = 0x66;

  // GK_NUMPAD7 (67) Numeric keypad 7 key
  const int GK_NUMPAD7 = 0x67;

  // GK_NUMPAD8 (68) Numeric keypad 8 key
  const int GK_NUMPAD8 = 0x68;

  // GK_NUMPAD9 (69) Numeric keypad 9 key
  const int GK_NUMPAD9 = 0x69;

  // GK_MULTIPLY (6A) Multiply key
  const int GK_MULTIPLY = 0x6A;

  // GK_ADD (6B) Add key
  const int GK_ADD = 0x6B;

  // GK_SEPARATOR (6C) Separator key
  const int GK_SEPARATOR = 0x6C;

  // GK_SUBTRACT (6D) Subtract key
  const int GK_SUBTRACT = 0x6D;

  // GK_DECIMAL (6E) Decimal key
  const int GK_DECIMAL = 0x6E;

  // GK_DIVIDE (6F) Divide key
  const int GK_DIVIDE = 0x6F;

  // GK_F1 (70) F1 key
  const int GK_F1 = 0x70;

  // GK_F2 (71) F2 key
  const int GK_F2 = 0x71;

  // GK_F3 (72) F3 key
  const int GK_F3 = 0x72;

  // GK_F4 (73) F4 key
  const int GK_F4 = 0x73;

  // GK_F5 (74) F5 key
  const int GK_F5 = 0x74;

  // GK_F6 (75) F6 key
  const int GK_F6 = 0x75;

  // GK_F7 (76) F7 key
  const int GK_F7 = 0x76;

  // GK_F8 (77) F8 key
  const int GK_F8 = 0x77;

  // GK_F9 (78) F9 key
  const int GK_F9 = 0x78;

  // GK_F10 (79) F10 key
  const int GK_F10 = 0x79;

  // GK_F11 (7A) F11 key
  const int GK_F11 = 0x7A;

  // GK_F12 (7B) F12 key
  const int GK_F12 = 0x7B;

  // GK_F13 (7C) F13 key
  const int GK_F13 = 0x7C;

  // GK_F14 (7D) F14 key
  const int GK_F14 = 0x7D;

  // GK_F15 (7E) F15 key
  const int GK_F15 = 0x7E;

  // GK_F16 (7F) F16 key
  const int GK_F16 = 0x7F;

  // GK_F17 (80H) F17 key
  const int GK_F17 = 0x80;

  // GK_F18 (81H) F18 key
  const int GK_F18 = 0x81;

  // GK_F19 (82H) F19 key
  const int GK_F19 = 0x82;

  // GK_F20 (83H) F20 key
  const int GK_F20 = 0x83;

  // GK_F21 (84H) F21 key
  const int GK_F21 = 0x84;

  // GK_F22 (85H) F22 key
  const int GK_F22 = 0x85;

  // GK_F23 (86H) F23 key
  const int GK_F23 = 0x86;

  // GK_F24 (87H) F24 key
  const int GK_F24 = 0x87;

  // GK_NUMLOCK (90) NUM LOCK key
  const int GK_NUMLOCK = 0x90;

  // GK_SCROLL (91) SCROLL LOCK key
  const int GK_SCROLL = 0x91;

  // GK_LSHIFT (A0) Left SHIFT key
  const int GK_LSHIFT = 0xA0;

  // GK_RSHIFT (A1) Right SHIFT key
  const int GK_RSHIFT = 0xA1;

  // GK_LCONTROL (A2) Left CONTROL key
  const int GK_LCONTROL = 0xA2;

  // GK_RCONTROL (A3) Right CONTROL key
  const int GK_RCONTROL = 0xA3;

  // GK_LMENU (A4) Left MENU key
  const int GK_LMENU = 0xA4;

  // GK_RMENU (A5) Right MENU key
  const int GK_RMENU = 0xA5;

  // GK_BROWSER_BACK (A6) Windows 2000/XP: Browser Back key
  const int GK_BROWSER_BACK = 0xA6;

  // GK_BROWSER_FORWARD (A7) Windows 2000/XP: Browser Forward key
  const int GK_BROWSER_FORWARD = 0xA7;

  // GK_BROWSER_REFRESH (A8) Windows 2000/XP: Browser Refresh key
  const int GK_BROWSER_REFRESH = 0xA8;

  // GK_BROWSER_STOP (A9) Windows 2000/XP: Browser Stop key
  const int GK_BROWSER_STOP = 0xA9;

  // GK_BROWSER_SEARCH (AA) Windows 2000/XP: Browser Search key
  const int GK_BROWSER_SEARCH = 0xAA;

  // GK_BROWSER_FAVORITES (AB) Windows 2000/XP: Browser Favorites key
  const int GK_BROWSER_FAVORITES = 0xAB;

  // GK_BROWSER_HOME (AC) Windows 2000/XP: Browser Start and Home key
  const int GK_BROWSER_HOME = 0xAC;

  // GK_VOLUME_MUTE (AD) Windows 2000/XP: Volume Mute key
  const int GK_VOLUME_MUTE = 0xAD;

  // GK_VOLUME_DOWN (AE) Windows 2000/XP: Volume Down key
  const int GK_VOLUME_DOWN = 0xAE;

  // GK_VOLUME_UP (AF) Windows 2000/XP: Volume Up key
  const int GK_VOLUME_UP = 0xAF;

  // GK_MEDIA_NEXT_TRACK (B0) Windows 2000/XP: Next Track key
  const int GK_MEDIA_NEXT_TRACK = 0xB0;

  // GK_MEDIA_PREV_TRACK (B1) Windows 2000/XP: Previous Track key
  const int GK_MEDIA_PREV_TRACK = 0xB1;

  // GK_MEDIA_STOP (B2) Windows 2000/XP: Stop Media key
  const int GK_MEDIA_STOP = 0xB2;

  // GK_MEDIA_PLAY_PAUSE (B3) Windows 2000/XP: Play/Pause Media key
  const int GK_MEDIA_PLAY_PAUSE = 0xB3;

  // GK_LAUNCH_MAIL (B4) Windows 2000/XP: Start Mail key
  const int GK_MEDIA_LAUNCH_MAIL = 0xB4;

  // GK_LAUNCH_MEDIA_SELECT (B5) Windows 2000/XP: Select Media key
  const int GK_MEDIA_LAUNCH_MEDIA_SELECT = 0xB5;

  // GK_LAUNCH_APP1 (B6) Windows 2000/XP: Start Application 1 key
  const int GK_MEDIA_LAUNCH_APP1 = 0xB6;

  // GK_LAUNCH_APP2 (B7) Windows 2000/XP: Start Application 2 key
  const int GK_MEDIA_LAUNCH_APP2 = 0xB7;

  // GK_OEM_1 (BA) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ';:' key
  const int GK_OEM_1 = 0xBA;

  // GK_OEM_PLUS (BB) Windows 2000/XP: For any country/region, the '+' key
  const int GK_OEM_PLUS = 0xBB;

  // GK_OEM_COMMA (BC) Windows 2000/XP: For any country/region, the ',' key
  const int GK_OEM_COMMA = 0xBC;

  // GK_OEM_MINUS (BD) Windows 2000/XP: For any country/region, the '-' key
  const int GK_OEM_MINUS = 0xBD;

  // GK_OEM_PERIOD (BE) Windows 2000/XP: For any country/region, the '.' key
  const int GK_OEM_PERIOD = 0xBE;

  // GK_OEM_2 (BF) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '/?' key
  const int GK_OEM_2 = 0xBF;

  // GK_OEM_3 (C0) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '`~' key
  const int GK_OEM_3 = 0xC0;

  // GK_OEM_4 (DB) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '[{' key
  const int GK_OEM_4 = 0xDB;

  // GK_OEM_5 (DC) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the '\|' key
  const int GK_OEM_5 = 0xDC;

  // GK_OEM_6 (DD) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the ']}' key
  const int GK_OEM_6 = 0xDD;

  // GK_OEM_7 (DE) Used for miscellaneous characters; it can vary by keyboard. Windows 2000/XP: For the US standard keyboard, the 'single-quote/double-quote' key
  const int GK_OEM_7 = 0xDE;

  // GK_OEM_8 (DF) Used for miscellaneous characters; it can vary by keyboard.
  const int GK_OEM_8 = 0xDF;

  // GK_OEM_102 (E2) Windows 2000/XP: Either the angle bracket key or the backslash key on the RT 102-key keyboard
  const int GK_OEM_102 = 0xE2;

  // GK_PROCESSKEY (E5) Windows 95/98/Me, Windows NT 4.0, Windows 2000/XP: IME PROCESS key
  const int GK_PROCESSKEY = 0xE5;

  // GK_PACKET (E7) Windows 2000/XP: Used to pass Unicode characters as if they were keystrokes. The GK_PACKET key is the low word of a 32-bit Virtual Key value used for non-keyboard input methods. For more information, see Remark in KEYBDINPUT,SendInput, WM_KEYDOWN, and WM_KEYUP
  const int GK_PACKET = 0xE7;

  // GK_ATTN (F6) Attn key
  const int GK_ATTN = 0xF6;

  // GK_CRSEL (F7) CrSel key
  const int GK_CRSEL = 0xF7;

  // GK_EXSEL (F8) ExSel key
  const int GK_EXSEL = 0xF8;

  // GK_EREOF (F9) Erase EOF key
  const int GK_EREOF = 0xF9;

  // GK_PLAY (FA) Play key
  const int GK_PLAY = 0xFA;

  // GK_ZOOM (FB) Zoom key
  const int GK_ZOOM = 0xFB;

  // GK_NONAME (FC) Reserved for future use
  const int GK_NONAME = 0xFC;

  // GK_PA1 (FD) PA1 key
  const int GK_PA1 = 0xFD;

  // GK_OEM_CLEAR (FE) Clear key
  const int GK_OEM_CLEAR = 0xFE;

  const int GK_UNKNOWN = 0;

} // namespace KeyCodes

} // namespace ultralight
