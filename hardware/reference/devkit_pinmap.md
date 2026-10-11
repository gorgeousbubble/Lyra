# 开发套件引脚总表（遗留参考）

> **这不是 Lyra 手表的设计文档。**
> 本文件记录的是**当前固件跑在 NXP MAPS-K64 + MAPS-DOCK 开发套件上**的引脚现状，
> 由源码反推而来，作为手表设计的输入依据保留。
>
> 手表本身的硬件方案见 **[`../watch/design.md`](../watch/design.md)**，
> 其中已剥离本文件里绝大部分内容（彩屏、FlexBus、外部 SRAM、SD 卡、LCM、摇杆、LED）。
>
> 互连图：[`devkit_interconnect.svg`](devkit_interconnect.svg)

## 当前运行平台

当前固件跑在开发套件上，**不是自制板**：

| 组成 | 说明 |
| --- | --- |
| 主板 | NXP MAPS-K64（MK64FX512VLQ12，144-LQFP） |
| 扩展板 | MAPS-DOCK（ILI9341 彩屏、ST7565R 单色屏、W25Q80、按键、摇杆、LED） |
| 外接模块 | MPU6050、MAX30102、SSD1306 OLED —— 杜邦线连接 |

套件本身的**正式原理图在仓库里**，不需要重新绘制：

- `../../reference/MAPS_MK64/MAPS-K64.pdf` — 主板
- `../../reference/MAPS_MK64/Dock.pdf` — 扩展板
- `../../reference/Kinetis K6X RM/K64P144M120SF5RM.pdf` — MCU 参考手册

`devkit_interconnect.svg` 的定位是**固件视角的互连关系图**：把散落在源码里的引脚宏集中成一张图，用于接线核对和二次设计输入。它不是投产图纸。

> 重要结论（详见 `../watch/design.md`）：三块显示器里**只有 SSD1306 OLED 在做 UI**。
> 彩屏只在开机贴一张 BMP，LCM 只显示调试变量页。下面的 FlexBus、外部 SRAM、SD 卡、
> LCM、摇杆、LED 章节对手表功能都是可删项，保留在此仅作追溯。

## 时钟配置

来源 `app/inc/conf.h`、`chip/src/mcg.c`

| 项 | 值 |
| --- | --- |
| 外部晶振 | 50 MHz |
| PLL | PLL100（PRDIV 24, VDIV 26） |
| Core / Bus / FlexBus / Flash | 100 / 50 / 25 / 25 MHz |
| RTC 振荡器 | 32.768 kHz，片内 16 pF 负载电容（`RTC_CR` SC16P） |
| 看门狗 | 启动时关闭（`startup/src/system_init.c`） |

## 引脚总表

### FlexBus — ILI9341 彩屏 + 外部 SRAM

16-bit 复用地址/数据，8080 只写方式。来源 `chip/inc/port_cfg.h`、`board/src/maps_dock_lcd.c`

| 信号 | 引脚 |
| --- | --- |
| FB_AD0 – AD4 | PTD6, PTD5, PTD4, PTD3, PTD2 |
| FB_AD5 – AD11 | PTC10, PTC9, PTC8, PTC7, PTC6, PTC5, PTC4 |
| FB_AD12 – AD14 | PTC2, PTC1, PTC0 |
| FB_AD15 – AD17 | PTB18, PTB17, PTB16 |
| FB_AD18 – AD20 | PTB11, PTB10, PTB9 |
| FB_ALE | PTD0 |
| FB_CS0（彩屏片选） | PTD1 |
| FB_CS2（SRAM 片选） | PTC18 |
| FB_R/W（WR） | PTC11 |
| FB_OE（RD） | PTB19 |
| FB_CLK | PTC3 |
| FB_BE23_16 | PTC16 |
| FB_BE31_24 | PTC17 |
| 背光 | PTD9 —— 已定义但固件从未驱动 |

地址窗口：彩屏 CS0 @ `0x6000_0000`，SRAM CS2 @ `0x8000_0000`。
命令/数据由 **FlexBus 地址位 A1** 区分（`0x6000_0000` 命令，`0x6000_0002` 数据），不占用 GPIO。
分辨率 320 × 240，RGB565。SRAM 窗口配置齐全但应用层从未访问。

### SPI2 — 两个从设备共用

来源 `chip/inc/port_cfg.h`、`chip/src/spi.c`

| 信号 | 引脚 | 去向 |
| --- | --- | --- |
| SPI2_SCK | PTD12 | 共用 |
| SPI2_SOUT (MOSI) | PTD13 | 共用 |
| SPI2_SIN (MISO) | PTD14 | 共用 |
| SPI2_PCS0 | PTD11 | W25Q80 Flash 片选 |
| SPI2_PCS1 | PTD15 | ST7565R LCM 片选 |
| LCM D/C | PTD10 | GPIO |

总线速率 10 Mbps，由 `MAPS_Dock_LCM_Init()` 一次设定；W25Q80 只追加片选，因此也跑在 10 MHz。
LCM 为 Manley ZYMG12864，128 × 64，8 页。

