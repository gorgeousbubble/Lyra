/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       attitude3d.c
 * @brief      3D attitude indicator: rotated XYZ axes on OLED using integer arithmetic
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "attitude3d.h"
#include "oled.h"
#include "oled_i2c.h"
#include <stdio.h>

/*
 * Integer fixed-point sin/cos table for 0..359 degrees.
 * Values are Q8 fixed-point: sin_table[a] = round(sin(a°) * 256)
 * Range: -256 .. +256
 */
static const int16 sin_table[360] = {
     0,   4,   9,  13,  18,  22,  27,  31,  36,  40,  44,  49,  53,  57,  62,  66,
    70,  75,  79,  83,  87,  91,  96, 100, 104, 108, 112, 116, 120, 124, 128, 132,
   136, 139, 143, 147, 150, 154, 158, 161, 165, 168, 171, 175, 178, 181, 184, 187,
   190, 193, 196, 199, 202, 205, 207, 210, 212, 215, 217, 220, 222, 224, 226, 228,
   230, 232, 234, 236, 237, 239, 241, 242, 243, 245, 246, 247, 248, 249, 250, 251,
   252, 252, 253, 254, 254, 254, 255, 255, 255, 255, 256, 255, 255, 255, 255, 254,
   254, 254, 253, 252, 252, 251, 250, 249, 248, 247, 246, 245, 243, 242, 241, 239,
   237, 236, 234, 232, 230, 228, 226, 224, 222, 220, 217, 215, 212, 210, 207, 205,
   202, 199, 196, 193, 190, 187, 184, 181, 178, 175, 171, 168, 165, 161, 158, 154,
   150, 147, 143, 139, 136, 132, 128, 124, 120, 116, 112, 108, 104, 100,  96,  91,
    87,  83,  79,  75,  70,  66,  62,  57,  53,  49,  44,  40,  36,  31,  27,  22,
    18,  13,   9,   4,   0,  -4,  -9, -13, -18, -22, -27, -31, -36, -40, -44, -49,
   -53, -57, -62, -66, -70, -75, -79, -83, -87, -91, -96,-100,-104,-108,-112,-116,
  -120,-124,-128,-132,-136,-139,-143,-147,-150,-154,-158,-161,-165,-168,-171,-175,
  -178,-181,-184,-187,-190,-193,-196,-199,-202,-205,-207,-210,-212,-215,-217,-220,
  -222,-224,-226,-228,-230,-232,-234,-236,-237,-239,-241,-242,-243,-245,-246,-247,
  -248,-249,-250,-251,-252,-252,-253,-254,-254,-254,-255,-255,-255,-255,-256,-255,
  -255,-255,-255,-254,-254,-254,-253,-252,-252,-251,-250,-249,-248,-247,-246,-245,
  -243,-242,-241,-239,-237,-236,-234,-232,-230,-228,-226,-224,-222,-220,-217,-215,
  -212,-210,-207,-205,-202,-199,-196,-193,-190,-187,-184,-181,-178,-175,-171,-168,
  -165,-161,-158,-154,-150,-147,-143,-139,-136,-132,-128,-124,-120,-116,-112,-108,
  -104,-100, -96, -91, -87, -83, -79, -75, -70, -66, -62, -57, -53, -49, -44, -40,
   -36, -31, -27, -22, -18, -13,  -9,  -4
};

/* cos(a) = sin(a+90) */
static inline int16 isin(int deg)
{
    deg = ((deg % 360) + 360) % 360;
    return sin_table[deg];
}
static inline int16 icos(int deg)
{
    return isin(deg + 90);
}

/* Draw a pixel into screen[64][16] frame buffer */
static void draw_pixel(uint8 screen[64][16], int x, int y)
{
    if (x >= 0 && x < 128 && y >= 0 && y < 64)
        screen[y][x >> 3] |= (0x01 << (7 - (x & 7)));
}

/*
 * Draw a line from (x0,y0) to (x1,y1) using Bresenham.
 * style: 0=solid, 1=dashed(4on/4off), 2=dotted(2on/2off)
 */
static void draw_line(uint8 screen[64][16], int x0, int y0, int x1, int y1, int style)
{
    int dx = x1 - x0, dy = y1 - y0;
    int adx = dx < 0 ? -dx : dx;
    int ady = dy < 0 ? -dy : dy;
    int sx = dx > 0 ? 1 : -1;
    int sy = dy > 0 ? 1 : -1;
    int err = adx - ady;
    int step = 0;

    while (1)
    {
        /* Style gate: solid=always, dashed=4on4off, dotted=2on2off */
        int draw = 1;
        if (style == 1) draw = ((step / 4) % 2 == 0);
        if (style == 2) draw = ((step / 2) % 2 == 0);

        if (draw) draw_pixel(screen, x0, y0);

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -ady) { err -= ady; x0 += sx; }
        if (e2 <  adx) { err += adx; y0 += sy; }
        step++;
    }
}

