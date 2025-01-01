/*
 * keyboard.cpp
 * 
 * Keyboard scan code helper functions.
 * 
 * This software is copyright 2024-2025 by Gary Hammond (ZL3GH) along
 * with all the software bugs herein. It is free to use for
 * non-commercial purposes.
 * 
 * WARNING: DO NOT USE this software in any medical device or for any 
 * other mission critical purpose.
 * 
 * Use of this software could result in a universe ending paradox so 
 * use entirely at your own risk. No warranties or guarantees are 
 * expressed or implied.
 */

#include <Arduino.h>
#include <EEPROM.h>

#include "globals.h"

#include "eeprom_utils.h"

bool ext_keys_enabled = false;

static unsigned int i;

struct key_code
{
  byte key;
  byte xt_code;
  byte at_code;
};

// Standard non-extended key codes
static const struct key_code key_table[] =
{
  {1, 0x29, 0x0E},
  {2, 0x02, 0x16},
  {3, 0x03, 0x1E},
  {4, 0x04, 0x26},
  {5, 0x05, 0x25},
  {6, 0x06, 0x2E},
  {7, 0x07, 0x36},
  {8, 0x08, 0x3D},
  {9, 0x09, 0x3E},
  {10, 0x0A, 0x46},
  {11, 0x0B, 0x45},
  {12, 0x0C, 0x4E},
  {13, 0x0D, 0x55},
  {15, 0x0E, 0x66},
  {16, 0x0F, 0x0D},
  {17, 0x10, 0x15},
  {18, 0x11, 0x1D},
  {19, 0x12, 0x24},
  {20, 0x13, 0x2D},
  {21, 0x14, 0x2C},
  {22, 0x15, 0x35},
  {23, 0x16, 0x3C},
  {24, 0x17, 0x43},
  {25, 0x18, 0x44},
  {26, 0x19, 0x4D},
  {27, 0x1A, 0x54},
  {28, 0x1B, 0x5B},
  {29, 0x2B, 0x5D},
  {30, 0x3A, 0x58},
  {31, 0x1E, 0x1C},
  {32, 0x1F, 0x1B},
  {33, 0x20, 0x23},
  {34, 0x21, 0x2B},
  {35, 0x22, 0x34},
  {36, 0x23, 0x33},
  {37, 0x24, 0x3B},
  {38, 0x25, 0x42},
  {39, 0x26, 0x4B},
  {40, 0x27, 0x4C},
  {41, 0x28, 0x52},
  {42, 0x2B, 0x5D},
  {43, 0x1C, 0x5A},
  {44, 0x2A, 0x12},
  {45, 0x56, 0x61},
  {46, 0x2C, 0x1A},
  {47, 0x2D, 0x22},
  {48, 0x2E, 0x21},
  {49, 0x2F, 0x2A},
  {50, 0x30, 0x32},
  {51, 0x31, 0x31},
  {52, 0x32, 0x3A},
  {53, 0x33, 0x41},
  {54, 0x34, 0x49},
  {55, 0x35, 0x4A},
  {57, 0x36, 0x59},
  {58, 0x1D, 0x14},
  {60, 0x38, 0x11},
  {61, 0x39, 0x29},
  {90, 0x45, 0x77},
  {91, 0x47, 0x6C},
  {92, 0x4B, 0x6B},
  {93, 0x4F, 0x69},
  {96, 0x48, 0x75},
  {97, 0x4C, 0x73},
  {98, 0x50, 0x72},
  {99, 0x52, 0x70},
  {100, 0x37, 0x7C},
  {101, 0x49, 0x7D},
  {102, 0x4D, 0x74},
  {103, 0x51, 0x7A},
  {104, 0x53, 0x71},
  {105, 0x4A, 0x7B},
  {106, 0x4E, 0x79},
  {110, 0x01, 0x76},
  {112, 0x3B, 0x05},
  {113, 0x3C, 0x06},
  {114, 0x3D, 0x04},
  {115, 0x3E, 0x0C},
  {116, 0x3F, 0x03},
  {117, 0x40, 0x0B},
  {118, 0x41, 0x83},
  {119, 0x42, 0x0A},
  {120, 0x43, 0x01},
  {121, 0x44, 0x09},
  {122, 0x57, 0x78},
  {123, 0x58, 0x07},
  {125, 0x46, 0x7E},
  {0, 0x00, 0x00}
};

