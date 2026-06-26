/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       framebuf.h
 * @brief      128x64 monochrome OLED framebuffer drawing primitives
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 *
 * Provides a single canonical set of pixel-level drawing helpers for the
 * 128x64 OLED framebuffer (uint8 screen[64][16], bit 7 = leftmost pixel).
 *
 * Previously every Render_*() function contained its own private static
 * copy of these helpers (dp / dline_h / dline_v / dc6 / ds6).  That led
 * to 9 identical copies spread across 9 source files.  Any bug fix had to
 * be applied to all 9 independently.  This module is the single source of
 * truth; all Render_*() files now include this header and call these
 * functions instead.
 *
 * All functions are declared static inline so the compiler can still
 * inline them at each call site (zero function-call overhead), while the
 * definitions live in framebuf.c to avoid multiple-definition errors.
 */

#ifndef __FRAMEBUF_H__
#define __FRAMEBUF_H__

#include "common.h"
#include "oled_i2c.h"   /* Oled_FontLib_6x8 */

/* -----------------------------------------------------------------------
 * Framebuffer type alias
 * screen[row][byte_col], bit 7 of byte_col*8 = leftmost pixel in that byte
 * ----------------------------------------------------------------------- */
typedef uint8 FrameBuf[64][16];

/* -----------------------------------------------------------------------
 * Shared global framebuffer
 *
 * One 1024-byte buffer shared by all Render_*() functions.
 * This is safe because:
 *   - Only one screen is rendered per main-loop iteration (the active menu
 *     item drives a single Render call per frame).
 *   - All Render functions call fb_clear() at the start, then write pixels,
 *     then call Oled_I2C_Draw_Picture_128x64() — no two Render functions
 *     run concurrently.
 *
 * Placing it here (BSS/data segment) instead of on the stack saves 1 KB of
 * stack per call frame and eliminates the per-frame memset loop.
 * fb_clear() still zeroes the buffer before each use.
 *
 * Usage in any Render_*() function:
 *   fb_clear(g_fb);
 *   fb_pixel(g_fb, x, y);
 *   ...
 *   Oled_I2C_Draw_Picture_128x64((const uint8 *)g_fb);
 * ----------------------------------------------------------------------- */
extern FrameBuf g_fb;

/* -----------------------------------------------------------------------
 * Primitive declarations
 * ----------------------------------------------------------------------- */

/** Set one pixel at (x, y).  Clipped silently if out of bounds. */
void fb_pixel(FrameBuf screen, int x, int y);

/** Draw a horizontal line from x0 to x1 at row y (inclusive). */
void fb_hline(FrameBuf screen, int y, int x0, int x1);

/** Draw a vertical line from y0 to y1 at column x (inclusive, auto-sorted). */
void fb_vline(FrameBuf screen, int x, int y0, int y1);

/** Draw one 6x8 character from Oled_FontLib_6x8 at pixel position (px, py). */
void fb_char6(FrameBuf screen, int px, int py, char ch);

/** Draw a NUL-terminated ASCII string using 6x8 font at (px, py). */
void fb_str6(FrameBuf screen, int px, int py, const char *s);

/** Fill the entire framebuffer with 0x00 (all pixels off). */
void fb_clear(FrameBuf screen);

#endif /* __FRAMEBUF_H__ */
