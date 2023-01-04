#include <Arduino.h>

// clang-format off
const int favico_ico_length = 4286;
const byte favico_ico[] PROGMEM = {0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x20, 0x20, 0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0xA8, 0x10, 0x00, 0x00, 0x16, 0x00, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFA, 0xFF, 0xFF, 0xFF, 0xFC, 0xFD, 0xEC, 0xCB, 0xFC, 0xFA, 0xC5, 0x5B, 0xFC, 0xF9, 0xBC, 0x3B, 0xFC, 0xFB, 0xCC, 0x70, 0xFC, 0xFD, 0xE9, 0xC1, 0xFC, 0xFF, 0xFC, 0xF6, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFD, 0xF8, 0xFC, 0xFE, 0xF4, 0xDE, 0xFC, 0xFE, 0xF5, 0xE0, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF5, 0xE1, 0xFC, 0xFE, 0xF7, 0xE7, 0xFC, 0xFF, 0xFE, 0xFD, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFA, 0xFF, 0xFF, 0xFF, 0xFC, 0xFE, 0xF3, 0xDE, 0xFF, 0xFA, 0xC6, 0x64, 0xFF, 0xF8, 0xB2, 0x21, 0xFF, 0xF8, 0xAA, 0x07, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF8, 0xAF, 0x14, 0xFF, 0xFA, 0xC4, 0x59, 0xFF, 0xFD, 0xE7, 0xBD, 0xFF, 0xFF, 0xFD, 0xFA, 0xFF, 0xFF, 0xFB, 0xF3, 0xFF, 0xFC, 0xD3, 0x88, 0xFF, 0xF9, 0xB4, 0x30, 0xFF, 0xF9, 0xB4, 0x26, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB5, 0x29, 0xFF, 0xF9, 0xB4, 0x26, 0xFF, 0xFA, 0xC2, 0x59, 0xFF, 0xFF, 0xFB, 0xF2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFD, 0xFC, 0xFD, 0xEA, 0xC7, 0xFF, 0xFE, 0xED, 0xCE, 0xFF, 0xFE, 0xF5, 0xE2, 0xFF, 0xFC, 0xE2, 0xB0, 0xFF, 0xFA, 0xC4, 0x58, 0xFF, 0xF8, 0xAD, 0x12, 0xFF, 0xF8, 0xA7, 0x00, 0xFF, 0xF8, 0xAD, 0x11, 0xFF, 0xFB, 0xCA, 0x6A, 0xFF, 0xFE, 0xF1, 0xD9, 0xFF, 0xFF, 0xFC, 0xF6, 0xFF, 0xFC, 0xD8, 0x94, 0xFF, 0xF9, 0xAE, 0x17, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF9, 0xBA, 0x3B, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFD, 0xE9, 0xC0, 0xFC, 0xF8, 0xB1, 0x26, 0xFF, 0xFC, 0xD6, 0x8E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF8, 0xFF, 0xFD, 0xE8, 0xC0, 0xFF, 0xFA, 0xC2, 0x50, 0xFF, 0xF8, 0xAA, 0x08, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB4, 0x29, 0xFF, 0xFC, 0xDE, 0xA4, 0xFF, 0xFF, 0xFC, 0xF8, 0xFF, 0xFD, 0xE6, 0xB9, 0xFF, 0xF9, 0xB6, 0x2E, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xD6, 0x88, 0xFC, 0xF8, 0xA8, 0x00, 0xFF, 0xFA, 0xC4, 0x54, 0xFF, 0xFF, 0xFD, 0xF9, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xF4, 0xFF, 0xFC, 0xDD, 0xA0, 0xFF, 0xF9, 0xB5, 0x2B, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF8, 0xAB, 0x0B, 0xFF, 0xFB, 0xCC, 0x71, 0xFF, 0xFE, 0xF8, 0xEB, 0xFF, 0xFE, 0xEF, 0xD5, 0xFF, 0xFA, 0xBD, 0x43, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFD, 0xE6, 0xB8, 0xFC, 0xF8, 0xAD, 0x0F, 0xFF, 0xF9, 0xB1, 0x1A, 0xFF, 0xFE, 0xF0, 0xD3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF0, 0xD5, 0xFF, 0xFD, 0xE5, 0xB9, 0xFF, 0xFF, 0xFE, 0xFC, 0xFF, 0xFE, 0xF1, 0xD7, 0xFF, 0xFA, 0xC2, 0x54, 0xFF, 0xF8, 0xAA, 0x04, 0xFF, 0xF8, 0xA9, 0x01, 0xFF, 0xFA, 0xBE, 0x49, 0xFF, 0xFE, 0xF0, 0xD8, 0xFF, 0xFE, 0xF6, 0xE5, 0xFF, 0xFA, 0xC5, 0x5C, 0xFF, 0xF8, 0xA9, 0x03, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFA, 0xF1, 0xFC, 0xFA, 0xBF, 0x46, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xFB, 0xD3, 0x81, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE6, 0xB8, 0xFF, 0xF8, 0xAD, 0x18, 0xFF, 0xFB, 0xCC, 0x70, 0xFF, 0xFE, 0xF4, 0xE2, 0xFF, 0xFF, 0xFA, 0xF2, 0xFF, 0xFB, 0xD2, 0x81, 0xFF, 0xF8, 0xAD, 0x10, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB6, 0x2E, 0xFF, 0xFD, 0xE7, 0xBD, 0xFF, 0xFF, 0xF9, 0xEF, 0xFF, 0xFB, 0xC9, 0x69, 0xFF, 0xF8, 0xAA, 0x04, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFC, 0xDC, 0x9B, 0xFF, 0xF8, 0xA9, 0x01, 0xFF, 0xF9, 0xB4, 0x27, 0xFF, 0xFE, 0xF3, 0xDB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF9, 0xB8, 0x35, 0xFF, 0xFD, 0xE6, 0xBA, 0xFF, 0xFF, 0xFD, 0xF9, 0xFF, 0xFC, 0xE0, 0xA7, 0xFF, 0xF9, 0xB1, 0x20, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB1, 0x1E, 0xFF, 0xFD, 0xE1, 0xAC, 0xFF, 0xFF, 0xF9, 0xEE, 0xFF, 0xFB, 0xCA, 0x6B, 0xFF, 0xF8, 0xA9, 0x04, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFE, 0xF7, 0xE7, 0xFF, 0xF9, 0xB8, 0x35, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xFB, 0xCE, 0x75, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF8, 0xAE, 0x14, 0xFF, 0xFC, 0xD5, 0x89, 0xFF, 0xFF, 0xFB, 0xF4, 0xFF, 0xFD, 0xE9, 0xC4, 0xFF, 0xF9, 0xB7, 0x31, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB0, 0x1E, 0xFF, 0xFE, 0xEF, 0xD3, 0xFF, 0xFF, 0xFA, 0xF2, 0xFF, 0xFB, 0xC9, 0x68, 0xFF, 0xF8, 0xA9, 0x03, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xDC, 0x9D, 0xFF, 0xF8, 0xAA, 0x05, 0xFF, 0xF8, 0xAF, 0x15, 0xFF, 0xFD, 0xE9, 0xC2, 0xFF, 0xFD, 0xE8, 0xBC, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x06, 0xFF, 0xFA, 0xC6, 0x5F, 0xFF, 0xFE, 0xF5, 0xE4, 0xFF, 0xFE, 0xF0, 0xD5, 0xFF, 0xFA, 0xBF, 0x4B, 0xFF, 0xF8, 0xAC, 0x12, 0xFF, 0xFC, 0xD9, 0x95, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFA, 0xF0, 0xFF, 0xFB, 0xCA, 0x6B, 0xFF, 0xF8, 0xAA, 0x03, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF9, 0xEF, 0xFF, 0xFA, 0xBF, 0x48, 0xFF, 0xF8, 0xA7, 0x00, 0xFF, 0xFA, 0xBE, 0x46, 0xFF, 0xFC, 0xDF, 0xA3, 0xFF, 0xF8, 0xAC, 0x09, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x01, 0xFF, 0xF9, 0xBB, 0x3D, 0xFF, 0xFE, 0xEE, 0xCF, 0xFF, 0xFF, 0xFA, 0xF2, 0xFF, 0xFD, 0xEC, 0xCA, 0xFF, 0xFE, 0xEE, 0xCE, 0xFF, 0xFE, 0xED, 0xCF, 0xFF, 0xFC, 0xD5, 0x8A, 0xFF, 0xFC, 0xE0, 0xA8, 0xFF, 0xFB, 0xC5, 0x5C, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xBA, 0x3B, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE8, 0xBE, 0xFF, 0xF9, 0xAE, 0x15, 0xFF, 0xF8, 0xAB, 0x0B, 0xFF, 0xFB, 0xCD, 0x70, 0xFF, 0xF8, 0xAD, 0x0C, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF9, 0xB5, 0x2C, 0xFF, 0xFD, 0xE7, 0xBC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF6, 0xE4, 0xFF, 0xF9, 0xB8, 0x35, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB0, 0x1B, 0xFF, 0xFD, 0xE3, 0xB1, 0xFF, 0xFA, 0xC1, 0x50, 0xFF, 0xF9, 0xB8, 0x3B, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFD, 0xFF, 0xFB, 0xD2, 0x80, 0xFF, 0xF8, 0xAA, 0x0D, 0xFF, 0xFB, 0xCC, 0x6C, 0xFF, 0xF8, 0xAD, 0x0C, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xB1, 0x1D, 0xFF, 0xFD, 0xE3, 0xB1, 0xFF, 0xFE, 0xEF, 0xD2, 0xFF, 0xF8, 0xAE, 0x17, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF8, 0xA9, 0x06, 0xFF, 0xFD, 0xE2, 0xAE, 0xFF, 0xFE, 0xF3, 0xDE, 0xFF, 0xFB, 0xCB, 0x74, 0xFF, 0xFF, 0xF9, 0xEE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF8, 0xEA, 0xFF, 0xFA, 0xBF, 0x51, 0xFF, 0xFB, 0xCA, 0x69, 0xFF, 0xF8, 0xAD, 0x0C, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xB0, 0x1B, 0xFF, 0xFC, 0xDF, 0xA5, 0xFF, 0xFB, 0xD2, 0x81, 0xFF, 0xF8, 0xB3, 0x27, 0xFF, 0xFA, 0xC6, 0x61, 0xFF, 0xFD, 0xE5, 0xB7, 0xFF, 0xFD, 0xE7, 0xBE, 0xFF, 0xFF, 0xF9, 0xEC, 0xFF, 0xFF, 0xFD, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xEE, 0xCF, 0xFF, 0xFB, 0xD5, 0x85, 0xFF, 0xF8, 0xAC, 0x0B, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xB0, 0x1B, 0xFF, 0xFD, 0xE2, 0xAB, 0xFF, 0xFE, 0xF7, 0xEA, 0xFF, 0xFD, 0xEC, 0xCA, 0xFF, 0xF9, 0xB6, 0x30, 0xFF, 0xF9, 0xB1, 0x1F, 0xFF, 0xFD, 0xE9, 0xC1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xB8, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xB1, 0x1E, 0xFF, 0xFD, 0xE7, 0xBC, 0xFF, 0xFE, 0xF1, 0xDA, 0xFF, 0xF9, 0xB7, 0x32, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB7, 0x31, 0xFF, 0xFE, 0xF1, 0xD7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xB4, 0x28, 0xFF, 0xFD, 0xEC, 0xCA, 0xFF, 0xFD, 0xE9, 0xC0, 0xFF, 0xF9, 0xB0, 0x1B, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xFA, 0xC2, 0x51, 0xFF, 0xFF, 0xF9, 0xED, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF9, 0xB9, 0x37, 0xFF, 0xFE, 0xF3, 0xDC, 0xFF, 0xFC, 0xDD, 0xA0, 0xFF, 0xF8, 0xAC, 0x0D, 0xFF, 0xF8, 0xA9, 0x02, 0xFF, 0xFB, 0xCF, 0x7A, 0xFF, 0xFF, 0xFD, 0xFA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xFA, 0xC2, 0x54, 0xFF, 0xFE, 0xF7, 0xE8, 0xFF, 0xFB, 0xCF, 0x78, 0xFF, 0xF8, 0xA9, 0x01, 0xFF, 0xF8, 0xAC, 0x0E, 0xFF, 0xFC, 0xE1, 0xAA, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x02, 0xFF, 0xFB, 0xCD, 0x73, 0xFF, 0xFE, 0xF5, 0xE5, 0xFF, 0xFA, 0xC0, 0x4C, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xF9, 0xB6, 0x2C, 0xFF, 0xFE, 0xF1, 0xD8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x0A, 0xFF, 0xFC, 0xDB, 0x9A, 0xFF, 0xFE, 0xED, 0xCD, 0xFF, 0xF9, 0xB2, 0x23, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xFB, 0xC8, 0x62, 0xFF, 0xFF, 0xFC, 0xF7, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF9, 0xBB, 0x3F, 0xFF, 0xFF, 0xFB, 0xF3, 0xFF, 0xFC, 0xDB, 0x99, 0xFF, 0xF8, 0xAA, 0x07, 0xFF, 0xF8, 0xAC, 0x0B, 0xFF, 0xFD, 0xE2, 0xAD, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3B, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFC, 0xF6, 0xFF, 0xFA, 0xC6, 0x5E, 0xFF, 0xF8, 0xA7, 0x00, 0xFF, 0xFA, 0xBC, 0x3E, 0xFF, 0xFF, 0xF8, 0xEB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xEF, 0xD2, 0xFF, 0xF9, 0xB2, 0x20, 0xFF, 0xF8, 0xA9, 0x02, 0xFF, 0xFC, 0xDB, 0x9A, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xD3, 0x82, 0xFF, 0xF8, 0xA8, 0x00, 0xFF, 0xFA, 0xBB, 0x3B, 0xFF, 0xFE, 0xF8, 0xEB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF4, 0xE0, 0xFF, 0xF9, 0xB6, 0x2C, 0xFF, 0xF8, 0xA9, 0x03, 0xFF, 0xFC, 0xDF, 0xA5, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFB, 0xD5, 0x86, 0xFF, 0xF8, 0xA7, 0x00, 0xFF, 0xFA, 0xC3, 0x52, 0xFF, 0xFF, 0xFC, 0xF6, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3C, 0xFF, 0xFF, 0xFA, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xF1, 0xD5, 0xFF, 0xF9, 0xB1, 0x1C, 0xFF, 0xF9, 0xB0, 0x19, 0xFF, 0xFE, 0xED, 0xCD, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xBA, 0xFF, 0xF8, 0xAB, 0x08, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAB, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF9, 0xBA, 0x3B, 0xFF, 0xFD, 0xEC, 0xCC, 0xFF, 0xFF, 0xFA, 0xF2, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xF9, 0xFF, 0xFA, 0xC2, 0x4F, 0xFF, 0xF8, 0xAB, 0x0E, 0xFF, 0xFD, 0xE8, 0xBF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xE7, 0xB9, 0xFF, 0xF8, 0xAA, 0x08, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xAA, 0x00, 0xFF, 0xF8, 0xA9, 0x00, 0xFF, 0xF9, 0xBA, 0x3D, 0xFF, 0xFA, 0xC7, 0x60, 0xFF, 0xFA, 0xBE, 0x4A, 0xFF, 0xFC, 0xE1, 0xAC, 0xFF, 0xFE, 0xF7, 0xEA, 0xFF, 0xFF, 0xFE, 0xFE, 0xFF, 0xFB, 0xCD, 0x70, 0xFF, 0xF9, 0xBA, 0x3D, 0xFF, 0xFE, 0xF7, 0xE8, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFD, 0xEC, 0xC8, 0xFF, 0xF9, 0xBB, 0x40, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBB, 0x3B, 0xFF, 0xF9, 0xBA, 0x38, 0xFF, 0xFB, 0xC8, 0x6A, 0xFF, 0xFA, 0xC7, 0x5F, 0xFF, 0xF8, 0xA6, 0x00, 0xFF, 0xF8, 0xAA, 0x07, 0xFF, 0xF9, 0xB5, 0x2F, 0xFF, 0xFC, 0xDE, 0xA5, 0xFF, 0xFC, 0xDA, 0x99, 0xFF, 0xFD, 0xEB, 0xCB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFA, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFF, 0xFF, 0xFC, 0xFF, 0xFD, 0xFA, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xF9, 0xEE, 0xFC, 0xFE, 0xFA, 0xF3, 0xFC, 0xFD, 0xEB, 0xC8, 0xFC, 0xFB, 0xCA, 0x69, 0xFC, 0xF9, 0xBA, 0x37, 0xFC, 0xFA, 0xC1, 0x4F, 0xFC, 0xFD, 0xEC, 0xCA, 0xFC, 0xFF, 0xF9, 0xED, 0xFC, 0xFF, 0xFF, 0xFE, 0xFC, 0xFF, 0xFF, 0xFF, 0xFA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};