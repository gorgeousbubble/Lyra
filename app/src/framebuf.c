/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       framebuf.c
 * @brief      128x64 monochrome OLED framebuffer drawing primitives
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "framebuf.h"

/* -----------------------------------------------------------------------
 * Shared global framebuffer (1024 bytes in BSS — zeroed at startup).
 * All Render_*() functions call fb_clear(g_fb) before writing pixels.
 * ----------------------------------------------------------------------- */
FrameBuf g_fb;

/* -----------------------------------------------------------------------
 * fb_pixel
 * ----------------------------------------------------------------------- */
void fb_pixel(FrameBuf screen, int x, int y)
{
    if ((unsigned)x < 128 && (unsigned)y < 64)
        screen[y][x >> 3] |= (uint8)(0x01 << (7 - (x & 7)));
}

/* -----------------------------------------------------------------------
 * fb_hline
 * ----------------------------------------------------------------------- */
void fb_hline(FrameBuf screen, int y, int x0, int x1)
{
    int x;
    for (x = x0; x <= x1; x++)
        fb_pixel(screen, x, y);
}

/* -----------------------------------------------------------------------
 * fb_vline
 * Auto-sorts y0/y1 so callers don't need to worry about direction.
 * ----------------------------------------------------------------------- */
void fb_vline(FrameBuf screen, int x, int y0, int y1)
{
    int y, tmp;
    if (y0 > y1) { tmp = y0; y0 = y1; y1 = tmp; }
    for (y = y0; y <= y1; y++)
        fb_pixel(screen, x, y);
}

/* -----------------------------------------------------------------------
 * fb_char6
 * Renders one character from Oled_FontLib_6x8 (6 columns × 8 rows).
 * Pixels outside the framebuffer bounds are silently clipped by fb_pixel.
 * ----------------------------------------------------------------------- */
void fb_char6(FrameBuf screen, int px, int py, char ch)
{
    int col, bit;
    uint8 c = (uint8)ch - 32;
    if (c >= 96) return;  /* printable ASCII 32..127 only */
    for (col = 0; col < 6; col++)
    {
        uint8 fb = Oled_FontLib_6x8[c][col];
        for (bit = 0; bit < 8; bit++)
            if (fb & (uint8)(1 << bit))
                fb_pixel(screen, px + col, py + bit);
    }
}

/* -----------------------------------------------------------------------
 * fb_str6
 * ----------------------------------------------------------------------- */
void fb_str6(FrameBuf screen, int px, int py, const char *s)
{
    while (*s)
    {
        fb_char6(screen, px, py, *s++);
        px += 6;
    }
}

/* -----------------------------------------------------------------------
 * fb_clear
 * ----------------------------------------------------------------------- */
void fb_clear(FrameBuf screen)
{
    int row, col;
    for (row = 0; row < 64; row++)
        for (col = 0; col < 16; col++)
            screen[row][col] = 0x00;
}