/*
 * Draw a 6x8 character into the frame buffer (same bit layout as all other Render_*)
 */
static void draw_char(uint8 screen[64][16], int px, int py, char ch)
{
    uint8 c = (uint8)ch - 32;
    if (c >= 96) return;
    for (int col = 0; col < 6; col++)
    {
        uint8 fb = Oled_FontLib_6x8[c][col];
        for (int bit = 0; bit < 8; bit++)
        {
            if (fb & (1 << bit))
                draw_pixel(screen, px + col, py + bit);
        }
    }
}

static void draw_str(uint8 screen[64][16], int px, int py, const char *s)
{
    while (*s) { draw_char(screen, px, py, *s++); px += 6; }
}

/*
 * Render_Attitude3D
 * -----------------
 * Draws rotating XYZ coordinate axes (left) and numeric readout (right).
 *
 * Rotation order: Yaw (Z) → Pitch (Y) → Roll (X)  (ZYX / Tait-Bryan)
 * The three axis endpoints in body frame (length AXIS_LEN in Q8):
 *   X body: (AXIS_LEN, 0, 0)
 *   Y body: (0, AXIS_LEN, 0)
 *   Z body: (0, 0, AXIS_LEN)
 *
 * Isometric screen projection (looking down at 30° from front-right):
 *   screen_x = proj_x * cos(30°) - proj_y * cos(30°)
 *   screen_y = proj_z * (-1) + (proj_x + proj_y) * sin(30°)/2
 * Simplified to two fixed vectors:
 *   screen_x += world_x * cx - world_y * cy
 *   screen_y += world_x * sx + world_y * sy - world_z * sz
 */
