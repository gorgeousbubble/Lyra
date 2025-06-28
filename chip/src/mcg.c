/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       mcg.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"
#include "mcg.h"

/*
**PLL锁相环频率结构体数组(EXTAL_50MHz)(MCGOUTCLK=(EXTAL_50MHz/(PRCIV+1)*(VDIV+16)/2))
*/
MCG_PLL_DIV MCG_PLL[PLLMAX]=
{
  {48,24,0},     //PLL48,PRDIV=24,VDIV=0
  {50,24,1},     //PLL50,PRDIV=24,VDIV=1
  {52,24,2},     //PLL52,PRDIV=24,VDIV=2
  {54,24,3},     //PLL54,PRDIV=24,VDIV=3
  {56,24,4},     //PLL56,PRDIV=24,VDIV=4
  {58,24,5},     //PLL58,PRDIV=24,VDIV=5
  {60,24,6},     //PLL60,PRDIV=24,VDIV=6
  {62,24,7},     //PLL62,PRDIV=24,VDIV=7
  {64,24,8},     //PLL64,PRDIV=24,VDIV=8
  {66,24,9},     //PLL66,PRDIV=24,VDIV=9
  {68,24,10},    //PLL68,PRDIV=24,VDIV=10
  {70,24,11},    //PLL70,PRDIV=24,VDIV=11
  {72,24,12},    //PLL72,PRDIV=24,VDIV=12
  {74,24,13},    //PLL74,PRDIV=24,VDIV=13
  {76,24,14},    //PLL76,PRDIV=24,VDIV=14
  {78,24,15},    //PLL78,PRDIV=24,VDIV=15
  {80,24,16},    //PLL80,PRDIV=24,VDIV=16
  {82,24,17},    //PLL82,PRDIV=24,VDIV=17
  {84,24,18},    //PLL84,PRDIV=24,VDIV=18
  {86,24,19},    //PLL86,PRDIV=24,VDIV=19
  {88,24,20},    //PLL88,PRDIV=24,VDIV=20
  {90,24,21},    //PLL90,PRDIV=24,VDIV=21
  {92,24,22},    //PLL92,PRDIV=24,VDIV=22
  {94,24,23},    //PLL94,PRDIV=24,VDIV=23
  {96,24,24},    //PLL96,PRDIV=24,VDIV=24
  {98,24,25},    //PLL98,PRDIV=24,VDIV=25
  {100,24,26},   //PLL100,PRDIV=24,VDIV=26
  {102,24,27},   //PLL102,PRDIV=24,VDIV=27
  {104,24,28},   //PLL104,PRDIV=24,VDIV=28
  {106,24,29},   //PLL106,PRDIV=24,VDIV=29
  {108,24,30},   //PLL108,PRDIV=24,VDIV=30
  {110,24,31},   //PLL110,PRDIV=24,VDIV=31
  {115,19,22},   //PLL115,PRDIV=19,VDIV=22
  {120,19,24},   //PLL120,PRDIV=19,VDIV=24*//PLL120,PRDIV=14,VDIV=12
  {125,19,26},   //PLL125,PRDIV=19,VDIV=26
  {130,19,28},   //PLL130,PRDIV=19,VDIV=28*//PLL130,PRDIV=14,VDIV=15
  {135,19,30},   //PLL135,PRDIV=19,VDIV=30
  {140,14,18},   //PLL140,PRDIV=14,VDIV=18
  {150,14,21},   //PLL150,PRDIV=14,VDIV=21
  {160,14,24},   //PLL160,PRDIV=14,VDIV=24
  {170,14,27},   //PLL170,PRDIV=14,VDIV=27
  {180,14,30},   //PLL180,PRDIV=14,VDIV=30
  {200,12,28},   //PLL200,PRDIV=12,VDIV=28
};

/*
**SIM时钟分频结构体
*/
MCG_SIM_DIV MCG_DIV=
{
  MK64_CORE_DIV,        //内核分频因子
  MK64_BUS_DIV,         //总线分频因子
  MK64_FLEX_DIV,        //Flex分频因子
  MK64_FLASH_DIV        //Flash分频因子
};

