/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       startup.s
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */
 
              ;AREA   StartUp, CODE, READONLY      ; name this block of code
              SECTION .noinit : CODE
              EXPORT Reset_Handler
Reset_Handler
              MOV       R0,#0
              MOV       R1,#0
              MOV       R2,#0
              MOV       R3,#0
              MOV       R4,#0
              MOV       R5,#0
              MOV       R6,#0
              MOV       R7,#0
              MOV       R8,#0
              MOV       R9,#0
              MOV       R10,#0
              MOV       R11,#0
              MOV       R12,#0
              CPSIE     i
              IMPORT    Start
              BL        Start
__done
              B         __done
              END
              