// Extended (pre-fixed with '0xEO) single byte key codes (Not 101+ extra navigation keys)
static const struct key_code key_table_ext[] =
{
  {59, 0x5B, 0x1F},   // Win
  {62, 0x38, 0x11},   // Right Alt
  {63, 0x5C, 0x27},   // Context Menu
  {64, 0x1D, 0x14},   // Right Shift
  {65, 0x5D, 0x2F},   // Alt Graphics
  {0, 0x00, 0x00}
};

// Extended (pre-fixed with '0xEO) single byte key codes 101+ extra navigation keys
static const struct key_code key_table_ext_nav[] =
{
  {0, 0xF0, 0xF0},    // key up
  {75, 0x52, 0x70},   // Insert
  {76, 0x53, 0x71},   // Delete
  {79, 0x4B, 0x6B},   // Left Arrow
  {80, 0x47, 0x6C},   // Home
  {81, 0x4F, 0x69},   // End
  {83, 0x48, 0x75},   // Up Arrow
  {84, 0x50, 0x72},   // Down Arrow
  {85, 0x49, 0x7D},   // Page Up
  {86, 0x51, 0x7A},   // Page Down
  {89, 0x4D, 0x74},   // Right Arrow
  {95, 0x35, 0x4A},   // Numpad /
  {108, 0x1C, 0x5A},  // Numpad Enter
  {0, 0x00, 0x00}
};

// Extended (pre-fixed with '0xEO) single byte key codes to have the 0xE0 stripped
static const struct key_code key_table_ext_strip[] =
{
  {95, 0x35, 0x4A},   // Numpad /
  {108, 0x1C, 0x5A},  // Numpad Enter
  {0, 0x00, 0x00}
};

//*************************************************************************
byte AT2XT(byte scan_code)
{
  for(i = 0; i < sizeof(key_table); i++)
  {
    if (key_table[i].at_code == scan_code)
    {
      return key_table[i].xt_code;
    }
  }
  return 0;
}

//*************************************************************************
byte AT2XTExt(byte scan_code)
{
  for(i = 0; i < sizeof(key_table_ext); i++)
  {
    if (key_table_ext[i].at_code == scan_code)
    {
      return key_table_ext[i].xt_code;
    }
  }
  return 0;
}

//*************************************************************************
byte AT2XTExtNav(byte scan_code)
{
  for(i = 0; i < sizeof(key_table_ext_nav); i++)
  {
    if (key_table_ext_nav[i].at_code == scan_code)
    {
      return key_table_ext_nav[i].xt_code;
    }
  }
  return 0;
}

//*************************************************************************
byte AT2XTExtStrip(byte scan_code)
{
  for(i = 0; i < sizeof(key_table_ext_strip); i++)
  {
    if (key_table_ext_strip[i].at_code == scan_code)
    {
      return key_table_ext_strip[i].xt_code;
    }
  }
  return 0;
}

//*************************************************************************
void k101Enabled(const bool value)
{
  if (value)
  {
    ext_keys_enabled = 1;
  }
  else
  {
    ext_keys_enabled = 0;
  }
  EEPROM.put(E_EXT_KEYS_ENABLED, ext_keys_enabled);
  eUpdateCrc();
}

//*************************************************************************
bool kGet101Enabled()
{
  EEPROM.get(E_EXT_KEYS_ENABLED, ext_keys_enabled);
  if (ext_keys_enabled == 0)
  {
    return false;
  }
  else
  {
    return true;
  }
}
