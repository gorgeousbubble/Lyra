/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       pdb.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "pdb.h"

/*
 *  @brief      PDB Initialization
 *  @since      v1.0
 *  Sample usage:       PDB_Init();//Initialize PDB0
 */
void PDB_Init(void)
{
  SIM_SCGC6 |= SIM_SCGC6_PDB_MASK;//PDB module clock enable
  
  //PDB state control register
  PDB_SC_REG(PDB0_BASE_PTR) = (0
                               | PDB_SC_CONT_MASK       //PDB continuous mode
                               | PDB_SC_TRGSEL(15)      //PDB software trigger
                               | PDB_SC_PDBEN_MASK      //PDB module enable
                               );
  
}