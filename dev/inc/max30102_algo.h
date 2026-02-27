/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       max30102_algo.h
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#ifndef __MAX30102_ALGO_H__
#define __MAX30102_ALGO_H__

#include "common.h"

#define true 1
#define false 0
#define FS 100
#define BUFFER_SIZE (FS * 5)
#define HR_FIFO_SIZE 7
#define MA4_SIZE 4     // DO NOT CHANGE
#define HAMMING_SIZE 5 // DO NOT CHANGE
#define min(x, y) ((x) < (y) ? (x) : (y))

extern void maxim_heart_rate_and_oxygen_saturation(uint32 *pun_ir_buffer, int32 n_ir_buffer_length, uint32 *pun_red_buffer, int32 *pn_spo2, int8 *pch_spo2_valid, int32 *pn_heart_rate, int8 *pch_hr_valid);
extern void maxim_find_peaks(int32 *pn_locs, int32 *pn_npks, int32 *pn_x, int32 n_size, int32 n_min_height, int32 n_min_distance, int32 n_max_num);
extern void maxim_peaks_above_min_height(int32 *pn_locs, int32 *pn_npks, int32 *pn_x, int32 n_size, int32 n_min_height);
extern void maxim_remove_close_peaks(int32 *pn_locs, int32 *pn_npks, int32 *pn_x, int32 n_min_distance);
extern void maxim_sort_ascend(int32 *pn_x, int32 n_size);
extern void maxim_sort_indices_descend(int32 *pn_x, int32 *pn_indx, int32 n_size);

#endif