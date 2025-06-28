/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       system_init.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "main.h"
#include "mcg.h"
#include "misc.h"
#include "system_init.h"
#include "wdog.h"

#pragma section = ".data"
#pragma section = ".data_init"
#pragma section = ".bss"
#pragma section = "CodeRelocate"
#pragma section = "CodeRelocateRam"

/*
** System Frequency
*/
uint16  MK64_Core_MHz=0;
uint32  MK64_Core_KHz=0;
uint32  MK64_Bus_KHz=0;

/*
 *  @brief      Copy interrupt vector tables and necessary data into RAM
 *  @since      v1.0
 */
void Common_StartUp(void)
{
    /* Declare a counter we'll use in all of the copy loops */
    uint32 n;

    /* Declare pointers for various data sections. These pointers
     * are initialized using values pulled in from the linker file
     */
    uint8 *data_ram, * data_rom, * data_rom_end;
    uint8 *bss_start, * bss_end;
    

    /* Get VECTOR_TABLE and VECTOR_RAM addresses from linker (*.icf)*/
    extern uint32 __VECTOR_TABLE[];
    extern uint32 __VECTOR_RAM[];
    
    /* Copy pointer from interrupt vector tables */
    uint32 *VECTOR_TABLE_P = &__VECTOR_TABLE[0];
    uint32 *VECTOR_RAM_P = &__VECTOR_RAM[0];

    /* Copy interrupt vector table from ROM to RAM */
    if (__VECTOR_RAM != __VECTOR_TABLE)
    {
        for (n=0; n<0x410; n++)
            *VECTOR_RAM_P++ = *VECTOR_TABLE_P++;
    }
    /* Specific latest interrupt vector table address __VECTOR_RAM */
    write_vtor((uint32)__VECTOR_RAM);
    //write_vtor((uint32)__VECTOR_TABLE);

    /* Copy variables data from ROM to RAM */
    data_ram = __section_begin(".data");
    data_rom = __section_begin(".data_init");
    data_rom_end = __section_end(".data_init");
    n = data_rom_end - data_rom;

    /* Copy data from ROM to RAM */
    while (n--)
        *data_ram++ = *data_rom++;

    /* Clean variable without initialize value */
    bss_start = __section_begin(".bss");
    bss_end = __section_end(".bss");

    /* Clean variable and provide zero value */
    n = bss_end - bss_start;
    while(n--)
        *bss_start++ = 0;

    /* Copy __ramfunc functions to RAM */
    uint8 *code_relocate_ram = __section_begin("CodeRelocateRam");
    uint8 *code_relocate = __section_begin("CodeRelocate");
    uint8 *code_relocate_end = __section_end("CodeRelocate");

    /* Copy functions from ROM to RAM */
    n = code_relocate_end - code_relocate;
    while (n--)
        *code_relocate_ram++ = *code_relocate++;
}

/*
 *  @brief      System Init (Setup System Frequency)
 *  @since      v1.0
 *  @note       void SysInit(void)
 */
void SysInit(void)
{
  MK64_Core_MHz = PLL_Init(MK64_CORE_PLL_CLK);
  MK64_Core_KHz = MK64_Core_MHz * 1000;
  MK64_Bus_KHz  = MK64_Core_KHz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> SIM_CLKDIV1_OUTDIV2_SHIFT) + 1);  
}

/*
 *  @brief      Kinetis Startup
 *  @since      v1.0
 *  @note       void Start(void)
 */
void Start(void)
{
  #if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
  SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));//FPU support
  #endif
  
  WDOG_Disable();// disable WDOG
  
  Common_StartUp();// Copy interrupt vector tables and data to RAM
  
  SysInit();//System Init with PLL Clock
  
  main();
  
  while(1);
}