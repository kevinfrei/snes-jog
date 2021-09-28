#pragma once

// Bit numbers in the switch value
#define BTN_NONE 0
#define BTN_UP 1
#define BTN_LEFT (1 << 1)
#define BTN_DOWN (1 << 14)
#define BTN_RIGHT (1 << 15)
#define BTN_X (1 << 3)
#define BTN_A (1 << 4)
#define BTN_Y (1 << 5)
#define BTN_B (1 << 6)
#define BTN_RBUMP (1 << 9)
#define BTN_LBUMP (1 << 13)
#define BTN_START (1 << 10)
#define BTN_SELECT (1 << 12)
#define BTN_JOGS (BTN_UP | BTN_DOWN | BTN_LEFT | BTN_RIGHT | BTN_A | BTN_B)
#define BTN_BBUMPS (BTN_LBUMP | BTN_RBUMP)
#define BTN_REMOVEBUMPS(val) ((val) & ~BTN_BBUMPS)
#define BTN_DPAD (BTN_UP | BTN_DOWN | BTN_LEFT | BTN_RIGHT)
#if 0
// This is helpful if you're trying to debug
const char* names[16] = {"U",
                         "L",
                         "NA-2",
                         "X",
                         "A",
                         "Y",
                         "B",
                         "NA-7",
                         "NA-8",
                         "BR",
                         "Start",
                         "NA-11",
                         "Select",
                         "BL",
                         "D",
                         "R"};
#endif
