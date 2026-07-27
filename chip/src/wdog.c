/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       wdog.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "misc.h"
#include "wdog.h"

/*
 *  @brief      WDOG unlock
 *  @since      v1.0
 */
void WDOG_Unlock(void)
{
  /* Save the ACTUAL interrupt-mask state. DisableInterrupts/EnableInterrupts
   * operate on PRIMASK (__disable_irq/__enable_irq), so we must sample PRIMASK
   * -- not BASEPRI -- otherwise the "was it enabled?" test is meaningless
   * (BASEPRI is normally 0) and we would re-enable interrupts even when the
   * caller had deliberately disabled them around a critical section.
   * PRIMASK: 0 = interrupts enabled, 1 = already disabled. */
  uint32 WDOG_Temp = __get_PRIMASK();

  DisableInterrupts; // close all interrupts (the two unlock writes are timed)

  WDOG_UNLOCK = 0xC520; // write 0xC520 and 0xD928 to the WDOG-UNLOCK register to unlock WDOG
  WDOG_UNLOCK = 0xD928;

  if (WDOG_Temp == 0) // interrupts were enabled before -> restore them
  {
    EnableInterrupts; // open all interrupts
  }
}

/*
 *  @brief      WDOG feed (counter update)
 *  @since      v1.0
 */
void WDOG_Feed(void)
{
  /* Sample PRIMASK (not BASEPRI) so we only re-enable interrupts if they were
   * enabled on entry. WDOG_Feed runs every main-loop iteration; the old code
   * always re-enabled, which would break a caller's critical section if it
   * ever fed the dog with interrupts disabled. */
  uint32 WDOG_Temp = __get_PRIMASK();

  DisableInterrupts; // close all interrupts (the two refresh writes are timed)

  WDOG_REFRESH = 0xA602; // update WDOG counter, Feed Dog
  WDOG_REFRESH = 0xB480;

  if (WDOG_Temp == 0) // interrupts were enabled before -> restore them
  {
    EnableInterrupts; // open all interrupts
  }
}

/*
 *  @brief      WDOG disable
 *  @since      v1.0
 */
void WDOG_Disable(void)
{
  WDOG_Unlock();                             // unlock WDOG,configure WDOG register
  WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK; // WDOGEN clear to 0, disable WDOG
}

/*
 *  @brief      WDOG enable
 *  @since      v1.0
 */
void WDOG_Enable(void)
{
  WDOG_Unlock();                            // unlock WDOG,configure WDOG register
  WDOG_STCTRLH |= WDOG_STCTRLH_WDOGEN_MASK; // WDOGEN set to 1, enable WDOG
}

/*
 *  @brief      WDOG init
 *  @param      WDOG_Cnt
 *  @since      v1.0
 */
void WDOG_Init(uint32 WDOG_Cnt)
{
  uint32 WDOG_Temp;

  ASSERT(WDOG_Cnt >= 4); // the minimum counting clock is 4 milliseconds

  /* On K64 the register reconfiguration must complete within ~256 bus cycles
   * of the unlock, so keep the WHOLE unlock->configure sequence atomic. We
   * disable interrupts first; WDOG_Unlock() then sees PRIMASK=1 and leaves
   * them disabled, so no ISR can intervene before WDOG_STCTRLH is written. */
  WDOG_Temp = __get_PRIMASK();
  DisableInterrupts;

  WDOG_Unlock(); // unlock WDOG,configure WDOG register

  WDOG_PRESC = WDOG_PRESC_PRESCVAL(0); // set the division factor to PRESSCVAL+1 (PRESSCVAL values range from 0 to 7)

  WDOG_TOVALH = WDOG_Cnt >> 16; // Set WDOG time
  WDOG_TOVALL = (uint16)(WDOG_Cnt & 0x0000ffff);

  WDOG_STCTRLH = (0 | WDOG_STCTRLH_WDOGEN_MASK    // WDOG enable
                  | WDOG_STCTRLH_ALLOWUPDATE_MASK // WDOG allow update
                  | WDOG_STCTRLH_STOPEN_MASK      // WDOG stop mode
                  | WDOG_STCTRLH_WAITEN_MASK      // WDOG wait mode
  );

  if (WDOG_Temp == 0)
  {
    EnableInterrupts;
  }
}