/*
 *  @brief      MCG 分频设置
 *  @param      PLLn    PLL锁相环时钟频率设置(选择PLL配置)
 *  @since      v1.0
 */
void MCG_DIV_Count(PLLn PLLx)
{
  uint16 MCG_PLL_Clock=0;
  
  ASSERT(PLLx < PLLMAX);
  
  MCG_PLL_Clock = MCG_PLL[PLLx].PLL_CLK;//选择相应PLL配置时钟
  
  //内核分频(CORE_CLK=PLL_CLK/(SIM_CLKDIV1_OUTDIV1+1))
  if(MCG_PLL_Clock <= MK64_CORE_CLK)
  {
    MCG_DIV.SIM_CORE_DIV = 0;//PLL_CLK/1
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 2)
  {
    MCG_DIV.SIM_CORE_DIV = 1;//PLL_CLK/2
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 3)
  {
    MCG_DIV.SIM_CORE_DIV = 2;//PLL_CLK/3
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 4)
  {
    MCG_DIV.SIM_CORE_DIV = 3;//PLL_CLK/4
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 5)
  {
    MCG_DIV.SIM_CORE_DIV = 4;//PLL_CLK/5
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 6)
  {
    MCG_DIV.SIM_CORE_DIV = 5;//PLL_CLK/6
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 7)
  {
    MCG_DIV.SIM_CORE_DIV = 6;//PLL_CLK/7
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 8)
  {
    MCG_DIV.SIM_CORE_DIV = 7;//PLL_CLK/8
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 9)
  {
    MCG_DIV.SIM_CORE_DIV = 8;//PLL_CLK/9
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 10)
  {
    MCG_DIV.SIM_CORE_DIV = 9;//PLL_CLK/10
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 11)
  {
    MCG_DIV.SIM_CORE_DIV = 10;//PLL_CLK/11
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 12)
  {
    MCG_DIV.SIM_CORE_DIV = 11;//PLL_CLK/12
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 13)
  {
    MCG_DIV.SIM_CORE_DIV = 12;//PLL_CLK/13
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 14)
  {
    MCG_DIV.SIM_CORE_DIV = 13;//PLL_CLK/14
  }
  else if(MCG_PLL_Clock <= MK64_CORE_CLK * 15)
  {
    MCG_DIV.SIM_CORE_DIV = 14;//PLL_CLK/15
  }
  else
  {
    MCG_DIV.SIM_CORE_DIV = 15;//PLL_CLK/16
  }
  
  //总线分频(BUS_CLK=PLL_CLK/(SIM_CLKDIV1_OUTDIV2+1))
  if(MCG_PLL_Clock <= MK64_BUS_CLK)
  {
    MCG_DIV.SIM_BUS_DIV = 0;//PLL_CLK/1
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 2)
  {
    MCG_DIV.SIM_BUS_DIV = 1;//PLL_CLK/2
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 3)
  {
    MCG_DIV.SIM_BUS_DIV = 2;//PLL_CLK/3
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 4)
  {
    MCG_DIV.SIM_BUS_DIV = 3;//PLL_CLK/4
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 5)
  {
    MCG_DIV.SIM_BUS_DIV = 4;//PLL_CLK/5
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 6)
  {
    MCG_DIV.SIM_BUS_DIV = 5;//PLL_CLK/6
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 7)
  {
    MCG_DIV.SIM_BUS_DIV = 6;//PLL_CLK/7
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 8)
  {
    MCG_DIV.SIM_BUS_DIV = 7;//PLL_CLK/8
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 9)
  {
    MCG_DIV.SIM_BUS_DIV = 8;//PLL_CLK/9
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 10)
  {
    MCG_DIV.SIM_BUS_DIV = 9;//PLL_CLK/10
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 11)
  {
    MCG_DIV.SIM_BUS_DIV = 10;//PLL_CLK/11
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 12)
  {
    MCG_DIV.SIM_BUS_DIV = 11;//PLL_CLK/12
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 13)
  {
    MCG_DIV.SIM_BUS_DIV = 12;//PLL_CLK/13
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 14)
  {
    MCG_DIV.SIM_BUS_DIV = 13;//PLL_CLK/14
  }
  else if(MCG_PLL_Clock <= MK64_BUS_CLK * 15)
  {
    MCG_DIV.SIM_BUS_DIV = 14;//PLL_CLK/15
  }
  else
  {
    MCG_DIV.SIM_BUS_DIV = 15;//PLL_CLK/16
  }
  
  //FLEX分频(FLEX_CLK=PLL_CLK/(SIM_CLKDIV1_OUTDIV3+1))
  if(MCG_PLL_Clock <= MK64_FLEX_CLK)
  {
    MCG_DIV.SIM_FLEX_DIV = 0;//PLL_CLK/1
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 2)
  {
    MCG_DIV.SIM_FLEX_DIV = 1;//PLL_CLK/2
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 3)
  {
    MCG_DIV.SIM_FLEX_DIV = 2;//PLL_CLK/3
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 4)
  {
    MCG_DIV.SIM_FLEX_DIV = 3;//PLL_CLK/4
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 5)
  {
    MCG_DIV.SIM_FLEX_DIV = 4;//PLL_CLK/5
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 6)
  {
    MCG_DIV.SIM_FLEX_DIV = 5;//PLL_CLK/6
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 7)
  {
    MCG_DIV.SIM_FLEX_DIV = 6;//PLL_CLK/7
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 8)
  {
    MCG_DIV.SIM_FLEX_DIV = 7;//PLL_CLK/8
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 9)
  {
    MCG_DIV.SIM_FLEX_DIV = 8;//PLL_CLK/9
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 10)
  {
    MCG_DIV.SIM_FLEX_DIV = 9;//PLL_CLK/10
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 11)
  {
    MCG_DIV.SIM_FLEX_DIV = 10;//PLL_CLK/11
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 12)
  {
    MCG_DIV.SIM_FLEX_DIV = 11;//PLL_CLK/12
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 13)
  {
    MCG_DIV.SIM_FLEX_DIV = 12;//PLL_CLK/13
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 14)
  {
    MCG_DIV.SIM_FLEX_DIV = 13;//PLL_CLK/14
  }
  else if(MCG_PLL_Clock <= MK64_FLEX_CLK * 15)
  {
    MCG_DIV.SIM_FLEX_DIV = 14;//PLL_CLK/15
  }
  else
  {
    MCG_DIV.SIM_FLEX_DIV = 15;//PLL_CLK/16
  }
  
  //FLASH分频(FLASH_CLK=PLL_CLK/(SIM_CLKDIV1_OUTDIV3+1))
  if(MCG_PLL_Clock <= MK64_FLASH_CLK)
  {
    MCG_DIV.SIM_FLASH_DIV = 0;//PLL_CLK/1
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 2)
  {
    MCG_DIV.SIM_FLASH_DIV = 1;//PLL_CLK/2
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 3)
  {
    MCG_DIV.SIM_FLASH_DIV = 2;//PLL_CLK/3
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 4)
  {
    MCG_DIV.SIM_FLASH_DIV = 3;//PLL_CLK/4
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 5)
  {
    MCG_DIV.SIM_FLASH_DIV = 4;//PLL_CLK/5
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 6)
  {
    MCG_DIV.SIM_FLASH_DIV = 5;//PLL_CLK/6
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 7)
  {
    MCG_DIV.SIM_FLASH_DIV = 6;//PLL_CLK/7
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 8)
  {
    MCG_DIV.SIM_FLASH_DIV = 7;//PLL_CLK/8
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 9)
  {
    MCG_DIV.SIM_FLASH_DIV = 8;//PLL_CLK/9
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 10)
  {
    MCG_DIV.SIM_FLASH_DIV = 9;//PLL_CLK/10
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 11)
  {
    MCG_DIV.SIM_FLASH_DIV = 10;//PLL_CLK/11
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 12)
  {
    MCG_DIV.SIM_FLASH_DIV = 11;//PLL_CLK/12
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 13)
  {
    MCG_DIV.SIM_FLASH_DIV = 12;//PLL_CLK/13
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 14)
  {
    MCG_DIV.SIM_FLASH_DIV = 13;//PLL_CLK/14
  }
  else if(MCG_PLL_Clock <= MK64_FLASH_CLK * 15)
  {
    MCG_DIV.SIM_FLASH_DIV = 14;//PLL_CLK/15
  }
  else
  {
    MCG_DIV.SIM_FLASH_DIV = 15;//PLL_CLK/16
  }
  
}

