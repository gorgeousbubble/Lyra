/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       health_score.h
 * @brief      Health comprehensive score — aggregates all sensor modules
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __HEALTH_SCORE_H__
#define __HEALTH_SCORE_H__

#include "common.h"
#include "activity_history.h"  /* single source of truth for the daily step goal */

/* -----------------------------------------------------------------------
 * Scoring model (total 100 points):
 *
 *  ACTIVITY  (25 pts): today's step count vs 10000-step goal
 *  SLEEP_DUR (15 pts): sleep duration vs 7-hour target
 *  SLEEP_QUA (30 pts): deep-sleep ratio (target > 25%)
 *  HEART     (15 pts): resting heart rate in 60-100 bpm normal range
 *  SPO2      (15 pts): blood oxygen saturation ≥ 95%
 *
 * Grade thresholds:
 *   90-100: EXCELLENT
 *   75-89 : GOOD
 *   60-74 : FAIR
 *   0 -59 : POOR
 * ----------------------------------------------------------------------- */

/* Score weights */
#define HS_WEIGHT_ACTIVITY   25
#define HS_WEIGHT_SLEEP_DUR  15
#define HS_WEIGHT_SLEEP_QUA  30
#define HS_WEIGHT_HEART      15
#define HS_WEIGHT_SPO2       15

/* Normal heart rate range (bpm) */
#define HS_HR_LOW    60
#define HS_HR_HIGH  100

/* Sleep targets.
 * HS_SLEEP_TARGET_S is the factory default. The live value is held in the
 * runtime variable Health_Sleep_Goal_S (seconds), which is user-configurable
 * through the Configure-Adjust menu and persisted in Flash (Sector 5 / Page 80).
 * The editor works in minutes (30-min steps); the sleep-duration score reads
 * Health_Sleep_Goal_S. */
#define HS_SLEEP_TARGET_S    (7UL * 3600UL)  /* factory default: 7 hours in seconds */
#define HS_SLEEP_GOAL_MIN_MIN   240          /* 4.0 h  (minutes) configurable lower bound */
#define HS_SLEEP_GOAL_MAX_MIN   720          /* 12.0 h (minutes) configurable upper bound */
#define HS_SLEEP_GOAL_STEP_MIN   30          /* 0.5 h  (minutes) KEY2/KEY3 increment */
#define HS_DEEP_TARGET_PCT   25              /* target deep sleep % */

extern uint32 Health_Sleep_Goal_S;           /* live, persisted sleep goal (seconds) */

/* Step goal — aliased to the single definition in activity_history.h so the
 * activity screen and the health score can never drift apart. */
#define HS_STEP_GOAL         ACTIVITY_GOAL_STEPS

typedef enum
{
    HS_GRADE_POOR      = 0,
    HS_GRADE_FAIR      = 1,
    HS_GRADE_GOOD      = 2,
    HS_GRADE_EXCELLENT = 3,
} HealthGrade;

typedef struct
{
    uint8  total;          /* 0-100: overall score               */
    uint8  score_activity; /* 0-25                               */
    uint8  score_sleep_dur;/* 0-15                               */
    uint8  score_sleep_qua;/* 0-30                               */
    uint8  score_heart;    /* 0-15                               */
    uint8  score_spo2;     /* 0-15                               */
    HealthGrade grade;
    uint8  hr_valid;       /* 1 = Heart_Rate data is available   */
    uint8  spo2_valid;     /* 1 = SPO2 data is available         */
} HealthScoreResult;

extern HealthScoreResult HealthScore;

/* Recompute the health score from current module states */
extern void HealthScore_Calculate(void);

/* Render the health score screen to OLED */
extern void Render_HealthScore(void);

#endif
