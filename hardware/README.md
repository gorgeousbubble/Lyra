# Lyra 手表 — 硬件

```
hardware/
├── README.md                          本文件
├── watch/                             ★ Lyra 手表本身的设计
│   ├── design.md                      硬件设计方案、引脚分配、决策记录
│   ├── architecture.svg               目标架构图（浏览器直接打开）
│   └── board_pins.h                   引脚图覆盖头，可直接拷进 app/inc/
├── reference/                         遗留参考：当前固件跑的开发套件
│   ├── devkit_pinmap.md               开发板引脚总表（设计输入依据）
│   └── devkit_interconnect.svg        开发板互连图
└── pcb/
    └── README.md                      PCB 状态与前置条件
```

## 一句话现状

Lyra 固件目前跑在 **NXP MAPS-K64 + MAPS-DOCK 开发套件** 上，`board/maps_dock_*` 是从旧项目复用的。
目标是做一块**自己的智能手表板**。`watch/` 是这个目标的设计；`reference/` 是从固件反推出的开发板现状，只作追溯用。

## 核心发现

固件里三块显示器，**只有 SSD1306 128×64 OLED 在做 UI** —— 全部 16 个菜单功能的 `Render_*()`
都收尾于 `Oled_I2C_Draw_Picture_128x64()`。ILI9341 彩屏只在 `main.c:164` 开机贴一张 BMP，
ST7565R LCM 只显示调试变量页。

因此手表只需要 **5 个器件**：OLED、MPU6050、MAX30102、蜂鸣器/马达、4 个按键。
信号引脚从开发板的约 80 根降到约 **27 根**，并省掉 150 KB flash、SD 卡 + FatFs 和外部 SRAM。

完整证据链与引脚分配见 [`watch/design.md`](watch/design.md)。

## 当前进度

| 阶段 | 状态 |
| --- | --- |
| 固件硬件依赖分析 | 完成 |
| 最小硬件集确定 | 完成 |
| 架构图 | 完成 |
| 引脚约束分析 | 完成（13 根可自由重排 / 10 根受约束，删 SPI2 后少 4 根） |
| 决策 1-5 | **全部已定** —— 见 `watch/design.md` |
| 时钟器件选型 | 完成（已查实：50 MHz 用有源晶振，32.768 kHz 用无源晶体） |
| 电源拓扑 | 完成（USB → 充电 IC → 锂聚 → 3.3 V LDO + 分压检测） |
| 100-LQFP 可用引脚核对 | **待你确认** — 我读不了那份 PDF |
| 主频实测验证 | **待你确认** — PLL 配置算术存疑 |
| KiCad 原理图 | 可以开始，等上面两项输入 |
| PCB | 未开始，布局需先定机械约束 |

## 决策结果

| # | 决策 | 选择 |
| --- | --- | --- |
| 1 | MCU 封装 | K64 100-LQFP（`VLL`），14 × 14 mm |
| 2 | 配置存储 | 片内 FlexNVM，删掉 W25Q80 |
| 3 | OLED 形态 | 成品 0.96" I2C 模块 |
| 4 | 提醒方式 | 振动马达 |
| 5 | 续航 | 原型优先，接受小时级 |

## 卡在哪

**一、100-LQFP 引脚可用性。** 删掉 SPI2 之后，真正需要查手册的受约束引脚只剩
`PTC14` / `PTC15`（UART4，备选 `PTE25` / `PTE24`）一对，加一份「100 脚上还剩哪些 GPIO」
的清单用来安置 13 根位带信号。晶振、SWD、RESET、ADC0_DP0 在任何封装上都必然存在。

我尝试从 `reference/Kinetis K6X RM/K64P144M120SF5RM.pdf` 提取引脚表失败
（写了 PDF 流解压器扫了 5438 个流，`LQFP` 零命中，该 PDF 文字为 CID 编码），
也没有联网工具。**另一条更省事的路**：装 KiCad 之后，它的符号库文件 `.kicad_sym`
是纯文本，里面就包含 `VLL` 封装的完整引脚表 —— 那个我能直接读。

**二、主频需要实测。** `MCG_PLL` 表把 `{PRDIV=24, VDIV=26}` 标为 100 MHz，
但 `PLL_Init()` 自己注释的公式算出来是 84 MHz。两者差 19%，会影响 PIT 定时和
UART 波特率。订 50 MHz 晶振前建议先在开发板上对表验证。详见 `watch/design.md`。
