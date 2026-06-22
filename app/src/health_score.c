/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       health_score.c
 * @brief      Health comprehensive score calculation and OLED display
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "health_score.h"
#include "activity_history.h"
#include "health.h"
#include "oled.h"
#include "oled_i2c.h"
#include "sleep_monitor.h"
#include <stdio.h>

/* -----------------------------------------------------------------------
 * Global
 * ----------------------------------------------------------------------- */
HealthScoreResult HealthScore = {0};

/* -----------------------------------------------------------------------
 * HealthScore_Calculate
 * ----------------------------------------------------------------------- */
void HealthScore_Calculate(void)
{
    /* --- 1. Activity score (0-25): steps / goal, capped at 25 --- */
    uint32 steps = Activity.today_steps;
    uint8 act_score;
    if (steps >= (uint32)HS_STEP_GOAL)
        act_score = HS_WEIGHT_ACTIVITY;
    else
        act_score = (uint8)((uint32)steps * HS_WEIGHT_ACTIVITY / HS_STEP_GOAL);

    /* --- 2. Sleep duration score (0-15): sleep_s / 7h --- */
    uint8 sdur_score;
    uint32 sleep_s = SleepMon.total_sleep_s;
    if (sleep_s >= HS_SLEEP_TARGET_S)
        sdur_score = HS_WEIGHT_SLEEP_DUR;
    else
        sdur_score = (uint8)((uint64)sleep_s * HS_WEIGHT_SLEEP_DUR / HS_SLEEP_TARGET_S);

    /* --- 3. Sleep quality score (0-30): deep_sleep% vs 25% target --- */
    uint8 squa_score;
    if (sleep_s == 0)
    {
        squa_score = 0;
    }
    else
    {
        uint32 deep_pct = SleepMon.total_deep_s * 100 / sleep_s;
        if (deep_pct >= (uint32)HS_DEEP_TARGET_PCT)
            squa_score = HS_WEIGHT_SLEEP_QUA;
        else
            squa_score = (uint8)(deep_pct * HS_WEIGHT_SLEEP_QUA / HS_DEEP_TARGET_PCT);
    }

    /* --- 4. Heart rate score (0-15) --- */
    uint8 hr_valid = (Heart_Rate > 0 && Heart_Rate < 300) ? 1 : 0;
    uint8 hr_score;
    if (!hr_valid)
    {
        /* No reading yet: give partial credit (sensors may not be worn) */
        hr_score = HS_WEIGHT_HEART / 2;
    }
    else if (Heart_Rate >= HS_HR_LOW && Heart_Rate <= HS_HR_HIGH)
    {
        /* Normal range: full marks */
        hr_score = HS_WEIGHT_HEART;
    }
    else
    {
        /* Out of range: proportional penalty */
        int32 dev;
        if (Heart_Rate < HS_HR_LOW)
            dev = HS_HR_LOW - (int32)Heart_Rate;     /* below 60 */
        else
            dev = (int32)Heart_Rate - HS_HR_HIGH;    /* above 100 */
        /* Each 10bpm over boundary = -3pts, floor at 0 */
        int32 penalty = (dev / 10) * 3;
        int32 s = (int32)HS_WEIGHT_HEART - penalty;
        hr_score = (s > 0) ? (uint8)s : 0;
    }

    /* --- 5. SpO2 score (0-15) --- */
    uint8 spo2_valid = (SPO2 >= 80 && SPO2 <= 100) ? 1 : 0;
    uint8 spo2_score;
    if (!spo2_valid)
    {
        spo2_score = HS_WEIGHT_SPO2 / 2;
    }
    else if (SPO2 >= 95)
    {
        spo2_score = HS_WEIGHT_SPO2;
    }
    else if (SPO2 >= 90)
    {
        /* 90-94%: partial */
        spo2_score = (uint8)(HS_WEIGHT_SPO2 * (SPO2 - 90) / 5 + HS_WEIGHT_SPO2 / 3);
        if (spo2_score > HS_WEIGHT_SPO2) spo2_score = HS_WEIGHT_SPO2;
    }
    else
    {
        spo2_score = 0;
    }

    /* --- Total --- */
    uint8 total = act_score + sdur_score + squa_score + hr_score + spo2_score;
    if (total > 100) total = 100;

    /* --- Grade --- */
    HealthGrade grade;
    if      (total >= 90) grade = HS_GRADE_EXCELLENT;
    else if (total >= 75) grade = HS_GRADE_GOOD;
    else if (total >= 60) grade = HS_GRADE_FAIR;
    else                  grade = HS_GRADE_POOR;

    /* Store results */
    HealthScore.total           = total;
    HealthScore.score_activity  = act_score;
    HealthScore.score_sleep_dur = sdur_score;
    HealthScore.score_sleep_qua = squa_score;
    HealthScore.score_heart     = hr_score;
    HealthScore.score_spo2      = spo2_score;
    HealthScore.grade           = grade;
    HealthScore.hr_valid        = hr_valid;
    HealthScore.spo2_valid      = spo2_valid;
}

/* -----------------------------------------------------------------------
 * Render helpers
 * ----------------------------------------------------------------------- */
static void dp(uint8 screen[64][16], int x, int y)
{
    if ((unsigned)x < 128 && (unsigned)y < 64)
        screen[y][x >> 3] |= (0x01 << (7 - (x & 7)));
}

static void dline_h(uint8 screen[64][16], int y, int x0, int x1)
{
    for (int x = x0; x <= x1; x++) dp(screen, x, y);
}

