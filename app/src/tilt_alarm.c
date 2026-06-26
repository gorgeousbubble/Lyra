/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       tilt_alarm.c
 * @brief      Tilt alarm: monitors pitch/roll and sounds buzzer when threshold exceeded
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "tilt_alarm.h"
#include "beep.h"
#include "oled.h"
#include "oled_i2c.h"
#include <stdio.h>

/* Beep pattern: 200ms on / 300ms off when triggered */
#define BEEP_ON_MS   200
#define BEEP_OFF_MS  300
#define BEEP_PERIOD  (BEEP_ON_MS + BEEP_OFF_MS)

/* Global state */
TiltAlarmState TiltAlarm = {
    TILT_THRESHOLD_DEFAULT,  /* threshold */
    1,                       /* enabled (monitoring on by default) */
    0,                       /* triggered */
    0.0f,                    /* max_pitch */
    0.0f,                    /* max_roll  */
    0                        /* beep_counter */
};

/*
 * TiltAlarm_Update
 * ----------------
 * Called every 10ms from main loop.
 * - Checks |pitch| and |roll| against threshold
 * - Updates peak values
 * - Drives buzzer in non-blocking 200ms on / 300ms off pattern
 */
void TiltAlarm_Update(float pitch, float roll)
{
    float abs_pitch = pitch < 0.0f ? -pitch : pitch;
    float abs_roll  = roll  < 0.0f ? -roll  : roll;

    /* Update peak values regardless of enabled state */
    if (abs_pitch > TiltAlarm.max_pitch) TiltAlarm.max_pitch = abs_pitch;
    if (abs_roll  > TiltAlarm.max_roll)  TiltAlarm.max_roll  = abs_roll;

    if (!TiltAlarm.enabled)
    {
        /* Alarm disabled: ensure buzzer is off */
        TiltAlarm.triggered = 0;
        Beep_Off();
        return;
    }

    /* Check threshold */
    TiltAlarm.triggered = (abs_pitch >= TiltAlarm.threshold ||
                           abs_roll  >= TiltAlarm.threshold) ? 1 : 0;

    if (TiltAlarm.triggered)
    {
        /* Non-blocking beep: advance counter by 10ms each call */
        TiltAlarm.beep_counter += 10;
        if (TiltAlarm.beep_counter >= BEEP_PERIOD)
            TiltAlarm.beep_counter = 0;

        if (TiltAlarm.beep_counter < BEEP_ON_MS)
            Beep_On();
        else
            Beep_Off();
    }
    else
    {
        TiltAlarm.beep_counter = 0;
        Beep_Off();
    }
}

/*
 * TiltAlarm_Toggle
 * ----------------
 * Toggle alarm on/off. When turning off, silence buzzer immediately.
 */
void TiltAlarm_Toggle(void)
{
    TiltAlarm.enabled = !TiltAlarm.enabled;
    if (!TiltAlarm.enabled)
    {
        TiltAlarm.triggered    = 0;
        TiltAlarm.beep_counter = 0;
        Beep_Off();
    }
}

/* Increase threshold */
void TiltAlarm_Threshold_Inc(void)
{
    TiltAlarm.threshold += TILT_THRESHOLD_STEP;
    if (TiltAlarm.threshold > TILT_THRESHOLD_MAX)
        TiltAlarm.threshold = TILT_THRESHOLD_MAX;
}

/* Decrease threshold */
void TiltAlarm_Threshold_Dec(void)
{
    TiltAlarm.threshold -= TILT_THRESHOLD_STEP;
    if (TiltAlarm.threshold < TILT_THRESHOLD_MIN)
        TiltAlarm.threshold = TILT_THRESHOLD_MIN;
}

/* Reset peak values */
void TiltAlarm_Reset_Peak(void)
{
    TiltAlarm.max_pitch = 0.0f;
    TiltAlarm.max_roll  = 0.0f;
}

/* -----------------------------------------------------------------------
 * Render_TiltAlarm
 * -----------------------------------------------------------------------
 * 128x64 OLED layout:
 *
 *   Row 0   (page 0): "TILT ALARM"  centered
 *   Row 8   (page 1): "THRES: +/-XX.X deg"
 *   Row 18  (page 2): separator line
 *   Row 24  (page 3): "P: +XX.X  [!!]" (!! shown when triggered on pitch)
 *   Row 32  (page 4): "R: +XX.X  [!!]"
 *   Row 42  (page 5): "PEAK P: XX.X"
 *   Row 50  (page 6): "PEAK R: XX.X"
 *   Row 57  (page 7): "K0:ON/OFF  K2-/K3+"
 *
 * Frame-buffer approach — zero flicker.
 * ----------------------------------------------------------------------- */

/* Helper: draw one 6x8 char into screen[64][16] */
/* Thin aliases onto the shared framebuf module.
 * dc/ds/dline are kept as local aliases to avoid renaming all call sites
 * inside Render_TiltAlarm. dline is a simplified full-width hline that
 * attitude3d does not need, so it stays local rather than going into framebuf. */
#include "framebuf.h"
#define dc(s,px,py,ch)   fb_char6((s),(px),(py),(ch))
#define ds(s,px,py,str)  fb_str6((s),(px),(py),(str))

/* Full-width horizontal separator line (x=2..125) — tilt_alarm specific */
static void dline(uint8 screen[64][16], int y)
{
    fb_hline(screen, y, 2, 125);
}

void Render_TiltAlarm(float pitch, float roll)
{
    /* Use the shared global framebuffer (saves 1 KB of stack per call). */
    #define screen g_fb
    int i, col;
    fb_clear(g_fb);
    (void)i; (void)col; /* suppress unused-variable warnings */

    float abs_pitch = pitch < 0.0f ? -pitch : pitch;
    float abs_roll  = roll  < 0.0f ? -roll  : roll;
    uint8 pitch_over = (abs_pitch >= TiltAlarm.threshold) ? 1 : 0;
    uint8 roll_over  = (abs_roll  >= TiltAlarm.threshold) ? 1 : 0;

    char buf[20];

    /* --- Title: "TILT ALARM" centered (10*6=60px, start x=34) --- */
    ds(screen, 34, 0, "TILT ALARM");

    /* --- Alarm status: "[ON] " or "[OFF]" + threshold --- */
    snprintf(buf, sizeof(buf), "%s THR:%2d",
             TiltAlarm.enabled ? "[ON] " : "[OFF]",
             (int)TiltAlarm.threshold);
    ds(screen, 4, 8, buf);

    /* --- Separator --- */
    dline(screen, 18);

    /* --- Pitch row --- */
    snprintf(buf, sizeof(buf), "P:%+6.1f", pitch);
    ds(screen, 4, 22, buf);
    if (pitch_over)
        ds(screen, 88, 22, "<!>");

    /* --- Roll row --- */
    snprintf(buf, sizeof(buf), "R:%+6.1f", roll);
    ds(screen, 4, 32, buf);
    if (roll_over)
        ds(screen, 88, 32, "<!>");

    /* --- Separator --- */
    dline(screen, 43);

    /* --- Peak pitch --- */
    snprintf(buf, sizeof(buf), "PK P:%.1f", TiltAlarm.max_pitch);
    ds(screen, 4, 46, buf);

    /* --- Peak roll --- */
    snprintf(buf, sizeof(buf), "PK R:%.1f", TiltAlarm.max_roll);
    ds(screen, 68, 46, buf);

    /* --- Hint bar at bottom --- */
    ds(screen, 0, 56, "K0:ON/OFF K2:- K3:+");

    /* Output */
    Oled_I2C_Draw_Picture_128x64((const uint8 *)screen);
}