void Render_Attitude3D(float pitch, float roll, float yaw)
{
    uint8 screen[64][16];
    int i, col;
    for (i = 0; i < 64; i++)
        for (col = 0; col < 16; col++)
            screen[i][col] = 0x00;

    /* --- Convert angles to integer degrees (wrap) --- */
    int ip = (int)pitch; ip = ((ip % 360) + 360) % 360;
    int ir = (int)roll;  ir = ((ir % 360) + 360) % 360;
    int iy = (int)yaw;   iy = ((iy % 360) + 360) % 360;

    /* --- Axis length in pixels --- */
    const int L = 22; /* half-screen diagonal, looks good at 22px */

    /* --- Origin at screen centre of left area: (40, 32) --- */
    const int OX = 40;
    const int OY = 32;

    /*
     * Rotate a body-frame unit vector (bx, by, bz) by Roll→Pitch→Yaw
     * All trig values are Q8 (×256). Final projection divides by 256.
     *
     * Rotation matrix (ZYX): R = Rz(yaw) * Ry(pitch) * Rx(roll)
     *
     * We rotate three unit vectors and project to 2D:
     *   World X axis (right):   bx=(1,0,0)
     *   World Y axis (forward): by=(0,1,0)
     *   World Z axis (up):      bz=(0,0,1)
     *
     * Isometric projection onto screen:
     *   sx = world_x * cos(30) - world_y * cos(30)  ≈ (world_x - world_y)*221/256
     *   sy = -(world_z*256 - (world_x+world_y)*128) / 256
     *      = -world_z + (world_x + world_y)*0.5
     */

    /* Precompute trig values (Q8) */
    int16 sr = isin(ir), cr = icos(ir);
    int16 sp = isin(ip), cp = icos(ip);
    int16 sy_v = isin(iy), cy = icos(iy);

    /*
     * Rotation matrix elements (Q8×Q8=Q16, we keep in Q8 by >>8)
     * R[row][col]:
     *   R[0][0] =  cy*cp
     *   R[0][1] =  cy*sp*sr - sy_v*cr
     *   R[0][2] =  cy*sp*cr + sy_v*sr
     *   R[1][0] =  sy_v*cp
     *   R[1][1] =  sy_v*sp*sr + cy*cr
     *   R[1][2] =  sy_v*sp*cr - cy*sr
     *   R[2][0] = -sp
     *   R[2][1] =  cp*sr
     *   R[2][2] =  cp*cr
     * All values ×256; we accumulate in int32 and shift right by 8 each step.
     */
    /* R[col] = column vector for body X,Y,Z axes projected to world */
    /* world = R * body_unit  → for X-axis body=(1,0,0): world = R col0 */

    /* World coords for body X axis */
    int wx_x = ((int32)cy * cp) >> 8;
    int wx_y = ((int32)sy_v * cp) >> 8;
    int wx_z = -sp;   /* already Q8 */

    /* World coords for body Y axis */
    int wy_x = ((((int32)cy * sp * sr) >> 8) - (((int32)sy_v * cr) >> 8));
    int wy_y = ((((int32)sy_v * sp * sr) >> 8) + (((int32)cy * cr) >> 8));
    int wy_z = ((int32)cp * sr) >> 8;

    /* World coords for body Z axis */
    int wz_x = ((((int32)cy * sp * cr) >> 8) + (((int32)sy_v * sr) >> 8));
    int wz_y = ((((int32)sy_v * sp * cr) >> 8) - (((int32)cy * sr) >> 8));
    int wz_z = ((int32)cp * cr) >> 8;

    /*
     * Isometric 2D projection (Q8 → pixels):
     *   The "221/256 ≈ cos(30°)" factor and "128/256 = sin(30°)/2 = 0.5"
     *   are baked in as integer shifts.
     *
     *   screen_dx = (wx - wy) * 221 / 256 * L / 256
     *   screen_dy = -(wz * 256 - (wx+wy)*128) / 256 * L / 256
     *   → simplify: screen_dx = (wx-wy)*L*221/65536
     *                screen_dy = -(wz*L - (wx+wy)*L/2)/256
     */
    #define PROJ_X(wx_, wy_) ( ((int32)((wx_) - (wy_)) * L * 221) >> 16 )
    #define PROJ_Y(wx_, wy_, wz_) ( (-(int32)((wz_)*L - ((wx_)+(wy_))*L/2)) >> 8 )

    /* Endpoints in screen pixels */
    int ex = OX + (int)PROJ_X(wx_x, wx_y);
    int ey = OY + (int)PROJ_Y(wx_x, wx_y, wx_z);

    int eyx = OX + (int)PROJ_X(wy_x, wy_y);
    int eyy = OY + (int)PROJ_Y(wy_x, wy_y, wy_z);

    int ezx = OX + (int)PROJ_X(wz_x, wz_y);
    int ezy = OY + (int)PROJ_Y(wz_x, wz_y, wz_z);

    #undef PROJ_X
    #undef PROJ_Y

    /* --- Draw axes --- */
    /* X axis: solid, label "X" */
    draw_line(screen, OX, OY, ex, ey, 0);
    /* arrowhead: 3 pixels perpendicular at tip */
    draw_pixel(screen, ex, ey);
    draw_pixel(screen, ex + 1, ey);
    draw_pixel(screen, ex, ey + 1);
    draw_char(screen, ex + 2, ey - 3, 'X');

    /* Y axis: dashed, label "Y" */
    draw_line(screen, OX, OY, eyx, eyy, 1);
    draw_pixel(screen, eyx, eyy);
    draw_char(screen, eyx + 2, eyy - 3, 'Y');

    /* Z axis: dotted, label "Z" */
    draw_line(screen, OX, OY, ezx, ezy, 2);
    draw_pixel(screen, ezx, ezy);
    draw_char(screen, ezx + 2, ezy - 3, 'Z');

    /* Origin dot */
    draw_pixel(screen, OX,   OY);
    draw_pixel(screen, OX+1, OY);
    draw_pixel(screen, OX,   OY+1);
    draw_pixel(screen, OX+1, OY+1);

    /* --- Vertical separator line --- */
    for (i = 2; i < 62; i++)
        draw_pixel(screen, 82, i);

    /* --- Numeric readout (right side, x=85) --- */
    char buf[12];

    /* "P:" label + value */
    draw_str(screen, 85, 4, "P:");
    snprintf(buf, sizeof(buf), "%+.1f", pitch);
    draw_str(screen, 85, 12, buf);

    /* "R:" label + value */
    draw_str(screen, 85, 24, "R:");
    snprintf(buf, sizeof(buf), "%+.1f", roll);
    draw_str(screen, 85, 32, buf);

    /* "Y:" label + value */
    draw_str(screen, 85, 44, "Y:");
    snprintf(buf, sizeof(buf), "%+.1f", yaw);
    draw_str(screen, 85, 52, buf);

    /* Output frame */
    Oled_I2C_Draw_Picture_128x64((const uint8 *)screen);
}