static void dc6(uint8 screen[64][16], int px, int py, char ch)
{
    uint8 c = (uint8)ch - 32;
    if (c >= 96) return;
    for (int col = 0; col < 6; col++)
    {
        uint8 fb = Oled_FontLib_6x8[c][col];
        for (int bit = 0; bit < 8; bit++)
            if (fb & (1 << bit)) dp(screen, px + col, py + bit);
    }
}

static void ds6(uint8 screen[64][16], int px, int py, const char *s)
{
    while (*s) { dc6(screen, px, py, *s++); px += 6; }
}

/* Draw a 12x24 character row into frame buffer */
static void draw12row(uint8 screen[64][16], int px, int py, int cidx, int row)
{
    for (int col = 0; col < 12; col++)
    {
        uint8 fb = Oled_FontLib_12x24[cidx * 36 + row * 12 + col];
        for (int bit = 0; bit < 8; bit++)
            if (fb & (1 << bit)) dp(screen, px + col, py + bit);
    }
}

/* Draw a score bar: label, current_pts, max_pts, x_label, y */
static void draw_bar(uint8 screen[64][16],
                     const char *label, int pts, int max_pts,
                     int x, int y)
{
    /* label (4 chars wide = 24px) */
    ds6(screen, x, y, label);

    /* Outer box: x+26..x+98 */
    int bx0 = x + 26;
    int bx1 = x + 97;
    dline_h(screen, y,     bx0, bx1);
    dline_h(screen, y + 7, bx0, bx1);
    for (int yy = y; yy <= y + 7; yy++)
    {
        dp(screen, bx0, yy);
        dp(screen, bx1, yy);
    }

    /* Fill: proportional to pts/max_pts */
    int fill_w = (max_pts > 0) ? (pts * (bx1 - bx0 - 2) / max_pts) : 0;
    for (int yy = y + 1; yy <= y + 6; yy++)
        for (int xx = bx0 + 1; xx <= bx0 + fill_w; xx++)
            dp(screen, xx, yy);

    /* Numeric score on right */
    char nbuf[6];
    snprintf(nbuf, sizeof(nbuf), "%d", pts);
    ds6(screen, bx1 + 2, y, nbuf);
}

/* -----------------------------------------------------------------------
 * Render_HealthScore
 *
 * Layout (128×64):
 *
 *   y= 0..25 : Large total score (12x24) centred on left,
 *              grade text on right
 *   y=26..27 : separator
 *   y=28..35 : Activity bar   (25 pts)
 *   y=36..43 : Sleep Dur bar  (15 pts)
 *   y=44..51 : Sleep Qua bar  (30 pts)
 *   y=52..59 : Heart Rate bar (15 pts)
 *   y=60..63 : SpO2 + hint (abbreviated)
 * ----------------------------------------------------------------------- */
void Render_HealthScore(void)
{
    /* Recalculate every time we render */
    HealthScore_Calculate();

    uint8 screen[64][16];
    for (int i = 0; i < 64; i++)
        for (int c = 0; c < 16; c++)
            screen[i][c] = 0x00;

    char buf[12];

    /* ---- Large total score (12x24) on left ---- */
    snprintf(buf, sizeof(buf), "%d", HealthScore.total);
    int score_len = 0;
    for (const char *p = buf; *p; p++) score_len++;
    int sx = (42 - score_len * 12) / 2;  /* centre in 0..41 */
    if (sx < 0) sx = 0;

    for (int i = 0; buf[i]; i++)
    {
        int cidx = (uint8)buf[i] - 32;
        if (cidx < 0 || cidx >= 96) continue;
        int px = sx + i * 12;
        draw12row(screen, px, 0,  cidx, 0);
        draw12row(screen, px, 8,  cidx, 1);
        draw12row(screen, px, 16, cidx, 2);
    }
    ds6(screen, sx + score_len * 12 + 2, 18, "pts");

    /* ---- Grade badge (right side x=44..127) ---- */
    const char *grade_str;
    switch (HealthScore.grade)
    {
    case HS_GRADE_EXCELLENT: grade_str = "EXCELLENT"; break;
    case HS_GRADE_GOOD:      grade_str = "GOOD";      break;
    case HS_GRADE_FAIR:      grade_str = "FAIR";      break;
    default:                 grade_str = "POOR";      break;
    }
    /* centre grade text in x=44..127 (84px) */
    int gw = 0;
    for (const char *p = grade_str; *p; p++) gw += 6;
    int gx = 44 + (84 - gw) / 2;
    ds6(screen, gx, 10, grade_str);

    /* Vertical separator between score and grade */
    for (int y = 0; y < 27; y++) dp(screen, 43, y);

    /* ---- Separator ---- */
    dline_h(screen, 26, 0, 127);
    dline_h(screen, 27, 0, 127);

    /* ---- Score bars (x=0, each row 8px tall) ---- */
    draw_bar(screen, "ACT ", HealthScore.score_activity,  HS_WEIGHT_ACTIVITY,  0, 28);
    draw_bar(screen, "SDUR", HealthScore.score_sleep_dur, HS_WEIGHT_SLEEP_DUR, 0, 37);
    draw_bar(screen, "SQUA", HealthScore.score_sleep_qua, HS_WEIGHT_SLEEP_QUA, 0, 46);
    draw_bar(screen, "HR  ", HealthScore.score_heart,     HS_WEIGHT_HEART,     0, 55);

    /* SpO2 abbreviated at bottom row */
    snprintf(buf, sizeof(buf), "SPO2:%d", HealthScore.score_spo2);
    ds6(screen, 80, 57, buf);

    Oled_I2C_Draw_Picture_128x64((const uint8 *)screen);
}