/*
 *  @brief      PLL锁相环频率设置
 *  @param      PLLn       频率设置参数
 *  @return     PLL频率（MHz）
 *  @since      v1.0
 *  @warning    此函数只能在 复位后没进行任何频率设置情况下调用，即MCG在FEI模式下才可调用
 *  Sample usage:       uint8 PLL_CLK=PLL_Init(PLL180)//设置PLL180
 */
uint8 PLL_Init(PLLn PLLx)
{
  uint8 i=0;
  uint32 Temp_Reg=0;
  
  MCG_DIV_Count(PLLx);
  
  //Reset进入FEI模式,FEI->FBE(FLL内部时钟源模式->FLL旁路外部时钟源模式)
  MCG_C2 &= ~MCG_C2_LP_MASK;    //旁路模式下允许PLL
  MCG_C2 |= MCG_C2_RANGE(1);    //OSC晶体振荡器选择高频率范围(3-8MHz)
  
  MCG_C1 &= ~MCG_C1_IREFS_MASK; //选择外部参考时钟
  MCG_C1 |= MCG_C1_CLKS(2);     //选择外部参考时钟
  MCG_C1 |= MCG_C1_FRDIV(7);    //参考时钟分频1536,50MHz/1536=33.33KHz(31.25KHz-39.0625KHz)
  
  while(MCG_S & MCG_S_IREFST_MASK);     //等待使用外部参考时钟
  while(((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2);      //等待选择外部参考时钟
  
  //进入FBE模式(FLL旁路时钟源模式)
  //FBE->PBE(FLL旁路外部时钟源模式->PLL旁路外部时钟源模式)
  Temp_Reg = FMC_PFAPR;
  
  FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK | FMC_PFAPR_M6PFD_MASK | FMC_PFAPR_M5PFD_MASK
                 | FMC_PFAPR_M4PFD_MASK | FMC_PFAPR_M3PFD_MASK | FMC_PFAPR_M2PFD_MASK
                 | FMC_PFAPR_M1PFD_MASK | FMC_PFAPR_M0PFD_MASK;
  
  SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(MCG_DIV.SIM_CORE_DIV) | SIM_CLKDIV1_OUTDIV2(MCG_DIV.SIM_BUS_DIV)
                  | SIM_CLKDIV1_OUTDIV3(MCG_DIV.SIM_FLEX_DIV) | SIM_CLKDIV1_OUTDIV4(MCG_DIV.SIM_FLASH_DIV);
  
  for(i=0;i<MCG_DIV.SIM_FLASH_DIV;i++);//Wait
  
  FMC_PFAPR = Temp_Reg;
  
  //设置PRDIV和VDIV分频因子
  MCG_C5 |= MCG_C5_PRDIV0(MCG_PLL[PLLx].PLL_PRDIV);     //EXTAL_MHz/(PRDIV+1) (8-16MHz)
  MCG_C6 |= MCG_C6_VDIV0(MCG_PLL[PLLx].PLL_VDIV);       //EXTAL_MHz/(PRDIV+1)*(VDIV+16)
  MCG_C6 |= MCG_C6_PLLS_MASK;                           //PLL Enable
  
  while(!(MCG_S & MCG_S_PLLST_MASK));                   //等待PLL时钟源选择
  while(!(MCG_S & MCG_S_LOCK0_MASK));                   //等待PLL锁定
  
  //进入PBE模式(PLL旁路时钟源模式)
  //PBE->PEE(PLL旁路时钟源模式->PLL外部时钟源模式)
  MCG_C1 &= ~MCG_C1_CLKS_MASK;                          //选择PLL输出时钟
  
  while(((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3);      //等待选择PLL时钟
  
  return MCG_PLL[PLLx].PLL_CLK;
}