### 软件 I2C（三条独立总线，GPIO 位翻转）

| 器件 | SCL | SDA | 7-bit 地址 |
| --- | --- | --- | --- |
| MPU6050 六轴 IMU | PTA29 | PTA28 | 0x68（AD0 = 0） |
| MAX30102 心率/血氧 | PTE10 | PTE11 | 0x57 |
| SSD1306 OLED 128×64 | PTE26 | PTE28 | 0x3C |

MPU6050 配置：125 Hz 采样、DLPF ≈ 20 Hz、±2 g、±250 °/s，INT 未接。
MAX30102：`PART_ID` 应读回 0x15，`INTR_ENABLE_1 = 0xC0`（PPG_RDY + ALC_OVF）。

> 三条总线固件都没有开内部上拉，**硬件上必须有外部上拉电阻**。

### MAX30102 旁路信号

| 功能 | 引脚 |
| --- | --- |
| INT（传感器输出，轮询读取） | PTA10 |
| LED 驱动 | PTA7 |

### 离散 I/O

| 器件 | 引脚 | 电气特性 |
| --- | --- | --- |
| 蜂鸣器 | PTE7 | 纯 GPIO 翻转，无 PWM ⇒ 需有源蜂鸣器或外加驱动级 |
| KEY0 – KEY3 | PTB21, PTB22, PTB23, PTB20 | 低电平有效，**需外部上拉** |
| 摇杆 上/下/左/右/确认 | PTA27, PTA26, PTA25, PTA24, PTA4 | 低电平有效，PTA4 即 NMI 引脚 |
| LED LD3 – LD6 | PTB7, PTB6, PTB5, PTB4 | **低电平点亮** |

### 硬件外设

| 外设 | 引脚 | 配置 |
| --- | --- | --- |
| UART4 | RX PTC14 / TX PTC15 | 115200 8N1，遥测输出 |
| I2C0（硬件） | SCL PTE24 / SDA PTE25 | 400 kHz，已初始化但应用层无调用 |
| ADC0 | ADC0_DP0 / ADC0_DM0 | 专用模拟脚，12-bit，100 Hz，Vref 3300 mV |

ADC 两路是 `app/src/adc_scope.c` 的双通道示波器输入，**不是电池电压检测**。

### 专用引脚

| 功能 | 引脚 |
| --- | --- |
| 主晶振 | EXTAL / XTAL（50 MHz） |
| RTC 晶振 | EXTAL32 / XTAL32（32.768 kHz） |
| 调试口 | SWD / JTAG = PTA0 – PTA3，另加 RESET_b |
| SDHC（保留未用） | PTE0 – PTE5 |

### 中断

来源 `app/src/init.c`

| 源 | 周期 | 优先级 |
| --- | --- | --- |
| PIT0 | 1 ms | 2 |
| PIT1 | 1 ms | 3 |
| RTC | 溢出 / 闹钟 | 4 |

PTC19 与 PTD15 是板载外部中断线，固件未使用（PTD15 同时是 LCM 片选）。

## 未初始化的外设

驱动代码存在但 `AllInit()` 从未调用，画图和选型时可忽略：

SDHC + FatFs、USB CDC（`lib/USB/`，VID 0x15A2）、CAN、DAC、I2S、PDB、LPTMR、DMA、FTM1/2/3、Flash 驱动。

`dev/src/oled.c` 的 SPI 版 OLED 为死代码，其引脚（PTB19 – PTB23）与按键和 FB_OE 冲突，只有 `oled_i2c.c` 被实际调用。

## 已知问题

**PTA7 被重复声明。** `app/inc/health.h:29` 定义 `IRD` 为 PTA7 的 GPIO，`:32` 又把 `RD` 定义成 `FTM0_CH4`，而 `FTM0_CH4_PIN` 正是 PTA7（`chip/inc/port_cfg.h:58`）。`app/src/health.c:55-57` 先初始化 PWM 再初始化 GPIO，GPIO 覆盖 PWM ⇒ `RD_Duty` 实际不起作用，硬件上只有一条网络。

**MCG 分频表与注释不符。** `chip/src/mcg.c:18` 的注释公式 `EXTAL/(PRDIV+1)*(VDIV+16)/2` 无法还原表中数值；表项符合 K60 的 `EXTAL/(PRDIV+1)×(VDIV+24)`。按此算 PLL 参考频率为 2 MHz，低于 K64 手册要求的 8–16 MHz —— 这张表疑似从 K60 代码继承而来，建议核对。

## 固件无法提供的信息

以下内容源码中完全没有痕迹，绘制完整原理图或 PCB 前必须由人确定：

供电方式与稳压器拓扑、去耦网络、是否有电池/充电管理/VBAT 备份域、背光驱动与限流、蜂鸣器驱动管、LED 限流电阻、软件 I2C 上拉阻值、FlexBus 串阻、VREFH/VREFL 接法、50 MHz 是无源晶体还是有源晶振、FB_CS2 后面 SRAM 的具体型号、彩屏模块/FPC 的具体型号、连接器与排针的物理映射。
