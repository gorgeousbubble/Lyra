/*
 *     COPYRIGHT NOTICE
 *     Copyright(c) 2025, alopex
 *     All rights reserved.
 *
 * @file       conf.c
 * @brief      MK64FX512VLQ12/MK64FN1M0VLQ12
 * @author     alopex
 * @version    v1.0
 * @date       2025-06-24
 */

#include "conf.h"

/*
 *  @brief      assert failed function
 *  @param      file    file path
 *  @param      line    line number
 *  @since      v1.0
 *  Sample usage:       assert_failed(__FILE__, __LINE__);
 */
void assert_failed(char *file, int line)
{
  while(1)
  {
    ;
  }
}

/*
 *  @brief      default interrupt service function
 *  @since      v1.0
 *  @note       this function insert into vector table
 */
void default_isr(void)
{
  return;
}


uint8 Vector_Str[][15] = 
{
  /******  Cortex-M4 Processor Exceptions Numbers ***********************************************************************/
    "",                         //0
    "",
    "NonMaskable"               , /*!< 2 Non Maskable Interrupt                              */
    "HardFault"                 , /*!< 3 Hard Fault                                          */
    "MemManagement"             , /*!< 4 Cortex-M4 Memory Management Interrupt               */
    "BusFault"                  , /*!< 5 Cortex-M4 Bus Fault Interrupt                       */
    "UsageFault"                , /*!< 6 Cortex-M4 Usage Fault Interrupt                     */
    "",                         //7
    "",
    "",                         //9
    "",
    "SVCall"                    , /*!< 11 Cortex-M4 SV Call Interrupt                        */
    "DebugMonitor"              , /*!< 12 Cortex-M4 Debug Monitor Interrupt                  */
    "",
    "PendSV"                    , /*!< 14 Cortex-M4 Pend SV Interrupt                        */
    "SysTick"                   , /*!< 15 Cortex-M4 System Tick Interrupt                    */
    /******  Kinetis K64 specific Interrupt Numbers *********************************************************************/
    "DMA0"                      , // DMA Channel 0 Transfer Complete
    "DMA1"                      , // DMA Channel 1 Transfer Complete
    "DMA2"                      , // DMA Channel 2 Transfer Complete
    "DMA3"                      , // DMA Channel 3 Transfer Complete
    "DMA4"                      , // DMA Channel 4 Transfer Complete
    "DMA5"                      , // DMA Channel 5 Transfer Complete
    "DMA6"                      , // DMA Channel 6 Transfer Complete
    "DMA7"                      , // DMA Channel 7 Transfer Complete
    "DMA8"                      , // DMA Channel 8 Transfer Complete
    "DMA9"                      , // DMA Channel 9 Transfer Complete
    "DMA10"                     , // DMA Channel 10 Transfer Complete
    "DMA11"                     , // DMA Channel 11 Transfer Complete
    "DMA12"                     , // DMA Channel 12 Transfer Complete
    "DMA13"                     , // DMA Channel 13 Transfer Complete
    "DMA14"                     , // DMA Channel 14 Transfer Complete
    "DMA15"                     , // DMA Channel 15 Transfer Complete
    "DMA_Error"                 , // DMA Error Interrupt
    "MCM"                       , // Normal Interrupt
    "FTFL"                      , // FTFL Interrupt
    "Read_Collision"            , // Read Collision Interrupt
    "LVD_LVW"                   , // Low Voltage Detect, Low Voltage Warning
    "LLW"                       , // Low Leakage Wakeup
    "Watchdog"                  , // WDOG Interrupt
    "RNG"                       , // RNGB Interrupt
    "I2C0"                      , // I2C0 interrupt
    "I2C1"                      , // I2C1 interrupt
    "SPI0"                      , // SPI0 Interrupt
    "SPI1"                      , // SPI1 Interrupt
    "I2S0_Tx"                   , // I2S0_Tx Interrupt
    "I2S0_Rx"                   , // I2S0_Rx Interrupt
    "Reserved46"                , // reserved interrupt 46
    "UART0_RX_TX"               , // UART0 Receive/Transmit interrupt
    "UART0_ERR"                 , // UART0 Error interrupt
    "UART1_RX_TX"               , // UART1 Receive/Transmit interrupt
    "UART1_ERR"                 , // UART1 Error interrupt
    "UART2_RX_TX"               , // UART2 Receive/Transmit interrupt
    "UART2_ERR"                 , // UART2 Error interrupt
    "UART3_RX_TX"               , // UART3 Receive/Transmit interrupt
    "UART3_ERR"                 , // UART3 Error interrupt
    "ADC0"                      , // ADC0 interrupt
    "CMP0"                      , // CMP0 interrupt
    "CMP1"                      , // CMP1 interrupt
    "FTM0"                      , // FTM0 fault, overflow and channels interrupt
    "FTM1"                      , // FTM1 fault, overflow and channels interrupt
    "FTM2"                      , // FTM2 fault, overflow and channels interrupt
    "CMT"                       , // CMT interrupt
    "RTC"                       , // RTC interrupt
    "RTC_Sec"                   , // RTC_Sec interrupt
    "PIT0"                      , // PIT timer channel 0 interrupt
    "PIT1"                      , // PIT timer channel 1 interrupt
    "PIT2"                      , // PIT timer channel 2 interrupt
    "PIT3"                      , // PIT timer channel 3 interrupt
    "PDB0"                      , // PDB0 Interrupt
    "USB0"                      , // USB0 interrupt
    "USBDCD"                    , // USBDCD Interrupt
    "Reserved71"                , // reserved interrupt 71
    "DAC0"                      , // DAC0 interrupt
    "MCG"                       , // MCG Interrupt
    "LPTimer"                   , // LPTimer interrupt
    "PORTA"                     , // Port A interrupt
    "PORTB"                     , // Port B interrupt
    "PORTC"                     , // Port C interrupt
    "PORTD"                     , // Port D interrupt
    "PORTE"                     , // Port E interrupt
    "SWI"                       , // SWI interrupt
    "SPI2"                      , // SPI2 Interrupt
    "UART4_RX_TX"               , // UART4 Receive/Transmit interrupt
    "UART4_ERR"                 , // UART4 Error interrupt
    "UART5_RX_TX"               , // UART5 Receive/Transmit interrupt
    "UART5_ERR"                 , // UART5 Error interrupt
    "CMP2"                      , // CMP2 interrupt
    "FTM3"                      , // FTM3 fault, overflow and channels interrupt
    "DAC1"                      , // DAC1 interrupt
    "ADC1"                      , // ADC1 interrupt
    "I2C2"                      , // I2C2 interrupt
    "CAN0_OR_MB_buf"            , // CAN0 OR'd Message Buffers Interrupt
    "CAN0_Bus_Off"              , // CAN0 Bus Off Interrupt
    "CAN0_Error"                , // CAN0 Error Interrupt
    "CAN0_Tx_Warning"           , // CAN0 Tx Warning Interrupt
    "CAN0_Rx_Warning"           , // CAN0 Rx Warning Interrupt
    "CAN0_Wake_Up"              , // CAN0 Wake Up Interrupt
    "SDHC"                      , // SDHC Interrupt
    "ENET_1588_Timer"           , // Ethernet MAC IEEE 1588 Timer Interrupt
    "ENET_Transmit"             , // Ethernet MAC Transmit Interrupt
    "ENET_Receive"              , // Ethernet MAC Receive Interrupt
    "ENET_Error"                , // Ethernet MAC Error and miscelaneous Interrupt
    "Reserved102"               , // reserved interrupt 102
    "Reserved103"               , // reserved interrupt 103
    "Reserved104"               , // reserved interrupt 104
    "Reserved105"               , // reserved interrupt 105
    "Reserved106"               , // reserved interrupt 106
    "Reserved107"               , // reserved interrupt 107
    "Reserved108"               , // reserved interrupt 108
    "Reserved109"               , // reserved interrupt 109
    "Reserved110"               , // reserved interrupt 110
    "Reserved111"               , // reserved interrupt 111
    "Reserved112"               , // reserved interrupt 112
    "Reserved113"               , // reserved interrupt 113
    "Reserved114"               , // reserved interrupt 114
    "Reserved115"               , // reserved interrupt 115
};